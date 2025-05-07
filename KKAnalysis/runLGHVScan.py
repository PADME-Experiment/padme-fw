import os
import subprocess

# Default number of events
# default_nevents = 1000000

# Loop over input files (run385.list to run395.list)
for run_number in range(310, 311):
    input_file = f"/home/mancinima/LeadGlassCalib2024/KKAnalysis/ListDir/run{run_number}.list"

    # Check if the file exists
    if os.path.isfile(input_file):
        output_file = f"/home/mancinima/LeadGlassCalib2024/KKAnalysis/output/run{run_number}.root"

        print(f"\nRunning leadglass.cpp for {input_file} -> output: {output_file} --\n")

        # Execute the LGRead command
        command = f"./LGRead -l {input_file} -o {output_file}"
        subprocess.run(command, shell=True)

    else:
        print(f"Warning: {input_file} does not exist, skipping.")

print("Processing completed.")
