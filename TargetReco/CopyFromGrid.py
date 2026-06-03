#!/usr/bin/env python3
import os
import subprocess
import sys
from multiprocessing import Pool
from functools import partial
import time

import re

# Matches: ..._lvl1_00_12.root  -> i=00, j=12
# (If your suffix differs slightly, tell me and I’ll adapt the regex.)
# RAWDATA format
RAW_RE = re.compile(r"^run_\d+_\d{8}_\d{6}_lvl1_(\d{2})_(\d+)\.root$")

# MMDATA / TMMDATA format
MM_RE = re.compile(r"^run\d+_run_(\d+)_(\d{8})_(\d{6})_(\d+)_(\d+)\.root$")

def mm_sort_key(fname: str):
    """
    MM/TMM naming:
      runNNN_run_nnnnnnn_yyyymmdd_hhmmss_unixtime.root
      runNNN_run_nnnnnnn_yyyymmdd_hhmmss_unixtime_iii.root

    The file without final _iii is treated as index 0.
    """

    base = fname.replace(".root", "")
    parts = base.split("_")

    # without final index
    if len(parts) == 6:
        return 0

    # with final index
    if len(parts) == 7:
        try:
            return int(parts[-1])
        except ValueError:
            return 10**12

    # malformed filename
    return 10**12

def file_sort_key(fname: str):

    # --- RAWDATA ---
    m = RAW_RE.match(fname)
    if m:
        i = int(m.group(1))   # 00..04
        j = int(m.group(2))   # event group
        return (0, j, i)

    # --- MMDATA / TMMDATA ---
    m = MM_RE.match(fname)
    if m:
        run = int(m.group(1))      # run_nnnnnnn (same as rawdata)
        index = int(m.group(5))    # iii (file number)
        return (1, run, index)

    # fallback
    return (9, fname)

# -----------------------------------------
# Colored output
# -----------------------------------------
GREEN = "\033[92m"
RED = "\033[91m"
RESET = "\033[0m"

# -----------------------------------------
# Simple ASCII progress bar
# -----------------------------------------
def ascii_progress(current, total, start_time, bar_length=40):
    fraction = current / total
    filled = int(bar_length * fraction)
    bar = "#" * filled + "-" * (bar_length - filled)

    elapsed = time.time() - start_time
    eta = (elapsed / current * (total - current)) if current > 0 else 0

    sys.stdout.write(
        f"\r[{bar}] {current}/{total} ({fraction*100:5.1f}%)  ETA: {int(eta)}s"
    )
    sys.stdout.flush()

# -----------------------------------------
# Copy function
# -----------------------------------------
def copy_file(remote_url, local_dir, filename):
    remote_file_url = remote_url + "/" + filename
    local_file_path = os.path.join(local_dir, filename)

    cmd = ["gfal-copy", remote_file_url, local_file_path]
    result = subprocess.run(
        cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True
    )

    return filename, result.returncode, result.stderr

