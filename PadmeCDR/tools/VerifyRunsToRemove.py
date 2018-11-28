#!/usr/bin/python -u

import os
import sys
import re
import time
import getopt
import subprocess

# User running CDR
cdr_user = os.environ['USER']

# Access information for DAQ data server
daq_user = "daq"
daq_keyfile = "/home/%s/.ssh/id_rsa_cdr"%cdr_user
daq_server_list = ["l1padme3","l1padme4"]

# Path to adler32 command on DAQ data server
daq_adler32_cmd = "/home/daq/DAQ/tools/adler32"

def set_year(year):

    global daq_path
    global cnaf_srm

    if (int(year) < 2018): return "error"

    # Path to top daq data directory on DAQ data server
    daq_path = "/data/DAQ/%s/rawdata"%year
    # SRM addresses for PADME DAQ data at LNF and at CNAF
    cnaf_srm = "srm://storm-fe-archive.cr.cnaf.infn.it:8444/srm/managerv2?SFN=/padmeTape/daq/%s/rawdata"%year

    return "ok"

def run_command(command):
    #print "> %s"%command
    p = subprocess.Popen(command,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,shell=True)
    return iter(p.stdout.readline,b'')

def get_checksum_cnaf(rawfile):
    a32 = ""
    cmd = "gfal-sum %s/%s adler32"%(cnaf_srm,rawfile);
    for line in run_command(cmd):
        try:
            (fdummy,a32) = line.rstrip().split()
        except:
            a32 = ""
    return a32

def get_checksum_daq(rawfile):
    a32 = ""
    cmd = "%s %s %s/%s"%(daq_ssh,daq_adler32_cmd,daq_path,rawfile)
    for line in run_command(cmd):
        try:
            (a32,fdummy) = line.rstrip().split()
        except:
            a32 = ""
    return a32

def print_help():
    print 'VerifyRunsToRemove [-s server] [-c] [-Y year] [-h]'
    print '  -s server  Select a single DAQ data server to check. Defalut: check both servers'
    print '  -c         Enable checksum verification of files (safer but VERY SLOW)'
    print '  -Y year    Look for data from given year (default: current year)'
    print '  -v         Run in verbose mode (repeat to increase verbose output)'
    print '  -h         Show this help message and exit'

def main(argv):

    # Make sure ssh command is correctly declared for everybody
    global daq_ssh

    try:
        opts,args = getopt.getopt(argv,"Y:s:cvh")
    except getopt.GetoptError:
        print_help()
        sys.exit(2)

    Checksum = False
    Year = time.strftime("%Y",time.gmtime())
    Selected_Server = ""
    Verbose = 0
    for opt,arg in opts:
        if opt == '-h':
            print_help()
            sys.exit()
        elif opt == '-c':
            Checksum = True
        elif opt == '-v':
            Verbose += 1
        elif opt == '-s':
            Selected_Server = arg
        elif opt == '-Y':
            Year = arg
        else:
            print "ERROR - Unknown option %s"%opt
            print_help()
            sys.exit(2)

    if (not set_year(Year) == "ok"):
        print "ERROR - Year %s is not allowed"%Year
        print_help()
        sys.exit(2)

    if (Selected_Server != "" and not Selected_Server in daq_server_list):
        print "ERROR - Server \'%s\' is unknown. Please choose in %s"%(Selected_Server,daq_server_list)
        print_help()
        sys.exit(2)

    for daq_server in daq_server_list:

        # Only run on required server (if defined)
        if (Selected_Server != "" and daq_server != Selected_Server): continue

        # SSH syntax to execute a command on the DAQ data server
        daq_ssh = "ssh -n -i %s -l %s %s"%(daq_keyfile,daq_user,daq_server)

        if Verbose: print "DEBUG - Checking server %s"%daq_server

        # Get list of runs on DAQ server
        run_list = []
        #cmd = "%s \'( cd %s; ls )\'"%(daq_ssh,daq_path)
        cmd = "%s \'( if [ -d %s ]; then cd %s; ls; fi )\'"%(daq_ssh,daq_path,daq_path)
        for line in run_command(cmd):
            run_list.append(line.rstrip())

        # Check if run was fully copied to CNAF
        for run in run_list:

            if Verbose: print "DEBUG - Server %s - Checking run %s"%(daq_server,run)

            all_at_cnaf = True

            # Get list of files for this run at CNAF
            cnaf_list = []
            for line in run_command("gfal-ls %s/%s"%(cnaf_srm,run)):
                if re.match("^gfal-ls error: ",line):
                    all_at_cnaf = False
                else:
                    cnaf_list.append(line.rstrip())

            # Run was not found at CNAF: do not remove
            if not all_at_cnaf:
                if Verbose: print "DEBUG - Run %s not found at CNAF"%run
                print "%s - %s - *** DO NOT REMOVE ***"%(daq_server,run)
                continue

            # Get list of files for this run on DAQ server
            daq_list = []
            cmd = "%s \'( cd %s; ls %s )\'"%(daq_ssh,daq_path,run)
            for line in run_command(cmd): daq_list.append(line.rstrip())

            # Check if all files are at CNAF
            for rawfile in daq_list:
                if (Verbose>2): print "DEBUG - Checking file %s"%rawfile
                if ( not rawfile in cnaf_list ):
                    if Verbose: print "DEBUG - File %s not found at CNAF"%rawfile
                    all_at_cnaf = False
                    if (Verbose<2): break
         
            # At least one file is not at CNAF: do not remove
            if not all_at_cnaf:
                print "%s - %s - *** DO NOT REMOVE ***"%(daq_server,run)
                continue
       
            # If checksum verification is not required, our job ends here
            if (not Checksum):
                # All is good: run can be removed form DAQ server
                print "%s - %s - can be removed"%(daq_server,run)
                continue

            # Check if all files have correct adler32 checksum
            for rawfile in daq_list:
                chksum_daq = get_checksum_daq("%s/%s"%(run,rawfile))
                chksum_cnaf = get_checksum_cnaf("%s/%s"%(run,rawfile))
                if (Verbose>2): print "DEBUG - File %s Checksum DAQ: \'%s\' CNAF: \'%s\'"%(rawfile,chksum_daq,chksum_cnaf)
                if ( chksum_daq == "" or chksum_cnaf == "" or chksum_daq != chksum_cnaf ):
                    # Checksum mismatch is BAD: issue a warning when found
                    print "### WARNING ### %s - Checksum mismatch (DAQ: \'%s\' - CNAF: \'%s\')"%(rawfile,chksum_daq,chksum_cnaf)
                    all_at_cnaf = False
                    if (Verbose<2): break

            # At least one file has wrong checksum: do not remove
            if not all_at_cnaf:
                print "%s - %s - *** DO NOT REMOVE ***"%(daq_server,run)
                continue

            # All is good: run can be removed form DAQ server
            print "%s - %s - can be removed"%(daq_server,run)

# Execution starts here
if __name__ == "__main__":
   main(sys.argv[1:])
