#!/usr/bin/env python3
import argparse
import csv
import math
import os
import shlex
import subprocess
import sys
import time
from collections import deque
from dataclasses import dataclass
from datetime import datetime
from pathlib import Path
from typing import Optional

SCRIPT_DIR = Path(__file__).resolve().parent
BUILD_SCRIPT = SCRIPT_DIR / "runRecoTMM.sh"
EXECUTABLE = SCRIPT_DIR / "RunRecoTMM"
LIST_DIR = SCRIPT_DIR / "TMMFileList"
OUTPUT_DIR = Path("/data9Vd1/padme/mancinima/RecoRun4_v0/TMM")
TIMES_FILE = Path("/home/mancinima/BeamMonitorRun4/padme-fw/RunIV_Times.txt")
RUN_LIST_FILE = Path("/home/mancinima/BeamMonitorRun4/padme-fw/UserAnalysis/config/run_level_offline_db_2025_full_withTMM.txt")
MAX_CORES = 20
DEFAULT_NBINS = None  # As in Target RunReco.py: supply -b or answer the prompt.
OUTPUT_PREFIX = "RecoTMM_v0_"


@dataclass(frozen=True)
class RunSpec:
    run_id: str
    det_run_id: int
    t_start: float
    t_stop: float
    n_bins: int
    list_file: Path

    def validate(self):
        if not self.run_id.isdigit() or int(self.run_id) <= 0:
            raise ValueError("Invalid PADME RunID: {}".format(self.run_id))
        if self.det_run_id < 0:
            raise ValueError("Run {}: invalid detector run ID {}".format(self.run_id, self.det_run_id))
        if not (math.isfinite(self.t_start) and math.isfinite(self.t_stop) and self.t_stop > self.t_start):
            raise ValueError("Run {}: require finite tStopUnix > tStartUnix".format(self.run_id))
        if self.n_bins < 1:
            raise ValueError("Run {}: nBins must be >= 1".format(self.run_id))


@dataclass
class RunningJob:
    spec: RunSpec
    core: Optional[int]
    process: subprocess.Popen
    log_handle: object
    output_file: Path
    log_file: Path


def normalize_run_id(value):
    """Allow both 90032 and 0090032 as the same run."""
    value = str(value).strip()
    if not value.isdigit() or int(value) <= 0:
        raise ValueError("Invalid RunID: {!r}".format(value))
    return str(int(value))


def get_list_file(run_id, list_dir):
    run_number = int(run_id)
    candidates = [
        list_dir / "TMMRaw_run_{}.list".format(run_number),
        list_dir / "TMMRaw_run_{:07d}.list".format(run_number),
    ]
    for candidate in candidates:
        if candidate.is_file() and candidate.stat().st_size > 0:
            return candidate
    return candidates[0]


def get_output_file(spec, output_dir, max_events):
    # Production naming matches Target/LG exactly: RecoTMM_v0_00nnnnn.root.
    # Limited-event tests use a distinct name to protect the complete output.
    basename = "{}{:07d}".format(OUTPUT_PREFIX, int(spec.run_id))
    if max_events > 0:
        basename += "_Nevt{}".format(max_events)
    return output_dir / (basename + ".root")


def get_log_file(spec, output_dir, max_events):
    suffix = "all" if max_events == 0 else str(max_events)
    return output_dir / "logs" / "run{:07d}_bins{}_nevt{}.log".format(
        int(spec.run_id), spec.n_bins, suffix
    )


