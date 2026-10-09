# #!/usr/bin/env python3

# import argparse
# import os
# import subprocess
# import sys
# import time
# import shlex
# from dataclasses import dataclass
# from pathlib import Path
# from typing import List, Optional, Tuple


# SCRIPT_DIR = Path(__file__).resolve().parent

# SOURCE = SCRIPT_DIR / "RecoTarget.cpp"
# EXECUTABLE = SCRIPT_DIR / "RecoTarget.exe"

# LIST_DIR = SCRIPT_DIR / "ListDir"
# OUTPUT_DIR = SCRIPT_DIR / "outputReco"
# LOG_DIR = OUTPUT_DIR / "logs"

# def compile_reco():
#     print("Compiling RecoTarget.cpp ...")

#     # Get the ROOT compiler/linker flags exactly as root-config provides them.
#     try:
#         root_cflags = subprocess.check_output(["root-config", "--cflags"], text=True).strip()

#         root_libs = subprocess.check_output(["root-config", "--libs"], text=True).strip()

#     except (FileNotFoundError, subprocess.CalledProcessError) as exc:
#         raise RuntimeError("Cannot execute root-config. Check that the ROOT environment is loaded.") from exc

#     cmd = [
#         "g++",
#         "-std=c++17",
#         "-O2",
#         "-o",
#         str(EXECUTABLE),
#         str(SOURCE),
#         "-I",
#         str(SCRIPT_DIR / "../PadmeRoot/include"),
#         "-L",
#         str(SCRIPT_DIR / "../PadmeRoot/lib"),
#         "-lPadmeRoot",
#     ]

#     # Convert strings such as:
#     #   -pthread -I/.../root/include
#     # into individual command-line arguments.
#     cmd += shlex.split(root_cflags)
#     cmd += shlex.split(root_libs)

#     print("Compilation command:")
#     print(" ".join(cmd))

#     result = subprocess.run(cmd, cwd=SCRIPT_DIR)

#     if result.returncode != 0:
#         raise RuntimeError("Compilation of RecoTarget.cpp failed")

#     if not EXECUTABLE.is_file():
#         raise RuntimeError(f"Compilation finished but executable was not created: {EXECUTABLE}")

#     print(f"Compilation successful: {EXECUTABLE}")


# @dataclass
# class RunningJob:
#     run_id: str
#     core: Optional[int]
#     process: subprocess.Popen
#     log_handle: object
#     output_file: Path
#     log_file: Path


# def get_list_file(run_id: str) -> Path:
#     return LIST_DIR / f"RawMerged_run_00{run_id}.list"


# def get_output_file(run_id: str) -> Path:
#     return OUTPUT_DIR / f"PROVA_00{run_id}.root"


# def get_log_file(run_id: str) -> Path:
#     return LOG_DIR / f"run_{run_id}.log"


# def taskset_available() -> bool:
#     try:
#         subprocess.run( ["taskset", "--version"], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, check=True)
#         return True
#     except (FileNotFoundError, subprocess.CalledProcessError):
#         return False


# def parse_arguments():
#     parser = argparse.ArgumentParser(description=("Queued launcher for RecoTarget.exe. At most --jobs reconstructions run concurrently."))
#     parser.add_argument("runs", nargs="*", help="Run numbers, e.g. 90032 90033")
#     parser.add_argument("-j", "--jobs", type=int, default=None, help="Maximum number of concurrent jobs")
#     parser.add_argument("--no-taskset", action="store_true", help="Do not pin jobs to CPU cores")
#     parser.add_argument("--max-events", type=int, default=0, help="Pass -n N to RecoTarget.exe")
#     parser.add_argument("-v", "--verbose", action="count", default=0, help="Pass -v to RecoTarget.exe; repeat for more verbosity")
    
#     return parser.parse_args()


# def collect_interactive_inputs(args):
#     runs = list(args.runs)

#     if not runs:
#         text = input("Enter RUN NUMBER(s) separated by spaces: ").strip()

#         if not text:
#             raise ValueError("No runs given")

#         runs = text.split()

