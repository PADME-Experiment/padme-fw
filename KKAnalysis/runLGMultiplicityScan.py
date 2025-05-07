import os
import subprocess

# # Source the environment setup script
# padme_init_script = "/home/mancinima/LeadGlassCalib2024/Configure/padme_init.sh"
# command = f"source {padme_init_script}"
# subprocess.call(command, shell=True, executable="/bin/bash")

# Default number of events
default_nevents = 1000000

# Define specific event limits for certain runs
custom_nevents = {
    "run389.list": 14000,
    "run390.list": 4500,
    "run395.list": 10500
}

# Loop over input files (run385.list to run395.list)
for run_number in range(385, 396):
    input_file = f"/home/mancinima/LeadGlassCalib2024/KKAnalysis/ListDir/run{run_number}.list"

    # Check if the file exists
    if os.path.isfile(input_file):
        output_file = f"/home/mancinima/LeadGlassCalib2024/KKAnalysis/output/run{run_number}.root"

        # Determine the number of events
        nevents = custom_nevents.get(input_file, default_nevents)

        print(f"\nRunning leadglass.cpp for {input_file} -> output: {output_file} -- up to {nevents} events\n")

        # Execute the LGRead command
        command = f"./LGRead -l {input_file} -o {output_file} -n {nevents}"
        subprocess.run(command, shell=True)

    else:
        print(f"Warning: {input_file} does not exist, skipping.")

print("Processing completed.")
