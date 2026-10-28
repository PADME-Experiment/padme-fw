#!/usr/bin/env python3
"""Time-binned TMM analysis queue, following RunReco.py's batch conventions.

Runs are selected from the FIRST COLUMN of the approved PADME run database.
No detector-run filtering: one analysis per PADME run and complete reconstructed file.
Does not need RunIV_Times.txt: TMM reconstruction already saved the UNIX anchors.
"""
import argparse
import csv
import os
import subprocess
import sys
import time
from collections import deque
from dataclasses import dataclass
from datetime import datetime
from pathlib import Path
from typing import Optional

SCRIPT_DIR = Path(__file__).resolve().parent
RUN_LIST_FILE = Path("/home/mancinima/BeamMonitorRun4/padme-fw/UserAnalysis/config/run_level_offline_db_2025_full_withTMM.txt")
INPUT_DIR = Path("/data9Vd1/padme/mancinima/RecoRun4_v0/TMM")
OUTPUT_DIR = Path("/data9Vd1/padme/mancinima/RecoRun4_v0/AnalysisTMM")
BASH_SCRIPT = SCRIPT_DIR / "RunAnalysisTMM.sh"
MACRO = SCRIPT_DIR / "AnalysisTMM.C"
MAX_CORES = 20
DEFAULT_JOBS = 20  # Analysis uses large TH3 histograms; increase up to 12 if RAM allows.


@dataclass(frozen=True)
class RunSpec:
    run_id: str
    input_file: Path
    output_file: Path
    log_file: Path


@dataclass
class RunningJob:
    spec: RunSpec
    core: Optional[int]
    process: subprocess.Popen
    log_handle: object


def normalize_run_id(value):
    value = str(value).strip()
    if not value.isdigit() or int(value) <= 0:
        raise ValueError("Invalid PADME RunID: {!r}".format(value))
    return str(int(value))


def read_runs(database_path):
    """Take only column 0, preserving database order and deduplicating run IDs."""
    if not database_path.is_file():
        raise FileNotFoundError("Run database not found: {}".format(database_path))
    runs = []
    seen = set()
    with database_path.open("r", encoding="utf-8-sig") as f:
        for lineno, raw in enumerate(f, 1):
            line = raw.split("#", 1)[0].strip()
            if not line:
                continue
            field = line.split()[0]
            if field.lower() in ("run", "runid", "run_id", "run_number", "runnumber", "runnum"):
                continue
            try:
                run = normalize_run_id(field)
            except ValueError as exc:
                raise ValueError("{}:{}: invalid run ID {}".format(database_path, lineno, field)) from exc
            if run not in seen:
                runs.append(run)
                seen.add(run)
            else:
                print("[WARN] repeated PADME run {:07d}; analysing it only once".format(int(run)), flush=True)
    if not runs:
        raise ValueError("Run database contains no valid runs: {}".format(database_path))
    return runs


def parse_args(argv=None):
    p = argparse.ArgumentParser(description="Batch AnalysisTMM: one output per PADME run, 12-core maximum")
    p.add_argument("runs", nargs="*", help="Optional PADME run IDs (must belong to the database); default: all")
    p.add_argument("-j", "--jobs", type=int, default=DEFAULT_JOBS,
                   help="Parallel analyses (default: {}; maximum: {}). Consider RAM per TH3.".format(DEFAULT_JOBS, MAX_CORES))
    p.add_argument("-b", "--expected-bins", type=int, default=0,
                   help="Optional input time-bin consistency check (0 disables check; does NOT rebin)")
    p.add_argument("--run-list", type=Path, default=RUN_LIST_FILE, help="Approved run TXT database; column 0")
    p.add_argument("--input-dir", type=Path, default=INPUT_DIR, help="Folder with RecoTMM_v0_XXXXXXX.root")
    p.add_argument("--output-dir", type=Path, default=OUTPUT_DIR, help="AnalysisTMM output folder")
    p.add_argument("--macro", type=Path, default=MACRO, help="AnalysisTMM.C location")
    p.add_argument("--bash-script", type=Path, default=BASH_SCRIPT, help="Single-run analysis helper")
    p.add_argument("--dry-run", action="store_true", help="Print selection, input/output and missing files; launch nothing")
    p.add_argument("--skip-existing", action="store_true", help="Skip nonempty output files (not ROOT-validated)")
    p.add_argument("--strict", action="store_true", help="Fail if any requested reconstruction ROOT file is missing")
    p.add_argument("--background", action="store_true", help="Detach one queue supervisor, not each individual run")
    p.add_argument("--foreground", action="store_true", help="Run queue in current terminal without prompting")
    p.add_argument("--no-taskset", action="store_true", help="Disable core pinning")
    p.add_argument("--worker", action="store_true", help=argparse.SUPPRESS)
    p.add_argument("--job", action="append", help=argparse.SUPPRESS)
    return p.parse_args(argv)


