#!/usr/bin/env python3

from pathlib import Path
import os
import re
import shutil
import sys

import ROOT


# Empty list: process all available runs.
#
# Example:
# RUNS = [
#     "122_20260721162033",
#     "123_20260721163045",
# ]
RUNS = []

# Skip runs whose output file already exists in RecoFiles.
SKIP_EXISTING_OUTPUT = True


def find_available_runs(directory: Path) -> list[str]:
    pattern = re.compile(
        r"^Events_run(\d+_\d{14})\.root$"
    )

    runs = []

    for input_file in directory.glob("Events_run*.root"):
        match = pattern.match(input_file.name)

        if match:
            runs.append(match.group(1))

    return sorted(runs)


def main() -> int:
    working_directory = Path(__file__).resolve().parent

    input_directory = working_directory / "EventsFiles"
    output_directory = working_directory / "RecoFiles"

    source_file = working_directory / "RecoEvents.C"
    header_file = working_directory / "RecoEvents.h"

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

    # Create RecoFiles if it does not already exist.
    output_directory.mkdir(parents=True, exist_ok=True)

    # Compile RecoEvents.C from the script directory.
    os.chdir(working_directory)

    ROOT.gROOT.SetBatch(True)

    print(f"Input directory:  {input_directory}")
    print(f"Output directory: {output_directory}")
    print(f"Compiling {source_file.name}...")

    compilation_result = ROOT.gSystem.CompileMacro(
        str(source_file),
        "k"
    )

    if compilation_result == 0:
        print(f"Error while compiling {source_file.name}")
        return 1

    if not hasattr(ROOT, "RecoEvents"):
        print("Error: RecoEvents class was not loaded by ROOT.")
        return 1

    print("Compilation completed.")

    if RUNS:
        runs_to_process = RUNS
    else:
        runs_to_process = find_available_runs(input_directory)

    if not runs_to_process:
        print(
            "No files matching "
            "Events_runNNN_YYYYMMDDHHMMSS.root "
            f"were found in {input_directory}."
        )
        return 0

    print(f"Found {len(runs_to_process)} runs.")

    processed = 0
    skipped = 0
    failed = 0

    for index, run_name in enumerate(runs_to_process, start=1):
        input_file = (
            input_directory
            / f"Events_run{run_name}.root"
        )

        # RecoEvents.C is expected to initially create this output
        # inside EventsFiles because it uses relative paths.
        temporary_output_file = (
            input_directory
            / f"V1_Reco_Events_run{run_name}.root"
        )

        final_output_file = (
            output_directory
            / f"V1_Reco_Events_run{run_name}.root"
        )

        print()
        print(f"[{index}/{len(runs_to_process)}] Run {run_name}")
        print(f"  Input:  {input_file}")
        print(f"  Output: {final_output_file}")

        if not input_file.exists():
            print(f"  Missing input file: {input_file}")
            failed += 1
            continue

        if SKIP_EXISTING_OUTPUT and final_output_file.exists():
            print(f"  Output already exists: {final_output_file}")
            print("  Skipping.")
            skipped += 1
            continue

        # Remove a leftover temporary output from a previous
        # interrupted or failed execution.
        if temporary_output_file.exists():
            print(
                "  Removing leftover temporary output: "
                f"{temporary_output_file.name}"
            )
            temporary_output_file.unlink()

        try:
            # events.C expects the input file in the current directory.
            os.chdir(input_directory)

            analysis = ROOT.RecoEvents(ROOT.TString(run_name))

            if analysis.fChain is None:
                print(
                    "  Error: cannot open the 'events' tree "
                    f"from {input_file.name}"
                )
                failed += 1
                del analysis
                continue

            analysis.Loop()
            del analysis

            if not temporary_output_file.exists():
                print(
                    "  Error: expected output file was not created:"
                )
                print(f"    {temporary_output_file}")
                failed += 1
                continue

            # Replace an existing output when skipping is disabled.
            if final_output_file.exists():
                final_output_file.unlink()

            shutil.move(
                str(temporary_output_file),
                str(final_output_file)
            )

            print(f"  Created: {final_output_file}")
            processed += 1

        except Exception as error:
            print(f"  Analysis failed: {error}")
            failed += 1

        finally:
            os.chdir(working_directory)

    print()
    print("Analysis completed")
    print(f"  Processed: {processed}")
    print(f"  Skipped:   {skipped}")
    print(f"  Failed:    {failed}")

    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())