#     jobs = args.jobs

#     if jobs is None:
#         text = input("How many concurrent reconstruction jobs? ").strip()

#         jobs = int(text)

#     if jobs < 1:
#         raise ValueError("Number of concurrent jobs must be >= 1")

#     if args.max_events < 0:
#         raise ValueError("--max-events must be >= 0")

#     return runs, jobs


# def launch_job(run_id: str, core: Optional[int], max_events: int, verbosity: int) -> RunningJob:

#     list_file = get_list_file(run_id)

#     if not list_file.is_file():
#         raise FileNotFoundError(f"List file not found: {list_file}")

#     output_file = get_output_file(run_id)
#     log_file = get_log_file(run_id)

#     cmd = [str(EXECUTABLE), "-l", str(list_file), "-o", str(output_file),]

#     if max_events > 0:
#         cmd += ["-n", str(max_events)]

#     for _ in range(verbosity):
#         cmd.append("-v")

#     if core is not None:
#         cmd = ["taskset", "-c", str(core),] + cmd

#     env = os.environ.copy()

#     # Prevent numerical libraries loaded by ROOT from spawning
#     # additional worker threads for each reconstruction process.
#     env.update(
#         {
#             "OMP_NUM_THREADS": "1",
#             "MKL_NUM_THREADS": "1",
#             "OPENBLAS_NUM_THREADS": "1",
#             "VECLIB_MAXIMUM_THREADS": "1",
#         }
#     )

#     log_handle = log_file.open("w")

#     process = subprocess.Popen(cmd, stdout=log_handle, stderr=subprocess.STDOUT, env=env)

#     return RunningJob(
#         run_id=run_id,
#         core=core,
#         process=process,
#         log_handle=log_handle,
#         output_file=output_file,
#         log_file=log_file,
#     )


# def main():
#     args = parse_arguments()

#     try:
#         runs, max_jobs = collect_interactive_inputs(args)
#     except (ValueError, EOFError) as exc:
#         print(f"ERROR: {exc}", file=sys.stderr)
#         return 1

#     try:
#         compile_reco()
#     except Exception as exc:
#         print(
#             f"ERROR: {exc}",
#             file=sys.stderr,
#         )
#         return 1

#     OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
#     LOG_DIR.mkdir(parents=True, exist_ok=True)

#     use_taskset = (not args.no_taskset and taskset_available())

#     # A core is considered available again only when the job that
#     # owns it has actually terminated.
#     available_cores = list(range(max_jobs))

#     pending = list(runs)
#     running: List[RunningJob] = []

#     results: List[Tuple[str, bool, str]] = []

#     start = time.time()

#     print(f"Starting {len(runs)} run(s), maximum {max_jobs} concurrent job(s).")

#     print("CPU pinning: " + ("enabled" if use_taskset else "disabled"))

#     while pending or running:

#         # -------------------------------------------------------------
#         # Start new jobs while slots are free.
#         # -------------------------------------------------------------
#         while pending and len(running) < max_jobs:

#             run_id = pending.pop(0)

#             list_file = get_list_file(run_id)

#             if not list_file.is_file():
#                 msg = (f"List file not found: {list_file}")
#                 print(f"[FAIL] run {run_id}: {msg}")
#                 results.append((run_id, False, msg))
#                 continue

#             core = None

#             if use_taskset:
#                 if not available_cores:
#                     break
#                 core = available_cores.pop(0)

#             try:
#                 job = launch_job(run_id=run_id, core=core, max_events=args.max_events, verbosity=args.verbose)
#             except Exception as exc:
#                 if core is not None:
#                     available_cores.append(core)
#                     available_cores.sort()

#                 msg = str(exc)
#                 print(f"[FAIL] run {run_id}: {msg}")
#                 results.append((run_id, False, msg))
#                 continue

#             running.append(job)

#             core_msg = (f", core {core}" if core is not None else "")

#             print(f"[START] run {run_id}" f"{core_msg}" f", PID {job.process.pid}" f", log {job.log_file}")

