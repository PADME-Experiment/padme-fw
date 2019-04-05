#!/usr/bin/python -u

import os
import re
import sys
import time
import getopt
import subprocess

SRC_SITE_LIST = [ "LNF", "CNAF" ]

LNF_SRM = "srm://atlasse.lnf.infn.it:8446/srm/managerv2?SFN=/dpm/lnf.infn.it/home/vo.padme.org"
CNAF_SRM = "srm://storm-fe-archive.cr.cnaf.infn.it:8444/srm/managerv2?SFN=/padmeTape"

def print_help():
    print 'PreStageRun -R run_name [-S src_site] [-Y year] [-h]'
    print '  -R run_name     Name of run to pre-stage'
    print '  -S src_site     Source site. Default: CNAF. Available %s'%SRC_SITE_LIST
    print '  -Y year         Specify year of data taking. Default: year from run name'
    print '  -h              Show this help message and exit'

def run_command(command):
    #print "> %s"%command
    p = subprocess.Popen(command,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,shell=True)
    return iter(p.stdout.readline, b'')

def now_str():
    return time.strftime("%Y-%m-%d %H:%M:%S",time.gmtime())

def check_status(ffile):
    cmd = "gfal-xattr %s user.status"%ffile
    for line in run_command(cmd):
        status = line.rstrip()
        if re.match("^gfal-xattr error: ",status):
            print "WARNING - gfal-xattr failed while checking status of file %s"%ffile
            return "ERROR"
        elif (status == "ONLINE" or status == "ONLINE_AND_NEARLINE"):
            return "ONLINE"
        else:
            return "STAGING"

def stage_in(file):

    cmd = "gfal-legacy-bringonline --timeout 0 %s"%file
    try:
        rc = subprocess.check_output(cmd.split(),stderr=subprocess.STDOUT)
    except subprocess.CalledProcessError as e:
        timeout = False
        for el in e.output.split("\n"):
            if re.match("^Exception: Timeout expired while polling",el): timeout = True
        if timeout:
            rc = "TIMEOUT"
        else:
            rc = e.output

    if rc != "TIMEOUT":
        print "WARNING - gfal-legacy-bringonline did not return a timeout"
        print rc
        return "ERROR"

    return "STAGING"

def main(argv):

    run = ""
    src_site = "CNAF"
    data_dir = ""
    year = ""
    src_srm = ""
    timeout = 96800
    fake = False

    try:
        opts,args = getopt.getopt(argv,"R:S:Y:fh")
    except getopt.GetoptError:
        print_help()
        sys.exit(2)

    for opt,arg in opts:
        if opt == '-h':
            print_help()
            sys.exit()
        elif opt == '-R':
            run = arg
        elif opt == '-S':
            if (not arg in SRC_SITE_LIST):
                print "ERROR - Invalid source site %s"%arg
                print_help()
                sys.exit(2)
            src_site = arg
        elif opt == '-Y':
            year = arg
        elif opt == '-f':
            fake = True

    if (not run):
        print "ERROR - No run name specified"
        print_help()
        sys.exit(2)

    if (not year):
        m = re.match("run_\d+_(\d\d\d\d)\d\d\d\d_\d\d\d\d\d\d",run)
        if m:
            year = m.group(1)
        else:
            print "ERROR - No year specified and unable to extract year from run name %s"%run
            print_help()
            sys.exit(2)

    # Define source and destination SRMs
    if (src_site == "LNF"):
        src_srm = LNF_SRM
    elif (src_site == "CNAF"):
        src_srm = CNAF_SRM

    # Source dir is always the official directory for all runs of given year
    data_dir = "/daq/%s/rawdata/%s"%(year,run)

    if fake: print "WARNING - Running in FAKE mode: no file will be checked but not staged"

    # Getting list of files for this run
    file_list = []
    for line in run_command("gfal-ls %s%s"%(src_srm,data_dir)):
        if re.match("^gfal-ls error: ",line):
            print line.rstrip()
            print "***ERROR*** gfal-ls returned error status while retrieving file list from %s%s"%(src_srm,data_dir)
            sys.exit(2)
        file_list.append(line.rstrip())
    file_list.sort()

    # Prepare list of full names
    fullname = {}
    for rawfile in file_list:
        fullname[rawfile] = "%s%s/%s"%(src_srm,data_dir,rawfile)

    # Checking stage status of each file
    warnings = 0
    status = {}
    print "%s - Checking staging status of files in run %s (%d files)"%(now_str(),run,len(file_list))
    for rawfile in file_list:
        status[rawfile] = check_status(fullname[rawfile])
        if status[rawfile] == "STAGING":
            status[rawfile] = stage_in(fullname[rawfile])
        print "%s - Status of %s is %s"%(now_str(),rawfile,status[rawfile])

    n = { "ONLINE": 0, "STAGING": 0, "ERROR": 0 }
    start_time = time.time()
    while True:
        n["ONLINE"]  = 0
        n["STAGING"] = 0
        n["ERROR"]   = 0
        for rawfile in file_list:
            if status[rawfile] == "STAGING":
                status[rawfile] = check_status(fullname[rawfile])
            n[status[rawfile]] += 1
        if n["STAGING"] == 0:
            print "%s - No files are in STAGING mode: exiting"%now_str()
            break
        if time.time()-start_time > timeout:
            print "%s - Timeout of %d seconds expired: exiting"%(now_str(),timeout)
            break
        print "%s - ONLINE %4d STAGING %4d ERROR %4d"%(now_str(),n["ONLINE"],n["STAGING"],n["ERROR"])
        time.sleep(60)

    print "= Final report ="
    for status in ( "ONLINE","STAGING","ERROR"):
        print "%-7s %4d"%(status,n[status])

# Execution starts here
if __name__ == "__main__":
   main(sys.argv[1:])
