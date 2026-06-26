#!/usr/bin/env python3

# Script per lanciare in batch più job di RecoTMM
# Legge da un file di testo la lista dei job (PadmeRunID, DetRunID, MaxEvents, NevtBlock) e lancia uno script bash per ciascuno
# Limita il numero di job paralleli a un valore specificato (default: numero di core CPU)
# Crea una directory di lavoro e una di log per ciascun job, con file di log dedicato
# Tiene traccia dei PID dei processi lanciati in un file di testo

#MARCO --> DA RIVEDERE!

import os
import sys
import time
import shlex
import shutil
import subprocess
from pathlib import Path
from datetime import datetime


# ============================================================
# Configurazione fissa
# ============================================================

RECO_SCRIPT = Path(__file__).resolve().parent / "runRecoTMM.sh"
DEFAULT_JOB_FILE = "jobs.txt"


def ask(prompt, default=None):
    if default is not None:
        ans = input(f"{prompt} [{default}]: ").strip()
        return ans if ans else str(default)
    return input(f"{prompt}: ").strip()


def read_jobs_file(filename):
    jobs = []

    with open(filename) as f:
        for iline, line in enumerate(f, start=1):
            line = line.split("#")[0].strip()

            if not line:
                continue

            parts = line.split()

            if len(parts) < 2 or len(parts) > 4:
                print(f"WARNING: riga {iline} ignorata: {line}")
                print("         formato atteso: PadmeRunID DetRunID [MaxEvents] [NevtBlock]")
                continue

            jobs.append(parts)

    return jobs


def prepare_workdir(src_dir, workdir):
    workdir.mkdir(parents=True, exist_ok=True)

    patterns = [
        "*.h",
        "*.hh",
        "*.hpp",
        "*.C",
        "*.cxx",
        "*.cpp",
        "*.cc",
    ]

    files = []

    for pattern in patterns:
        files.extend(src_dir.glob(pattern))

    for src in files:
        dst = workdir / src.name

        if dst.exists():
            continue

        try:
            os.symlink(src.resolve(), dst)
        except OSError:
            shutil.copy2(src, dst)


def launch_job(job, reco_script, src_dir, logdir, workbase):
    padme_run = job[0]
    det_run = job[1]

    max_events = job[2] if len(job) >= 3 else "0"
    nevt_block = job[3] if len(job) >= 4 else "10000"

    tag = f"padme{padme_run}_det{det_run}"
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")

    workdir = workbase / f"{tag}_{timestamp}"
    logfile = logdir / f"{tag}_{timestamp}.log"

    prepare_workdir(src_dir, workdir)

    command = (
        f"source {shlex.quote(str(reco_script))} "
        f"{padme_run} {det_run} {max_events} {nevt_block}"
    )

    with open(logfile, "w") as log:
        log.write("============================================================\n")
        log.write(f"START      = {datetime.now()}\n")
        log.write(f"PadmeRunID = {padme_run}\n")
        log.write(f"DetRunID   = {det_run}\n")
        log.write(f"MaxEvents  = {max_events}\n")
        log.write(f"NevtBlock  = {nevt_block}\n")
        log.write(f"Workdir    = {workdir}\n")
        log.write(f"Command    = {command}\n")
        log.write("============================================================\n\n")
        log.flush()

        proc = subprocess.Popen(
            ["nohup", "bash", "-lc", command],
            cwd=workdir,
            stdout=log,
            stderr=subprocess.STDOUT,
            start_new_session=True,
        )

    print(f"Lanciato {tag}  PID={proc.pid}")
    print(f"  log: {logfile}")

    return proc, tag, logfile


def main():
    print()
    print("=== Submit RecoTMM jobs ===")
    print()

    reco_script = RECO_SCRIPT.resolve()

    if not reco_script.exists():
        print(f"ERROR: script bash non trovato: {reco_script}")
        sys.exit(1)

    src_dir = reco_script.parent

    job_file = Path(
        ask("File txt con lista job", DEFAULT_JOB_FILE)
    ).expanduser().resolve()

    if not job_file.exists():
        print(f"ERROR: file job non trovato: {job_file}")
        sys.exit(1)

    jobs = read_jobs_file(job_file)

    if not jobs:
        print("Nessun job valido trovato.")
        sys.exit(0)

    ncores = os.cpu_count() or 1
    max_parallel = int(ask("Numero massimo di job paralleli", ncores))

    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")

    logdir = Path(f"logs_reco_{timestamp}").resolve()
    workbase = Path(f"work_reco_{timestamp}").resolve()

    logdir.mkdir(parents=True, exist_ok=True)
    workbase.mkdir(parents=True, exist_ok=True)

    print()
    print(f"Script bash   : {reco_script}")
    print(f"File job      : {job_file}")
    print(f"Directory src : {src_dir}")
    print(f"Log dir       : {logdir}")
    print(f"Work dir      : {workbase}")
    print(f"Job totali    : {len(jobs)}")
    print(f"Job paralleli : {max_parallel}")
    print()

    print("Primi job trovati:")
    for job in jobs[:5]:
        print("  " + " ".join(job))

    if len(jobs) > 5:
        print(f"  ... altri {len(jobs) - 5}")

    print()

    answer = ask("Procedo? y/N", "N")

    if answer.lower() not in ["y", "yes"]:
        print("Interrotto.")
        sys.exit(0)

    pidfile = logdir / "submitted_jobs.txt"
    running = []

    with open(pidfile, "w") as pf:
        for job in jobs:

            while len(running) >= max_parallel:
                still_running = []

                for proc, tag, logfile in running:
                    ret = proc.poll()

                    if ret is None:
                        still_running.append((proc, tag, logfile))
                    else:
                        print(f"Finito {tag} con status {ret}")

                running = still_running

                if len(running) >= max_parallel:
                    time.sleep(2)

            proc, tag, logfile = launch_job(
                job,
                reco_script,
                src_dir,
                logdir,
                workbase,
            )

            running.append((proc, tag, logfile))

            pf.write(
                f"{datetime.now()}  PID={proc.pid}  {tag}  log={logfile}\n"
            )
            pf.flush()

    print()
    print("Tutti i job sono stati sottomessi.")
    print(f"PID file: {pidfile}")
    print()

    while running:
        still_running = []

        for proc, tag, logfile in running:
            ret = proc.poll()

            if ret is None:
                still_running.append((proc, tag, logfile))
            else:
                print(f"Finito {tag} con status {ret}")

        running = still_running

        if running:
            time.sleep(5)

    print()
    print("Done.")


if __name__ == "__main__":
    main()