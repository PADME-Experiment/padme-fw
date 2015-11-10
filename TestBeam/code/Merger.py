#!/usr/bin/python

import sys
import signal
import getopt
import os
import re
import time

# This will use the $PADME environment variable if set
PADME = os.getenv('PADME',"..")

# Dir where all log files are kept
log_dir = PADME+"/RunControl/log"

# Path where one can find the "data" directory containing DAQ files
daq_data_dir = PADME+"/RunControl"

# Temporary files used during monitoring
merged_dir = PADME+"/TestBeam/tmp"
merged_list = merged_dir+"/monitor.list"
merged_file_template = merged_dir+"/monitor"
merged_last_file = merged_dir+"/merged_last_file"

# Executable used to merge events and write RAW files
padme_merge = PADME+"/Level1/PadmeLevel1.exe"
#padme_monitor = PADME+"/TestBeam/Monitor.exe"

def signal_handler(signal, frame):
    print('Ctrl+C received: exiting')
    sys.exit(0)

# Get name of run to monitor from line arg
run_name = "dummy_run"
try:
    opts, args = getopt.getopt(sys.argv[1:],"hr:")
except getopt.GetoptError:
    print 'Merger -r <run name>'
    sys.exit(2)
for opt, arg in opts:
    if opt == '-h':
        print 'Merger -r <run name>'
        sys.exit()
    elif opt == '-r':
        run_name = arg
print 'Monitoring run',run_name
log_path = log_dir+"/"+run_name
if (not os.path.exists(log_path)):
    print "Path",log_path,"does not exist"
    sys.exit(1)
if (not os.path.isdir(log_path)):
    print "Path",log_path,"is not a directory"
    sys.exit(1)

# Get list of log files and boards for this run
print 'Looking for log files in',log_path
re_board = re.compile("^"+run_name+"_b(\d\d).log")
log_list = []
boardid_list = []
for top,dirs,files in os.walk(log_path):
    if (top == log_path):
        for f in files:
            m = re_board.search(f)
            if (m):
                boardid = int(m.group(1))
                print "Found board",boardid
                log_list.append(log_path+"/"+f)
                boardid_list.append(boardid)

# Show user what we found
for l in range(len(log_list)):
    print "Board",boardid_list[l],"Log",l,log_list[l]

# Loop forever (use CTRL-C to kill)
last_merged_slice = 0
disaligned_counter = 0
re_fileclose = re.compile("Closed file \'(.+)\' after")
signal.signal(signal.SIGINT, signal_handler)
while(1):

    # Look for data files for each board
    daqfiles = []
    for l in range(len(log_list)):
        daqfiles.append([])
        f = open(log_list[l])
        for r in f:
            m = re_fileclose.search(r)
            if (m): daqfiles[l].append(m.group(1))

    # Verify if number of files corresponds
    all_ok = 1
    for l in range(len(log_list)):
        print "Board",boardid_list[l],"has",len(daqfiles[l]),"DAQ files"
        if (len(daqfiles[l]) != len(daqfiles[0])): all_ok = 0
    if (all_ok):
        disaligned_counter = 0
    else:
        disaligned_counter += 1
        if (disaligned_counter<5):
            time.sleep(1)
            continue
        else:
            print "ERROR: files disaligned for more than 5 seeconds"
            sys.exit(1)

    n_data_files = len(daqfiles[0])

    # Check if new files have appeared and merge them
    if (n_data_files>last_merged_slice):

        # Create file with list of files to merge
        if os.path.exists(merged_list): os.remove(merged_list)
        f = open(merged_list,"w")
        for l in range(len(log_list)): f.write(str(l)+" "+daqfiles[l][-1]+"\n")
        f.close()

        # Merge list of files
        merged_file = merged_file_template+"_"+str(n_data_files)+".root"
        if os.path.exists(merged_file): os.remove(merged_file)
        cmd = padme_merge+" -d "+daq_data_dir+" -l "+merged_list+" -o "+merged_file+" -n 0"
        os.system(cmd)

        # Tell Monitor name of last merged file
        f = open(merged_last_file,"w")
        f.write(merged_file)
        f.close()

        # Run graphical monitor program
        #cmd = padme_monitor+" -i "+merged_file
        #os.system(cmd)

        # Update merged slices counter
        last_merged_slice = n_data_files

        # Sleep a small time (remove?)
        time.sleep(1)

    else:

        # Sleep for a while
        print "Nothing new to show: sleeping"
        time.sleep(10)
