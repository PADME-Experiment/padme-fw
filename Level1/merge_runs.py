#! /usr/bin/python

import os
import subprocess

executable = "./PadmeLevel1.exe"
input_data_dir = "../RunControl"

#./PadmeLevel1.exe -r 31 -d ../RunControl -o data/run_31 > log/merge_run_31.log
for run in range(82,169):

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
