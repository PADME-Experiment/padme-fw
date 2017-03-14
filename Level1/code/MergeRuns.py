#! /usr/bin/python

import os
import sys
import getopt
import subprocess

# Define executable used to merge a single run
executable = os.environ['PADME']+"/Level1/PadmeLevel1.exe"

# Define directory where the "data" directory (containing the files from DAQ) can be found
input_data_dir = "/home/daq/DAQ"
#input_data_dir = "./test"

# Define directory where the rawdata files (ROOT format) will be written
output_data_dir = "/home/daq/DAQ/rawdata"
#output_data_dir = "./test/rawdata"

# Define directory where the log from the merge process will be written
log_file_dir = "/home/daq/DAQ/log"
#log_file_dir = "./test/log"

def print_help():
    print 'MergeRuns (-s|--start) start_run [(-e|--end) end_run] [-n n_evt] [--db-update|--no-db-update][-h]'
    print
    print '(-s|--start) Specifies the first run to merge. Must be specified.'
    print '(-e|--end)   Specifies the last run to merge.'
    print '             If -e is not specified, only one run will be merged'
    print
    print '-n Defines the max number of events to write in each output file.'
    print '   If set to 0, all events will go to the same file.'
    print '   Default: 100000 events per file.'
    print
    print '--(no-)db-update Total number of events merged will (not) be written to DB.'
    print '                 Default: do not update the DB.'
    print
    print '-h Show this help and exit.'

def main(argv):

    start = 0
    end = 0
    nevt = 100000
    dbupdate = 0

    try:
        opts, args = getopt.getopt(argv,"hs:e:n:",["start=","end=","db-update","no-db-update"])
    except getopt.GetoptError:
        print_help()
        sys.exit(2)

    for opt, arg in opts:
        if opt == '-h':
            print_help()
            sys.exit()
        elif opt == '-n':
            nevt = int(arg)
        elif opt in ("-s", "--start"):
            start = int(arg)
        elif opt in ("-e", "--end"):
            end = int(arg)
        elif opt == "--db-update":
            dbupdate = 1
        elif opt == "--no-db-update":
            dbupdate = 0

    if (start <= 0):
        print 'ERROR: start run must be specified and must be >0'
        print
        print_help()
        sys.exit(2)

    if (end > 0 and end < start):
        print 'ERROR: if specified, end run must be > start run'
        print
        print_help()
        sys.exit(2)

    if (nevt < 0):
        print 'ERROR: number of events per file must be >=0 (see help)'
        print
        print_help()
        sys.exit(2)

    if (end == 0):
        print "MergeRuns - Merging run",start
        end = start
    else:
        print "MergeRuns - Merging runs from",start,"to",end

    print "Using executable",executable

    if (nevt == 0):
        print "All events from each run will be written to a single output file"
    else:
        print "Each output file will contain up to",nevt,"events"

    if (dbupdate):
        print "DB will be updated with the total number of merged events for each run"
    else:
        print "DB will not be updated"

    ret = raw_input('Press RETURN to proceed or Q to quit:')
    if (ret == "q" or ret == "Q"):
        print "MergeRuns - Exiting"
        sys.exit()

    # Example of command called
    #./PadmeLevel1.exe -r 31 -u -d /home/daq/DAQ -o /home/daq/DAQ/rawdata/run_31 -n 100000 > /home/daq/DAQ/log/merge_run_31.log
    for run in range(start,end+1):

        print "=== Merging run",run,"==="

        # Define output file template
        output_file = "%s/run_%d"%(output_data_dir,run)

        # Create log file and connect it
        log_file = "%s/merge_run_%d.log"%(log_file_dir,run)
        if os.path.exists(log_file): os.remove(log_file)
        log_handle = open(log_file,"w")

        # Start merging process.
        print "Outfile template",output_file
        print "Logfile name    ",log_file
        rc = 0
        if (dbupdate == 1):
            rc = subprocess.call([executable,"-r",str(run),"-u","-d",input_data_dir,"-o",output_file,"-n",str(nevt)],stdout=log_handle,stderr=subprocess.STDOUT)
        else:
            rc = subprocess.call([executable,"-r",str(run),"-d",input_data_dir,"-o",output_file,"-n",str(nevt)],stdout=log_handle,stderr=subprocess.STDOUT)
        if (rc != 0):
            print "*** ERROR *** Merging process returned",rc,"- Please check logfile"

        log_handle.close()

# Execution starts here
if __name__ == "__main__":
   main(sys.argv[1:])
