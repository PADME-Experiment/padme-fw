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
color_warn  = "#FFA500"
color_alarm = "#CC0000"
color_off   = "#0000CC"

# Timeline files
timeline_storage = [
    "padmeui",
    "l1padme3",
    "l1padme4",
    "l0padme1",
    "lnfdisk",
    "cnaftape",
    "kloetape",
    "kloedisk"
]
timeline_file = {
    "padmeui" :"%s/log/timeline_padmeui.log"%cdr_dir,
    "l1padme3":"%s/log/timeline_l1padme3.log"%cdr_dir,
    "l1padme4":"%s/log/timeline_l1padme4.log"%cdr_dir,
    "l0padme1":"%s/log/timeline_l0padme1.log"%cdr_dir,
    "lnfdisk" :"%s/log/timeline_lnfdisk.log"%cdr_dir,
    "cnaftape":"%s/log/timeline_cnaftape.log"%cdr_dir,
    "kloetape":"%s/log/timeline_kloetape.log"%cdr_dir,
    "kloedisk":"%s/log/timeline_kloedisk.log"%cdr_dir
}

####################
### padmeui data ###
####################

# Path for root filesystem device (needed for df)
pui_root_fs = "/dev/mapper/centos_l0padme2-root"

# Warning and alarm levels (in %) for padmeui
pui_level_warn = 60
pui_level_alarm = 85

#############################
### DAQ data servers data ###
#############################

# Access information for DAQ data server
daq_server_list = ("l1padme3","l1padme4","l0padme1")
daq_user = "daq"
daq_keyfile = "/home/%s/.ssh/id_rsa_cdr"%cdr_user

# Path to top disk mount point on DAQ data server
daq_path = "/data"

# Warning and alarm levels (in %) for DAQ disk servers
daq_level_warn = 60
daq_level_alarm = 85

################################
### LNF disk occupation data ###
################################

# Path to file with summary occupation info
#lnf_summary_file = "/home/%s/du-padme_dpm.ouput"%cdr_user
lnf_summary_file = "/home/%s/du-padme/padme_spazio-occupato.output"%cdr_user

# Total available space in TB
lnf_disk_tot_TB = 80.

#################################
### CNAF tape occupation data ###
#################################

# Path to file with summary occupation info
cnaf_summary_file = "/home/%s/du-padme/cnaf_spazio-occupato.output"%cdr_user

# Total available space in TB
cnaf_tape_tot_TB = 500.

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

# Total available tape space in TB
kloe_tape_tot_TB = 350.

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

def get_pui_info():

    disk_total = "0"
    disk_used  = "0"
    disk_avail = "0"
    disk_usepc = "0"
    cmd = "/bin/df -BG --output=size,used,avail,pcent %s"%pui_root_fs
    for line in run_command(cmd):
        #print line.rstrip()
        #rc = re.match("^\s*(\S+)G\s+(\S+)G\s+(\S+)G\s+(\S+)%%.*$",line)
        rc = re.match("^\s*(\S+)G\s+(\S+)G\s+(\S+)G\s+(\d+).*",line)
        if rc:
            disk_total = rc.group(1)
            disk_used  = rc.group(2)
            disk_avail = rc.group(3)
            disk_usepc = rc.group(4)
    return (disk_total,disk_used,disk_avail,disk_usepc,)

def get_daq_info(server):

    daq_ssh = "ssh -i %s -l %s %s"%(daq_keyfile,daq_user,server)

    disk_total = "0"
    disk_used  = "0"
    disk_avail = "0"
    disk_usepc = "0"
    #cmd = "%s df -m %s"%(daq_ssh,daq_path)
    cmd = "%s /bin/df -BG --output=size,used,avail,pcent %s"%(daq_ssh,daq_path)
    for line in run_command(cmd):
        #print line.rstrip()
        #rc = re.match("^\S+\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)%%\s+%s\s*$"%daq_path,line.rstrip())
        rc = re.match("^\s*(\S+)G\s+(\S+)G\s+(\S+)G\s+(\d+).*",line)
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

def get_lnf_info():

    disk_use = "0"
    #cmd = "tail -2 %s"%lnf_summary_file
    #for line in run_command(cmd):
    #    rc = re.match("^\s*(\d+)B\s*$",line)
    #    if rc: disk_use = rc.group(1)
    cmd = "tail -1 %s"%lnf_summary_file
    for line in run_command(cmd):
        rc = re.match("^\s*(\d\d\d\d\d\d\d\d)_(\d\d\d\d)\s+(\d+)\s*$",line)
        if rc:
            read_date = rc.group(1)
            read_time = rc.group(2)
            disk_use = rc.group(3)
    return disk_use

