#!/usr/bin/python -u

import os
import re
import sys
import time
import getopt
import subprocess

SITE_LIST = [ "LNF", "CNAF" ]

LNF_SRM = "srm://atlasse.lnf.infn.it:8446/srm/managerv2?SFN=/dpm/lnf.infn.it/home/vo.padme.org"
CNAF_SRM = "srm://storm-fe-archive.cr.cnaf.infn.it:8444/srm/managerv2?SFN=/padmeTape"

LOCAL_AREA = "/data05/padme/leonardi/rawdata"

def print_help():
    print 'RecoverRun -R run_name [-S src_site] [-Y year] [-D dst_dir] [-h]'
    print '  -R run_name     Name of run to recover'
    print '  -S src_site     Source site. Default: CNAF. Available %s'%SITE_LIST
    print '  -Y year         Specify year of data taking to copy. Default: year from run name'
    print '  -D dst_dir      Destination directory. Default: %s'%LOCAL_AREA
    print '  -h              Show this help message and exit'

def main(argv):

    srm = CNAF_SRM
    dst_area = LOCAL_AREA
    run = ""
    year = ""

    try:
        opts,args = getopt.getopt(argv,"R:S:Y:D:h")
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
            if (arg == "LNF"):
                srm = LNF_SRM
            elif (arg == "CNAF"):
                srm = CNAF_SRM
            else:
                print "ERROR - Unknown site %s"%arg
                print_help()
                sys.exit(2)
        elif opt == '-Y':
            year = arg
        elif opt == '-D':
            dst_area = arg

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

    src_dir = "/daq/%s/rawdata/%s"%(year,run)
    dst_dir = "%s/%s"%(dst_area,run)

    print "%s - Restoring run %s from %s%s to %s"%(now_str(),run,srm,src_dir,dst_dir)

    # Create directory on local area to hold run files
    print "%s - Creating local dir %s"%(now_str(),dst_dir)
    try:
        os.makedirs(dst_dir)
    except OSError:
        print "WARNING - Directory %s already exists or cannot be created"%dst_dir

    file_list = []
    for line in run_command("gfal-ls %s%s"%(srm,src_dir)):
        if re.match("^gfal-ls error: ",line):
            print "***ERROR*** gfal-ls returned error status while retrieving file list from %s%s"%(srm,src_dir)
            sys.exit(2)
        file_list.append(line.rstrip())
    file_list.sort()

    print "%s - Starting recovery of run %s (%d files)"%(now_str(),run,len(file_list))
    for rawfile in (file_list):
        print "%s - Copying file %s"%(now_str(),rawfile)
        cmd = "gfal-copy -p -t 3600 -T 3600 %s%s/%s file://%s/%s"%(srm,src_dir,rawfile,dst_dir,rawfile);
        for line in run_command(cmd): print(line.rstrip())

    print "%s - Run %s recovered"%(now_str(),run)

def run_command(command):
    print("> %s"%command)
    p = subprocess.Popen(command,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,shell=True)
    return iter(p.stdout.readline, b'')

def now_str():
    return time.strftime("%Y-%m-%d %H:%M:%S",time.gmtime())

# Execution starts here
if __name__ == "__main__":
   main(sys.argv[1:])