def paths_for_run(run, args):
    label = "{:07d}".format(int(run))
    return RunSpec(run, args.input_dir / "RecoTMM_v0_{}.root".format(label),
                   args.output_dir / "AnalysisTMM_v0_{}.root".format(label),
                   args.output_dir / "logs" / "run{}_AnalysisTMM.log".format(label))


def select_specs(args):
    if args.worker:
        if not args.job:
            raise ValueError("Internal worker received no --job specifications")
        return [paths_for_run(normalize_run_id(run), args) for run in args.job], []

    approved = read_runs(args.run_list)
    approved_set = set(approved)
    if args.runs:
        runs = [normalize_run_id(r) for r in args.runs]
        if len(set(runs)) != len(runs):
            raise ValueError("Duplicate requested RunID")
        unknown = [run for run in runs if run not in approved_set]
        if unknown:
            raise ValueError("Run(s) absent from database: {}".format(", ".join(unknown)))
    else:
        runs = approved

    specs = []
    skipped = []
    for run in runs:
        spec = paths_for_run(run, args)
        if not spec.input_file.is_file() or spec.input_file.stat().st_size == 0:
            skipped.append((run, "missing/empty reconstruction: {}".format(spec.input_file)))
        elif args.skip_existing and spec.output_file.is_file() and spec.output_file.stat().st_size > 0:
            skipped.append((run, "output already exists: {}".format(spec.output_file)))
        else:
            specs.append(spec)

    print("[SELECTION] database={} selected={} ready={} skipped={}".format(
        len(approved), len(runs), len(specs), len(skipped)), flush=True)
    print("[SELECTION] run database: {}".format(args.run_list), flush=True)
    print("[SELECTION] input dir:    {}".format(args.input_dir), flush=True)
    print("[SELECTION] output dir:   {}".format(args.output_dir), flush=True)
    print("[SELECTION] expected bins: {} (0=any input binning)".format(args.expected_bins), flush=True)
    for s in specs[:12]:
        print("[READY] {:07d}: {} -> {}".format(int(s.run_id), s.input_file, s.output_file), flush=True)
    if len(specs) > 12:
        print("[READY] ... and {} more run(s)".format(len(specs) - 12), flush=True)
    for run, reason in skipped:
        print("[SKIP] {:07d}: {}".format(int(run), reason), flush=True)
    if args.strict and any(not reason.startswith("output already exists") for _, reason in skipped):
        raise ValueError("Strict mode: missing reconstruction inputs")
    return specs, skipped


def taskset_available():
    try:
        return subprocess.run(["taskset", "--version"], stdout=subprocess.DEVNULL,
                              stderr=subprocess.DEVNULL).returncode == 0
    except FileNotFoundError:
        return False


def decide_background(args):
    if args.worker or args.foreground:
        return False
    if args.background:
        return True
    if not sys.stdin.isatty():
        return False
    return input("Run analysis queue in background? (yes/no): ").strip().lower() in ("yes", "y")


