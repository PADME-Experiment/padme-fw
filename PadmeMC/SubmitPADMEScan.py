#!/usr/bin/env python3
"""
SubmitPADMEScan.py - scansione in energia del fascio con PADMEMC (Geant4)

Per ogni energia letta da un file esterno:
  * riscala i campi dei dipoli DHSTB001/DHSTB002 (B proporzionale a p),
  * (opzionale) riscala lo spread in momento mantenendo dp/p costante,
  * crea  <outdir>/<tag>/E<p>MeV/Run<i>/  con la macro .mac dedicata,
  * genera la lista dei job e la passa a GNU parallel, che ne tiene
    in esecuzione al massimo N contemporaneamente.

File delle energie (una riga per punto, '#' = commento):
    # p[MeV/c]   [n. eventi opzionale]
    250
    270.3
    300          20000

Esempi:
    ./SubmitPADMEScan.py energies.txt -n 20 -j 16
    ./SubmitPADMEScan.py energies.txt -n 20 -j 16 --dry-run      # solo directory e macro
    ./SubmitPADMEScan.py energies.txt -n 20 -j 16 --resume       # rilancia solo i job non terminati
    ./SubmitPADMEScan.py energies.txt -n 20 -j 16 --resume --dry-run   # solo la lista

--resume: un job e' terminato se nelle ultime 25 righe del suo PADMEMC.log
compare "Run terminated". Gli altri (falliti, interrotti, mai partiti)
vengono elencati; dopo conferma si cancellano *.txt e *.root nella loro
cartella Run e si rilanciano con un nuovo joblog (joblog.resume-<data>.txt).
Prima vanno fermati parallel e i PADMEMC ancora attivi: lo script controlla.
Vanno usati lo stesso file di energie e lo stesso -n del lancio originale.
    ./SubmitPADMEScan.py energies.txt -n 50 -j 24 --setup /path/setup_geant4.sh

Monitoraggio:   tail -f <outdir>/<tag>/joblog.txt
Stop:           kill $(cat <outdir>/<tag>/parallel.pid)   (parallel termina i figli)

Richiede GNU parallel (AlmaLinux 9: dnf install epel-release && dnf install parallel).
"""

import argparse
import os
import random
import shlex
import shutil
import subprocess
import sys
import time
from pathlib import Path
from string import Template

# ---------------------------------------------------------------------------
# Configurazione di riferimento (punto in cui i campi sono tarati)
# ---------------------------------------------------------------------------
P_REF = 270.3            # MeV/c
MOM_SPREAD_REF = 3.5     # MeV/c a P_REF

# Campi magnetici da riscalare linearmente con il momento:
# (comando Geant4, valore a P_REF, unita'). Aggiungere qui eventuali
# altri elementi (es. gradienti dei quadrupoli) se PADMEMC li espone via macro.
SCALED_FIELDS = [
    ("/Detector/BeamLine/DHSTB001_FieldY", -0.5230, "tesla"),
    ("/Detector/BeamLine/DHSTB002_FieldY", -0.5230, "tesla"),
]

DEFAULT_OUTDIR = "/data9Vd1/padme/raggi/"
DEFAULT_EXE = "PADMEMC"
DEFAULT_GDML = "gdml"

