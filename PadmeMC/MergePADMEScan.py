#!/usr/bin/env python3
"""
MergePADMEScan.py - unisce l'output di una produzione di SubmitPADMEScan.py

Per ogni cartella di energia  <prod>/E<p>MeV/ :
  * concatena tutti i Run*/beam_output.txt  ->  E<p>MeV/beam_output_E<p>MeV.txt
  * unisce con hadd tutti i Run*/UBTF.root  ->  E<p>MeV/E<p>MeV.root

Vengono usate solo le Run il cui job risulta terminato con successo nel
joblog di GNU parallel (joblog*.txt della produzione): job ancora in corso,
in coda o falliti sono esclusi e segnalati. Con --all si prende tutto
quello che c'e' su disco.

Esempi:
    ./MergePADMEScan.py /data9Vd1/padme/raggi/EnergyScan
    ./MergePADMEScan.py /data9Vd1/padme/raggi/EnergyScan -E 250 270.3
    ./MergePADMEScan.py /data9Vd1/padme/raggi/EnergyScan --dry-run

Richiede hadd nel PATH (source <root>/bin/thisroot.sh).

Copia dei file uniti sul Mac (lanciare SUL MAC, dopo il merge su tier2):
    ./MergePADMEScan.py /data9Vd1/padme/raggi/EnergyScan --fetch
    ./MergePADMEScan.py /data9Vd1/padme/raggi/EnergyScan --fetch -E 250 270.3
    ./MergePADMEScan.py /data9Vd1/padme/raggi/EnergyScan --fetch utente@host --dest ~/altro
Con --fetch <prod> e' il percorso SULLA MACCHINA REMOTA: non viene fatto
nessun merge, si copiano con un solo scp i file E*/E<p>MeV.root,
E*/beam_output_E<p>MeV.txt e production_info.txt in <dest>/<tag>/.
"""

import argparse
import posixpath
import re
import shlex
import shutil
import subprocess
import sys
from pathlib import Path

DEFAULT_REMOTE = "tier2-ui005.lnf.infn.it"
DEFAULT_DEST = "/Users/raggi/Work/RunIVAnalysis"


def run_index(path):
    m = re.fullmatch(r"Run(\d+)", path.name)
    return int(m.group(1)) if m else None


def energy_value(path):
    m = re.fullmatch(r"E([0-9.eE+-]+)MeV", path.name)
    try:
        return float(m.group(1)) if m else None
    except ValueError:
        return None


def read_joblogs(prod):
    """Ritorna {cartella Run: True/False} dall'ultima esecuzione di ogni job."""
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


def concat_txt(inputs, output):
    tmp = output.with_name(output.name + ".tmp")
    with open(tmp, "wb") as out:
        for f in inputs:
            with open(f, "rb") as src:
                shutil.copyfileobj(src, out, 1 << 20)
                # un file troncato senza newline finale non deve
                # incollarsi alla prima riga del successivo
                if src.tell() > 0:
                    src.seek(-1, 2)
                    if src.read(1) != b"\n":
                        out.write(b"\n")
    tmp.replace(output)


def fetch(args):
    """Copia via scp i file uniti dalla macchina remota in <dest>/<tag>/."""
    remote = args.prod.rstrip("/")
    tag = posixpath.basename(remote)
    dest = Path(args.dest).expanduser() / tag
    host = args.fetch

    if args.energies:
        etags = [f"E{e:g}MeV" for e in args.energies]
    else:
        etags = ["E*MeV"]
    sources = [f"{remote}/production_info.txt"]
    for et in etags:
        if not args.no_root:
            sources.append(f"{remote}/{et}/{et}.root")
        if not args.no_txt:
            sources.append(f"{remote}/{et}/beam_output_{et}.txt")

    cmd = ["scp", "-p"] + [f"{host}:{s}" for s in sources] + [str(dest) + "/"]
    print(f"Da   {host}:{remote}\nA    {dest}")
    print(" ".join(shlex.quote(c) for c in cmd))
    if args.dry_run:
        print("\n--dry-run: nessun file copiato.")
        return
    dest.mkdir(parents=True, exist_ok=True)
    rc = subprocess.call(cmd)
    copied = sorted(p.name for p in dest.iterdir() if p.is_file())
    print(f"\nIn {dest}: " + (", ".join(copied) if copied else "nessun file"))
    if rc != 0:
        sys.exit(f"scp terminato con errore {rc}: alcuni file potrebbero "
                 "mancare (merge non ancora fatto per qualche energia?)")


