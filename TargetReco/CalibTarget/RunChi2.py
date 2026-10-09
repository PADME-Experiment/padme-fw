#!/usr/bin/env python3

import argparse
import subprocess
import sys
from pathlib import Path


# -----------------------------------------------------------------------------
# Runs to analyse
# -----------------------------------------------------------------------------
#
# Keep the same convenient workflow as the old RunChi2.py:
# edit this list with (run, beam energy in MeV) pairs, then launch the script.
#
RUNS = [
    # (80344, 293.5),
    # (80357, 292.0),
    (80415, 283.0),
    (80426, 281.5),
    # (80427, 281.5),
    # (80459, 271.0),
    # (80472, 271.0),
    (80534, 294.25),
    # (80581, 289.75),
    # (80615, 285.25),
    # (80632, 280.75),
    # (80649, 277.75),
    # (80651, 277.75),
    # (80677, 270.25),
]


SCRIPT_DIR = Path(__file__).resolve().parent

SOURCE = SCRIPT_DIR / "ChannelChi2.cpp"
EXECUTABLE = SCRIPT_DIR / "ChannelChi2"


def find_target_reco_root() -> Path:
    """
    Find the TargetReco directory by walking upward from this script.

    The expected project root contains outputReco/ and/or CalibTarget/.
    This avoids hard-coding /home/<user>/... paths.
    """
    candidates = [SCRIPT_DIR] + list(SCRIPT_DIR.parents)

    for path in candidates:
        if (path / "outputReco").exists() and (path / "CalibTarget").exists():
            return path

    for path in candidates:
        if (path / "outputReco").exists():
            return path

    raise RuntimeError(
        "Cannot locate the TargetReco project root automatically. "
        "Use --project-root."
    )


def parse_arguments():
    parser = argparse.ArgumentParser(
        description=(
            "Compile ChannelChi2.cpp once and analyse the configured "
            "(run, beam-energy) pairs."
        )
    )

    parser.add_argument(
        "--project-root",
        type=Path,
        default=None,
        help=(
            "Path to TargetReco. If omitted, the script searches its "
            "parent directories."
        ),
    )

    parser.add_argument(
        "--reco-dir",
        type=Path,
        default=None,
        help=(
            "Directory containing the reconstruction ROOT files. "
            "Default: <project-root>/outputReco"
        ),
    )

    parser.add_argument(
        "--calibration-file",
        type=Path,
        default=None,
        help=(
            "X-strip calibration file. Default: "
            "<project-root>/CalibTarget/outputCalibration/"
            "TargetCalibrationConst_NewCharge2.txt"
        ),
    )

    parser.add_argument(
        "--output-dir",
        type=Path,
        default=None,
        help=(
            "Analysis output directory. Default: "
            "<project-root>/CalibTarget/outputCalibration/Run4Monitor"
        ),
    )

    parser.add_argument(
        "--input-template",
        # default="Reco_run_00{run}.root",
        default="Reco2Tree_run_00{run}.root",
        help=(
            "Input filename template relative to --reco-dir. "
            "It must contain {run}. "
            "Example for temporary files: 'PROVA_00{run}.root'"
        ),
    )

    parser.add_argument(
        "--output-tag",
        default="v2",
        help=(
            "Tag appended to output filenames. Default: v2"
        ),
    )

    parser.add_argument(
        "--no-compile",
        action="store_true",
        help="Use an existing ChannelChi2 executable instead of compiling.",
    )

    return parser.parse_args()


def run_cmd(cmd, cwd=None):
    print("\n>>> " + " ".join(str(x) for x in cmd))

    subprocess.run(
        [str(x) for x in cmd],
        cwd=str(cwd) if cwd is not None else None,
        check=True,
    )