def parse_args(argv=None):
    p = argparse.ArgumentParser(description="Time-binned TMM reconstruction using a 12-core queued runner")
    p.add_argument("runs", nargs="*", help="Optional approved PADME RunIDs; default: all approved runs")
    p.add_argument("-j", "--jobs", type=int, default=MAX_CORES, help="Concurrent jobs (default/max: 12)")
    p.add_argument("-b", "--nbins", type=int, default=DEFAULT_NBINS, help="Time bins per run; must match Target/LG")
    p.add_argument("--times-file", type=Path, default=TIMES_FILE, help="RunID, optional detector-ID(s), UNIX start/stop (detector IDs ignored)")
    p.add_argument("--run-list", type=Path, default=RUN_LIST_FILE, help="Approved runs (first column only)")
    p.add_argument("--list-dir", type=Path, default=LIST_DIR, help="TMM input lists: run<RunID>.list")
    p.add_argument("--output-dir", type=Path, default=OUTPUT_DIR, help="Directory for ROOT outputs and logs (default: /data9Vd1/padme/mancinima/RecoRun4_v0/TMM)")
    p.add_argument("--dry-run", action="store_true", help="Inspect selection and time bounds; do not compile/launch")
    p.add_argument("--skip-existing", action="store_true", help="Skip nonempty outputs (not ROOT-validated)")
    p.add_argument("--strict", action="store_true", help="Fail if any selected run lacks time data or an input list")
    p.add_argument("--background", action="store_true", help="Launch detached queue after compilation")
    p.add_argument("--foreground", action="store_true", help="Run queue in foreground without asking")
    p.add_argument("--no-taskset", action="store_true", help="Do not pin each job to one CPU core")
    p.add_argument("--max-events", type=int, default=0, help="Process first N input entries per run; 0=all (TEST ONLY, uses separate _NevtN output name)")
    p.add_argument("--skip-build", action="store_true", help="Reuse an already compiled RunRecoTMM executable")
    p.add_argument("-v", "--verbose", action="count", default=0, help="Print launcher commands; not passed to C++")
    p.add_argument("--worker", action="store_true", help=argparse.SUPPRESS)
    p.add_argument("--job", nargs=6, action="append", metavar=("RUN", "DET", "START", "STOP", "NBINS", "LIST"), help=argparse.SUPPRESS)
    return p.parse_args(argv)


def read_approved_runs(path):
    if not path.is_file():
        raise FileNotFoundError("Approved run list not found: {}".format(path))
    runs = []
    seen = set()
    with path.open("r", encoding="utf-8-sig") as handle:
        for line_number, raw in enumerate(handle, 1):
            line = raw.split("#", 1)[0].strip()
            if not line:
                continue
            col0 = line.split()[0]
            if col0.lower() in ("run", "runid", "run_id", "run_number", "runnumber", "runnum"):
                continue
            try:
                run = normalize_run_id(col0)
            except ValueError as exc:
                raise ValueError("{}:{}: invalid RunID {!r}".format(path, line_number, col0)) from exc
            if run in seen:
                print("[WARN] Duplicate approved RunID {}, keeping first entry".format(run), flush=True)
                continue
            seen.add(run)
            runs.append(run)
    if not runs:
        raise ValueError("No approved runs found in {}".format(path))
    return runs


def _extract_unix_interval(columns, path, line_number):
    """Read UNIX boundaries, tolerating multiple detector IDs before them.

    Standard rows have start/stop in columns 2/3. If the detector-ID field
    contains separate whitespace-delimited IDs, find the first adjacent pair
    of plausible UNIX-second timestamps instead. The extra human-readable
    columns in RunIV_Times.txt are ignored.
    """
    def usable(a, b):
        return (math.isfinite(a) and math.isfinite(b) and
                1.e8 <= a < 1.e11 and 1.e8 <= b < 1.e11 and b > a)

    # Preserve the Target/LG parser's column positions for ordinary records.
    if len(columns) >= 4:
        try:
            start, stop = float(columns[2]), float(columns[3])
            if usable(start, stop):
                return start, stop
        except ValueError:
            pass

    # e.g. '90032 705 706 1770000000 1770003600 ...'
    for i in range(1, len(columns) - 1):
        try:
            start, stop = float(columns[i]), float(columns[i + 1])
        except ValueError:
            continue
        if usable(start, stop):
            return start, stop

    raise ValueError("{}:{}: cannot identify valid UNIX start/stop timestamps: {}".format(
        path, line_number, " ".join(columns[:6])))


def read_run_times(path, selected_ids):
    """Get PADME-run UNIX windows; ignore detector IDs completely.

    The C++ -d option is only a run label; pass the neutral value 0. If the
    same PADME RunID appears in several records (one per detector subrun),
    merge their time intervals so that the one ROOT output covers all data.
    """
    if not path.is_file():
        raise FileNotFoundError("UNIX time database not found: {}".format(path))
    by_run = {}
    merged_runs = set()
    with path.open("r", encoding="utf-8-sig") as handle:
        for line_number, raw in enumerate(handle, 1):
            line = raw.split("#", 1)[0].strip()
            if not line:
                continue
            columns = line.split()
            if columns[0].lower() in ("runid", "run", "run_id"):
                continue
            try:
                run = normalize_run_id(columns[0])
            except ValueError:
                continue
            if run not in selected_ids:
                continue
            start, stop = _extract_unix_interval(columns, path, line_number)
            if run in by_run:
                _old_det, old_start, old_stop = by_run[run]
                start, stop = min(start, old_start), max(stop, old_stop)
                merged_runs.add(run)
            # DetRunID is deliberately unused. C++ requires -d, so pass 0.
            by_run[run] = (0, start, stop)

    for run in sorted(merged_runs, key=int):
        _, start, stop = by_run[run]
        print("[WARN] Run {:07d} has multiple time records: using the combined "
              "UNIX interval [{:.6f}, {:.6f}). Check Target/LG uses the same "
              "bounds for bin alignment.".format(int(run), start, stop), flush=True)
    return by_run


