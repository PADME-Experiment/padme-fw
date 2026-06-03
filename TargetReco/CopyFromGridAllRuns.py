#!/usr/bin/env python3
import os
import re
import sys
import time
import argparse
import subprocess
from multiprocessing import Pool

# Matches: run_XXXXX_YYYYMMDD_HHMMSS_lvl1_00_12.root  -> i=00, j=12
FILE_RE = re.compile(r"^run_\d+_\d{8}_\d{6}_lvl1_(\d{2})_(\d+)\.root$")

def file_sort_key(fname: str):
    """
    Order rule:
      for each j: i = 00,01,02,03,04
      then next j
    => sort by (j, i)
    """
    m = FILE_RE.match(fname)
    if not m:
        return (10**12, 10**12, fname)

    i = int(m.group(1))      # 00..04
    j = int(m.group(2))      # 0..N (numeric, not lexical)
    return (j, i, fname)

# -----------------------------------------
# Colored output (auto-disabled if not TTY or --no-color)
# -----------------------------------------
GREEN = "\033[92m"
RED   = "\033[91m"
YEL   = "\033[93m"
RESET = "\033[0m"

def colorize(s: str, c: str, use_color: bool) -> str:
    return f"{c}{s}{RESET}" if use_color else s

# -----------------------------------------
# Progress (TTY: bar; non-TTY: periodic lines)
# -----------------------------------------
def progress_update(done, total, start_time, is_tty: bool, last_print_state: dict, every_n=200, every_s=5.0):
    now = time.time()
    if total <= 0:
        return

    elapsed = now - start_time
    eta = (elapsed / done * (total - done)) if done > 0 else 0.0

    if is_tty:
        bar_length = 40
        fraction = done / total
        filled = int(bar_length * fraction)
        bar = "#" * filled + "-" * (bar_length - filled)
        sys.stdout.write(f"\r[{bar}] {done}/{total} ({fraction*100:5.1f}%)  ETA: {int(eta)}s")
        sys.stdout.flush()
        if done == total:
            sys.stdout.write("\n")
        return

    # non-TTY (nohup): print occasionally
    last_t = last_print_state.get("t", 0.0)
    last_n = last_print_state.get("n", 0)
    if (done - last_n) >= every_n or (now - last_t) >= every_s or done == total:
        last_print_state["t"] = now
        last_print_state["n"] = done
        print(f"[{time.strftime('%F %T')}] Done {done}/{total}  ETA ~{int(eta)}s", flush=True)

# -----------------------------------------
# GFAL helpers
# -----------------------------------------
def run_cmd(cmd):
    return subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True)

def gfal_ls(url: str):
    r = run_cmd(["gfal-ls", url])
    if r.returncode != 0:
        raise RuntimeError(r.stderr.strip() or f"gfal-ls failed for {url}")
    return [line.strip() for line in r.stdout.splitlines() if line.strip()]

def normalize_run(run_token: str) -> str:
    """
    Accepts '12345', '00123', etc. Returns 5-digit string (left-padded).
    """
    t = run_token.strip()
    if not t.isdigit():
        raise ValueError(f"Run '{run_token}' is not numeric")
    return t.zfill(5)

def find_full_run_dir(remote_url: str, run5: str):
    """
    Original pattern uses: run_prefix = f"run_00{run_short}_"
    where run_short is 5 digits.
    """
    run_prefix = f"run_00{run5}_"
    dirs = gfal_ls(remote_url)
    matches = [d for d in dirs if d.startswith(run_prefix)]
    if not matches:
        return None
    # deterministic choice if multiple
    matches.sort()
    return matches[0]

# -----------------------------------------
# Copy task (picklable for multiprocessing)
# -----------------------------------------
def copy_task(task):
    """
    task = (run_id, remote_run_url, local_dir, filename, skip_existing)
    """
    run_id, remote_run_url, local_dir, filename, skip_existing = task
    local_path = os.path.join(local_dir, filename)

    if skip_existing and os.path.exists(local_path) and os.path.getsize(local_path) > 0:
        return (run_id, filename, 0, "SKIPPED (exists)")

    remote_file_url = remote_run_url + "/" + filename
    cmd = ["gfal-copy", remote_file_url, local_path]
    r = run_cmd(cmd)
    return (run_id, filename, r.returncode, r.stderr.strip())

# -----------------------------------------
# Parse runs from args
# -----------------------------------------
def parse_runs(runs_args, runs_file):
    runs = []

    # From --runs (supports commas too)
    if runs_args:
        for tok in runs_args:
            for part in tok.split(","):
                part = part.strip()
                if part:
                    runs.append(part)

    # From --runs-file (whitespace and/or commas per line)
    if runs_file:
        with open(runs_file, "r") as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith("#"):
                    continue
                for part in re.split(r"[,\s]+", line):
                    part = part.strip()
                    if part:
                        runs.append(part)

    # Deduplicate preserving order
    seen = set()
    out = []
    for r in runs:
        if r not in seen:
            seen.add(r)
            out.append(r)
    return out