#         # -------------------------------------------------------------
#         # Poll running jobs.
#         # -------------------------------------------------------------
#         still_running: List[RunningJob] = []

#         for job in running:

#             return_code = job.process.poll()

#             if return_code is None:
#                 still_running.append(job)
#                 continue

#             job.log_handle.close()

#             if job.core is not None:
#                 available_cores.append(job.core)
#                 available_cores.sort()

#             success = return_code == 0

#             if success:
#                 msg = str(job.output_file)
#                 print(f"[ OK ] run {job.run_id}: {job.output_file}")
#             else:
#                 msg = (f"exit code {return_code}; see {job.log_file}")
#                 print(f"[FAIL] run {job.run_id}: {msg}")

#             results.append((job.run_id, success, msg))

#         running = still_running

#         if pending or running:
#             time.sleep(0.2)

#     elapsed = time.time() - start

#     successful = [item for item in results if item[1]]
#     failed = [item for item in results if not item[1]]

#     print("\n=== Summary ===")
#     print(f"Successful: {len(successful)}")
#     print(f"Failed    : {len(failed)}")
#     print(f"Elapsed   : {elapsed:.1f} s")

#     if failed:
#         print("\nFailed runs:")
#         for run_id, _, message in failed:
#             print(f"  {run_id}: {message}")

#     return 0 if not failed else 1


# if __name__ == "__main__":
#     raise SystemExit(main())
 
 

## second version below


#!/usr/bin/env python3

import argparse
import os
import shlex
import subprocess
import sys
import time
from dataclasses import dataclass
from datetime import datetime
from pathlib import Path
from typing import List, Optional, Tuple


SCRIPT_DIR = Path(__file__).resolve().parent

SOURCE = SCRIPT_DIR / "RecoTarget.cpp"
EXECUTABLE = SCRIPT_DIR / "RecoTarget.exe"

LIST_DIR = SCRIPT_DIR / "ListDir"
OUTPUT_DIR = SCRIPT_DIR / "outputReco"
LOG_DIR = OUTPUT_DIR / "logs"


# =============================================================================
# Compilation
# =============================================================================

def compile_reco():
    print("Compiling RecoTarget.cpp ...")

    try:
        root_cflags = subprocess.check_output(
            ["root-config", "--cflags"],
            universal_newlines=True,
        ).strip()

        root_libs = subprocess.check_output(
            ["root-config", "--libs"],
            universal_newlines=True,
        ).strip()

    except (FileNotFoundError, subprocess.CalledProcessError) as exc:
        raise RuntimeError(
            "Cannot execute root-config. "
            "Check that the ROOT environment is loaded."
        ) from exc

    cmd = [
        "g++",
        "-std=c++17",
        "-O2",
    ]

    cmd += shlex.split(root_cflags)

    cmd += [
        "-I",
        str((SCRIPT_DIR / "../PadmeRoot/include").resolve()),
        str(SOURCE),
        "-L",
        str((SCRIPT_DIR / "../PadmeRoot/lib").resolve()),
        "-lPadmeRoot",
    ]

    cmd += shlex.split(root_libs)

    cmd += [
        "-o",
        str(EXECUTABLE),
    ]

    print("Compilation command:")
    print(" ".join(cmd))

    result = subprocess.run(
        cmd,
        cwd=SCRIPT_DIR,
    )

    if result.returncode != 0:
        raise RuntimeError(
            "Compilation of RecoTarget.cpp failed"
        )

    if not EXECUTABLE.is_file():
        raise RuntimeError(
            "Compilation finished but executable was not created: "
            f"{EXECUTABLE}"
        )

    print(f"Compilation successful: {EXECUTABLE}")


# =============================================================================
# Job bookkeeping
# =============================================================================

@dataclass
class RunningJob:
    run_id: str
    core: Optional[int]
    process: subprocess.Popen
    log_handle: object
    output_file: Path
    log_file: Path


def get_list_file(run_id: str) -> Path:
    return LIST_DIR / f"RawMerged_run_00{run_id}.list"


