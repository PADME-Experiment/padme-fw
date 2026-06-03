#!/usr/bin/env python3
import subprocess
import sys

# --- EDIT HERE: your (run, energy) pairs ---
runs = [
    # (80344, 293.5),
    (80357, 292.0),
    (80415, 283.0),
    # (80427, 281.5),
    # (80459, 271.0),
    # (80472, 271.0),
    (80534, 294.25),
    # # (80581, 289.75),
    # (80615, 285.25),
    # # (80632, 280.75),
    # (80649, 277.75),
    # (80651, 277.75),
    # (80677, 270.25),
]

def run_cmd(cmd):
    print("\n>>> " + " ".join(map(str, cmd)))
    subprocess.run(cmd, check=True)

def main():
    # 1) Compile once
    compile_cmd = [
        "g++", "-std=c++11", "-Wall", "-O2",
        "TargetAnalysis.cpp", "-o", "TargetAnalysis",
    ]

    # Python 3.6: use universal_newlines=True (instead of text=True)
    root_flags = subprocess.check_output(
        ["root-config", "--cflags", "--libs"],
        universal_newlines=True
    ).split()

    compile_cmd += root_flags + ["-lMinuit"]
    run_cmd(compile_cmd)

    # 2) Execute for all pairs
    for run, energy in runs:
        run_cmd(["./TargetAnalysis", str(run), str(energy)])

    print("\nDone.")

if __name__ == "__main__":
    try:
        main()
    except subprocess.CalledProcessError as e:
        print("\nERROR: command failed with exit code {}".format(e.returncode), file=sys.stderr)
        sys.exit(e.returncode)