def parse_args():
    ap = argparse.ArgumentParser(
        description="Merge di beam_output.txt e UBTF.root per energia",
        formatter_class=argparse.RawDescriptionHelpFormatter, epilog=__doc__)
    ap.add_argument("prod", help="cartella della produzione (<outdir>/<tag>)")
    ap.add_argument("-E", "--energies", type=float, nargs="+",
                    help="unisce solo queste energie [MeV] (default: tutte)")
    ap.add_argument("--txt", default="beam_output.txt",
                    help="nome del file txt nelle Run (default beam_output.txt)")
    ap.add_argument("--root", default="UBTF.root",
                    help="nome del file root nelle Run (default UBTF.root)")
    ap.add_argument("--all", action="store_true",
                    help="ignora il joblog e usa tutte le Run presenti su disco")
    ap.add_argument("--no-txt", action="store_true", help="salta il merge dei txt")
    ap.add_argument("--no-root", action="store_true", help="salta hadd")
    ap.add_argument("--hadd-opts", default="",
                    help="opzioni extra per hadd, es. \"-j 8\"")
    ap.add_argument("--fetch", nargs="?", const=DEFAULT_REMOTE, default=None,
                    metavar="[UTENTE@]HOST",
                    help="invece del merge, copia via scp i file uniti da HOST "
                         f"(default {DEFAULT_REMOTE}); prod e' il percorso remoto")
    ap.add_argument("--dest", default=DEFAULT_DEST,
                    help=f"cartella locale per --fetch (default {DEFAULT_DEST}); "
                         "i file vanno in <dest>/<tag>/")
    ap.add_argument("--dry-run", action="store_true",
                    help="mostra cosa verrebbe fatto senza scrivere nulla")
    return ap.parse_args()


def main():
    args = parse_args()
    if args.fetch:
        fetch(args)
        return
    prod = Path(args.prod).resolve()
    if not prod.is_dir():
        sys.exit(f"ERRORE: cartella non trovata: {prod}")

    hadd = shutil.which("hadd")
    if not args.no_root and hadd is None and not args.dry_run:
        sys.exit("ERRORE: hadd non trovato nel PATH (source <root>/bin/thisroot.sh)")

    edirs = sorted((d for d in prod.iterdir()
                    if d.is_dir() and energy_value(d) is not None),
                   key=energy_value)
    if args.energies:
        wanted = set(args.energies)
        edirs = [d for d in edirs if energy_value(d) in wanted]
        missing = wanted - {energy_value(d) for d in edirs}
        if missing:
            print("ATTENZIONE: energie non trovate: "
                  + ", ".join(f"{e:g}" for e in sorted(missing)))
    if not edirs:
        sys.exit(f"ERRORE: nessuna cartella E<p>MeV in {prod}")

    status = {}
    if not args.all:
        status = read_joblogs(prod)
        if not status:
            sys.exit(f"ERRORE: nessun joblog*.txt leggibile in {prod}; "
                     "usa --all per unire comunque tutto quello che c'e'")

    failures = 0
    for edir in edirs:
        tag = edir.name
        runs = sorted((r for r in edir.iterdir()
                       if r.is_dir() and run_index(r) is not None), key=run_index)
        if args.all:
            done, notdone, failed = runs, [], []
        else:
            done = [r for r in runs if status.get(r.resolve()) is True]
            failed = [r for r in runs if status.get(r.resolve()) is False]
            notdone = [r for r in runs if r.resolve() not in status]

        print(f"\n=== {tag}: {len(runs)} Run, {len(done)} terminate"
              + (f", {len(failed)} fallite" if failed else "")
              + (f", {len(notdone)} in corso/in coda" if notdone else ""))
        if failed:
            print("  fallite (escluse): " + " ".join(r.name for r in failed))

        jobs = []
        if not args.no_txt:
            jobs.append((args.txt, edir / f"beam_output_{tag}.txt"))
        if not args.no_root:
            jobs.append((args.root, edir / f"{tag}.root"))

        for fname, output in jobs:
            inputs = [r / fname for r in done
                      if (r / fname).is_file() and (r / fname).stat().st_size > 0]
            absent = [r.name for r in done if r / fname not in inputs]
            if absent:
                print(f"  {fname} mancante o vuoto in: {' '.join(absent)}")
            if not inputs:
                print(f"  {fname}: niente da unire, {output.name} non creato")
                continue
            print(f"  {fname}: {len(inputs)} file -> {output}")
            if args.dry_run:
                continue

            if fname == args.root:
                cmd = [hadd, "-f", "-k"] + shlex.split(args.hadd_opts) \
                      + [str(output)] + [str(f) for f in inputs]
                res = subprocess.run(cmd, stdout=subprocess.PIPE,
                                     stderr=subprocess.STDOUT, text=True)
                if res.returncode != 0:
                    failures += 1
                    print(f"  ERRORE hadd (exit {res.returncode}):")
                    print("    " + "\n    ".join(res.stdout.strip().splitlines()[-15:]))
            else:
                concat_txt(inputs, output)
                with open(output, "rb") as f:
                    nlines = sum(buf.count(b"\n")
                                 for buf in iter(lambda: f.read(1 << 20), b""))
                print(f"    {nlines} righe")

    if failures:
        sys.exit(f"\n{failures} merge con hadd falliti")
    print("\nFatto.")


if __name__ == "__main__":
    main()
