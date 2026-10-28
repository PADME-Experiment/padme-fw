#!/usr/bin/env python3

import argparse
import csv
import math
import os
import shlex
import shutil
import tempfile
import subprocess
import sys
import time
from collections import deque
from dataclasses import dataclass
from datetime import datetime
from pathlib import Path
from typing import Optional

SCRIPT_DIR = Path(__file__).resolve().parent
SOURCE = SCRIPT_DIR / "RecoTarget_v1.cpp"
EXECUTABLE = SCRIPT_DIR / "RecoTarget_v1.exe"
LIST_DIR = SCRIPT_DIR / "ListDir"
OUTPUT_DIR = Path("/data9Vd1/padme/mancinima/RecoRun4_v0/TargetLG_1tree")
LOG_DIR = OUTPUT_DIR / "logs"
SCRATCH_DIR = Path("/tmp") / os.environ.get("USER", "mancinima") / "targetreco_scratch"

# Time database supplied by the user.  Each row begins with:
# RunID detId T_start_Unix T_stop_Unix [human-readable UTC fields ...]
TIMES_FILE = Path("/home/mancinima/BeamMonitorRun4/padme-fw/RunIV_Times.txt")
RUN_LIST_FILE = Path("/home/mancinima/BeamMonitorRun4/padme-fw/UserAnalysis/config/run_level_offline_db_2025_full_withTMM.txt")
MAX_CORES = 20

# If None, provide -b/--nbins on the command line or enter it interactively.
# You can instead set a fixed default number of bins here.
DEFAULT_NBINS = None

OUTPUT_PREFIX = "RecoTarget_v0_run"


@dataclass(frozen=True)
class RunSpec:
    run_id: str
    t_start: float
    t_stop: float
    n_bins: int

    def validate(self):
        if not self.run_id.isdigit() or int(self.run_id) <= 0:
            raise ValueError("Run ID must be a positive integer: {}".format(self.run_id))
        if not (math.isfinite(self.t_start) and math.isfinite(self.t_stop) and self.t_stop > self.t_start):
            raise ValueError("Run {}: require finite t_stop > t_start".format(self.run_id))
        if self.n_bins < 1:
            raise ValueError("Run {}: n_bins must be >= 1".format(self.run_id))


@dataclass
class RunningJob:
    spec: RunSpec
    core: Optional[int]
    process: subprocess.Popen
    log_handle: object
    output_file: Path  # Final NFS destination
    log_file: Path
    scratch_dir: Path
    scratch_output: Path


def get_list_file(run_id):
    return LIST_DIR / "RawMerged_run_{:07d}.list".format(int(run_id))


def get_output_file(run_id):
    return OUTPUT_DIR / "{}{:07d}.root".format(OUTPUT_PREFIX, int(run_id))


def get_log_file(run_id):
    return LOG_DIR / "run_{:07d}.log".format(int(run_id))


def compile_reco():
    if not SOURCE.is_file():
        raise FileNotFoundError("C++ source not found: {}".format(SOURCE))
    print("Compiling {} ...".format(SOURCE.name), flush=True)
    root_cflags = shlex.split(subprocess.check_output(["root-config", "--cflags"], universal_newlines=True))
    root_libs = shlex.split(subprocess.check_output(["root-config", "--libs"], universal_newlines=True))
    cmd = ["g++", "-std=c++17", "-O2"] + root_cflags + [
        "-I", str((SCRIPT_DIR / "../PadmeRoot/include").resolve()), str(SOURCE),
        "-L", str((SCRIPT_DIR / "../PadmeRoot/lib").resolve()), "-lPadmeRoot"
    ] + root_libs + ["-o", str(EXECUTABLE)]
    print(" ".join(shlex.quote(x) for x in cmd), flush=True)
    subprocess.run(cmd, cwd=SCRIPT_DIR, check=True)
    if not EXECUTABLE.is_file():
        raise RuntimeError("Compiler returned success but executable is missing: {}".format(EXECUTABLE))
    print("Compilation successful: {}".format(EXECUTABLE), flush=True)


