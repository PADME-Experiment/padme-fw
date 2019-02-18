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

def main(argv):

    run = ""
    src_site = "CNAF"
    data_dir = ""
    year = ""
    src_srm = ""
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

    # Checking stage status of each file
    warnings = 0
    files_to_stage_list = []
    print "%s - Checking staging status of files in run %s (%d files)"%(now_str(),run,len(file_list))
    for rawfile in file_list:
        print
        print "%s - Checking file %s"%(now_str(),rawfile)
        cmd = "gfal-xattr %s%s/%s user.status"%(src_srm,data_dir,rawfile)
        for line in run_command(cmd):
            status = line.rstrip()
            if re.match("^gfal-xattr error: ",status):
                warnings += 1
                print "WARNING - gfal-xattr failed while checking status of file %s"%rawfile
            elif (status == "ONLINE" or status == "ONLINE_AND_NEARLINE"):
                print "%s - File %s status is %s"%(now_str(),rawfile,status)
            else:
                print "%s - File %s status is %s: staging it"%(now_str(),rawfile,status)
                files_to_stage_list.append(rawfile)

    # Stage in all files which are not ONLINE
    for rawfile in files_to_stage_list:
        print
        print "%s - Staging file %s"%(now_str(),rawfile)
        cmd = "gfal-legacy-bringonline %s%s/%s"%(src_srm,data_dir,rawfile)
        if fake:
            print "> %s"%cmd
        else:
            for line in run_command(cmd):
                print line.rstrip()
                if re.match("^gfal-legacy-bringonline error: ",line):
                    warnings += 1
                    print "WARNING - gfal-legacy-bringonline failed while staging file %s"%rawfile

    # Re-check stage status of unstaged files after staging
    print "%s - Re-checking staging status of unstaged files in run %s (%d files)"%(now_str(),run,len(files_to_stage_list))
    for rawfile in files_to_stage_list:
        print
        print "%s - Checking file %s"%(now_str(),rawfile)
        cmd = "gfal-xattr %s%s/%s user.status"%(src_srm,data_dir,rawfile)
        for line in run_command(cmd):
            status = line.rstrip()
            if re.match("^gfal-xattr error: ",status):
                warnings += 1
                print "WARNING - gfal-xattr failed while checking status of file %s"%rawfile
            elif (status == "ONLINE" or status == "ONLINE_AND_NEARLINE"):
                print "%s - File %s status is %s"%(now_str(),rawfile,status)
            else:
                print "%s - WARNING - File %s status is still %s"%(now_str(),rawfile,status)
                files_to_stage_list.append(rawfile)

    print
    if warnings:
        print "%s - WARNING - Run %s may not be fully staged: %d files failed checking and/or staging"%(now_str(),run,warnings)
    else:
        print "%s - Run %s is now fully staged: you can use it"%(now_str(),run)

def run_command(command):
    print "> %s"%command
    p = subprocess.Popen(command,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,shell=True)
    return iter(p.stdout.readline, b'')

def now_str():
    return time.strftime("%Y-%m-%d %H:%M:%S",time.gmtime())

# Execution starts here
if __name__ == "__main__":
   main(sys.argv[1:])