def get_cnaf_info():

    tape_use = "0"
    cmd = "tail -1 %s"%cnaf_summary_file
    for line in run_command(cmd):
        rc = re.match("^\s*(\d\d\d\d\d\d\d\d)_(\d\d\d\d)\s+(\d+)\s*$",line)
        if rc:
            read_date = rc.group(1)
            read_time = rc.group(2)
            tape_use = rc.group(3)
    return tape_use

def append_timeline_info(storage,now,data_list):
    with open(timeline_file[storage],"a") as tlf:
        tlf.write("%s"%now)
        for d in data_list: tlf.write(" %s"%d)
        tlf.write("\n")

def format_timeline_info(storage,mode):
    fmt = "["
    first = True
    with open(timeline_file[storage],"r") as tlf:
        for l in tlf:
            m = re.match("^(\S+) (\S+) (\S+) (\S+)",l)
            if m:
                if first:
                    first = False
                else:
                    fmt += ","
                if mode == "PERCENT":
                    fmt += "[\"%s\",%s]"%(m.group(1),m.group(4))
                elif mode == "USED":
                    fmt += "[\"%s\",%s]"%(m.group(1),m.group(2))
                elif mode == "FREE":
                    fmt += "[\"%s\",%s]"%(m.group(1),str(int(m.group(3))-int(m.group(2))))
    fmt += "]"
    return fmt

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

        now_time = time.time()

        mh = open("/tmp/%s"%monitor_file,"w")
        mh.write("PLOTID CDR_status\n")
        mh.write("PLOTNAME PADME CDR Status - %s\n"%now_str())
        mh.write("PLOTTYPE activetext\n")
        mh.write("DATA [ ")

        ### Get DAQ servers disk info ###
        for daq_server in daq_server_list:
            (daq_tot,daq_use,daq_avl,daq_opc) = get_daq_info(daq_server)
            #daq_tot_TB = float(daq_tot)/1024/1024
            #daq_use_TB = float(daq_use)/1024/1024
            #daq_avl_TB = float(daq_avl)/1024/1024
            daq_color = color_ok
            if (int(daq_opc)>daq_level_warn): daq_color = color_warn
            if (int(daq_opc)>daq_level_alarm): daq_color = color_alarm
            #mh.write("{\"title\":\"%s\",\"current\":{\"value\":\"Used:%4.1f TB of %4.1f TB (%s%%)\",\"col\":\"%s\"}}"%(daq_server,daq_use_TB,daq_tot_TB,daq_opc,daq_color))
            mh.write("{\"title\":\"%s\",\"current\":{\"value\":\"Used:%s GB of %s GB (%s%%)\",\"col\":\"%s\"}}"%(daq_server,daq_use,daq_tot,daq_opc,daq_color))
            append_timeline_info(daq_server,now_time,(daq_use,daq_tot,daq_opc))

            mh.write(",")

        ### Get LNF disk system info ###
        lnf_disk_use = get_lnf_info()
        lnf_disk_use_TB = float(lnf_disk_use)/1024/1024/1024/1024
        lnf_disk_opc = str(int(100.*lnf_disk_use_TB/lnf_disk_tot_TB))
        lnf_disk_color = color_ok
        if lnf_disk_use_TB > lnf_disk_tot_TB: lnf_disk_color = color_warn
        mh.write("{\"title\":\"LNF Disk\",\"current\":{\"value\":\"Used:%6.1f TB of %6.1f TB (%s%%)\",\"col\":\"%s\"}}"%(lnf_disk_use_TB,lnf_disk_tot_TB,lnf_disk_opc,lnf_disk_color))
        append_timeline_info("lnfdisk",now_time,(lnf_disk_use_TB,lnf_disk_tot_TB,lnf_disk_opc))

        mh.write(",")

        ### Get CNAF tape library info ###
        cnaf_tape_use = get_cnaf_info()
        cnaf_tape_use_TB = float(cnaf_tape_use)/1024/1024/1024/1024
        cnaf_tape_opc = str(int(100.*cnaf_tape_use_TB/cnaf_tape_tot_TB))
        cnaf_tape_color = color_ok
        if cnaf_tape_use_TB > cnaf_tape_tot_TB: cnaf_tape_color = color_warn
        mh.write("{\"title\":\"CNAF Tape\",\"current\":{\"value\":\"Used:%6.1f TB of %6.1f TB (%s%%)\",\"col\":\"%s\"}}"%(cnaf_tape_use_TB,cnaf_tape_tot_TB,cnaf_tape_opc,cnaf_tape_color))
        append_timeline_info("cnaftape",now_time,(cnaf_tape_use_TB,cnaf_tape_tot_TB,cnaf_tape_opc))

        mh.write(",")

        ### Get KLOE tape library info ###
        (kloe_tape_use,kloe_disk_tot,kloe_disk_avl,kloe_disk_opc) = get_kloe_info()

        kloe_tape_use_TB = float(kloe_tape_use)
        kloe_tape_opc = str(int(100.*kloe_tape_use_TB/kloe_tape_tot_TB))
        kloe_tape_color = color_ok
        if kloe_tape_use_TB > kloe_tape_tot_TB: kloe_tape_color = color_warn
        mh.write("{\"title\":\"KLOE Tape\",\"current\":{\"value\":\"Used:%6.1f TB of %6.1f TB (%s%%)\",\"col\":\"%s\"}}"%(kloe_tape_use_TB,kloe_tape_tot_TB,kloe_tape_opc,kloe_tape_color))
        append_timeline_info("kloetape",now_time,(kloe_tape_use_TB,kloe_tape_tot_TB,kloe_tape_opc))

        mh.write(",")

        kloe_disk_tot_TB = float(kloe_disk_tot)/1024/1024
        kloe_disk_avl_TB = float(kloe_disk_avl)/1024/1024
        kloe_disk_use_TB = kloe_disk_tot_TB-kloe_disk_avl_TB
        kloe_disk_color = color_ok
        if (int(kloe_disk_opc)>daq_level_warn):  kloe_disk_color = color_warn
        if (int(kloe_disk_opc)>daq_level_alarm): kloe_disk_color = color_alarm
        mh.write("{\"title\":\"KLOE Disk\",\"current\":{\"value\":\"Used:%6.1f TB of %6.1f TB (%s%%)\",\"col\":\"%s\"}}"%(kloe_disk_use_TB,kloe_disk_tot_TB,kloe_disk_opc,kloe_disk_color))
        append_timeline_info("kloedisk",now_time,(kloe_disk_use_TB,kloe_disk_tot_TB,kloe_disk_opc))

        mh.write(",")

        ### Get PADMEUI disk info ###
        (pui_tot,pui_use,pui_avl,pui_opc) = get_pui_info()
        pui_color = color_ok
        if (int(pui_opc)>pui_level_warn): pui_color = color_warn
        if (int(pui_opc)>pui_level_alarm): pui_color = color_alarm
        mh.write("{\"title\":\"PADMEUI Disk\",\"current\":{\"value\":\"Used:%s GB of %s GB (%s%%)\",\"col\":\"%s\"}}"%(pui_use,pui_tot,pui_opc,pui_color))
        append_timeline_info("padmeui",now_time,(pui_use,pui_tot,pui_opc))

        #mh.write(",")

        mh.write(" ]\n")

        mh.write("\n")

        mh.write("PLOTID CDR_DAQ_timeline\n")
        mh.write("PLOTNAME PADME CDR - DAQ Servers - %s\n"%now_str())
        mh.write("PLOTTYPE timeline\n")
        mh.write("TITLE_X Time\n")
        mh.write("TITLE_Y Occupation(%)\n")
        mh.write("RANGE_Y 0. 100.\n")
        mh.write("MODE [ \"lines\" , \"lines\" , \"lines\" ]\n")
        mh.write("COLOR [ \"ff0000\" , \"0000ff\" , \"00ff00\" ]\n")
        mh.write("LEGEND [ \"l1padme3\" , \"l1padme4\" , \"l0padme1\" ]\n")
        mh.write("DATA [ ")
        mh.write(format_timeline_info("l1padme3","PERCENT"))
        mh.write(" , ")
        mh.write(format_timeline_info("l1padme4","PERCENT"))
        mh.write(" , ")
        mh.write(format_timeline_info("l0padme1","PERCENT"))
        mh.write("]\n")

        mh.write("\n")

        mh.write("PLOTID CDR_Tape_timeline\n")
        mh.write("PLOTNAME PADME CDR - Storage - %s\n"%now_str())
        mh.write("PLOTTYPE timeline\n")
        mh.write("TITLE_X Time\n")
        mh.write("TITLE_Y Occupation(TB)\n")
        mh.write("RANGE_Y 0. 500.\n")
        mh.write("MODE [ \"lines\" , \"lines\" , \"lines\" ]\n")
        mh.write("COLOR [ \"ff0000\" , \"0000ff\" , \"00ff00\" ]\n")
        mh.write("LEGEND [ \"LNF Disk\" , \"CNAF Tape\" , \"KLOE Tape\" ]\n")
        mh.write("DATA [ ")
        mh.write(format_timeline_info("lnfdisk","USED"))
        mh.write(" , ")
        mh.write(format_timeline_info("cnaftape","USED"))
        mh.write(" , ")
        mh.write(format_timeline_info("kloetape","USED"))
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

