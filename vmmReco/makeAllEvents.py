#!/usr/bin/env python3

from pathlib import Path
import os
import re
import subprocess
import sys

import ROOT


# Empty list: process all available runs.
RUNS = []

# Skip runs whose output file already exists in EventsFiles.
SKIP_EXISTING_OUTPUT = True

# Internal argument used to process one run in a separate process.
SINGLE_RUN_OPTION = "--single-run"


def find_available_runs(directory: Path) -> list[str]:
    pattern = re.compile(r"^run(\d+_\d{14})\.root$")

    runs = []

    for input_file in directory.glob("run*.root"):
        match = pattern.match(input_file.name)

        if match:
            runs.append(match.group(1))

    return sorted(runs)


def process_single_run(
    run_name: str,
    working_directory: Path,
    input_directory: Path,
    output_directory: Path,
    source_file: Path,
) -> int:
    input_file = input_directory / f"run{run_name}.root"

    # Temporary symbolic link:
    #
    # EventsFiles/runXXX.root -> VMMData/runXXX.root
    linked_input_file = output_directory / f"run{run_name}.root"

    output_file = output_directory / f"Events_run{run_name}.root"

    if not input_file.exists():
        print(f"Error: input file not found: {input_file}")
        return 1

    output_directory.mkdir(parents=True, exist_ok=True)

    os.chdir(working_directory)

    ROOT.gROOT.SetBatch(True)

    print(f"Compiling {source_file.name}...")

    compilation_result = ROOT.gSystem.CompileMacro(
        str(source_file),
        "k"
    )

    if compilation_result == 0:
        print(f"Error while compiling {source_file.name}")
        return 1

    if not hasattr(ROOT, "VMMAnalysis"):
        print("Error: VMMAnalysis class was not loaded by ROOT.")
        return 1

    try:
        if linked_input_file.is_symlink():
            linked_input_file.unlink()
        elif linked_input_file.exists():
            print(
                "Error: a real file already exists where the temporary "
                "input link should be:"
            )
            print(f"  {linked_input_file}")
            return 1

        linked_input_file.symlink_to(input_file.resolve())

        # VMMAnalysis sees the input link and creates the output directly
        # inside EventsFiles.
        os.chdir(output_directory)

        analysis = ROOT.VMMAnalysis(ROOT.TString(run_name))

        if analysis.fChain is None:
            print(
                f"Error: cannot open tree from {input_file.name}"
            )
            del analysis
            return 1

        analysis.Loop()
        del analysis

        if not output_file.exists():
            print("Error: expected output file was not created:")
            print(f"  {output_file}")
            return 1

        print(f"Created: {output_file}")
        return 0

    except Exception as error:
        print(f"Analysis failed: {error}")
        return 1

    finally:
        os.chdir(working_directory)

        if linked_input_file.is_symlink():
            linked_input_file.unlink()


def main() -> int:
    working_directory = Path(__file__).resolve().parent

    input_directory = working_directory / "VMMData"
    output_directory = working_directory / "EventsFiles"

    source_file = working_directory / "VMMAnalysis.C"
    header_file = working_directory / "VMMAnalysis.h"

    if not source_file.exists():
        print(f"Error: file not found: {source_file}")
        return 1

    if not header_file.exists():
        print(f"Error: file not found: {header_file}")
        return 1

    if not input_directory.exists():
        print(f"Error: input directory not found: {input_directory}")
        return 1

    if not input_directory.is_dir():
        print(f"Error: input path is not a directory: {input_directory}")
        return 1

    output_directory.mkdir(parents=True, exist_ok=True)

    # Child-process mode: process exactly one run.
    if len(sys.argv) == 3 and sys.argv[1] == SINGLE_RUN_OPTION:
        return process_single_run(
            run_name=sys.argv[2],
            working_directory=working_directory,
            input_directory=input_directory,
            output_directory=output_directory,
            source_file=source_file,
        )

    if len(sys.argv) != 1:
        print(f"Usage: {Path(sys.argv[0]).name}")
        return 1

    if RUNS:
        runs_to_process = RUNS
    else:
        runs_to_process = find_available_runs(input_directory)

    print(f"Input directory:  {input_directory}")
    print(f"Output directory: {output_directory}")

    if not runs_to_process:
        print(
            "No files matching "
            "runNNN_YYYYMMDDHHMMSS.root "
            f"were found in {input_directory}."
        )
        return 0

    print(f"Found {len(runs_to_process)} runs.")

    processed = 0
    skipped = 0
    failed = 0

    script_file = Path(__file__).resolve()

    for index, run_name in enumerate(runs_to_process, start=1):
        input_file = input_directory / f"run{run_name}.root"
        output_file = output_directory / f"Events_run{run_name}.root"

        print()
        print(f"[{index}/{len(runs_to_process)}] Run {run_name}")
        print(f"  Input:  {input_file}")
        print(f"  Output: {output_file}")

        if not input_file.exists():
            print(f"  Missing input file: {input_file}")
            failed += 1
            continue

        if SKIP_EXISTING_OUTPUT and output_file.exists():
            print(f"  Output already exists: {output_file}")
            print("  Skipping.")
            skipped += 1
            continue

        # Run every analysis in a fresh Python/ROOT process.
        result = subprocess.run(
            [
                sys.executable,
                str(script_file),
                SINGLE_RUN_OPTION,
                run_name,
            ],
            cwd=working_directory,
        )

        if result.returncode == 0 and output_file.exists():
            processed += 1
        else:
            if result.returncode < 0:
                signal_number = -result.returncode
                print(
                    f"  Analysis process was killed by signal "
                    f"{signal_number}."
                )
            else:
                print(
                    f"  Analysis failed with exit code "
                    f"{result.returncode}."
                )

            failed += 1

    print()
    print("Analysis completed")
    print(f"  Processed: {processed}")
    print(f"  Skipped:   {skipped}")
    print(f"  Failed:    {failed}")

    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())