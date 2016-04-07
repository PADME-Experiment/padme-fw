#! /usr/bin/python

import os
import sys
import getopt
import subprocess

executable = "./PadmeLevel1.exe"
input_data_dir = "../RunControl"

def main(argv):

    start = 0
    end = 0

    try:
        opts, args = getopt.getopt(argv,"hs:e:",["start=","end="])
    except getopt.GetoptError:
        print 'merge_runs.py -s start_run -e end_run'
        sys.exit(2)

    for opt, arg in opts:
        if opt == '-h':
            print 'merge_runs.py -s start_run -e end_run'
            sys.exit()
        elif opt in ("-s", "--start"):
            start = int(arg)
        elif opt in ("-e", "--end"):
            end = int(arg)

    if (start <= 0) or (end <= 0) or (start > end):
        print 'merge_runs.py -s start_run -e end_run'
        print '  with start_run >= end_run > 0 both included'
        sys.exit(2)

    # Example of command called
    #./PadmeLevel1.exe -r 31 -d ../RunControl -o data/run_31 -n 100000 > log/merge_run_31.log
    for run in range(start,end+1):

        print "Merging run",run
        output_file = "data/run_%d"%run

        # Create log file and connect it
        log_file = "log/merge_run_%d.log"%run
        if os.path.exists(log_file): os.remove(log_file)
        log_handle = open(log_file,"w")

        # Start merging process. All events written to 1 file (!!!)
        #print "Run",run,"output file",output_file,"log file",log_file
        subprocess.call([executable,"-r",str(run),"-d",input_data_dir,"-o",output_file,"-n","100000"],stdout=log_handle,stderr=subprocess.STDOUT)

        log_handle.close()

# Execution starts here
if __name__ == "__main__":
   main(sys.argv[1:])