MACRO_TEMPLATE = Template("""\
# Macro generata da SubmitPADMEScan.py il $date
# Momento fascio: $momentum MeV/c  (riferimento $pref MeV/c, scala $scale)
/control/verbose 0
/run/verbose 1
/tracking/verbose 0
#Detector and line configuration
/Detector/Setup 50
/Detector/EnableStructure BeamLine
/Detector/DisableSubDetector LeadGlass
/Detector/SetMagneticFieldValue 0. tesla
$fields
/Detector/BeamLine/EnableQuadrupoles true
/Detector/CrossMagneticVolume none
# Half of collimators aperture

# No hits/digis/MCTruth in MCData.root (only UBTF.root and beam_output.txt are needed)
/output/DisableDetectorIO ECal
/output/DisableDetectorIO Target
/output/DisableDetectorIO PVeto
/output/DisableDetectorIO EVeto
/output/DisableDetectorIO HEPVeto
/output/DisableDetectorIO SAC
/output/DisableDetectorIO TPix
/output/DisableDetectorIO ETag
/output/DisableDetectorIO MM
/output/DisableDetectorIO TMM
/output/DisableDetectorIO LeadGlass
/output/DisableDetectorIO MCTruth
/settings/EnableMCTruth false

/run/initialize
$seeds
/beam/n_e+_per_bunch $ne_per_bunch
$poisson
/beam/bunch_time_length 250. ns
/beam/momentum $momentum MeV
/beam/position_spread_on true
/beam/position_x_spread 5. mm
/beam/position_y_spread 5. mm
/beam/momentum_spread_on true
/beam/momentum_spread $mom_spread MeV
/beam/emittance_on true
/beam/emittance_x 1.5 mrad
/beam/emittance_y 1.5 mrad
/beam/beamspot_on false

#STARTING at the entrance of DHSTB001
/beam/position_x 11820.0 mm
/beam/position_y 0. cm
/beam/position_z -10833.18 mm
/beam/direction -1. 0. 0.

/run/beamOn $nevents
""")


def read_energies(path, default_nev):
    """Ritorna una lista di (momento, n_eventi) dal file di input."""
    points = []
    with open(path) as f:
        for lineno, raw in enumerate(f, 1):
            line = raw.split("#", 1)[0].strip()
            if not line:
                continue
            cols = line.replace(",", " ").split()
            try:
                p = float(cols[0])
                nev = int(float(cols[1])) if len(cols) > 1 else default_nev
            except ValueError:
                sys.exit(f"ERRORE {path}:{lineno}: riga non valida: {raw.rstrip()}")
            if p <= 0 or nev <= 0:
                sys.exit(f"ERRORE {path}:{lineno}: momento ed eventi devono essere > 0")
            points.append((p, nev))
    if not points:
        sys.exit(f"ERRORE: nessuna energia trovata in {path}")
    seen = set()
    for p, _ in points:
        if p in seen:
            sys.exit(f"ERRORE: energia {p:g} MeV ripetuta in {path}")
        seen.add(p)
    return points


TERMINATED_TAG = "Run terminated"
TAIL_LINES = 25


def run_terminated(rdir, nlines=TAIL_LINES):
    """True se nelle ultime nlines righe di Run*/PADMEMC.log c'e' 'Run terminated'."""
    log = rdir / "PADMEMC.log"
    if not log.is_file():
        return False
    with open(log, "rb") as f:
        f.seek(0, 2)
        size = f.tell()
        block = 0
        while True:
            block = min(size, max(block * 2, 64 * 1024))
            f.seek(size - block)
            data = f.read(block)
            # nlines righe complete (+1 per l'eventuale riga parziale iniziale)
            if block == size or data.count(b"\n") > nlines + 1:
                break
    lines = data.decode(errors="replace").splitlines()[-nlines:]
    return any(TERMINATED_TAG in l for l in lines)


def read_joblog_status(prod):
    """{cartella Run: True/False} dall'ultima esecuzione registrata nei joblog*.txt."""
    status = {}
    for log in sorted(prod.glob("joblog*.txt")):
        with open(log) as f:
            for line in f:
                cols = line.rstrip("\n").split("\t")
                if len(cols) < 9 or cols[0] == "Seq":
                    continue
                try:
                    start = float(cols[2])
                    ok = int(cols[6]) == 0 and int(cols[7]) == 0
                    tokens = shlex.split(cols[8])
                    rdir = Path(tokens[tokens.index("cd") + 1]).resolve()
                except (ValueError, IndexError):
                    continue
                if rdir not in status or start >= status[rdir][0]:
                    status[rdir] = (start, ok)
    return {k: v[1] for k, v in status.items()}