def make_spec(run, det, start, stop, bins, list_file):
    spec = RunSpec(normalize_run_id(run), int(det), float(start), float(stop), int(bins), Path(list_file))
    spec.validate()
    return spec


def collect_specs(args):
    if args.worker:
        if not args.job:
            raise ValueError("Internal worker has no --job entries")
        return [make_spec(*values) for values in args.job], []

    approved = read_approved_runs(args.run_list)
    approved_set = set(approved)
    if args.runs:
        requested = [normalize_run_id(x) for x in args.runs]
        if len(requested) != len(set(requested)):
            raise ValueError("Duplicate RunIDs on command line")
        unknown = [x for x in requested if x not in approved_set]
        if unknown:
            raise ValueError("Run(s) not in approved list: {}".format(", ".join(unknown)))
    else:
        requested = approved

    n_bins = args.nbins
    if n_bins is None:
        if not sys.stdin.isatty():
            raise ValueError("Specify -b/--nbins when running noninteractively")
        n_bins = int(input("Number of TMM/Target shared time bins: ").strip())
    if n_bins < 1:
        raise ValueError("-b/--nbins must be >= 1")

    times = read_run_times(args.times_file, set(requested))
    selected = []
    skipped = []
    for run in requested:
        if run not in times:
            skipped.append((run, "missing UNIX time record"))
            continue
        list_file = get_list_file(run, args.list_dir)
        if not list_file.is_file() or list_file.stat().st_size == 0:
            skipped.append((run, "missing/empty TMM file list: {}".format(list_file)))
            continue
        det, start, stop = times[run]
        spec = make_spec(run, det, start, stop, n_bins, list_file)
        output_file = get_output_file(spec, args.output_dir, args.max_events)
        if args.skip_existing and output_file.is_file() and output_file.stat().st_size > 0:
            skipped.append((run, "output already exists (not ROOT-validated): {}".format(output_file)))
            continue
        selected.append(spec)

    print("[SELECTION] approved={} requested={} ready={} skipped={} bins/run={}".format(
        len(approved), len(requested), len(selected), len(skipped), n_bins), flush=True)
    print("[SELECTION] time database: {}".format(args.times_file), flush=True)
    print("[SELECTION] TMM lists: {}".format(args.list_dir), flush=True)
    print("[SELECTION] ROOT output: {}".format(args.output_dir), flush=True)
    preview = selected if args.verbose else selected[:8]
    for spec in preview:
        print("  run={:07d} t=[{:.6f}, {:.6f}) bins={} list={}".format(
            int(spec.run_id), spec.t_start, spec.t_stop, spec.n_bins, spec.list_file), flush=True)
    if len(selected) > len(preview):
        print("  ... and {} more ready runs (use -v to print all)".format(len(selected) - len(preview)), flush=True)
    for run, reason in skipped:
        print("[SKIP] {:07d}: {}".format(int(run), reason), flush=True)

    if args.strict and any(not reason.startswith("output already exists") for _, reason in skipped):
        raise ValueError("Strict selection: one or more requested runs lack input files or time metadata")
    return selected, skipped


def compile_reco():
    if not BUILD_SCRIPT.is_file():
        raise FileNotFoundError("TMM build script not found: {}".format(BUILD_SCRIPT))
    cmd = ["bash", str(BUILD_SCRIPT), "--build-only"]
    print("[BUILD] {}".format(" ".join(shlex.quote(x) for x in cmd)), flush=True)
    subprocess.run(cmd, cwd=SCRIPT_DIR, check=True)
    if not EXECUTABLE.is_file():
        raise RuntimeError("Compilation returned successfully but {} is missing".format(EXECUTABLE))


