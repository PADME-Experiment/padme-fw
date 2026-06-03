#!/usr/bin/env python3
import os
import subprocess
import sys

GREEN = "\033[92m"
RED = "\033[91m"
RESET = "\033[0m"

def get_run_dirs(remote_url, run_prefix):
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

    run_dirs = [d for d in dirs_listing.stdout.splitlines() if d.startswith(run_prefix)]
    if not run_dirs:
        print(f"{RED}No directory found for run prefix {run_prefix}{RESET}")
        return None

    return run_dirs[0]

def list_files(remote_run_url):
    files_listing = subprocess.run(
        ["gfal-ls", remote_run_url],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        universal_newlines=True
    )

    if files_listing.returncode != 0:
        print(f"{RED}ERROR:{RESET} Cannot read run directory:")
        print(files_listing.stderr)
        return []

    return files_listing.stdout.splitlines()


def main():
    year = input("Please enter YEAR: ").strip()
    datadir = input("Please enter DATADIR (rawdata/mm/tmm): ").strip().lower()

    if datadir not in ["rawdata", "mm", "tmm"]:
        print(f"{RED}ERROR:{RESET} datadir must be rawdata, mm or tmm.")
        sys.exit(1)

    # Correct base path (validated by user)
    remote_base = "root://atlasse.lnf.infn.it//dpm/lnf.infn.it/home/vo.padme.org"

    remote_dir = f"/daq/{year}/{datadir}"
    remote_url = remote_base + remote_dir

    run_input = input("Please enter RUN NUMBER(s) (nnnnn) separated by spaces: ").strip()
    run_list = run_input.split()

    local_dir = "/home/mancinima/LeadGlassCalib2024/padme-fw/TargetRun4/ListDir/"
    os.makedirs(local_dir, exist_ok=True)

    for run_short in run_list:
        print(f"\nProcessing run {run_short} ...")
        run_prefix = f"run_00{run_short}_"

        full_run_dir = get_run_dirs(remote_url, run_prefix)
        if not full_run_dir:
            continue

        remote_run_url = remote_url + "/" + full_run_dir

        print(f"Found full run directory: {full_run_dir}")

        files = list_files(remote_run_url)
        num_files = len(files)
        print(f"Number of files in this run: {num_files}")

        if num_files == 0:
            print("No files found. Skipping.")
            continue

        # Write correct .list paths
        list_file_path = os.path.join(local_dir, f"run_00{run_short}.list")
        with open(list_file_path, "w") as f:
            for filename in files:
                f.write(f"{remote_run_url}/{filename}\n")

        print(f"{GREEN}file.list created with {num_files} entries: {list_file_path}{RESET}")

        # Print 1/100 sample
        step = max(num_files // 100, 1)
        print("\nSample of paths written to file.list:")
        for i, filename in enumerate(files):
            if i % step == 0:
                print(f" - {remote_run_url}/{filename}")

        print(f"\nTotal paths printed: {num_files // step + 1 if num_files > 100 else num_files}")


if __name__ == "__main__":
    main()


# #!/usr/bin/env python3
# import os
# import sys

# GREEN = "\033[92m"
# RED = "\033[91m"
# RESET = "\033[0m"

# def get_run_dir_local(base_dir, run_prefix):
#     """Return the first local directory in base_dir that starts with run_prefix."""
#     try:
#         entries = os.listdir(base_dir)
#     except OSError as e:
#         print(f"{RED}ERROR:{RESET} Cannot read local directory: {base_dir}")
#         print(e)
#         sys.exit(1)

#     run_dirs = [
#         d for d in entries
#         if d.startswith(run_prefix) and os.path.isdir(os.path.join(base_dir, d))
#     ]

#     if not run_dirs:
#         print(f"{RED}No directory found for run prefix {run_prefix}{RESET}")
#         return None

#     # Keep behavior similar to gfal-ls (deterministic choice)
#     run_dirs.sort()
#     return run_dirs[0]

# def list_files_local(run_dir_path):
#     """List regular files inside a local run directory."""
#     try:
#         entries = os.listdir(run_dir_path)
#     except OSError as e:
#         print(f"{RED}ERROR:{RESET} Cannot read run directory: {run_dir_path}")
#         print(e)
#         return []

#     files = [
#         f for f in entries
#         if os.path.isfile(os.path.join(run_dir_path, f))
#     ]
#     files.sort()
#     return files

# def main():
#     year = input("Please enter YEAR: ").strip()
#     datadir = input("Please enter DATADIR (rawdata/mm/tmm): ").strip().lower()
#     if datadir not in ["rawdata", "mm", "tmm"]:
#         print(f"{RED}ERROR:{RESET} datadir must be rawdata, mm or tmm.")
#         sys.exit(1)

#     # Local base path that contains /daq/<YEAR>/<DATADIR>
#     local_base = input(f"Please enter LOCAL BASE path (the folder that contains {datadir}): ").strip()
#     if not local_base:
#         print(f"{RED}ERROR:{RESET} local base path is empty.")
#         sys.exit(1)

#     local_dir_to_scan = os.path.join(local_base, datadir, year)
#     if not os.path.isdir(local_dir_to_scan):
#         print(f"{RED}ERROR:{RESET} This local directory does not exist:")
#         print(local_dir_to_scan)
#         sys.exit(1)

#     run_input = input("Please enter RUN NUMBER(s) (nnnnn) separated by spaces: ").strip()
#     run_list = run_input.split()
#     if not run_list:
#         print(f"{RED}ERROR:{RESET} No run numbers provided.")
#         sys.exit(1)

#     # Where to save the .list files
#     out_dir = "/home/mancinima/LeadGlassCalib2024/padme-fw/TargetRun4/ListDir/"
#     os.makedirs(out_dir, exist_ok=True)

#     for run_short in run_list:
#         print(f"\nProcessing run {run_short} ...")
#         run_prefix = f"run_00{run_short}_"

#         full_run_dir = get_run_dir_local(local_dir_to_scan, run_prefix)
#         if not full_run_dir:
#             continue

#         run_path = os.path.join(local_dir_to_scan, full_run_dir)
#         print(f"Found full run directory: {full_run_dir}")
#         print(f"Local run path: {run_path}")

#         files = list_files_local(run_path)
#         num_files = len(files)
#         print(f"Number of files in this run: {num_files}")

#         if num_files == 0:
#             print("No files found. Skipping.")
#             continue

#         list_file_path = os.path.join(out_dir, f"run_00{run_short}.list")
#         with open(list_file_path, "w") as f:
#             for filename in files:
#                 f.write(os.path.join(run_path, filename) + "\n")

#         print(f"{GREEN}file.list created with {num_files} entries: {list_file_path}{RESET}")

#         step = max(num_files // 100, 1)
#         print("\nSample of paths written to file.list:")
#         printed = 0
#         for i, filename in enumerate(files):
#             if i % step == 0:
#                 print(f" - {os.path.join(run_path, filename)}")
#                 printed += 1

#         print(f"\nTotal paths printed: {printed}")

# if __name__ == "__main__":
#     main()