def get_output_file(run_id: str) -> Path:
    # Keep your current test naming.
    # Change PROVA_ to Reco_run_ when you want the production filename.
    return OUTPUT_DIR / f"Reco2Tree_run_00{run_id}.root"


def get_log_file(run_id: str) -> Path:
    return LOG_DIR / f"run_{run_id}.log"


def taskset_available() -> bool:
    try:
        subprocess.run(
            ["taskset", "--version"],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
            check=True,
        )
        return True
    except (FileNotFoundError, subprocess.CalledProcessError):
        return False


# =============================================================================
# Command-line options
# =============================================================================

def parse_arguments():
    parser = argparse.ArgumentParser(
        description=(
            "Queued launcher for RecoTarget.exe. "
            "Can run in foreground or detach the whole queue in background."
        )
    )

    parser.add_argument(
        "runs",
        nargs="*",
        help="Run numbers, e.g. 90032 90033",
    )

    parser.add_argument(
        "-j",
        "--jobs",
        type=int,
        default=None,
        help="Maximum number of concurrent reconstruction jobs",
    )

    parser.add_argument(
        "--background",
        action="store_true",
        help=(
            "Launch the queue in background and return the terminal immediately"
        ),
    )

    parser.add_argument(
        "--foreground",
        action="store_true",
        help="Force foreground execution and skip the interactive background prompt",
    )

    parser.add_argument(
        "--no-taskset",
        action="store_true",
        help="Do not pin jobs to CPU cores",
    )

    parser.add_argument(
        "--max-events",
        type=int,
        default=0,
        help="Pass -n N to RecoTarget.exe",
    )

    parser.add_argument(
        "-v",
        "--verbose",
        action="count",
        default=0,
        help="Pass -v to RecoTarget.exe; repeat for more verbosity",
    )

    # Internal flag used by the detached worker.
    # It prevents recompilation and prevents the worker from detaching again.
    parser.add_argument(
        "--worker",
        action="store_true",
        help=argparse.SUPPRESS,
    )

    return parser.parse_args()


def collect_interactive_inputs(args):
    runs = list(args.runs)

    if not runs:
        text = input(
            "Enter RUN NUMBER(s) separated by spaces: "
        ).strip()

        if not text:
            raise ValueError("No runs given")

        runs = text.split()

    jobs = args.jobs

    if jobs is None:
        text = input(
            "How many concurrent reconstruction jobs? "
        ).strip()

        jobs = int(text)

    if jobs < 1:
        raise ValueError(
            "Number of concurrent jobs must be >= 1"
        )

    if args.max_events < 0:
        raise ValueError(
            "--max-events must be >= 0"
        )

    return runs, jobs


def choose_background_mode(args) -> bool:
    if args.worker:
        return False

    if args.background and args.foreground:
        raise ValueError(
            "Use either --background or --foreground, not both"
        )

    if args.background:
        return True

    if args.foreground:
        return False

    answer = input(
        "Launch the reconstruction queue in background? (yes/no): "
    ).strip().lower()

    return answer in ("y", "yes")


# =============================================================================
# One reconstruction process
# =============================================================================

def launch_job(
    run_id: str,
    core: Optional[int],
    max_events: int,
    verbosity: int,
) -> RunningJob:

    list_file = get_list_file(run_id)

    if not list_file.is_file():
        raise FileNotFoundError(
            f"List file not found: {list_file}"
        )

    output_file = get_output_file(run_id)
    log_file = get_log_file(run_id)

    cmd = [
        str(EXECUTABLE),
        "-l",
        str(list_file),
        "-o",
        str(output_file),
    ]

    if max_events > 0:
        cmd += ["-n", str(max_events)]

    for _ in range(verbosity):
        cmd.append("-v")

    if core is not None:
        cmd = [
            "taskset",
            "-c",
            str(core),
        ] + cmd

    env = os.environ.copy()

    # Prevent numerical libraries loaded by ROOT from spawning
    # additional worker threads for each reconstruction process.
    env.update(
        {
            "OMP_NUM_THREADS": "1",
            "MKL_NUM_THREADS": "1",
            "OPENBLAS_NUM_THREADS": "1",
            "VECLIB_MAXIMUM_THREADS": "1",
        }
    )

    log_handle = log_file.open("w")

    process = subprocess.Popen(
        cmd,
        stdout=log_handle,
        stderr=subprocess.STDOUT,
        env=env,
        cwd=SCRIPT_DIR,
    )

    return RunningJob(
        run_id=run_id,
        core=core,
        process=process,
        log_handle=log_handle,
        output_file=output_file,
        log_file=log_file,
    )


