#!/usr/bin/python

import os
import sys
import re
import getopt
import subprocess

# User running CDR
cdr_user = os.environ['USER']

############################
### DAQ data server data ###
############################

# Access information for DAQ data server
daq_user = "daq"
daq_keyfile = "/home/%s/.ssh/id_rsa_cdr"%cdr_user
daq_server_list = ["l1padme3","l1padme4"]

# Path to top daq data directory on DAQ data server
daq_path = "/data/DAQ/2018/rawdata"

# SRM addresses for PADME DAQ data at LNF and at CNAF
cnaf_srm = "srm://storm-fe-archive.cr.cnaf.infn.it:8444/srm/managerv2?SFN=/padmeTape/daq/2018/rawdata"

def run_command(command):
    p = subprocess.Popen(command,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,shell=True)
    return iter(p.stdout.readline,b'')

def check_file(rawfile):
    for line in run_command("gfal-ls %s/%s"%(cnaf_srm,rawfile)):
        if re.match("^gfal-ls error: ",line): return "error"
    return "ok"

def main(argv):

    for daq_server in daq_server_list:

        # SSH syntax to execute a command on the DAQ data server
        daq_ssh = "ssh -i %s -l %s %s"%(daq_keyfile,daq_user,daq_server)

        # Get list of runs in server
        run_list = []
        #print "Getting list of runs on server %s"%daq_server
        cmd = "%s \'( cd %s; ls )\'"%(daq_ssh,daq_path)
        for line in run_command(cmd):
            run_list.append(line.rstrip())

        # Check if run is at CNAF
        for run in run_list:
            all_at_cnaf = True

            # Get list of files for this run at CNAF
            cnaf_list = []
            for line in run_command("gfal-ls %s/%s"%(cnaf_srm,run)):
                if re.match("^gfal-ls error: ",line):
                    all_at_cnaf = False
                else:
                    cnaf_list.append(line.rstrip())

            # At least part of the run is at CNAF: check if all files are there
            if all_at_cnaf:
                # Get list of files on server
                cmd = "%s \'( cd %s; ls %s )\'"%(daq_ssh,daq_path,run)
                for line in run_command(cmd):
                    rawfile = line.rstrip()
                    if ( not rawfile in cnaf_list ):
                        #print "           %s is not at CNAF"%rawfile
                        all_at_cnaf = False
                        break

            if all_at_cnaf:
                print "%s - %s - can be removed"%(daq_server,run)
            else:
                print "%s - %s - *** DO NOT REMOVE ***"%(daq_server,run)

# Execution starts here
if __name__ == "__main__":
   main(sys.argv[1:])
