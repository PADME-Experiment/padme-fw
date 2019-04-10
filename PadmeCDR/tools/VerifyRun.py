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
    global lnf_srm
    global cnaf_srm

    if (int(year) < 2018): return "error"

    # Path to top daq data directory on DAQ data server
    daq_path = "/data/DAQ/%s/rawdata"%year
    # SRM addresses for PADME DAQ data at LNF and at CNAF
    lnf_srm = "srm://atlasse.lnf.infn.it:8446/srm/managerv2?SFN=/dpm/lnf.infn.it/home/vo.padme.org/daq/%s/rawdata"%year
    cnaf_srm = "srm://storm-fe-archive.cr.cnaf.infn.it:8444/srm/managerv2?SFN=/padmeTape/daq/%s/rawdata"%year

    return "ok"

def run_command(command):
    if Verbose: print "> %s"%command
    p = subprocess.Popen(command,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,shell=True)
    return iter(p.stdout.readline,b'')

def get_checksum_lnf(rawfile):
    a32 = ""
    cmd = "gfal-sum %s/%s adler32"%(lnf_srm,rawfile);
    for line in run_command(cmd):
        try:
            (fdummy,a32) = line.rstrip().split()
        except:
            a32 = ""
    return a32

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
    print 'VerifyRun -r run -s server [-c] [-Y year] [-h]'
    print '  -r run     Select run to verify'
    print '  -s server  Select DAQ data server where run is stored'
    print '  -Y year    Look for data from given year (default: current year)'
    print '  -v         Run in verbose mode'
    print '  -h         Show this help message and exit'

def main(argv):

    global daq_ssh
    global Verbose

    try:
        opts,args = getopt.getopt(argv,"r:Y:s:cvh")
    except getopt.GetoptError:
        print_help()
        sys.exit(2)

    Run = ""
    Server = ""
    Checksum = True
    Year = time.strftime("%Y",time.gmtime())
    Verbose = 0
    for opt,arg in opts:
        if opt == '-h':
            print_help()
            sys.exit()
        elif opt == '-c':
            Checksum = True
        elif opt == '-v':
            Verbose += 1
        elif opt == '-r':
            Run = arg
        elif opt == '-s':
            Server = arg
        elif opt == '-Y':
            Year = arg
        else:
            print "ERROR - Unknown option %s"%opt
            print_help()
            sys.exit(2)

    if (Run == ""):
        print "ERROR - Run was not specified"
        print_help()
        sys.exit(2)

    if (not set_year(Year) == "ok"):
        print "ERROR - Year %s is not allowed"%Year
        print_help()
        sys.exit(2)

    if (Server == ""):
        print "ERROR - Server not specified. Please choose in %s"%daq_server_list
        print_help()
        sys.exit(2)

    if (not Server in daq_server_list):
        print "ERROR - Server \'%s\' is unknown. Please choose in %s"%(Server,daq_server_list)
        print_help()
        sys.exit(2)

    # SSH syntax to execute a command on the DAQ data server
    daq_ssh = "ssh -n -i %s -l %s %s"%(daq_keyfile,daq_user,Server)

    if Verbose: print "DEBUG - Checking run %s on server %s"%(Run,Server)

    # Get list of files for this run on DAQ server
    daq_list = []
    cmd = "%s \'( cd %s; ls %s )\'"%(daq_ssh,daq_path,Run)
    for line in run_command(cmd):
        if re.match("^.*No such file or directory",line):
            print "ERROR - Run %s not found on DAQ server %s"%(Run,Server)
            sys.exit(2)
        else:
            daq_list.append(line.rstrip())

    # Get list of files for this run at LNF
    lnf_list = []
    cmd = "gfal-ls %s/%s"%(lnf_srm,Run)
    for line in run_command(cmd):
        if re.match("^gfal-ls error: ",line):
            if re.match("^.*No such file or directory",line):
                print "WARNING - Run %s not found at LNF"%Run
            else:
                print "ERROR - Unable to access LNF"
                print line.rstrip()
                sys.exit(2)
        else:
            lnf_list.append(line.rstrip())

    # Get list of files for this run at CNAF
    cnaf_list = []
    cmd = "gfal-ls %s/%s"%(cnaf_srm,Run)
    for line in run_command(cmd):
        if re.match("^gfal-ls error: ",line):
            if re.match("^.*No such file or directory",line):
                print "WARNING - Run %s not found at CNAF"%Run
            else:
                print "ERROR - Unable to access CNAF"
                print line.rstrip()
                sys.exit(2)
        else:
            cnaf_list.append(line.rstrip())

    # Check if all files are at CNAF
    for rawfile in daq_list:

        print "Checking file %s"%rawfile

        a32_daq = "undef"
        a32_lnf = "undef"
        a32_cnaf = "undef"

        a32_daq = get_checksum_daq("%s/%s"%(Run,rawfile))
        #if Verbose: print "DEBUG - File %s - DAQ checksum %s"%(rawfile,a32_daq)

        at_lnf = True
        if ( not rawfile in lnf_list ):
            at_lnf = False
            #if Verbose: print "DEBUG - File %s not found at LNF"%rawfile
        else:
            a32_lnf = get_checksum_lnf("%s/%s"%(Run,rawfile))
            #if Verbose: print "DEBUG - File %s - LNF checksum %s"%(rawfile,a32_lnf)

        at_cnaf = True
        if ( not rawfile in cnaf_list ):
            at_cnaf = False
            #if Verbose: print "DEBUG - File %s not found at CNAF"%rawfile
        else:
            a32_cnaf = get_checksum_cnaf("%s/%s"%(Run,rawfile))
            #if Verbose: print "DEBUG - File %s - CNAF checksum %s"%(rawfile,a32_cnaf)

        if Verbose: print "DEBUG - File %s - Checksums: DAQ \'%s\' LNF \'%s\' CNAF \'%s\'"%(rawfile,a32_daq,a32_lnf,a32_cnaf)

        if (a32_daq == ""): print "File %s - Unable to get DAQ checksum"%rawfile

        if (at_lnf):
            if (a32_lnf == ""):
                print "File %s - Unable to get LNF checksum"%rawfile
            elif (a32_daq != "" and a32_daq != a32_lnf):
                print "File %s - LNF checksum mismatch - DAQ %s - LNF %s"%(rawfile,a32_daq,a32_lnf)
        else:
            print "File %s - Not at LNF"%rawfile

        if (at_cnaf):
            if (a32_cnaf == ""):
                print "File %s - Unable to get CNAF checksum"%rawfile
            elif (a32_daq != "" and a32_daq != a32_cnaf):
                print "File %s - CNAF checksum mismatch - DAQ %s - CNAF %s"%(rawfile,a32_daq,a32_cnaf)
        else:
            print "File %s - Not at CNAF"%rawfile

# Execution starts here
if __name__ == "__main__":
   main(sys.argv[1:])