# =============================================================================
# Detached background worker
# =============================================================================

def launch_background_worker(
    runs: List[str],
    max_jobs: int,
    args,
) -> int:

    LOG_DIR.mkdir(parents=True, exist_ok=True)

    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")

    launcher_log = (
        LOG_DIR /
        f"RunReco_launcher_{timestamp}.log"
    )

    pid_file = (
        LOG_DIR /
        "RunReco_launcher.pid"
    )

    cmd = [
        sys.executable,
        str(Path(__file__).resolve()),
        "--worker",
        "--foreground",
        "--jobs",
        str(max_jobs),
    ]

    if args.no_taskset:
        cmd.append("--no-taskset")

    if args.max_events > 0:
        cmd += [
            "--max-events",
            str(args.max_events),
        ]

    for _ in range(args.verbose):
        cmd.append("-v")

    cmd += runs

    # The detached worker gets no terminal input.
    # All of its stdout/stderr go to one launcher log.
    with launcher_log.open("w") as log_handle:
        process = subprocess.Popen(
            cmd,
            cwd=SCRIPT_DIR,
            stdin=subprocess.DEVNULL,
            stdout=log_handle,
            stderr=subprocess.STDOUT,
            start_new_session=True,
            close_fds=True,
            env=os.environ.copy(),
        )

    pid_file.write_text(f"{process.pid}\n")

    print()
    print("Background reconstruction queue started.")
    print(f"PID          : {process.pid}")
    print(f"Launcher log : {launcher_log}")
    print(f"PID file     : {pid_file}")
    print()
    print("Monitor with:")
    print(f"  tail -f {launcher_log}")
    print()
    print("Check process with:")
    print(f"  ps -fp {process.pid}")
    print()

    return 0


# =============================================================================
# Queue manager
# =============================================================================