def compile_analyser():
    print("Compiling ChannelChi2.cpp ...")

    if not SOURCE.is_file():
        raise FileNotFoundError(
            f"Source file not found: {SOURCE}"
        )

    try:
        root_flags = subprocess.check_output(
            ["root-config", "--cflags", "--libs"],
            universal_newlines=True,
        ).split()
    except (FileNotFoundError, subprocess.CalledProcessError) as exc:
        raise RuntimeError(
            "Cannot execute root-config. "
            "Check that the ROOT environment is loaded."
        ) from exc

    compile_cmd = [
        "g++",
        "-std=c++17",
        "-Wall",
        "-Wextra",
        "-O2",
        str(SOURCE),
        "-o",
        str(EXECUTABLE),
    ]

    compile_cmd += root_flags
    compile_cmd += ["-lMinuit"]

    run_cmd(
        compile_cmd,
        cwd=SCRIPT_DIR,
    )

    if not EXECUTABLE.is_file():
        raise RuntimeError(
            f"Compilation succeeded but executable was not created: "
            f"{EXECUTABLE}"
        )

    print(f"Compilation successful: {EXECUTABLE}")


def main():
    args = parse_arguments()

    project_root = (
        args.project_root.resolve()
        if args.project_root is not None
        else find_target_reco_root().resolve()
    )

    reco_dir = (
        args.reco_dir.resolve()
        if args.reco_dir is not None
        else project_root / "outputReco"
    )

    calibration_file = (
        args.calibration_file.resolve()
        if args.calibration_file is not None
        else (
            project_root
            / "CalibTarget"
            / "outputCalibration"
            / "TargetCalibrationConst_NewCharge2.txt"
        )
    )

    output_dir = (
        args.output_dir.resolve()
        if args.output_dir is not None
        else (
            project_root
            / "CalibTarget"
            / "outputCalibration"
            / "Run4Monitor"
        )
    )

    if "{run}" not in args.input_template:
        raise ValueError(
            "--input-template must contain '{run}'"
        )

    print(f"Project root     : {project_root}")
    print(f"Reco directory  : {reco_dir}")
    print(f"Calibration file: {calibration_file}")
    print(f"Output directory: {output_dir}")
    print(f"Input template  : {args.input_template}")

    if not reco_dir.is_dir():
        raise FileNotFoundError(
            f"Reconstruction directory not found: {reco_dir}"
        )

    if not calibration_file.is_file():
        raise FileNotFoundError(
            f"Calibration file not found: {calibration_file}"
        )

    output_dir.mkdir(
        parents=True,
        exist_ok=True,
    )

    if not args.no_compile:
        compile_analyser()
    elif not EXECUTABLE.is_file():
        raise FileNotFoundError(
            f"--no-compile was requested but executable is missing: "
            f"{EXECUTABLE}"
        )

    failures = []

    for run, energy in RUNS:

        input_file = (
            reco_dir
            / args.input_template.format(run=run)
        )

        output_root = (
            output_dir
            / f"TargetAnalysis_run_00{run}_{args.output_tag}.root"
        )

        output_txt = (
            output_dir
            / f"TargetAnalysis_run_00{run}_{args.output_tag}.txt"
        )

        if not input_file.is_file():
            message = (
                f"input reconstruction file not found: {input_file}"
            )

            print(
                f"\n[FAIL] run {run}: {message}",
                file=sys.stderr,
            )

            failures.append(
                (run, message)
            )

            continue

        cmd = [
            EXECUTABLE,
            str(run),
            str(energy),
            input_file,
            calibration_file,
            output_root,
            output_txt,
        ]

        try:
            run_cmd(
                cmd,
                cwd=SCRIPT_DIR,
            )

            print(
                f"[ OK ] run {run}: {output_root}"
            )

        except subprocess.CalledProcessError as exc:
            message = (
                f"ChannelChi2 exited with code {exc.returncode}"
            )

            print(
                f"[FAIL] run {run}: {message}",
                file=sys.stderr,
            )

            failures.append(
                (run, message)
            )

    print("\n=== Analysis summary ===")
    print(f"Requested runs: {len(RUNS)}")
    print(f"Successful    : {len(RUNS) - len(failures)}")
    print(f"Failed        : {len(failures)}")

    if failures:
        print("\nFailed runs:")
        for run, message in failures:
            print(f"  {run}: {message}")

        return 1

    print("\nDone.")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())

    except (
        FileNotFoundError,
        RuntimeError,
        ValueError,
    ) as exc:
        print(
            f"ERROR: {exc}",
            file=sys.stderr,
        )
        raise SystemExit(1)
