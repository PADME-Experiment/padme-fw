#il mio primo scriptino con Python per fargli fare lo scan
import subprocess
import os
import sys
import shutil


# Check if the correct number of command-line arguments is provided
if len(sys.argv) != 4:
    print("Usage: python script.py <num_iterations> <thetaX beam dir [Rad]> <thetaY beam dir [Rad]>")
    sys.exit(1)

# Get the number of iterations from command-line arguments
try:
    num_iterations = int(sys.argv[1])
    thetaX = float(sys.argv[2])
    thetaY = float(sys.argv[3])
except ValueError:
    print("Error: Invalid number of iterations - missing angles - try: python script.py <num_iterations> <thetaX beam dir [Rad]> <thetaY beam dir [Rad]>")
    sys.exit(1)

# Read energy values from an external file (energy_values.txt)
with open("/home/mancinima/padme-fw/varieMarco/Run4EnPoints.txt", "r") as f:
    energy_value = f.read().splitlines()

energy_value = [f'{float(energy):.2f}' for energy in energy_value]

# Define NameProcess values
name_process = ["BhabhaSCh", "BhabhaFull", "AAProduction"]

# Paths to your C++ file and output directory
path_to = "/home/mancinima/padme-fw/varieMarco"
cpp_file = "BkgSens"
output_directory = "/home/mancinima/padme-fw/varieMarco/Run4Projection"

# Check if the directory exists, and if it does, delete it recursively
if os.path.exists(output_directory):
    shutil.rmtree(output_directory)
# Create a directory to store individual output ROOT files
os.makedirs(output_directory, exist_ok=True)

# Run the routine multiple times
for i in range(num_iterations):
    # Generate input file names and output ROOT file name for this iteration
    input_root_file0 = f"{path_to}/CalcHEPFiles/{name_process[0]}/{name_process[0]}_{energy_value[i]}.root"
    input_root_file1 = f"{path_to}/CalcHEPFiles/{name_process[1]}/{name_process[1]}_{energy_value[i]}.root"
    input_root_file2 = f"{path_to}/CalcHEPFiles/{name_process[2]}/{name_process[2]}_{energy_value[i]}.root"
    output_root_file = f"{output_directory}/output_{i}.root"

    # Compile and run the C++ program with the current input files using root-config
    compile_command = f"g++ -o {cpp_file} {cpp_file}.cpp `root-config --cflags --libs`"
    subprocess.run(compile_command, shell=True)

    # Run the C++ program with the current input files
    run_command = f"./{cpp_file} {energy_value[i]} {thetaX} {thetaY} {input_root_file0} {input_root_file1} {input_root_file2} {output_root_file}"
    subprocess.run(run_command, shell=True)

    print(f"Iteration {i}: Output saved as {output_root_file}")

# Merge all individual output ROOT files into a single file
merged_output_file = f"{output_directory}/merged_output.root"
merge_command = f"hadd -f {merged_output_file} {output_directory}/*.root"
os.system(merge_command)

print(f"All {num_iterations} iterations completed. Individual output ROOT files saved in '{output_directory}' directory.")