def parse_args():
    p = argparse.ArgumentParser(description="Time-binned Target reconstruction, with optional detached queue")
    p.add_argument("runs", nargs="*", help="Optional subset of the approved run-list; by default run ALL entries in that file")
    p.add_argument("-j", "--jobs", type=int, default=MAX_CORES, help="Concurrent jobs (default {}; maximum {})".format(MAX_CORES, MAX_CORES))
    p.add_argument("-b", "--nbins", type=int, default=DEFAULT_NBINS, help="Number of time bins per run (same N for all selected runs)")
    p.add_argument("--times-file", type=Path, default=TIMES_FILE, help="UNIX time lookup database")
    p.add_argument("--run-list", type=Path, default=RUN_LIST_FILE, help="Approved run selection file; column 0 only")
    p.add_argument("--dry-run", action="store_true", help="Check run selection, time intervals and input lists, without compiling or launching")
    p.add_argument("--skip-existing", action="store_true", help="Skip nonempty output ROOT files (use for resuming interrupted production)")
    p.add_argument("--strict", action="store_true", help="Fail before launch if an approved run has no time record or list file")
    p.add_argument("--background", action="store_true", help="Detach queue after compiling and return terminal")
    p.add_argument("--foreground", action="store_true", help="Run queue in foreground without asking")
    p.add_argument("--no-taskset", action="store_true", help="Do not pin jobs to CPU cores")
    p.add_argument("--scratch-dir", type=Path, default=SCRATCH_DIR, help="Local disk scratch base directory (default: {})".format(SCRATCH_DIR))
    p.add_argument("--max-events", type=int, default=0, help="Pass -n (TEST ONLY: future bins can appear empty)")
    p.add_argument("--graph-stride", type=int, default=1, help="Store every Nth accepted event in 2 TGraphs (default 1 = all; Blocks/TH1/TH2 unchanged)")
    p.add_argument("-v", "--verbose", action="count", default=0, help="Pass -v to C++ executable; repeatable")
    p.add_argument("--worker", action="store_true", help=argparse.SUPPRESS)
    p.add_argument("--job", nargs=4, action="append", metavar=("RUN", "START", "STOP", "NBINS"), help=argparse.SUPPRESS)
    return p.parse_args()


def new_spec(run_id, t_start, t_stop, n_bins):
    spec = RunSpec(str(run_id), float(t_start), float(t_stop), int(n_bins))
    spec.validate()
    return spec


def normalize_run_id(value):
    """Identify the same run whether written 80284 or 0080284."""
    value = str(value).strip()
    if not value.isdigit() or int(value) <= 0:
        raise ValueError("Invalid RunID: {!r}".format(value))
    return str(int(value))


def read_approved_runs(run_list_path):
    """Read *only column zero*; preserve file order and deduplicate run IDs."""
    if not run_list_path.is_file():
        raise FileNotFoundError("Approved run-list not found: {}".format(run_list_path))

    run_ids = []
    seen = set()
    with run_list_path.open("r", encoding="utf-8-sig") as handle:
        for line_number, line in enumerate(handle, 1):
            stripped = line.strip()
            if not stripped or stripped.startswith("#"):
                continue
            first_column = stripped.split()[0]
            if first_column.lower() in ("run", "runid", "run_id", "run_number", "runnumber", "runnum"):
                continue
            try:
                run_id = normalize_run_id(first_column)
            except ValueError as exc:
                raise ValueError("{}:{}: invalid run number in column 0: {!r}".format(run_list_path, line_number, first_column)) from exc
            if run_id in seen:
                print("[WARN] Duplicate RunID {:07d} on line {}, using only first occurrence".format(int(run_id), line_number), flush=True)
                continue
            seen.add(run_id)
            run_ids.append(run_id)

    if not run_ids:
        raise ValueError("No runs found in approved run-list {}".format(run_list_path))
    return run_ids