def check_nothing_running(prod, exe):
    """Esce con errore se il parallel della produzione o dei PADMEMC sono attivi."""
    pidfile = prod / "parallel.pid"
    if pidfile.is_file():
        try:
            pid = int(pidfile.read_text().split()[0])
        except (ValueError, IndexError):
            pid = None
        if pid:
            alive = True
            try:
                os.kill(pid, 0)
            except ProcessLookupError:
                alive = False
            except PermissionError:
                pass
            cmdline = Path(f"/proc/{pid}/cmdline")
            if alive and cmdline.exists() and b"parallel" not in cmdline.read_bytes():
                alive = False          # PID riusato da un altro processo
            if alive:
                sys.exit(f"ERRORE: GNU parallel della produzione e' ancora attivo "
                         f"(PID {pid}). Fermalo prima:  kill {pid}")
    pgrep = shutil.which("pgrep")
    if pgrep is None:
        print("ATTENZIONE: pgrep non disponibile, non controllo i PADMEMC attivi")
        return
    out = subprocess.run([pgrep, "-f", str(exe)], capture_output=True, text=True).stdout.split()
    pids = [p for p in out if p != str(os.getpid())]
    if pids:
        sys.exit(f"ERRORE: {len(pids)} processi PADMEMC della produzione ancora attivi "
                 f"(PID {' '.join(pids)}). Aspetta che finiscano o fermali "
                 f"(kill {' '.join(pids)}) prima del --resume.")


def energy_tag(p):
    return f"E{p:g}MeV"


def make_seeds(rng, used):
    while True:
        s = (rng.randint(1, 2**31 - 1), rng.randint(1, 2**31 - 1))
        if s not in used:
            used.add(s)
            return s


def parse_args():
    ap = argparse.ArgumentParser(
        description="Scansione in energia PADMEMC con GNU parallel",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__)
    ap.add_argument("energies", help="file con l'elenco dei momenti [MeV/c]")
    ap.add_argument("-n", "--njobs", type=int, default=50,
                    help="job identici per ogni energia (default 50)")
    ap.add_argument("-j", "--max-parallel", type=int,
                    default=max(1, (os.cpu_count() or 2) - 2),
                    help="job in esecuzione contemporanea (default nproc-2)")
    ap.add_argument("-e", "--events", type=int, default=10000,
                    help="eventi per job se non dati nel file (default 10000)")
    ap.add_argument("--ne-per-bunch", type=int, default=3500,
                    help="e+ per bunch (default 3500; con 1 la macro spegne la Poisson)")
    ap.add_argument("-o", "--outdir", default=DEFAULT_OUTDIR,
                    help=f"directory base di output (default {DEFAULT_OUTDIR})")
    ap.add_argument("-t", "--tag", default="EnergyScan",
                    help="nome della produzione (sottodirectory di outdir)")
    ap.add_argument("--exe", default=DEFAULT_EXE,
                    help="eseguibile PADMEMC (default ./PADMEMC)")
    ap.add_argument("--gdml", default=DEFAULT_GDML,
                    help="directory gdml (default ./gdml)")
    ap.add_argument("--copy-gdml", action="store_true",
                    help="copia gdml in ogni Run (default: link a una copia unica)")
    ap.add_argument("--abs-spread", action="store_true",
                    help=f"spread in momento fisso a {MOM_SPREAD_REF} MeV "
                         "(default: dp/p costante)")
    ap.add_argument("--no-seeds", action="store_true",
                    help="non scrivere /random/setSeeds nelle macro")
    ap.add_argument("--seed", type=int, default=None,
                    help="seme base per generare i semi dei job (default: orologio)")
    ap.add_argument("--setup", default=None,
                    help="script da 'sourcare' prima di ogni job (env Geant4/ROOT)")
    ap.add_argument("--delay", type=float, default=1.0,
                    help="secondi tra l'avvio di due job (default 1)")
    ap.add_argument("--nice", type=int, default=0,
                    help="priorita' nice dei job (default 0)")
    ap.add_argument("--parallel-opts", default="",
                    help="opzioni extra per GNU parallel, es. \"--memfree 4G\"")
    ap.add_argument("--resume", action="store_true",
                    help="rilancia solo i job senza 'Run terminated' nelle ultime "
                         "25 righe di PADMEMC.log (chiede conferma, poi cancella "
                         "*.txt e *.root di quei job)")
    ap.add_argument("--foreground", action="store_true",
                    help="resta attaccato a parallel invece di lanciarlo in background")
    ap.add_argument("--dry-run", action="store_true",
                    help="crea directory, macro e lista job ma non lancia nulla")
    return ap.parse_args()


