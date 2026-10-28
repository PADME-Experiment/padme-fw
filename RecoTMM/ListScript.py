#!/usr/bin/env python3
"""Create the input ROOT-file lists expected by RunReco.py for all runs in RunIV_Times.txt.

Only column 0 (RunID) of the timing database is read. No time values or detId
fields are used here. The resulting lists are ordered naturally so that raw
file fragments numbered 1, 2, ..., 10 are not sorted as 1, 10, 2.
"""

import argparse
import glob
import os
import re
import sys
from pathlib import Path

# Defaults match the existing Run IV reconstruction launcher.
TIMES_FILE = Path("/home/mancinima/BeamMonitorRun4/padme-fw/RunIV_Times.txt")
BASE_DIR = Path("/data9Vd1/padme/leonardi/padme/daq/2025/tmmdata")
LIST_DIR = Path("/home/mancinima/BeamMonitorRun4/padme-fw/RecoTMM/TMMFileList")


def natural_key(path):
    """Sort numbered file fragments in numeric, not lexicographic, order."""
    return [int(part) if part.isdigit() else part.lower() for part in re.split(r"(\d+)", str(path))]


def read_run_ids(times_file):
    """Read unique RunIDs from the FIRST column; ignore every other field."""
    if not times_file.is_file():
        raise FileNotFoundError("Timing database not found: {}".format(times_file))

    runs = []
    seen = set()
    with times_file.open("r", encoding="utf-8-sig") as handle:
        for line_number, line in enumerate(handle, 1):
            stripped = line.strip()
            if not stripped or stripped.startswith("#"):
                continue
            first_column = stripped.split()[0]
            if first_column.lower() in ("runid", "run_id"):
                continue
            if not first_column.isdigit() or int(first_column) <= 0:
                raise ValueError("{}:{}: invalid RunID in first column: {!r}".format(times_file, line_number, first_column))
            run_id = int(first_column)
            if run_id in seen:
                print("[DUPLICATE] run {:07d} repeated at line {}; processing only once".format(run_id, line_number), flush=True)
                continue
            seen.add(run_id)
            runs.append(run_id)

    if not runs:
        raise ValueError("No RunIDs found in {}".format(times_file))
    return runs


def find_run_directory(base_dir, run_id):
    """Require exactly one run_0080357_* directory for RunID 0080357."""
    pattern = str(base_dir / "run_{:07d}_*".format(run_id))
    matches = sorted((Path(path) for path in glob.glob(pattern) if Path(path).is_dir()), key=natural_key)
    if len(matches) == 0:
        raise FileNotFoundError("No directory matches {}".format(pattern))
    if len(matches) > 1:
        raise ValueError("Multiple directories match {}: {}".format(pattern, ", ".join(str(p) for p in matches)))
    return matches[0]


def find_root_files(directory):
    """Collect only ROOT files, recursively, in natural chronological order."""
    files = []
    for parent, subdirs, filenames in os.walk(directory):
        subdirs.sort(key=natural_key)
        for filename in filenames:
            if filename.lower().endswith(".root"):
                full_path = Path(parent) / filename
                if full_path.is_file():
                    files.append(full_path)
    return sorted(files, key=lambda path: natural_key(path.relative_to(directory)))


def parse_args():
    parser = argparse.ArgumentParser(description="Write TMMRaw_run_XXXXXXX.list for every run in the timing database")
    parser.add_argument("--times-file", type=Path, default=TIMES_FILE, help="Database containing RunID in column 0")
    parser.add_argument("--base-dir", type=Path, default=BASE_DIR, help="Directory containing run_XXXXXXX_* raw-data folders")
    parser.add_argument("--list-dir", type=Path, default=LIST_DIR, help="Destination directory for .list files")
    parser.add_argument("--dry-run", action="store_true", help="Discover files and report results without writing any lists")
    parser.add_argument("--skip-existing", action="store_true", help="Keep already existing .list files unchanged")
    return parser.parse_args()


def main():
    args = parse_args()
    try:
        runs = read_run_ids(args.times_file)
        if not args.base_dir.is_dir():
            raise FileNotFoundError("Raw-data directory not found: {}".format(args.base_dir))
        if not args.dry_run:
            args.list_dir.mkdir(parents=True, exist_ok=True)
    except (ValueError, OSError) as exc:
        print("ERROR: {}".format(exc), file=sys.stderr)
        return 1

    print("Timing database : {}".format(args.times_file), flush=True)
    print("Raw data        : {}".format(args.base_dir), flush=True)
    print("List directory  : {}".format(args.list_dir), flush=True)
    print("Unique RunIDs   : {}".format(len(runs)), flush=True)

    written = 0
    skipped_existing = 0
    failed = []
    total_root_files = 0

    for index, run_id in enumerate(runs, 1):
        list_path = args.list_dir / "TMMRaw_run_{:07d}.list".format(run_id)
        if args.skip_existing and list_path.is_file():
            skipped_existing += 1
            print("[{}/{}] [SKIP EXISTING] {}".format(index, len(runs), list_path.name), flush=True)
            continue
        try:
            directory = find_run_directory(args.base_dir, run_id)
            root_files = find_root_files(directory)
            if not root_files:
                raise ValueError("No .root files found in {}".format(directory))
            total_root_files += len(root_files)
            if not args.dry_run:
                temporary_path = list_path.with_name(list_path.name + ".tmp")
                try:
                    with temporary_path.open("w", encoding="utf-8") as handle:
                        for root_file in root_files:
                            handle.write(str(root_file) + "\n")
                    temporary_path.replace(list_path)
                finally:
                    if temporary_path.exists():
                        temporary_path.unlink()
            written += 1
            verb = "CHECK" if args.dry_run else "OK"
            print("[{}/{}] [{}] run {:07d}: {} ROOT files -> {}".format(index, len(runs), verb, run_id, len(root_files), list_path.name), flush=True)
        except (OSError, ValueError) as exc:
            failed.append((run_id, str(exc)))
            print("[{}/{}] [MISSING] run {:07d}: {}".format(index, len(runs), run_id, exc), flush=True)

    print("\n=== Summary ===")
    print("Database runs         : {}".format(len(runs)))
    print("{} lists   : {}".format("Discovered" if args.dry_run else "Written", written))
    print("Already present       : {}".format(skipped_existing))
    print("Total ROOT file paths : {}".format(total_root_files))
    print("Failed/missing runs   : {}".format(len(failed)))
    for run_id, reason in failed:
        print("  {:07d}: {}".format(run_id, reason))

    return 1 if failed else 0


if __name__ == "__main__":
    raise SystemExit(main())