def read_run_times(database_path, selected_ids):
    """Read UNIX timestamps for selected RunIDs only; detId and UTC fields are ignored."""
    if not database_path.is_file():
        raise FileNotFoundError("Run-time database not found: {}".format(database_path))

    by_run = {}
    with database_path.open("r", encoding="utf-8-sig") as handle:
        for line_number, line in enumerate(handle, 1):
            stripped = line.strip()
            if not stripped or stripped.startswith("#"):
                continue
            columns = stripped.split(maxsplit=4)
            if columns[0].lower() == "runid":
                continue
            try:
                run_id = normalize_run_id(columns[0])
            except ValueError:
                # Other non-data records are irrelevant to the selected production runs.
                continue
            if run_id not in selected_ids:
                continue
            if len(columns) < 4:
                raise ValueError("{}:{}: missing timestamps for run {}".format(database_path, line_number, run_id))
            try:
                t_start = float(columns[2])
                t_stop = float(columns[3])
            except ValueError as exc:
                raise ValueError("{}:{}: invalid UNIX timestamps for run {}".format(database_path, line_number, run_id)) from exc
            if not (math.isfinite(t_start) and math.isfinite(t_stop) and t_stop > t_start):
                raise ValueError("{}:{}: invalid interval for run {}".format(database_path, line_number, run_id))
            if run_id in by_run:
                raise ValueError("{}:{}: duplicate timestamp record for RunID {}".format(database_path, line_number, run_id))
            by_run[run_id] = (t_start, t_stop)
    return by_run


def collect_specs(args):
    if args.worker:
        if not args.job:
            raise ValueError("Internal worker received no --job specifications")
        return [new_spec(*item) for item in args.job], []

    approved = read_approved_runs(args.run_list)
    approved_set = set(approved)
    if args.runs:
        requested = [normalize_run_id(item) for item in args.runs]
        if len(set(requested)) != len(requested):
            raise ValueError("Duplicate run numbers requested")
        forbidden = [run_id for run_id in requested if run_id not in approved_set]
        if forbidden:
            raise ValueError("Run(s) not in approved run-list: {}".format(", ".join(forbidden)))
        run_ids = requested
    else:
        run_ids = approved

    n_bins = args.nbins
    if n_bins is None:
        if not sys.stdin.isatty():
            raise ValueError("Number of time bins is required: pass -b/--nbins")
        n_bins = int(input("Number of time bins for each run: ").strip())
    if n_bins < 1:
        raise ValueError("-b/--nbins must be >= 1")

    database = read_run_times(args.times_file, set(run_ids))
    specs = []
    skipped = []
    for run_id in run_ids:
        list_file = get_list_file(run_id)
        if run_id not in database:
            skipped.append((run_id, "no time record in {}".format(args.times_file)))
            continue
        if not list_file.is_file() or list_file.stat().st_size == 0:
            skipped.append((run_id, "missing or empty input list {}".format(list_file)))
            continue
        if args.skip_existing:
            out_file = get_output_file(run_id)
            if out_file.is_file() and out_file.stat().st_size > 0:
                skipped.append((run_id, "output already exists (not ROOT-validated): {}".format(out_file)))
                continue
        t_start, t_stop = database[run_id]
        specs.append(new_spec(run_id, t_start, t_stop, n_bins))

    print("[SELECTION] approved={} requested={} ready={} skipped={} bins/run={}".format(len(approved), len(run_ids), len(specs), len(skipped), n_bins), flush=True)
    print("[SELECTION] run list: {}".format(args.run_list), flush=True)
    print("[SELECTION] UNIX times: {}".format(args.times_file), flush=True)
    print("[SELECTION] output directory: {}".format(OUTPUT_DIR), flush=True)
    if specs:
        print("[SELECTION] first ready: {}".format(", ".join("{:07d}".format(int(s.run_id)) for s in specs[:8])), flush=True)
    for run_id, reason in skipped:
        print("[SKIP] {:07d}: {}".format(int(run_id), reason), flush=True)

    strict_skips = [(run_id, reason) for run_id, reason in skipped if not reason.startswith("output already exists") ]
    if args.strict and strict_skips:
        raise ValueError("Strict mode: {} requested runs have missing input data".format(len(strict_skips)))
    return specs, skipped


