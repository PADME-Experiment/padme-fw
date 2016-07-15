#!/usr/bin/python

import sys
import signal
import getopt
import os
import time
import subprocess

from MergerClass import MergerClass

# Define global variables used in the signal_handler
gui_process = 0
gui_log_handle = 0

def main():

    # This will use the $PADME environment variable if set
    PADME = os.getenv('PADME',"..")

    # Dir where all log files are kept
    #log_dir = PADME+"/RunControl/log"
    log_dir = "/home/daq/DAQ/log"

    # Path where one can find the "data" directory containing DAQ files
    daq_data_dir = PADME+"/RunControl"

    # Temporary files used during monitoring
    merged_dir = PADME+"/TestBeam/tmp"
    merged_list = merged_dir+"/monitor.list"
    merged_file_template = merged_dir+"/monitor"
    merged_last_file = merged_dir+"/monitor_last_file"

    # Executable used to merge events and write RAW files
    padme_merge = PADME+"/Level1/PadmeLevel1.exe"

    # Executable which handles the OnlineMonitor GUI
    padme_monitor = PADME+"/TestBeam/Monitor.exe"

    # Log file for the OnlineMonitor GUI
    gui_log_file = "Monitor.log"

    # Get name of run to monitor from line arg
    run_name = "dummy_run"
    try:
        opts, args = getopt.getopt(sys.argv[1:],"hr:")
    except getopt.GetoptError:
        print 'OnlineMonitor -r <run name>'
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print 'OnlineMonitor -r <run name>'
            sys.exit()
        elif opt == '-r':
            run_name = arg
    print 'Monitoring run',run_name

    # Verify that required run is running (or was runned)
    log_path = log_dir+"/"+run_name
    if (not os.path.exists(log_path)):
        print "Path",log_path,"does not exist"
        sys.exit(1)
    if (not os.path.isdir(log_path)):
        print "Path",log_path,"is not a directory"
        sys.exit(1)

    # Open GUI log file
    if os.path.exists(gui_log_file): os.remove(gui_log_file)
    global gui_log_handle
    gui_log_handle = open(gui_log_file,"w")

    # Start graphical monitor program
    #cmd = padme_monitor+" -i "+merged_file
    #os.system(cmd)
    global gui_process
    try:
        gui_process = subprocess.Popen([padme_monitor,"-i",merged_last_file],stdout=gui_log_handle,stderr=subprocess.STDOUT,bufsize=1)
    except OSError as e:
        print "ERROR executing",padme_monitor,"-",e
        return 0
    print "Online Monitor GUI started with process id",gui_process.pid

    # Instantiate the Merger program and configure it
    merger = MergerClass()
    merger.run_name             = run_name
    merger.log_path             = log_path
    merger.daq_data_dir         = daq_data_dir
    merger.merged_list          = merged_list
    merger.merged_file_template = merged_file_template
    merger.merged_last_file     = merged_last_file
    merger.padme_merge          = padme_merge

    # Catch CTRL-C to exit gracefully
    signal.signal(signal.SIGINT, signal_handler)

    # Pass control to Merger
    merger.Start()

def signal_handler(signal,frame):
    global gui_process
    print('Ctrl+C received: exiting')
    # Kill GUI process
    print "Terminating GUI"
    gui_process.terminate()
    time.sleep(2)
    gui_process.poll()
    if (gui_process.returncode == None):
        print "Problem terminating GUI: killing it"
        gui_process.kill()
    global gui_log_handle
    gui_log_handle.close()
    sys.exit(0)

# Execution starts here
main()
