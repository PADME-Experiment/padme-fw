#!/usr/bin/env python3

from pathlib import Path
import os
import re
import sys

import ROOT


def extract_run_name(argument: str) -> str | None:
    """
    Accept either:

      122_20260721162033

    or:

      New_Events_run122_20260721162033.root
    """

    file_name = Path(argument).name

    run_pattern = re.compile(r"^(\d+_\d{14})$")
    file_pattern = re.compile(r"^New_Events_run(\d+_\d{14})\.root$")

    match = run_pattern.match(file_name)

    if match:
        return match.group(1)

    match = file_pattern.match(file_name)

    if match:
        return match.group(1)

    return None


def main() -> int:
    if len(sys.argv) != 2:
        print(
            f"Usage:\n"
            f"  {Path(sys.argv[0]).name} RUN_NAME\n\n"
            f"or:\n"
            f"  {Path(sys.argv[0]).name} "
            f"New_Events_runRUN_NAME.root\n\n"
            f"Example:\n"
            f"  {Path(sys.argv[0]).name} "
            f"122_20260721162033"
        )
        return 1

    run_name = extract_run_name(sys.argv[1])

    if run_name is None:
        print(
            "Error: invalid run name.\n"
            "Expected format:\n"
            "  122_20260721162033\n"
            "or:\n"
            "  New_Events_run122_20260721162033.root"
        )
        return 1

    working_directory = Path(__file__).resolve().parent

    source_file = working_directory / "events.C"
    header_file = working_directory / "events.h"

    input_file = (working_directory / f"New_Events_run{run_name}.root")
    output_file = (working_directory / f"Reco_New_Events_run{run_name}.root")

    if not source_file.exists():
        print(f"Error: file not found: {source_file}")
        return 1

    if not header_file.exists():
        print(f"Error: file not found: {header_file}")
        return 1

    if not input_file.exists():
        print(f"Error: input file not found: {input_file}")
        return 1

    # ROOT and the C++ code use relative file paths.
    os.chdir(working_directory)

    ROOT.gROOT.SetBatch(True)

    print(f"Run:         {run_name}")
    print(f"Input file:  {input_file.name}")
    print(f"Output file: {output_file.name}")
    print()
    print(f"Compiling {source_file.name}...")

    compilation_result = ROOT.gSystem.CompileMacro(str(source_file),"k")
    if compilation_result == 0:
        print(f"Error while compiling {source_file.name}")
        return 1

    if not hasattr(ROOT, "events"):
        print("Error: events class was not loaded by ROOT.")
        return 1

    print("Compilation completed.")
    print()
    print(f"Processing run {run_name}...")

    try:
        analysis = ROOT.events(ROOT.TString(run_name))

        if analysis.fChain is None:
            print(
                "Error: cannot open the 'events' tree "
                f"from {input_file.name}"
            )
            del analysis
            return 1

        analysis.Loop()
        del analysis

    except Exception as error:
        print(f"Analysis failed: {error}")
        return 1

    if not output_file.exists():
        print()
        print(
            "Error: expected output file was not created:"
        )
        print(f"  {output_file}")
        return 1

    print()
    print("Analysis completed successfully.")
    print(f"Created: {output_file.name}")

    return 0


if __name__ == "__main__":
    sys.exit(main())