#!/usr/bin/env python3
import subprocess
import sys
from pathlib import Path

# ============================================
# Usage examples:
#   python run_target_calib.py file1.root file2.root file3.root
#   python run_target_calib.py *.root
# ============================================

def run_macro(root_file):
    cmd = [
        "root",
        "-l",
        "-b",
        "-q",
        f'TargetCalib.cpp+("{root_file}")'
    ]

    print(f"\n --> Processing: {root_file}")
    subprocess.run(cmd)


def main():
    if len(sys.argv) < 2:
        print("Usage: python run_target_calib.py file1.root file2.root ...")
        sys.exit(1)

    files = sys.argv[1:]

    for f in files:
        if not Path(f).exists():
            print(f"File not found: {f}")
            continue
        run_macro(f)


if __name__ == "__main__":
    main()
