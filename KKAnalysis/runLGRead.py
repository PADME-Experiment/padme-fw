import argparse
import subprocess
import os

main_path = f"/home/mancinima/LeadGlassCalib2024/KKAnalysis/"

def get_nevents(input_file):
    """Define custom event numbers for specific runs."""
    default_nevents = 100000
    custom_events = { #read by Katerina scripts
        "run313.list": 21000,
        "run315.list": 15000,
        "run321.list": 10100,
        "run322.list": 7100,
        "run326.list": 14500,
        "run328.list": 13600,
        "run333.list": 7800,
        "run337.list": 16000,
        "run339.list": 9000,
        "run342.list": 5500,
        "run348.list": 11000,
        "run349.list": 17500,
        "run352.list": 2100,
        "run362.list": 2400,
        "run364.list": 6000,
        "run365.list": 500,
        "run367.list": 3300,
        "run369.list": 7000,
        "run370.list": 1000,
        "run389.list": 14000,
        "run390.list": 4500,
        "run395.list": 10500
        # Add other exceptions here if needed
    }
    return custom_events.get(input_file, default_nevents)

def main(start, end): #, use_nevents):
    """Run LGRead for the specified range of runs."""
    for run_number in range(start, end + 1):
        input_file = main_path + f"ListDir/run{run_number}.list"
        
        if os.path.isfile(input_file):
            output_file = main_path + f"output/runs/run{run_number}.root"
            nevents = get_nevents(input_file) # if use_nevents else None
        
            # if use_nevents and nevents:
            print(f"\nRunning LGRead for {input_file} -> output: {output_file} - N_ev: {nevents}\n")
            command = ["./LGRead", "-l", input_file, "-o", output_file, "-n", str(nevents)]
            # command = ["./LGRead", "-l", input_file, "-o", output_file]
            # command.extend(["-n", str(nevents)])
            # else:
            #     print(f"\nRunning LGRead for {input_file} -> output: {output_file}\n")
            #     command = ["./LGRead", "-l", input_file, "-o", output_file]

            process = subprocess.run(command)
            print(process.stdout)
            print(process.stderr)
        else:
            print(f"Warning: {input_file} does not exist, skipping")
    
    print("Processing completed.")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Run LGRead for a range of runs")
    parser.add_argument("-s", "--start", type=int, required=True, help="Start run number")
    parser.add_argument("-e", "--end", type=int, required=True, help="End run number")
    # parser.add_argument("--no-nevents", action="store_false", dest="use_nevents", help="Run LGRead without specifying the -n option")
    args = parser.parse_args()
    
    main(args.start, args.end) #, args.use_nevents)