def taskset_available():
    try:
        subprocess.run(["taskset", "--version"], check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        return True
    except (FileNotFoundError, subprocess.CalledProcessError):
        return False


def choose_background(args):
    if args.background and args.foreground:
        raise ValueError("Choose either --background or --foreground")
    if args.worker or args.foreground:
        return False
    if args.background:
        return True
    if not sys.stdin.isatty():
        return False
    return input("Launch TMM queue in background? (yes/no): ").strip().lower() in ("y", "yes")


def start_job(spec, core, args):
    output_file = get_output_file(spec, args.output_dir, args.max_events)
    log_file = get_log_file(spec, args.output_dir, args.max_events)
    cmd = [str(EXECUTABLE), "-l", str(spec.list_file),
           "-r", str(int(spec.run_id)), "-d", str(spec.det_run_id),
           "-n", str(args.max_events), "-s", repr(spec.t_start),
           "-t", repr(spec.t_stop), "-b", str(spec.n_bins),
           "-o", str(output_file)]
    if core is not None:
        cmd = ["taskset", "-c", str(core)] + cmd
    env = os.environ.copy()
    env.update({"OMP_NUM_THREADS": "1", "MKL_NUM_THREADS": "1",
                "OPENBLAS_NUM_THREADS": "1", "VECLIB_MAXIMUM_THREADS": "1"})
    if args.verbose:
        print("[CMD ] {}".format(" ".join(shlex.quote(x) for x in cmd)), flush=True)
    log_file.parent.mkdir(parents=True, exist_ok=True)
    log_handle = log_file.open("w", encoding="utf-8")
    try:
        process = subprocess.Popen(cmd, cwd=SCRIPT_DIR, stdin=subprocess.DEVNULL,
                                   stdout=log_handle, stderr=subprocess.STDOUT, env=env)
    except Exception:
        log_handle.close()
        raise
    return RunningJob(spec, core, process, log_handle, output_file, log_file)


def detach_worker(specs, args):
    logs = args.output_dir / "logs"
    logs.mkdir(parents=True, exist_ok=True)
    stamp = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    launcher_log = logs / "RunRecoTMM_launcher_{}.log".format(stamp)
    pid_file = logs / "RunRecoTMM_launcher.pid"
    cmd = [sys.executable, "-u", str(Path(__file__).resolve()),
           "--worker", "--foreground", "--jobs", str(args.jobs),
           "--output-dir", str(args.output_dir), "--max-events", str(args.max_events)]
    if args.no_taskset:
        cmd.append("--no-taskset")
    if args.verbose:
        cmd.extend(["-v"] * args.verbose)
    for spec in specs:
        cmd.extend(["--job", spec.run_id, str(spec.det_run_id), repr(spec.t_start),
                    repr(spec.t_stop), str(spec.n_bins), str(spec.list_file)])
    with launcher_log.open("w", encoding="utf-8") as handle:
        process = subprocess.Popen(cmd, cwd=SCRIPT_DIR, stdin=subprocess.DEVNULL,
                                   stdout=handle, stderr=subprocess.STDOUT,
                                   start_new_session=True, close_fds=True)
    pid_file.write_text("{}\n".format(process.pid))
    print("Background TMM queue worker PID: {}".format(process.pid))
    print("Launcher log: {}".format(launcher_log))
    print("Monitor: tail -f {}".format(launcher_log))
    return 0


def run_queue(specs, args):
    logs = args.output_dir / "logs"
    logs.mkdir(parents=True, exist_ok=True)
    pending = deque(specs)
    running = []
    results = []
    started = time.time()
    allowed_cores = sorted(os.sched_getaffinity(0)) if hasattr(os, "sched_getaffinity") else list(range(os.cpu_count() or 1))
    if not allowed_cores:
        raise RuntimeError("No allowed CPU cores")
    max_jobs = min(args.jobs, MAX_CORES, len(allowed_cores))
    use_taskset = (not args.no_taskset) and taskset_available()
    if args.jobs > max_jobs:
        print("[WARN] Limiting concurrent TMM reconstructions to {} allowed cores".format(max_jobs), flush=True)
    free_cores = allowed_cores[:max_jobs]
    print("[QUEUE] runs={} concurrent={} taskset={}".format(len(specs), max_jobs, use_taskset), flush=True)

    try:
        while pending or running:
            while pending and len(running) < max_jobs:
                spec = pending.popleft()
                core = free_cores.pop(0) if use_taskset else None
                try:
                    job = start_job(spec, core, args)
                except Exception as exc:
                    if core is not None:
                        free_cores.append(core)
                        free_cores.sort()
                    results.append((spec, "FAILED", str(exc)))
                    print("[FAIL] run {}: {}".format(spec.run_id, exc), flush=True)
                    continue
                running.append(job)
                print("[START] run={:07d} det={} PID={} core={} bins={} log={}".format(
                    int(spec.run_id), spec.det_run_id, job.process.pid, core, spec.n_bins, job.log_file), flush=True)

            remaining = []
            for job in running:
                rc = job.process.poll()
                if rc is None:
                    remaining.append(job)
                    continue
                job.log_handle.close()
                if job.core is not None:
                    free_cores.append(job.core)
                    free_cores.sort()
                if rc == 0 and (not job.output_file.is_file() or job.output_file.stat().st_size == 0):
                    rc = "missing/empty output ROOT file"
                if rc == 0:
                    results.append((job.spec, "OK", ""))
                    print("[ OK ] run={:07d} output={}".format(int(job.spec.run_id), job.output_file), flush=True)
                else:
                    reason = "exit code {}".format(rc) if isinstance(rc, int) else str(rc)
                    results.append((job.spec, "FAILED", reason))
                    print("[FAIL] run={:07d} reason={} log={}".format(
                        int(job.spec.run_id), reason, job.log_file), flush=True)
            running = remaining
            if pending or running:
                time.sleep(0.2)
    except KeyboardInterrupt:
        print("\n[STOP] Interrupt received: terminating currently running jobs", flush=True)
        for job in running:
            job.process.terminate()
        for job in running:
            try:
                job.process.wait(timeout=5)
            except subprocess.TimeoutExpired:
                job.process.kill()
                job.process.wait()
            job.log_handle.close()
        raise

    report_file = logs / "RunRecoTMM_results_{}.csv".format(datetime.now().strftime("%Y%m%d_%H%M%S_%f"))
    with report_file.open("w", newline="", encoding="utf-8") as handle:
        writer = csv.writer(handle)
        writer.writerow(("RunID", "DetRunID", "tStartUnix", "tStopUnix", "nBins", "Status", "Reason", "OutputROOT", "Log"))
        for spec, status, reason in results:
            writer.writerow(("{:07d}".format(int(spec.run_id)), spec.det_run_id, spec.t_start,
                             spec.t_stop, spec.n_bins, status, reason,
                             str(get_output_file(spec, args.output_dir, args.max_events)),
                             str(get_log_file(spec, args.output_dir, args.max_events))))
    failures = [(spec, reason) for spec, status, reason in results if status != "OK"]
    print("\n=== TMM reconstruction summary ===", flush=True)
    print("Success={} Failed={} Elapsed={:.1f}s".format(len(results) - len(failures), len(failures), time.time() - started), flush=True)
    print("Report: {}".format(report_file), flush=True)
    if args.worker:
        pid_file = logs / "RunRecoTMM_launcher.pid"
        try:
            if pid_file.read_text().strip() == str(os.getpid()):
                pid_file.unlink()
        except OSError:
            pass
    return 1 if failures else 0


def main(argv=None):
    args = parse_args(argv)
    try:
        args.list_dir = args.list_dir.expanduser().resolve()
        args.output_dir = args.output_dir.expanduser().resolve()
        args.run_list = args.run_list.expanduser().resolve()
        args.times_file = args.times_file.expanduser().resolve()
        if not 1 <= args.jobs <= MAX_CORES:
            raise ValueError("-j/--jobs must be between 1 and {}".format(MAX_CORES))
        if args.max_events < 0:
            raise ValueError("--max-events must be >= 0")
        if args.background and args.foreground:
            raise ValueError("Choose either --background or --foreground")
        specs, skipped = collect_specs(args)
        if args.dry_run:
            print("[DRY RUN] no compilation or reconstruction", flush=True)
            return 0
        if not specs:
            print("[DONE] No pending jobs ({} skipped)".format(len(skipped)), flush=True)
            return 0 if all(reason.startswith("output already exists") for _, reason in skipped) else 1
        if args.max_events:
            print("[WARN] TEST MODE: only first {} input entries per run, some bins may be unexamined".format(args.max_events), flush=True)
        background = choose_background(args)
        if not args.worker:
            if args.skip_build:
                if not EXECUTABLE.is_file():
                    raise FileNotFoundError("--skip-build: executable not found: {}".format(EXECUTABLE))
            else:
                compile_reco()
        elif not EXECUTABLE.is_file():
            raise FileNotFoundError("Worker cannot find executable {}".format(EXECUTABLE))
        if background:
            return detach_worker(specs, args)
        return run_queue(specs, args)
    except KeyboardInterrupt:
        print("Interrupted", file=sys.stderr)
        return 130
    except (ValueError, EOFError, FileNotFoundError, subprocess.CalledProcessError, RuntimeError, OSError) as exc:
        print("ERROR: {}".format(exc), file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
