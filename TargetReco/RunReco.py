#!/usr/bin/env python3
import subprocess
import os
import sys
import time

GREEN = "\033[92m"
RED   = "\033[91m"
YELLOW= "\033[93m"
RESET = "\033[0m"

EXECUTABLE = "./TargetRun4_OnResonance.exe" #"./TargetRun4_OnResonance_old.exe"
LIST_DIR   = "ListDir"
OUTPUT_DIR = "./outputReco"

def check_taskset():
    """Return True if taskset is available"""
    try:
        subprocess.check_output(["taskset", "--version"], stderr=subprocess.STDOUT)
        return True
    except Exception:
        return False

def launch_job(runid, core=None):
    """Launch a single job with optional core pinning"""
    run_prefix = f"run_00{runid}_"
    list_file = os.path.join(LIST_DIR, f"run_00{runid}.list")
    if not os.path.isfile(list_file):
        return None, f"List file not found: {list_file}"

    os.makedirs(OUTPUT_DIR, exist_ok=True)
    output_file = os.path.join(OUTPUT_DIR, f"Reco_run_00{runid}.root")

    cmd = [EXECUTABLE, "-l", list_file, "-o", output_file]

    if core is not None:
        cmd = ["taskset", "-c", str(core)] + cmd

    # limit threading inside the executable
    env = os.environ.copy()
    env.update({
        "OMP_NUM_THREADS": "1",
        "MKL_NUM_THREADS": "1",
        "OPENBLAS_NUM_THREADS": "1",
        "VECLIB_MAXIMUM_THREADS": "1"
    })

    try:
        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, env=env)
        return proc, output_file
    except FileNotFoundError:
        return None, f"Executable '{EXECUTABLE}' not found"

def main():
    print("=== TargetRun4 SAFE QUEUED LAUNCHER ===\n")

    run_input = input("Enter RUN NUMBER(s) (nnnnn) separated by spaces: ").strip()
    if not run_input:
        print(f"{RED}No runs given. Exiting.{RESET}")
        sys.exit(1)
    run_ids = run_input.split()

    try:
        n_cores = int(input("How many CPU cores to use? ").strip())
        if n_cores < 1:
            raise ValueError
    except ValueError:
        print(f"{RED}Invalid number of cores.{RESET}")
        sys.exit(1)

    use_nohup = input("Launch in background (nohup)? (yes/no): ").strip().lower() in ("y", "yes")
    taskset_ok = check_taskset()

    total_jobs = len(run_ids)
    running = []
    results = []
    completed = 0
    cores = list(range(n_cores))
    queue = list(run_ids)
    start_time = time.time()

    print(f"\nStarting queued execution: {total_jobs} runs, max {n_cores} concurrent jobs.\n")

    while queue or running:
        # launch jobs if there is free slot
        while queue and len(running) < n_cores:
            runid = queue.pop(0)
            core = cores[len(running) % n_cores] if taskset_ok else None

            if use_nohup:
                # background mode
                log_file = os.path.join(OUTPUT_DIR, f"logs/run_{runid}.log")
                os.makedirs(os.path.dirname(log_file), exist_ok=True)
                cmd_str = f"nohup {'taskset -c '+str(core) if core else ''} {EXECUTABLE} -l {LIST_DIR}/run_00{runid}.list -o {OUTPUT_DIR}/Reco_run_00{runid}.root > {log_file} 2>&1 & echo $!"
                pid_bytes = subprocess.check_output(cmd_str, shell=True, universal_newlines=True)
                pid = pid_bytes.strip()
                print(f"{GREEN}Launched run {runid} PID={pid} log={log_file}{RESET}")
                results.append((runid, True, f"Background PID={pid}"))
            else:
                proc, info = launch_job(runid, core)
                if proc is None:
                    results.append((runid, False, info))
                    completed += 1
                else:
                    running.append((runid, proc, info))

        # check running processes
        still_running = []
        for runid, proc, info in running:
            ret = proc.poll()
            if ret is not None:
                # finished
                stdout, stderr = proc.communicate()
                success = ret == 0
               msg = info if success else (stderr.decode().splitlines()[0] if stderr else f"exit {ret}")
                results.append((runid, success, msg))
                completed += 1
            else:
                still_running.append((runid, proc, info))
        running = still_running

        # progress bar
        if not use_nohup:
            fraction = completed / total_jobs
            filled = int(fraction*40)
            bar = "#"*filled + "-"*(40-filled)
            elapsed = time.time() - start_time
            eta = int((elapsed/completed*(total_jobs-completed)) if completed>0 else 0)
            print(f"\r[{bar}] {completed}/{total_jobs} ETA:{eta}s", end="", flush=True)

        time.sleep(0.2)

    print("\n\nAll jobs finished. Summary:\n")
    ok = [r for r in results if r[1]]
    fail = [r for r in results if not r[1]]
    print(f"{GREEN}SUCCESS: {len(ok)}{RESET}")
    print(f"{RED}FAILED : {len(fail)}{RESET}")
    if fail:
        for runid, _, msg in fail:
            print(f" - {runid}: {msg}")
    print("\nDone.")
    
if __name__ == "__main__":
    main()
