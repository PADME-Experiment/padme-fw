#!/usr/bin/python

import os
import sys
import re
import time
import getopt
import subprocess
import daemon
import daemon.pidfile

from Logger import Logger

# Get position of CDR main directory from PADME_CDR_DIR environment variable
# Default to current dir if not set
cdr_dir = os.getenv('PADME_CDR_DIR',".")

# User running CDR
cdr_user = os.environ['USER']

# Path to stop_cdr file: when file appears, server will remove it and gently exit
stop_cdr_file = "%s/run/stop_cdr_Monitor"%cdr_dir

# Define log and lock files
log_file = "%s/log/CDRMonitor.log"%cdr_dir
pid_file = "%s/run/lock_Monitor"%cdr_dir

# Define time in sec to pause between checks
monitor_pause = 300

# Access information for monitor server
monitor_server = "l0padme3"
monitor_user = "monitor"
monitor_keyfile = "/home/%s/.ssh/id_rsa_cdr"%cdr_user

# Directory where monitor file will be stored
monitor_dir = "/home/monitor/PadmeMonitor/watchdir"

# File with monitor information
monitor_file = "CDRMonitor.txt"

# Define background colors to use for none/ok/warn/alarm/off
color_none  = "#FFFFFF"
color_ok    = "#00CC00"
color_warn  = "#CCCC00"
color_alarm = "#CC0000"
color_off   = "#0000CC"

#############################
### DAQ data servers data ###
#############################

# Access information for DAQ data server
daq_server_list = ("l1padme3","l1padme4")
daq_user = "daq"
daq_keyfile = "/home/%s/.ssh/id_rsa_cdr"%cdr_user

# Path to top disk mount point on DAQ data server
daq_path = "/data"

# Warning and alarm levels (in %) for DAQ disk servers
daq_level_warn = 60
daq_level_alarm = 85

##############################
### KLOE tape library data ###
##############################

# Access information for KLOE front end
kloe_server = "fibm15"
kloe_user = "pdm"
kloe_keyfile = "/home/%s/.ssh/id_rsa_cdr"%cdr_user

# SSH syntax to execute a command on KLOE front end
kloe_ssh = "ssh -i %s -l %s %s"%(kloe_keyfile,kloe_user,kloe_server)

# Path to top padme directory on KLOE front end
#kloe_path = "/pdm/padme/daq"
kloe_path = "/pdm"

# Tool to get KLOE tape occupation
kloe_tape_app = "/pdm/bin/padme_sum"

# Warning and alarm levels (in %) for KLOE disk servers
kloe_level_warn = 60
kloe_level_alarm = 85

def check_stop_cdr():
    if (os.path.exists(stop_cdr_file)):
        if (os.path.isfile(stop_cdr_file)):
            print "- Stop request file %s found. Removing it and exiting..."%stop_cdr_file
            os.remove(stop_cdr_file)
        else:
            print "- WARNING - Stop request at path %s found but IT IS NOT A FILE."%stop_cdr_file
            print "- I will not try to remove it but I will exit anyway..."
        print ""
        print "=== Exiting CDRMonitor ==="
        sys.exit(0)

def main(argv):

    # Creat daemon contex for run monitoring
    context = daemon.DaemonContext(
        working_directory = cdr_dir,
        umask = 0o002,
        pidfile = daemon.pidfile.PIDLockFile(pid_file)
    )

    # Become a daemon and start the production
    print "Starting CDRMonitor in background"
    context.open()
    start_monitor()
    context.close()

def get_daq_info(server):

    daq_ssh = "ssh -i %s -l %s %s"%(daq_keyfile,daq_user,server)

    disk_total = "0"
    disk_used  = "0"
    disk_avail = "0"
    disk_usepc = "0"
    cmd = "%s df -m %s"%(daq_ssh,daq_path)
    for line in run_command(cmd):
        #print line.rstrip()
        rc = re.match("^\S+\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)%%\s+%s\s*$"%daq_path,line.rstrip())
        if rc:
            disk_total = rc.group(1)
            disk_used  = rc.group(2)
            disk_avail = rc.group(3)
            disk_usepc = rc.group(4)

    return (disk_total,disk_used,disk_avail,disk_usepc,)