def detach_worker(specs, args):
    log_dir = args.output_dir / "logs"
    log_dir.mkdir(parents=True, exist_ok=True)
    stamp = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    launcher_log = log_dir / "RunAnalysisTMM_launcher_{}.log".format(stamp)
    pid_file = log_dir / "RunAnalysisTMM_launcher.pid"
    cmd = [sys.executable, "-u", str(Path(__file__).resolve()), "--worker", "--foreground",
           "--jobs", str(args.jobs), "--expected-bins", str(args.expected_bins),
           "--input-dir", str(args.input_dir), "--output-dir", str(args.output_dir),
           "--macro", str(args.macro), "--bash-script", str(args.bash_script)]
    if args.no_taskset:
        cmd.append("--no-taskset")
    for s in specs:
        cmd += ["--job", s.run_id]
    with launcher_log.open("w") as log:
        proc = subprocess.Popen(cmd, cwd=SCRIPT_DIR, stdin=subprocess.DEVNULL,
                                stdout=log, stderr=subprocess.STDOUT, start_new_session=True,
                                close_fds=True)
    pid_file.write_text(str(proc.pid) + "\n")
    print("[BACKGROUND] queue supervisor PID={}".format(proc.pid), flush=True)
    print("[BACKGROUND] log: {}".format(launcher_log), flush=True)
    print("[BACKGROUND] monitor: tail -f {}".format(launcher_log), flush=True)
    return 0


def start_job(spec, core, args):
    cmd = ["bash", str(args.bash_script), "-i", str(spec.input_file), "-o", str(spec.output_file),
           "-b", str(args.expected_bins), "--macro", str(args.macro)]
    if core is not None:
        cmd = ["taskset", "-c", str(core)] + cmd
    env = os.environ.copy()
    env.update({"OMP_NUM_THREADS": "1", "MKL_NUM_THREADS": "1", "OPENBLAS_NUM_THREADS": "1",
                "VECLIB_MAXIMUM_THREADS": "1", "NUMEXPR_NUM_THREADS": "1"})
    spec.log_file.parent.mkdir(parents=True, exist_ok=True)
    log_handle = spec.log_file.open("w", encoding="utf-8")
    try:
        process = subprocess.Popen(cmd, cwd=SCRIPT_DIR, stdout=log_handle,
                                   stderr=subprocess.STDOUT, env=env)
    except Exception:
        log_handle.close()
        raise
    return RunningJob(spec, core, process, log_handle)


def has_success_marker(logfile, output_file):
    """Avoid marking a ROOT early-return as success (ROOT can exit with status 0)."""
    needle = "ANALYSISTMM_FINISHED: {}".format(output_file)
    try:
        with logfile.open("rb") as handle:
            handle.seek(0, os.SEEK_END)
            offset = handle.tell()
            handle.seek(max(0, offset - 16384))
            return needle.encode("utf-8") in handle.read()
    except OSError:
        return False