def choose_background(args):
    if args.background and args.foreground:
        raise ValueError("Choose either --background or --foreground")
    if args.worker or args.foreground:
        return False
    if args.background:
        return True
    if not sys.stdin.isatty():
        return False
    return input("Launch reconstruction queue in background? (yes/no): ").strip().lower() in ("yes", "y")


def taskset_available():
    try:
        subprocess.run(["taskset", "--version"], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, check=True)
        return True
    except (FileNotFoundError, subprocess.CalledProcessError):
        return False


def start_job(spec, core, max_events, verbosity, graph_stride):
    list_file = get_list_file(spec.run_id)
    if not list_file.is_file():
        raise FileNotFoundError("Input list file not found: {}".format(list_file))
    out_file = get_output_file(spec.run_id)
    log_file = get_log_file(spec.run_id)
    scratch_dir = Path(tempfile.mkdtemp(prefix="run_{:07d}_".format(int(spec.run_id)), dir=str(SCRATCH_DIR)))
    scratch_output = scratch_dir / out_file.name
    cmd = [str(EXECUTABLE), "-l", str(list_file), "-o", str(scratch_output),
           "-s", repr(spec.t_start), "-t", repr(spec.t_stop), "-b", str(spec.n_bins)]
    if max_events > 0:
        cmd += ["-n", str(max_events)]
    cmd += ["-g", str(graph_stride)]
    cmd += ["-v"] * verbosity
    if core is not None:
        cmd = ["taskset", "-c", str(core)] + cmd

    env = os.environ.copy()
    env.update({"OMP_NUM_THREADS": "1", "MKL_NUM_THREADS": "1", "OPENBLAS_NUM_THREADS": "1", "VECLIB_MAXIMUM_THREADS": "1"})
    try:
        handle = log_file.open("w")
        try:
            proc = subprocess.Popen(cmd, cwd=SCRIPT_DIR, stdout=handle, stderr=subprocess.STDOUT, env=env)
        except Exception:
            handle.close()
            raise
    except Exception:
        shutil.rmtree(scratch_dir)
        raise
    return RunningJob(spec, core, proc, handle, out_file, log_file, scratch_dir, scratch_output)


def prepare_scratch(scratch_dir):
    """Create a disk-backed scratch base; avoid using RAM-backed /tmp."""
    global SCRATCH_DIR
    SCRATCH_DIR = scratch_dir.expanduser().resolve()
    SCRATCH_DIR.mkdir(parents=True, exist_ok=True)
    if shutil.which("findmnt"):
        fs_type = subprocess.check_output(
            ["findmnt", "-n", "-o", "FSTYPE", "-T", str(SCRATCH_DIR)],
            text=True,
        ).strip().lower()
        if fs_type in ("tmpfs", "ramfs") or fs_type.startswith("nfs"):
            raise RuntimeError(
                "Scratch {} is on {}. Choose local disk storage with --scratch-dir "
                "(RAM-backed tmpfs would consume memory).".format(SCRATCH_DIR, fs_type)
            )
    print("[SCRATCH] writing reconstruction outputs to {}".format(SCRATCH_DIR), flush=True)


def move_completed_root(job):
    """Move across filesystems: copy to hidden NFS file, rename, clean local."""
    source = job.scratch_output
    destination = job.output_file
    if not source.is_file() or source.stat().st_size == 0:
        raise RuntimeError("missing/empty scratch ROOT file: {}".format(source))

    # Do not expose a partially transferred .root file to downstream analyses.
    fd, intermediate = tempfile.mkstemp(
        prefix=".{}-".format(destination.name), suffix=".transfer",
        dir=str(destination.parent),
    )
    intermediate = Path(intermediate)
    try:
        with os.fdopen(fd, "wb") as dest, source.open("rb") as src:
            shutil.copyfileobj(src, dest, length=4 * 1024 * 1024)
            dest.flush()
            os.fsync(dest.fileno())
        if intermediate.stat().st_size != source.stat().st_size:
            raise OSError("NFS transfer size mismatch for {}".format(destination))
        os.replace(intermediate, destination)  # Atomic within the NFS directory.
    except Exception:
        intermediate.unlink(missing_ok=True)
        raise  # Preserve the original scratch ROOT for recovery.

    # Only delete the local result after NFS publication has succeeded.
    try:
        shutil.rmtree(job.scratch_dir)
    except OSError as exc:
        print("[WARN] ROOT transferred, but could not clean {}: {}".format(job.scratch_dir, exc), flush=True)