# -----------------------------------------
# Main
# -----------------------------------------
def main():
    ap = argparse.ArgumentParser(
        description="Copy PADME DAQ files for one or more runs via GFAL, with parallel transfers (nohup-friendly)."
    )
    ap.add_argument("--year", help="YEAR (e.g. 2025)")
    ap.add_argument("--datadir", choices=["rawdata", "mm", "tmm"], help="DATADIR: rawdata/mm/tmm")
    ap.add_argument("--runs", nargs="*", help="Run numbers (many). Space-separated and/or comma-separated.")
    ap.add_argument("--runs-file", help="File containing run numbers (whitespace and/or commas, # comments allowed).")
    ap.add_argument("--max-files", default="all", help="How many files per run: integer or 'all' (default: all).")
    ap.add_argument("--jobs", type=int, default=1, help="Total parallel gfal-copy jobs (default: 1).")
    ap.add_argument("--local-base", default="/data9Vd1/padme/mancinima", help="Local base output dir.")
    ap.add_argument("--yes", action="store_true", help="Do not prompt; proceed automatically.")
    ap.add_argument("--dry-run", action="store_true", help="List what would be copied, then exit.")
    ap.add_argument("--skip-missing-runs", action="store_true", help="If a run dir is missing, warn and continue.")
    ap.add_argument("--skip-existing", action="store_true", help="Skip files that already exist locally with size > 0.")
    ap.add_argument("--no-color", action="store_true", help="Disable colored output.")
    args = ap.parse_args()

    # If running interactively and missing args, prompt (but don't prompt under nohup)
    interactive = sys.stdin.isatty() and not args.yes
    if not args.year:
        if interactive:
            args.year = input("Please enter YEAR: ").strip()
        else:
            print("ERROR: --year is required for non-interactive/nohup usage.", file=sys.stderr)
            sys.exit(1)

    if not args.datadir:
        if interactive:
            args.datadir = input("Please enter DATADIR (rawdata/mm/tmm): ").strip().lower()
        else:
            print("ERROR: --datadir is required for non-interactive/nohup usage.", file=sys.stderr)
            sys.exit(1)

    run_tokens = parse_runs(args.runs, args.runs_file)
    if not run_tokens:
        if interactive:
            one = input("Please enter RUN NUMBER (nnnnn): ").strip()
            run_tokens = [one]
        else:
            print("ERROR: Provide runs via --runs and/or --runs-file for non-interactive/nohup usage.", file=sys.stderr)
            sys.exit(1)

    # max-files parsing
    max_files = None
    if str(args.max_files).strip().lower() not in ("all", "a", ""):
        try:
            max_files = int(args.max_files)
            if max_files <= 0:
                raise ValueError
        except ValueError:
            print("ERROR: --max-files must be a positive integer or 'all'.", file=sys.stderr)
            sys.exit(1)

    use_color = (not args.no_color) and sys.stdout.isatty()
    is_tty = sys.stdout.isatty()

    remote_base = (
        "srm://atlasse.lnf.infn.it:8446/srm/managerv2"
        "?SFN=/dpm/lnf.infn.it/home/vo.padme.org"
    )
    remote_dir = f"/daq/{args.year}/{args.datadir}"
    remote_url = remote_base + remote_dir

    # Basic tool existence check
    for tool in ("gfal-ls", "gfal-copy"):
        if subprocess.call(["which", tool], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL) != 0:
            print(colorize(f"ERROR: {tool} not found in PATH.", RED, use_color), file=sys.stderr)
            sys.exit(1)

    # Build copy task list across all runs
    all_tasks = []
    run_meta = {}  # run_id -> dict
    print(f"Remote base: {remote_dir}")
    print(f"Runs requested: {', '.join(run_tokens)}\n")

    for rtok in run_tokens:
        try:
            run5 = normalize_run(rtok)
        except ValueError as e:
            print(colorize(f"ERROR: {e}", RED, use_color), file=sys.stderr)
            sys.exit(1)

        run_id = run5
        print(f"--- Run {run_id} ---")

        try:
            full_run_dir = find_full_run_dir(remote_url, run5)
        except Exception as e:
            msg = f"Cannot list remote dir for run {run_id}: {e}"
            if args.skip_missing_runs:
                print(colorize("WARN: " + msg, YEL, use_color))
                continue
            print(colorize("ERROR: " + msg, RED, use_color), file=sys.stderr)
            sys.exit(1)

        if not full_run_dir:
            msg = f"No directory found for run prefix run_00{run5}_"
            if args.skip_missing_runs:
                print(colorize("WARN: " + msg, YEL, use_color))
                continue
            print(colorize("ERROR: " + msg, RED, use_color), file=sys.stderr)
            sys.exit(1)

        remote_run_url = remote_url + "/" + full_run_dir
        print(f"Found: {full_run_dir}")

        try:
            files_all = gfal_ls(remote_run_url)
        except Exception as e:
            msg = f"Cannot list run directory {remote_run_url}: {e}"
            if args.skip_missing_runs:
                print(colorize("WARN: " + msg, YEL, use_color))
                continue
            print(colorize("ERROR: " + msg, RED, use_color), file=sys.stderr)
            sys.exit(1)

        if not files_all:
            print(colorize("No files found. Skipping.", YEL, use_color))
            continue

        files_sorted = sorted(files_all, key=file_sort_key)
        if max_files is not None:
            files_sorted = files_sorted[:max_files]

        local_dir = os.path.join(args.local_base, args.datadir, str(args.year), full_run_dir)
        os.makedirs(local_dir, exist_ok=True)

        run_meta[run_id] = {
            "full_run_dir": full_run_dir,
            "remote_run_url": remote_run_url,
            "local_dir": local_dir,
            "nfiles": len(files_sorted),
        }

        for fn in files_sorted:
            all_tasks.append((run_id, remote_run_url, local_dir, fn, args.skip_existing))

        print(f"Files queued: {len(files_sorted)}")
        print(f"Local dir:    {local_dir}\n")

    if not all_tasks:
        print(colorize("Nothing to do (no tasks queued).", YEL, use_color))
        return

    # Confirm / dry-run
    total = len(all_tasks)
    print(f"TOTAL files to copy: {total}")
    print(f"Parallel jobs:       {args.jobs}")
    if args.skip_existing:
        print("Skip existing:       yes")

    if args.dry_run:
        print("\nDRY RUN. First 20 tasks:")
        for t in all_tasks[:20]:
            run_id, _, local_dir, fn, _ = t
            print(f"  Run {run_id}: {fn} -> {local_dir}")
        if total > 20:
            print(f"  ... and {total-20} more")
        return

    if not args.yes and interactive:
        confirm = input(f"\nProceed and copy these {total} files? (yes/no): ").strip().lower()
        if confirm != "yes":
            print("Aborted by user.")
            sys.exit(0)
    elif not args.yes and not interactive:
        print("ERROR: Non-interactive mode requires --yes (or use --dry-run).", file=sys.stderr)
        sys.exit(1)

    # Copy
    print("\nStarting copy...\n")
    start = time.time()
    last_print_state = {}

    results = []
    if args.jobs <= 1:
        done = 0
        for task in all_tasks:
            results.append(copy_task(task))
            done += 1
            progress_update(done, total, start, is_tty, last_print_state)
    else:
        with Pool(processes=args.jobs) as pool:
            done = 0
            for res in pool.imap_unordered(copy_task, all_tasks, chunksize=1):
                results.append(res)
                done += 1
                progress_update(done, total, start, is_tty, last_print_state)

        if is_tty:
            # ensure newline after carriage-return bar
            print()

    # Summaries
    per_run = {rid: {"ok": 0, "fail": 0, "failed": []} for rid in run_meta.keys()}
    total_ok = 0
    total_fail = 0

    for run_id, fname, rc, err in results:
        if rc == 0:
            total_ok += 1
            if run_id in per_run:
                per_run[run_id]["ok"] += 1
        else:
            total_fail += 1
            if run_id in per_run:
                per_run[run_id]["fail"] += 1
                per_run[run_id]["failed"].append((fname, err))

    print("\n--------------------------------------------")
    print(colorize(f" ✓ OK:   {total_ok}", GREEN, use_color))
    print(colorize(f" ✗ FAIL: {total_fail}", RED, use_color))

    # Per-run breakdown
    print("\nPer-run summary:")
    for rid in sorted(per_run.keys()):
        ok = per_run[rid]["ok"]
        fail = per_run[rid]["fail"]
        print(f"  Run {rid}: OK {ok}  FAIL {fail}")

    if total_fail == 0:
        print(colorize("\nALL DONE — EVERYTHING IS OK!", GREEN, use_color))
    else:
        print(colorize("\nCompleted with problems — some files failed.", RED, use_color))
        # show up to 10 failures overall
        shown = 0
        for rid in sorted(per_run.keys()):
            for fname, err in per_run[rid]["failed"]:
                if shown >= 10:
                    break
                first_line = err.splitlines()[0] if err else "(no stderr)"
                print(f" - Run {rid}: {fname} -> {first_line}")
                shown += 1
            if shown >= 10:
                break
        if total_fail > 10:
            print(f"... and {total_fail-10} more failures.")
    print("--------------------------------------------\n")

if __name__ == "__main__":
    main()
 