def run_queue(specs, args):
    log_dir = args.output_dir / "logs"
    log_dir.mkdir(parents=True, exist_ok=True)
    allowed = sorted(os.sched_getaffinity(0)) if hasattr(os, "sched_getaffinity") else list(range(os.cpu_count() or 1))
    if not allowed:
        raise RuntimeError("No CPU cores allowed")
    max_jobs = min(args.jobs, MAX_CORES, len(allowed))
    pin = not args.no_taskset and taskset_available()
    free = allowed[:max_jobs]
    pending = deque(specs)
    active = []
    records = []
    start_time = time.monotonic()
    print("[QUEUE] {} analyses; max parallel {}; taskset={} (12-core maximum)".format(
        len(specs), max_jobs, pin), flush=True)
    try:
        while pending or active:
            while pending and len(active) < max_jobs:
                spec = pending.popleft()
                core = free.pop(0) if pin else None
                try:
                    job = start_job(spec, core, args)
                except Exception as exc:
                    if core is not None:
                        free.append(core)
                        free.sort()
                    records.append((spec, "FAILED", str(exc)))
                    print("[FAIL] run {:07d}: {}".format(int(spec.run_id), exc), flush=True)
                    continue
                active.append(job)
                print("[START] run {:07d} PID={} core={} log={}".format(
                    int(spec.run_id), job.process.pid, core, spec.log_file), flush=True)
            still_running = []
            for job in active:
                rc = job.process.poll()
                if rc is None:
                    still_running.append(job)
                    continue
                job.log_handle.close()
                if job.core is not None:
                    free.append(job.core)
                    free.sort()
                if rc == 0 and not job.spec.output_file.is_file():
                    rc = "missing output ROOT file"
                elif rc == 0 and job.spec.output_file.stat().st_size <= 0:
                    rc = "empty output ROOT file"
                elif rc == 0 and not has_success_marker(job.spec.log_file, job.spec.output_file):
                    rc = "missing analysis-complete marker"
                if rc == 0:
                    status, reason = "OK", ""
                    print("[ OK ] run {:07d}: {}".format(int(job.spec.run_id), job.spec.output_file), flush=True)
                else:
                    status = "FAILED"
                    reason = "exit code {}".format(rc) if isinstance(rc, int) else str(rc)
                    print("[FAIL] run {:07d}: {} (see {})".format(
                        int(job.spec.run_id), reason, job.spec.log_file), flush=True)
                records.append((job.spec, status, reason))
            active = still_running
            if pending or active:
                time.sleep(0.2)
    except KeyboardInterrupt:
        print("[STOP] cancelling active jobs", flush=True)
        for job in active:
            job.process.terminate()
        for job in active:
            try:
                job.process.wait(timeout=5)
            except subprocess.TimeoutExpired:
                job.process.kill()
                job.process.wait()
            job.log_handle.close()
        raise

    stamp = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    report = log_dir / "RunAnalysisTMM_results_{}.csv".format(stamp)
    with report.open("w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(("RunID", "Status", "Reason", "InputROOT", "OutputROOT", "Log"))
        for spec, status, reason in records:
            writer.writerow(("{:07d}".format(int(spec.run_id)), status, reason,
                             str(spec.input_file), str(spec.output_file), str(spec.log_file)))
    failures = [(s, reason) for s, status, reason in records if status != "OK"]
    print("\n=== TMM analysis results ===", flush=True)
    print("OK={} FAILED={} elapsed={:.1f}s".format(len(records) - len(failures),
          len(failures), time.monotonic() - start_time), flush=True)
    print("CSV: {}".format(report), flush=True)
    if args.worker:
        pidfile = log_dir / "RunAnalysisTMM_launcher.pid"
        try:
            if pidfile.read_text().strip() == str(os.getpid()):
                pidfile.unlink()
        except OSError:
            pass
    return 1 if failures else 0


def main(argv=None):
    args = parse_args(argv)
    try:
        args.run_list = args.run_list.expanduser().resolve()
        args.input_dir = args.input_dir.expanduser().resolve()
        args.output_dir = args.output_dir.expanduser().resolve()
        args.macro = args.macro.expanduser().resolve()
        args.bash_script = args.bash_script.expanduser().resolve()
        if not 1 <= args.jobs <= MAX_CORES:
            raise ValueError("-j must be between 1 and {}".format(MAX_CORES))
        if args.expected_bins < 0:
            raise ValueError("-b must be >= 0 (0 disables checking)")
        if args.background and args.foreground:
            raise ValueError("Choose one of --background and --foreground")
        specs, skipped = select_specs(args)
        if args.dry_run:
            print("[DRY RUN] no analysis started", flush=True)
            return 0
        if not specs:
            print("[DONE] No runs to analyze", flush=True)
            return 0 if not skipped or all("output already exists" in reason for _, reason in skipped) else 1
        if not args.macro.is_file() or not args.bash_script.is_file():
            raise FileNotFoundError("AnalysisTMM.C or RunAnalysisTMM.sh not found")
        if decide_background(args):
            return detach_worker(specs, args)
        return run_queue(specs, args)
    except KeyboardInterrupt:
        return 130
    except (ValueError, EOFError, FileNotFoundError, OSError, RuntimeError) as exc:
        print("ERROR: {}".format(exc), file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
