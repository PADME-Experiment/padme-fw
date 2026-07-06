import os
import glob

# Base directory where all run folders are stored
# base_directory = "/data9Vd1/padme/mancinima/Target_Run4"
base_directory = "/data9Vd1/padme/leonardi/padme/daq/2025/rawfile/"

# List of run numbers to process
# run_numbers = [str(i) for i in range(318, 301)] #add more runs as needed 
# run_numbers = [646, 647, 649, 650] #add more runs as needed
run_numbers = [357]

for run_number in run_numbers:
    # Search for the directory that matches "run_0000{run_number}_*"
    search_pattern = os.path.join(base_directory, f"run_0080{run_number}_*")
    matching_dirs = glob.glob(search_pattern)

    # Check if we found exactly one match
    if len(matching_dirs) == 0:
        print(f"Error: No directory found for run_00{run_number}_* in {base_directory}")
        continue
    elif len(matching_dirs) > 1:
        print(f"Error: Multiple directories found for run_0000{run_number}_*. Please check:")
        print("\n".join(matching_dirs))
        continue

    # Use the found directory
    directory = matching_dirs[0]
    output_file = f"run{run_number}.list"

    print(f"Using directory: {directory}")

    # Check if directory exists
    if not os.path.exists(directory):
        print(f"Error: Directory '{directory}' does not exist.")
        continue

    # Collect file paths
    file_paths = []
    for root, _, files in sorted(os.walk(directory)):
        for file in sorted(files):
            full_path = os.path.join(root, file)
            print(f"Found file: {full_path}")  # Debugging line
            file_paths.append(full_path)

    # Check if files were found
    if not file_paths:
        print(f"No files found for run {run_number}.")
        continue

    # Write to output file
    with open(output_file, "w") as f:
        f.write("\n".join(file_paths))

    print(f"File list saved to {output_file}")