def main():
    args = parse_args()
    indir = Path.cwd()

    points = read_energies(args.energies, args.events)
    exe_src = (indir / args.exe).resolve()
    gdml_src = (indir / args.gdml).resolve()
    if not exe_src.is_file():
        sys.exit(f"ERRORE: eseguibile non trovato: {exe_src}")
    if not gdml_src.is_dir():
        sys.exit(f"ERRORE: directory gdml non trovata: {gdml_src}")
    if args.setup and not Path(args.setup).is_file():
        sys.exit(f"ERRORE: script di setup non trovato: {args.setup}")

    parallel = shutil.which("parallel")
    if parallel is None and not args.dry_run:
        sys.exit("ERRORE: GNU parallel non trovato. Su AlmaLinux 9:\n"
                 "  sudo dnf install epel-release && sudo dnf install parallel")

    prod = Path(args.outdir).resolve() / args.tag
    # --resume deve trovare una produzione gia' lanciata, non crearne una nuova
    if args.resume and not (prod / "jobs.txt").is_file():
        msg = [f"ERRORE: --resume ma {prod} non e' una produzione esistente "
               "(manca jobs.txt). Controlla -o e -t."]
        base = Path(args.outdir).resolve()
        found = sorted(base.glob("*/production_info.txt")) if base.is_dir() else []
        if found:
            msg.append(f"Produzioni trovate in {base}:")
            for info_f in found:
                first = info_f.read_text(errors="replace").splitlines()[:1]
                msg.append(f"  -t {info_f.parent.name:<20} {first[0] if first else ''}")
        else:
            msg.append(f"Nessuna produzione (production_info.txt) in {base}")
        sys.exit("\n".join(msg))
    prod.mkdir(parents=True, exist_ok=True)
    joblog = prod / "joblog.txt"
    if not args.resume and joblog.exists():
        joblog.rename(prod / f"joblog.{time.strftime('%Y%m%d-%H%M%S')}.txt")

    # --resume: niente della produzione deve essere in esecuzione
    if args.resume:
        check_nothing_running(prod, prod / "PADMEMC")

    # Copia unica di eseguibile e gdml nella produzione: una ricompilazione
    # in INDIR non tocca i job in corso.
    exe = prod / "PADMEMC"
    if not (args.resume and exe.exists()):
        shutil.copy2(exe_src, exe)
        exe.chmod(0o755)
    gdml_shared = prod / "gdml"
    if not args.copy_gdml and not (args.resume and gdml_shared.exists()):
        if gdml_shared.exists():
            shutil.rmtree(gdml_shared)
        shutil.copytree(gdml_src, gdml_shared)

    # MR 27/11/2026 single particle (1 e+ per bunch): Poisson spread off, otherwise
    # PADMEMC (Poisson on by default) would give 0, 1, 2, ... e+ per event
    poisson = ("# single particle: esattamente 1 e+ per evento\n/beam/n_e+_poisson_on false"
               if args.ne_per_bunch == 1 else "")

    seed_base = args.seed if args.seed is not None else int(time.time())
    rng = random.Random(seed_base)
    used_seeds = set()
    date = time.strftime("%Y-%m-%d %H:%M:%S")
    macro_name = "run.mac"

    jobs = []
    jobdirs = []        # cartella Run di ogni job (stesso ordine di jobs)
    info = [f"# Produzione {args.tag}  creata {date}",
            f"# INDIR {indir}",
            f"# exe {exe_src}",
            f"# njobs/energia {args.njobs}  seed_base {seed_base}",
            "# p[MeV/c]  scala  " + "  ".join(c.split("/")[-1] for c, _, _ in SCALED_FIELDS)
            + "  dp[MeV]  nev/job"]

    for ie, (p, nev) in enumerate(points):
        scale = p / P_REF
        fields = [(cmd, bref * scale, unit) for cmd, bref, unit in SCALED_FIELDS]
        field_lines = "\n".join(f"{cmd} {b:.5f} {unit}" for cmd, b, unit in fields)
        spread = MOM_SPREAD_REF if args.abs_spread else MOM_SPREAD_REF * scale
        info.append(f"{p:10g} {scale:7.4f}  " + "  ".join(f"{b:+.5f}" for _, b, _ in fields)
                    + f"  {spread:.4f}  {nev}")

        edir = prod / energy_tag(p)
        edir.mkdir(exist_ok=True)
        for i in range(args.njobs):
            # i semi vanno estratti sempre, anche per le Run saltate,
            # cosi' --resume riproduce gli stessi semi a parita' di --seed
            s1, s2 = make_seeds(rng, used_seeds)
            rdir = edir / f"Run{i}"
            rdir.mkdir(exist_ok=True)

            if not args.resume:
                for f in rdir.glob("*.root"):
                    f.unlink()

            seeds = "" if args.no_seeds else f"/random/setSeeds {s1} {s2}"
            macro = MACRO_TEMPLATE.substitute(
                date=date, momentum=f"{p:g}", pref=f"{P_REF:g}",
                scale=f"{scale:.6f}", fields=field_lines, seeds=seeds,
                ne_per_bunch=args.ne_per_bunch, poisson=poisson, mom_spread=f"{spread:.4f}",
                nevents=nev)
            # con --resume non si tocca la macro esistente (semi originali)
            if not (args.resume and (rdir / macro_name).exists()):
                (rdir / macro_name).write_text(macro)

            g = rdir / "gdml"
            if g.is_symlink() or g.is_file():
                g.unlink()
            elif g.is_dir() and not args.copy_gdml:
                shutil.rmtree(g)
            if args.copy_gdml:
                if not g.exists():
                    shutil.copytree(gdml_src, g)
            else:
                g.symlink_to(gdml_shared)

            cmd = ""
            if args.setup:
                cmd += f". {shlex.quote(str(Path(args.setup).resolve()))} >/dev/null 2>&1 && "
            cmd += (f"cd {shlex.quote(str(rdir))} && "
                    f"{shlex.quote(str(exe))} {macro_name} > PADMEMC.log 2>&1")
            jobs.append(cmd)
            jobdirs.append(rdir)

    joblist = prod / "jobs.txt"
    joblist.write_text("\n".join(jobs) + "\n")
    info_file = prod / "production_info.txt"
    if not (args.resume and info_file.exists()):
        info_file.write_text("\n".join(info) + "\n")

    print(f"Produzione:       {prod}")
    print(f"Energie:          {', '.join(f'{p:g}' for p, _ in points)} MeV/c")
    print(f"Job totali:       {len(jobs)}  ({args.njobs} per energia)")
    print(f"Max contemporanei:{args.max_parallel}")
    print(f"Lista job:        {joblist}")
    print("\n".join(info[4:]))

    if args.resume:
        # un job e' finito se il suo PADMEMC.log termina con "Run terminated"
        todo = [i for i, r in enumerate(jobdirs) if not run_terminated(r)]
        status = read_joblog_status(prod)
        print(f"\n--resume: controllo '{TERMINATED_TAG}' nelle ultime "
              f"{TAIL_LINES} righe di PADMEMC.log")
        todo_set = set(todo)
        for p, _ in points:
            idx = [i for i, r in enumerate(jobdirs) if r.parent.name == energy_tag(p)]
            bad = [jobdirs[i].name for i in idx if i in todo_set]
            print(f"  {energy_tag(p):>14}: {len(idx) - len(bad)}/{len(idx)} terminati"
                  + (f", da rilanciare: {' '.join(bad)}" if bad else ""))
        # terminati secondo il log ma falliti per parallel: il merge li esclude
        incoh = [r for i, r in enumerate(jobdirs)
                 if i not in todo_set and status.get(r.resolve()) is False]
        if incoh:
            print(f"\nATTENZIONE: {len(incoh)} Run hanno '{TERMINATED_TAG}' nel log ma "
                  "sono fallite nel joblog (MergePADMEScan le esclude):")
            print("  " + " ".join(f"{r.parent.name}/{r.name}" for r in incoh))
        if not todo:
            print("\nTutti i job risultano terminati: niente da rilanciare.")
            return
        print(f"\nJob da rilanciare: {len(todo)} su {len(jobs)}")
        if args.dry_run:
            print("--dry-run: nessun file cancellato, nessun job lanciato.")
            return
        if not sys.stdin.isatty():
            sys.exit("ERRORE: serve la conferma da terminale; nessun file toccato.")
        try:
            ans = input(f"Cancello *.txt e *.root nelle {len(todo)} cartelle Run e le "
                        "rilancio? [s/N] ").strip().lower()
        except (EOFError, KeyboardInterrupt):
            ans = ""
        if ans not in ("s", "si", "si'", "sì", "y", "yes"):
            sys.exit("Annullato: nessun file toccato, nessun job lanciato.")
        ndel = 0
        for i in todo:
            for pat in ("*.txt", "*.root"):
                for f in jobdirs[i].glob(pat):
                    if f.is_file() and not f.is_symlink():
                        f.unlink()
                        ndel += 1
        print(f"Cancellati {ndel} file di output.")
        stamp = time.strftime("%Y%m%d-%H%M%S")
        joblist = prod / f"jobs.resume-{stamp}.txt"
        joblist.write_text("\n".join(jobs[i] for i in todo) + "\n")
        # nuovo joblog: il merge legge tutti i joblog*.txt e per ogni Run
        # usa l'esecuzione piu' recente
        joblog = prod / f"joblog.resume-{stamp}.txt"
        print(f"Lista job da rilanciare: {joblist}")
    elif args.dry_run:
        print("\n--dry-run: nessun job lanciato.")
        return

    pcmd = [parallel, "--will-cite", "-j", str(args.max_parallel),
            "--delay", str(args.delay), "--joblog", str(joblog)]
    if args.nice:
        pcmd += ["--nice", str(args.nice)]
    pcmd += shlex.split(args.parallel_opts)
    pcmd += ["-a", str(joblist)]

    print("\n" + " ".join(shlex.quote(c) for c in pcmd))
    if args.foreground:
        sys.exit(subprocess.call(pcmd))

    with open(prod / "parallel.out", "a") as out:
        proc = subprocess.Popen(pcmd, stdin=subprocess.DEVNULL, stdout=out,
                                stderr=subprocess.STDOUT, start_new_session=True)
    (prod / "parallel.pid").write_text(f"{proc.pid}\n")
    print(f"\nGNU parallel lanciato in background (PID {proc.pid}), "
          "sopravvive al logout.")
    print(f"Avanzamento: tail -f {joblog}")
    print(f"Stop:        kill {proc.pid}")


if __name__ == "__main__":
    main()
