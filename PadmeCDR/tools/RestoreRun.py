#!/usr/bin/python -u

import os
import re
import sys
import time
import getopt
import subprocess

SRC_SITE_LIST = [ "LNF", "CNAF" ]
DST_SITE_LIST = [ "LNF", "CNAF" , "LOCAL"]

LNF_SRM = "srm://atlasse.lnf.infn.it:8446/srm/managerv2?SFN=/dpm/lnf.infn.it/home/vo.padme.org"
CNAF_SRM = "srm://storm-fe-archive.cr.cnaf.infn.it:8444/srm/managerv2?SFN=/padmeTape"

def print_help():
    print 'RestoreRun -R run_name [-S src_site] [-D dst_site] [-d dst_dir] [-Y year] [-h]'
    print '  -R run_name     Name of run to recover'
    print '  -S src_site     Source site. Default: CNAF. Available %s'%SRC_SITE_LIST
    print '  -D dst_site     Destination site. Default: LOCAL. Available %s'%DST_SITE_LIST
    print '  -d dst_dir      Destination directory'
    print '  -Y year         Specify year of data taking to copy. Default: year from run name'
    print '  -h              Show this help message and exit'

def main(argv):

    run = ""
    src_site = "CNAF"
    src_dir = ""
    dst_site = "LOCAL"
    dst_dir = ""
    year = ""
    src_srm = ""
    dst_srm = ""
    fake = False

    try:
        opts,args = getopt.getopt(argv,"R:S:D:d:Y:fh")
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
        elif opt == '-D':
            if (not arg in DST_SITE_LIST):
                print "ERROR - Invalid destination site %s"%arg
                print_help()
                sys.exit(2)
            dst_site = arg
        elif opt == '-d':
            dst_dir = arg
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

    if (src_site == dst_site):
        print "ERROR - Source and destination sites are the same: %s and %s"%(src_site,dst_site)
        print_help()
        sys.exit(2)

    # Define source and destination SRMs
    if (src_site == "LNF"):
        src_srm = LNF_SRM
    elif (src_site == "CNAF"):
        src_srm = CNAF_SRM
    if (dst_site == "LNF"):
        dst_srm = LNF_SRM
    elif (dst_site == "CNAF"):
        dst_srm = CNAF_SRM

    # Source dir is always the official directory of desired runs in given year
    src_dir = "/daq/%s/rawdata/%s"%(year,run)

    # If destination dir is not explicitly declared...
    if (not dst_dir):
        if (dst_site == "LNF" or dst_site == "CNAF"):
            # ...use official directory if using a storage system at LNF/CNAF
            dst_dir = src_dir
        else:
            # ...use subdir of current directory if LOCAL
            dst_dir = "%s/%s"%(os.getcwd(),run)

    if (dst_site == "LOCAL"):
        print "%s - Restoring run %s from %s to local dir %s"%(now_str(),run,src_site,dst_dir)
    else:
        print "%s - Restoring run %s from %s to %s"%(now_str(),run,src_site,dst_site)

    if fake: print "WARNING - Running in FAKE mode: no file will be copied"

    # This should be handled by -p option in gfal-copy
    ## Create directory on local area to hold run files
    #print "%s - Creating local dir %s"%(now_str(),dst_dir)
    #try:
    #    os.makedirs(dst_dir)
    #except OSError:
    #    print "WARNING - Directory %s already exists or cannot be created"%dst_dir

    file_list = []
    for line in run_command("gfal-ls %s%s"%(src_srm,src_dir)):
        if re.match("^gfal-ls error: ",line):
            print line.rstrip()
            print "***ERROR*** gfal-ls returned error status while retrieving file list from %s%s"%(src_srm,src_dir)
            sys.exit(2)
        file_list.append(line.rstrip())
    file_list.sort()

    print "%s - Starting recovery of run %s (%d files)"%(now_str(),run,len(file_list))
    warnings = 0
    for rawfile in file_list:
        print "%s - Copying file %s"%(now_str(),rawfile)
        if (dst_site == "LOCAL"):
            cmd = "gfal-copy -p -t 3600 -T 3600 %s%s/%s file://%s/%s"%(src_srm,src_dir,rawfile,dst_dir,rawfile);
        else:
            cmd = "gfal-copy -p --checksum ADLER32 -t 3600 -T 3600 %s%s/%s %s%s/%s"%(src_srm,src_dir,rawfile,dst_srm,dst_dir,rawfile);
        if fake:
            print "> %s"%cmd
        else:
            for line in run_command(cmd):
                print line.rstrip()
                if re.match("^gfal-copy error: ",line):
                    warnings += 1
                    print "WARNING - gfal-copy failed while copying file %s"%rawfile

    print "%s - Run %s recovered"%(now_str(),run)
    if warnings:
        print "WARNING - copy of %d files failed: please check log"%warnings

def run_command(command):
    print "> %s"%command
    p = subprocess.Popen(command,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,shell=True)
    return iter(p.stdout.readline, b'')

def now_str():
    return time.strftime("%Y-%m-%d %H:%M:%S",time.gmtime())

# Execution starts here
if __name__ == "__main__":
   main(sys.argv[1:])