def detach_worker(specs, jobs, args):
    LOG_DIR.mkdir(parents=True, exist_ok=True)
    stamp = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    launcher_log = LOG_DIR / "RunReco_launcher_{}.log".format(stamp)
    pid_path = LOG_DIR / "RunReco_launcher.pid"
    cmd = [sys.executable, "-u", str(Path(__file__).resolve()), "--worker", "--foreground", "--jobs", str(jobs)]
    if args.no_taskset:
        cmd.append("--no-taskset")
    cmd += ["--scratch-dir", str(args.scratch_dir)]
    if args.max_events:
        cmd += ["--max-events", str(args.max_events)]
    cmd += ["--graph-stride", str(args.graph_stride)]
    cmd += ["-v"] * args.verbose
    for spec in specs:
        cmd += ["--job", spec.run_id, repr(spec.t_start), repr(spec.t_stop), str(spec.n_bins)]
    with launcher_log.open("w") as log_handle:
        proc = subprocess.Popen(cmd, cwd=SCRIPT_DIR, stdin=subprocess.DEVNULL, stdout=log_handle,
                                stderr=subprocess.STDOUT, start_new_session=True, close_fds=True, env=os.environ.copy())
    pid_path.write_text("{}\n".format(proc.pid))
    print("Background reconstruction worker PID: {}".format(proc.pid))
    print("Launcher log: {}".format(launcher_log))
    print("Monitor: tail -f {}".format(launcher_log))
    return 0