# -----------------------------------------
# Main script
# -----------------------------------------
def main():

    # ----------------------------
    # 1. User Inputs
    # ----------------------------
    year = input("Please enter YEAR: ").strip()
    datadir = input("Please enter DATADIR (rawdata/mmdata/tmmdata): ").strip().lower()
    run_short = input("Please enter RUN NUMBER (nnnnn): ").strip()

    if datadir not in ["rawdata", "mmdata", "tmmdata"]:
        print(f"{RED}ERROR:{RESET} datadir must be rawdata, mmdata or tmmdata.")
        sys.exit(1)

    remote_base = (
        "srm://atlasse.lnf.infn.it:8446/srm/managerv2"
        "?SFN=/dpm/lnf.infn.it/home/vo.padme.org"
    )
    remote_dir = f"/daq/{year}/{datadir}"
    remote_url = remote_base + remote_dir

    run_prefix = f"run_00{run_short}_"

    print("\nListing remote directory to find full run folder...")
    try:
        dirs_listing = subprocess.run(
            ["gfal-ls", remote_url],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            universal_newlines=True
        )
    except FileNotFoundError:
        print(f"{RED}ERROR: gfal-ls command not found.{RESET}")
        sys.exit(1)

    if dirs_listing.returncode != 0:
        print(f"{RED}ERROR:{RESET} Cannot read remote directory:")
        print(dirs_listing.stderr)
        sys.exit(1)

    # Find full run directory
    run_dirs = [d for d in dirs_listing.stdout.splitlines() if d.startswith(run_prefix)]
    if not run_dirs:
        print(f"{RED}No directory found for run prefix {run_prefix}{RESET}")
        sys.exit(1)

    full_run_dir = run_dirs[0]  # take the first match
    remote_run_url = remote_url + "/" + full_run_dir
    print(f"\nFound full run directory: {full_run_dir}")

    # ----------------------------
    # 2. List files inside run directory
    # ----------------------------
    files_listing = subprocess.run(
        ["gfal-ls", remote_run_url],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        universal_newlines=True
    )

    if files_listing.returncode != 0:
        print(f"{RED}ERROR:{RESET} Cannot read run directory:")
        print(files_listing.stderr)
        sys.exit(1)

    files_all = files_listing.stdout.splitlines()
    if len(files_all) == 0:
        print("No files found. Exiting.")
        sys.exit(0)

    # Choose sorting depending on datadir
    if datadir == "rawdata":
        files_sorted = sorted(files_all, key=file_sort_key)
    else:  # mmdata or tmmdata
        files_sorted = sorted(files_all, key=mm_sort_key)

    print(f"Number of files in this run: {len(files_sorted)}")

    # ----------------------------
    # 3. Let user choose how many files to copy
    # ----------------------------
    while True:
        howmany = input(
            f"\nHow many files do you want to copy? (1-{len(files_sorted)} or 'all'): "
        ).strip().lower()

        if howmany in ("all", "a", ""):
            files = files_sorted
            break

        try:
            n = int(howmany)
            if 1 <= n <= len(files_sorted):
                files = files_sorted[:n]
                break
            else:
                print(f"Please enter a number between 1 and {len(files_sorted)}, or 'all'.")
        except ValueError:
            print("Please enter a valid integer or 'all'.")

    print(f"\nSelected {len(files)} files to copy.")
    print("First files in the copy order:")
    for f in files[:10]:
        print("  ", f)
    if len(files) > 10:
        print("  ...")

    confirm = input(f"\nProceed and copy these {len(files)} files? (yes/no): ").strip().lower()
    if confirm != "yes":
        print("Aborted by user.")
        sys.exit(0)

    # ----------------------------
    # 4. Local directory
    # ----------------------------
    local_dir = f"/data9Vd1/padme/mancinima/{datadir}/{year}/{full_run_dir}/"
    local_confirm = input(
        f"\nDo you want me to copy all files into {local_dir}? (yes/no):"
    ).strip().lower()

    if local_confirm != "yes":
        print("Aborted by user.")
        sys.exit(0)

    os.makedirs(local_dir, exist_ok=True)
    print(f"\nLocal directory: {local_dir}")

    # ----------------------------
    # 5. Parallel copy?
    # ----------------------------
    use_parallel = input(
        "\nDo you want to use parallel copy with multiprocessing? (yes/no): "
    ).strip().lower()

    num_cores = 1
    if use_parallel == "yes":
        while True:
            try:
                num_cores = int(input("How many cores do you want to use (1-30)? "))
                if 1 <= num_cores <= 30:
                    break
                else:
                    print("Please enter a number between 1 and 30.")
            except ValueError:
                print("Please enter a valid integer.")

    # ----------------------------
    # 6. Copy files with ASCII progress bar
    # ----------------------------
    print("\nStarting copy...\n")
    copy_func = partial(copy_file, remote_run_url, local_dir)
    results = []

    start = time.time()

    if use_parallel == "yes":
        with Pool(num_cores) as pool:
            for i, res in enumerate(pool.imap_unordered(copy_func, files), 1):
                results.append(res)
                ascii_progress(i, len(files), start)
    else:
        for i, f in enumerate(files, 1):
            results.append(copy_func(f))
            ascii_progress(i, len(files), start)

    print("\n")  # newline after progress bar

    # ----------------------------
    # 7. Final summary
    # ----------------------------
    failed = [r for r in results if r[1] != 0]
    ok = len(results) - len(failed)

    print("\n--------------------------------------------")
    print(f"{GREEN} ✓ OK:   {ok}{RESET}")
    print(f"{RED} ✗ FAIL: {len(failed)}{RESET}")

    if len(failed) == 0:
        print(f"\n{GREEN}ALL DONE — EVERYTHING IS OK!{RESET}")
    else:
        print(f"\n{RED}Completed with problems — some files failed.{RESET}")
        for fname, _, err in failed[:10]:  # show first 10 failed files
            print(f" - {fname}  ->  {RED}{err.splitlines()[0]}{RESET}")
        if len(failed) > 10:
            print(f"... and {len(failed)-10} more failures.")
    print("--------------------------------------------\n")


if __name__ == "__main__":
    main()