def get_kloe_info():

    tape_occ = "0"
    cmd = "%s %s"%(kloe_ssh,kloe_tape_app)
    for line in run_command(cmd):
        #print line.rstrip()
        rc = re.match("^\S+\s+/pdm\s+(\S+)\s*$",line.rstrip())
        if rc: tape_occ = rc.group(1)

    disk_tot = ""
    disk_avl = ""
    disk_opc = ""
    cmd = "%s df -m %s"%(kloe_ssh,kloe_path)
    for line in run_command(cmd):
        #print line.rstrip()
        rc = re.match("^\S+\s+(\S+)\s+(\S+)\s+(\S+)%%.*%s\s*$"%kloe_path,line.rstrip())
        if rc:
            disk_tot = rc.group(1)
            disk_avl = rc.group(2)
            disk_opc = rc.group(3)

    return(tape_occ,disk_tot,disk_avl,disk_opc)

def run_command(command):
    print "> %s"%command
    p = subprocess.Popen(command,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,shell=True)
    return iter(p.stdout.readline,b'')

def start_monitor():

    # Redefine print to send output to log file
    sys.stdout = Logger(log_file)
    sys.stderr = sys.stdout
    #if mode == "i": sys.stdout.interactive = True

    print "=== Starting CDRMonitor ==="

    while(True):

        mh = open("/tmp/%s"%monitor_file,"w")
        mh.write("PLOTID CDR_status\n")
        mh.write("PLOTNAME PADME CDR Status - %s\n"%now_str())
        mh.write("PLOTTYPE activetext\n")
        mh.write("DATA [ ")

        for daq_server in daq_server_list:
            (daq_tot,daq_use,daq_avl,daq_opc) = get_daq_info(daq_server)
            #daq_tot_TB = float(daq_tot)/1024/1024
            daq_use_TB = float(daq_use)/1024/1024
            #daq_avl_TB = float(daq_avl)/1024/1024
            daq_color = color_ok
            if (int(daq_opc)>daq_level_warn): daq_color = color_warn
            if (int(daq_opc)>daq_level_alarm): daq_color = color_alarm
            mh.write("{\"title\":\"%s\",\"current\":{\"value\":\"Used:%4.1fTB (%s%%)\",\"col\":\"%s\"}}"%(daq_server,daq_use_TB,daq_opc,daq_color))
            mh.write(",")

        (kloe_tape_occ,kloe_disk_tot,kloe_disk_avl,kloe_disk_opc) = get_kloe_info()
        kloe_tape_TB = float(kloe_tape_occ)
        kloe_tape_color = color_none
        kloe_disk_tot_TB = float(kloe_disk_tot)/1024/1024
        kloe_disk_avl_TB = float(kloe_disk_avl)/1024/1024
        kloe_disk_use_TB = kloe_disk_tot_TB-kloe_disk_avl_TB
        kloe_disk_color = color_ok
        if (int(kloe_disk_opc)>daq_level_warn):  kloe_disk_color = color_warn
        if (int(kloe_disk_opc)>daq_level_alarm): kloe_disk_color = color_alarm
        #mh.write("{\"title\":\"KLOE Tape\",\"current\":{\"value\":\"Used:%6.1f TB\",\"col\":\"%s\"}}"%(kloe_tape_TB,kloe_tape_color))
        mh.write("{\"title\":\"KLOE Tape\",\"current\":{\"value\":\"Used:%6.1f TB\"}}"%kloe_tape_TB)
        mh.write(",")
        mh.write("{\"title\":\"KLOE Disk\",\"current\":{\"value\":\"Used:%6.1f TB (%s%%)\",\"col\":\"%s\"}}"%(kloe_disk_use_TB,kloe_disk_opc,kloe_disk_color))
        #mh.write(",")

        mh.write("]\n")
        mh.close()

        cmd = "scp -i %s /tmp/%s %s@%s:%s/%s"%(monitor_keyfile,monitor_file,monitor_user,monitor_server,monitor_dir,monitor_file)
        for line in run_command(cmd): print line.rstrip()

        # Pause monitor_pause seconds while checking every 10sec for stop file
        n_pause = monitor_pause/10
        for n in range(n_pause):
            check_stop_cdr()
            time.sleep(10)

def now_str():
    return time.strftime("%Y-%m-%d %H:%M:%S",time.gmtime())

# Execution starts here
if __name__ == "__main__":
   main(sys.argv[1:])