def run_queue(
    runs: List[str],
    max_jobs: int,
    args,
) -> int:

    OUTPUT_DIR.mkdir(
        parents=True,
        exist_ok=True,
    )

    LOG_DIR.mkdir(
        parents=True,
        exist_ok=True,
    )

    use_taskset = (
        not args.no_taskset
        and taskset_available()
    )

    # A core is returned to this pool only after the job using it has ended.
    available_cores = list(
        range(max_jobs)
    )

    pending = list(runs)

    running: List[RunningJob] = []

    results: List[
        Tuple[str, bool, str]
    ] = []

    start = time.time()

    print(
        f"Starting {len(runs)} run(s), "
        f"maximum {max_jobs} concurrent job(s)."
    )

    print(
        "CPU pinning: "
        + (
            "enabled"
            if use_taskset
            else "disabled"
        )
    )

    while pending or running:

        # ---------------------------------------------------------------------
        # Start new jobs while slots are available.
        # ---------------------------------------------------------------------
        while (
            pending
            and len(running) < max_jobs
        ):

            run_id = pending.pop(0)

            list_file = get_list_file(
                run_id
            )

            if not list_file.is_file():

                msg = (
                    "List file not found: "
                    f"{list_file}"
                )

                print(
                    f"[FAIL] run {run_id}: "
                    f"{msg}"
                )

                results.append(
                    (
                        run_id,
                        False,
                        msg,
                    )
                )

                continue

            core = None

            if use_taskset:

                if not available_cores:
                    break

                core = (
                    available_cores.pop(0)
                )

            try:

                job = launch_job(
                    run_id=run_id,
                    core=core,
                    max_events=args.max_events,
                    verbosity=args.verbose,
                )

            except Exception as exc:

                if core is not None:
                    available_cores.append(
                        core
                    )
                    available_cores.sort()

                msg = str(exc)

                print(
                    f"[FAIL] run {run_id}: "
                    f"{msg}"
                )

                results.append(
                    (
                        run_id,
                        False,
                        msg,
                    )
                )

                continue

            running.append(job)

            core_msg = (
                f", core {core}"
                if core is not None
                else ""
            )

            print(
                f"[START] run {run_id}"
                f"{core_msg}"
                f", PID {job.process.pid}"
                f", log {job.log_file}"
            )

        # ---------------------------------------------------------------------
        # Poll running jobs.
        # ---------------------------------------------------------------------
        still_running: List[
            RunningJob
        ] = []

        for job in running:

            return_code = (
                job.process.poll()
            )

            if return_code is None:

                still_running.append(
                    job
                )

                continue

            job.log_handle.close()

            if job.core is not None:

                available_cores.append(
                    job.core
                )

                available_cores.sort()

            success = (
                return_code == 0
            )

            if success:

                msg = str(
                    job.output_file
                )

                print(
                    f"[ OK ] run "
                    f"{job.run_id}: "
                    f"{job.output_file}"
                )

            else:

                msg = (
                    f"exit code "
                    f"{return_code}; "
                    f"see {job.log_file}"
                )

                print(
                    f"[FAIL] run "
                    f"{job.run_id}: "
                    f"{msg}"
                )

            results.append(
                (
                    job.run_id,
                    success,
                    msg,
                )
            )

        running = still_running

        if pending or running:
            time.sleep(0.2)


    elapsed = (
        time.time() - start
    )

    successful = [
        item
        for item in results
        if item[1]
    ]

    failed = [
        item
        for item in results
        if not item[1]
    ]

    print()
    print("=== Summary ===")

    print(
        f"Successful: "
        f"{len(successful)}"
    )

    print(
        f"Failed    : "
        f"{len(failed)}"
    )

    print(
        f"Elapsed   : "
        f"{elapsed:.1f} s"
    )

    if failed:

        print()
        print("Failed runs:")

        for (
            run_id,
            _,
            message,
        ) in failed:

            print(
                f"  {run_id}: "
                f"{message}"
            )

    # If this is the detached worker, remove the PID file when done.
    if args.worker:
        pid_file = (
            LOG_DIR /
            "RunReco_launcher.pid"
        )

        try:
            if pid_file.exists():
                pid_file.unlink()
        except OSError:
            pass

    return (
        0
        if not failed
        else 1
    )


# =============================================================================
# main
# =============================================================================

def main():
    args = parse_arguments()

    try:
        runs, max_jobs = (
            collect_interactive_inputs(
                args
            )
        )

    except (
        ValueError,
        EOFError,
    ) as exc:

        print(
            f"ERROR: {exc}",
            file=sys.stderr,
        )

        return 1


    # The normal interactive process compiles once.
    # The detached --worker must NOT compile again.
    if not args.worker:

        try:
            compile_reco()

        except Exception as exc:

            print(
                f"ERROR: {exc}",
                file=sys.stderr,
            )

            return 1

    else:

        if not EXECUTABLE.is_file():

            print(
                "ERROR: detached worker cannot "
                "find the compiled executable: "
                f"{EXECUTABLE}",
                file=sys.stderr,
            )

            return 1


    OUTPUT_DIR.mkdir(
        parents=True,
        exist_ok=True,
    )

    LOG_DIR.mkdir(
        parents=True,
        exist_ok=True,
    )


    try:
        background = (
            choose_background_mode(
                args
            )
        )

    except ValueError as exc:

        print(
            f"ERROR: {exc}",
            file=sys.stderr,
        )

        return 1


    if background:

        return launch_background_worker(
            runs=runs,
            max_jobs=max_jobs,
            args=args,
        )


    return run_queue(
        runs=runs,
        max_jobs=max_jobs,
        args=args,
    )


if __name__ == "__main__":
    raise SystemExit(main())