def run_queue(specs, max_jobs, args):
    LOG_DIR.mkdir(parents=True, exist_ok=True)
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    prepare_scratch(args.scratch_dir)
    pending = deque(specs)
    running = []
    failures = []
    successes = 0
    results = []
    start_time = time.time()

    use_taskset = not args.no_taskset and taskset_available()
    available_cores = list(sorted(os.sched_getaffinity(0))) if hasattr(os, "sched_getaffinity") else list(range(os.cpu_count() or 1))
    if max_jobs > len(available_cores):
        print("WARNING: restricting parallel jobs to {} allowed CPU cores".format(len(available_cores)), flush=True)
        max_jobs = len(available_cores)
    if not available_cores:
        raise RuntimeError("No CPU cores available to this process")
    print("Starting {} run(s), at most {} concurrent, taskset={}".format(len(specs), max_jobs, use_taskset), flush=True)

    while pending or running:
        while pending and len(running) < max_jobs:
            spec = pending.popleft()
            core = available_cores.pop(0) if use_taskset else None
            try:
                job = start_job(spec, core, args.max_events, args.verbose, args.graph_stride)
            except Exception as exc:
                if core is not None:
                    available_cores.append(core)
                    available_cores.sort()
                failures.append((spec.run_id, str(exc)))
                results.append((spec.run_id, "FAILED", str(exc), str(get_output_file(spec.run_id)), str(get_log_file(spec.run_id))))
                print("[FAIL] run {}: {}".format(spec.run_id, exc), flush=True)
                continue
            running.append(job)
            print("[START] run {} PID {} core={} bins={} t=[{}, {}) log={}".format(
                spec.run_id, job.process.pid, core, spec.n_bins, spec.t_start, spec.t_stop, job.log_file), flush=True)

        remaining = []
        for job in running:
            result = job.process.poll()
            if result is None:
                remaining.append(job)
                continue
            job.log_handle.close()
            if job.core is not None:
                available_cores.append(job.core)
                available_cores.sort()
            if result == 0:
                try:
                    print("[MOVE] run {:07d}: {} -> {}".format(
                        int(job.spec.run_id), job.scratch_output, job.output_file), flush=True)
                    move_completed_root(job)
                except Exception as exc:
                    result = "scratch-to-NFS transfer failed: {}; local output at {}".format(exc, job.scratch_output)
            if result != 0:
                reason = "exit code {}".format(result) if isinstance(result, int) else str(result)
                failures.append((job.spec.run_id, "{}; {}".format(reason, job.log_file)))
                results.append((job.spec.run_id, "FAILED", reason, str(job.output_file), str(job.log_file)))
                print("[FAIL] run {}: {}; log {}; scratch {}".format(job.spec.run_id, reason, job.log_file, job.scratch_dir), flush=True)
            else:
                successes += 1
                results.append((job.spec.run_id, "OK", "", str(job.output_file), str(job.log_file)))
                print("[ OK ] run {}: {} (scratch cleaned)".format(job.spec.run_id, job.output_file), flush=True)
        running = remaining
        if pending or running:
            time.sleep(10)

    stamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    report_file = LOG_DIR / "RunReco_results_{}.csv".format(stamp)
    with report_file.open("w", newline="") as handle:
        writer = csv.writer(handle)
        writer.writerow(("RunID", "Status", "Reason", "OutputROOT", "Log"))
        for run_id, status, reason, output_file, log_file in results:
            writer.writerow(("{:07d}".format(int(run_id)), status, reason, output_file, log_file))
    print("\n=== Summary ===\nSuccessful: {}\nFailed: {}\nElapsed: {:.1f} s\nReport: {}".format(
        successes, len(failures), time.time() - start_time, report_file), flush=True)
    for run_id, problem in failures:
        print("  {}: {}".format(run_id, problem), flush=True)

    if args.worker:
        pid_path = LOG_DIR / "RunReco_launcher.pid"
        try:
            if pid_path.read_text().strip() == str(os.getpid()):
                pid_path.unlink()
        except OSError:
            pass
    return 1 if failures else 0


def main():
    args = parse_args()
    try:
        if args.jobs < 1 or args.jobs > MAX_CORES:
            raise ValueError("-j/--jobs must be between 1 and {}".format(MAX_CORES))
        if args.max_events < 0:
            raise ValueError("--max-events must be >= 0")
        if args.graph_stride < 1:
            raise ValueError("--graph-stride must be >= 1")
        if args.background and args.foreground:
            raise ValueError("Choose either --background or --foreground")

        specs, skipped = collect_specs(args)
        if args.dry_run:
            print("[DRY RUN] no compilation and no reconstruction launched", flush=True)
            return 1 if skipped and args.strict else 0
        if not specs:
            print("[DONE] No pending jobs ({} skipped).".format(len(skipped)), flush=True)
            return 0 if all(reason.startswith("output already exists") for _, reason in skipped) else 1

        if skipped and not args.worker:
            LOG_DIR.mkdir(parents=True, exist_ok=True)
            skipped_file = LOG_DIR / "RunReco_skipped_{}.csv".format(datetime.now().strftime("%Y%m%d_%H%M%S_%f"))
            with skipped_file.open("w", newline="") as handle:
                writer = csv.writer(handle)
                writer.writerow(("RunID", "Reason"))
                writer.writerows(("{:07d}".format(int(run_id)), reason) for run_id, reason in skipped)
            print("[SELECTION] skipped-run report: {}".format(skipped_file), flush=True)

        background = choose_background(args)
        if not args.worker:
            compile_reco()
        elif not EXECUTABLE.is_file():
            raise FileNotFoundError("Worker cannot locate compiled executable {}".format(EXECUTABLE))
        if background:
            return detach_worker(specs, args.jobs, args)
        return run_queue(specs, args.jobs, args)
    except (ValueError, EOFError, FileNotFoundError, subprocess.CalledProcessError, RuntimeError, OSError) as exc:
        print("ERROR: {}".format(exc), file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
