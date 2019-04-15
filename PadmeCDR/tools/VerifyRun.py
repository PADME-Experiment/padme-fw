#!/usr/bin/python -u

import os
import re
import sys
import time
import getopt
import subprocess

# List of available sites
SITE_LIST = [ "LNF", "LNF2", "CNAF" , "KLOE" , "DAQ" ]

# User running CDR
CDR_USER = os.environ['USER']

# Access information for DAQ data server
DAQ_USER = "daq"
DAQ_KEYFILE = "/home/%s/.ssh/id_rsa_cdr"%CDR_USER
DAQ_SERVERS = [ "l1padme3", "l1padme4" ]

# Access information for KLOE tape library
KLOE_SERVER = "fibm15"
KLOE_USER = "pdm"
KLOE_KEYFILE = "/home/%s/.ssh/id_rsa_cdr"%CDR_USER

# Path to adler32 command on DAQ data server
DAQ_ADLER32_CMD = "/home/daq/DAQ/tools/adler32"

# SRM addresses for storage elements at LNF and CNAF
LNF_SRM = "srm://atlasse.lnf.infn.it:8446/srm/managerv2?SFN=/dpm/lnf.infn.it/home/vo.padme.org"
LNF2_SRM = "srm://atlasse.lnf.infn.it:8446/srm/managerv2?SFN=/dpm/lnf.infn.it/home/vo.padme.org_scratch"
CNAF_SRM = "srm://storm-fe-archive.cr.cnaf.infn.it:8444/srm/managerv2?SFN=/padmeTape"

def print_help():
    print 'VerifyRun -R run_name [-S src_site] [-D dst_site] [-s daq_server] [-Y year] [-c] [-v] [-h]'
    print '  -R run_name     Name of run to verify'
    print '  -S src_site     Source site. Default: CNAF. Available %s'%SITE_LIST
    print '  -D dst_site     Destination site. Default: LNF. Available %s'%SITE_LIST
    print '  -s daq_server   Name of data server if src_site or dst_site is DAQ. Available %s'%DAQ_SERVERS
    print '  -Y year         Specify year of data taking. Default: year from run name'
    print '  -c              Enable checksum verification (very time consuming!)'
    print '  -v              Enable verbose mode (repeat to increase level)'
    print '  -h              Show this help message and exit'

def get_checksum_lnf(file,year):
    a32 = ""
    path = "/daq/%s/rawdata/%s"%(year,file)
    cmd = "gfal-sum %s%s adler32"%(LNF_SRM,path);
    for line in run_command(cmd):
        try:
            (fdummy,a32) = line.rstrip().split()
        except:
            a32 = ""
    return a32

def get_checksum_lnf2(file,year):
    a32 = ""
    path = "/daq/%s/rawdata/%s"%(year,file)
    cmd = "gfal-sum %s%s adler32"%(LNF2_SRM,path);
    for line in run_command(cmd):
        try:
            (fdummy,a32) = line.rstrip().split()
        except:
            a32 = ""
    return a32

def get_checksum_cnaf(file,year):
    a32 = ""
    path = "/daq/%s/rawdata/%s"%(year,file)
    cmd = "gfal-sum %s%s adler32"%(CNAF_SRM,path);
    for line in run_command(cmd):
        try:
            (fdummy,a32) = line.rstrip().split()
        except:
            a32 = ""
    return a32

def get_checksum_daq(file,year,server):
    a32 = ""
    path = "/data/DAQ/%s/rawdata/%s"%(year,file)
    cmd = "ssh -n -i %s -l %s %s %s %s"%(DAQ_KEYFILE,DAQ_USER,server,DAQ_ADLER32_CMD,path)
    for line in run_command(cmd):
        try:
            (a32,fdummy) = line.rstrip().split()
        except:
            a32 = ""
    return a32

def get_file_list_daq(run,year,server):
    file_list = []
    file_size = {}
    missing = False
    run_dir = "/data/DAQ/%s/rawdata/%s"%(year,run)
    daq_ssh = "ssh -n -i %s -l %s %s"%(DAQ_KEYFILE,DAQ_USER,server)
    cmd = "%s \'( cd %s; ls -l )\'"%(daq_ssh,run_dir)
    for line in run_command(cmd):
        if ( re.match("^ls: cannot access ",line) ):
            missing = True
            break
        m = re.match("^\s*\S+\s+\S+\s+\S+\s+\S+\s+(\d+)\s+\S+\s+\S+\s+\S+\s+(\S+)\s*$",line.rstrip())
        if (m):
            file_list.append(m.group(2))
            file_size[m.group(2)] = int(m.group(1))
    return (missing,file_list,file_size)

def get_file_list_cnaf(run,year):
    file_list = []
    file_size = {}
    missing = False
    run_dir = "/daq/%s/rawdata/%s"%(year,run)
    cmd = "gfal-ls -l %s%s"%(CNAF_SRM,run_dir)
    for line in run_command(cmd):
        if ( re.match("^gfal-ls error: ",line) ):
            missing = True
            break
        m = re.match("^\s*\S+\s+\S+\s+\S+\s+\S+\s+(\d+)\s+\S+\s+\S+\s+\S+\s+(\S+)\s*$",line.rstrip())
        if (m):
            file_list.append(m.group(2))
            file_size[m.group(2)] = int(m.group(1))
    return (missing,file_list,file_size)

def get_file_list_lnf(run,year):
    file_list = []
    file_size = {}
    missing = False
    run_dir = "/daq/%s/rawdata/%s"%(year,run)
    cmd = "gfal-ls -l %s%s"%(LNF_SRM,run_dir)
    for line in run_command(cmd):
        if ( re.match("^gfal-ls error: ",line) ):
            missing = True
            break
        m = re.match("^\s*\S+\s+\S+\s+\S+\s+\S+\s+(\d+)\s+\S+\s+\S+\s+\S+\s+(\S+)\s*$",line.rstrip())
        if (m):
            file_list.append(m.group(2))
            file_size[m.group(2)] = int(m.group(1))
    return (missing,file_list,file_size)

def get_file_list_lnf2(run,year):
    file_list = []
    file_size = {}
    missing = False
    run_dir = "/daq/%s/rawdata/%s"%(year,run)
    cmd = "gfal-ls -l %s%s"%(LNF2_SRM,run_dir)
    for line in run_command(cmd):
        if ( re.match("^gfal-ls error: ",line) ):
            missing = True
            break
        m = re.match("^\s*\S+\s+\S+\s+\S+\s+\S+\s+(\d+)\s+\S+\s+\S+\s+\S+\s+(\S+)\s*$",line.rstrip())
        if (m):
            file_list.append(m.group(2))
            file_size[m.group(2)] = int(m.group(1))
    return (missing,file_list,file_size)

def get_file_list_kloe(run,year):
    file_list = []
    file_size = {}
    missing = False
    run_dir = "/data/DAQ/%s/rawdata/%s"%(year,run)
    kloe_ssh = "ssh -n -i %s -l %s %s"%(KLOE_KEYFILE,KLOE_USER,KLOE_SERVER)
    cmd = "%s \'( dsmc query archive /pdm/padme/daq/%s/rawdata/%s/\*.root )\'"%(kloe_ssh,year,run)
    for line in run_command(cmd):
        if ( re.match("^ANS1092W No files matching search criteria were found",line) ):
            missing = True
            break
        #m = re.match("^\s*(\S+)\s+\S+\s+\S+\s+\S+\s+(\S+)\s+.*$",line.rstrip())
        m = re.match("^\s*([0-9,]+)\s+\S+\s+\S+\s+\S+\s+(\S+)\s+.*$",line.rstrip())
        if (m):
            #print "Match %s %s - %s\n"%(m.group(1),m.group(2),line.rstrip())
            file_name = os.path.basename(m.group(2))
            file_list.append(file_name)
            file_size[file_name] = int(m.group(1).replace(',',''))
    return (missing,file_list,file_size)

def main(argv):

    run = ""
    src_site = "CNAF"
    src_string = ""
    dst_site = "LNF"
    dst_string = ""
    daq_srv = ""
    year = ""
    checksum = False
    verbose = 0

    try:
        opts,args = getopt.getopt(argv,"R:S:D:s:Y:cvh")
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
            if (not arg in SITE_LIST):
                print "ERROR - Invalid source site %s"%arg
                print_help()
                sys.exit(2)
            src_site = arg
        elif opt == '-D':
            if (not arg in SITE_LIST):
                print "ERROR - Invalid destination site %s"%arg
                print_help()
                sys.exit(2)
            dst_site = arg
        elif opt == '-s':
            if (not arg in DAQ_SERVERS):
                print "ERROR - Invalid DAQ data server %s"%arg
                print_help()
                sys.exit(2)
            daq_srv = arg
        elif opt == '-Y':
            year = arg
        elif opt == '-c':
            checksum = True
        elif opt == '-v':
            verbose += 1

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

    if ( (src_site == "DAQ" or dst_site == "DAQ") and daq_srv == "" ):
        print "ERROR - One site is DAQ but no data server specified"
        print_help()
        sys.exit(2)

    if ( (src_site == "KLOE" or dst_site == "KLOE") and checksum ):
        print "WARNING - KLOE site does not support checksum verification: switching checksum off"
        checksum = False

    # Define string to use to respresent sites
    src_string = src_site
    if (src_site == "DAQ"): src_string += "(%s)"%daq_srv
    dst_string = dst_site
    if (dst_site == "DAQ"): dst_string += "(%s)"%daq_srv

    if verbose:
        print
        print "=== VerifyRun %s between %s and %s ==="%(run,src_string,dst_string)
        if checksum:
            print "WARNING - Checksum is enabled: verification will take a long time..."

    # Get list of files at source site
    if (src_site == "DAQ"):
        (src_missing,src_file_list,src_file_size) = get_file_list_daq(run,year,daq_srv)
    elif (src_site == "LNF"):
        (src_missing,src_file_list,src_file_size) = get_file_list_lnf(run,year)
    elif (src_site == "LNF2"):
        (src_missing,src_file_list,src_file_size) = get_file_list_lnf2(run,year)
    elif (src_site == "CNAF"):
        (src_missing,src_file_list,src_file_size) = get_file_list_cnaf(run,year)
    elif (src_site == "KLOE"):
        (src_missing,src_file_list,src_file_size) = get_file_list_kloe(run,year)
    if verbose:
        if src_missing:
            print "%s - at %-13s run %s is missing"%(now_str(),src_string,run)
        else:
            print "%s - at %-13s run %s contains %d files"%(now_str(),src_string,run,len(src_file_list))

    # Get list of files at destination site
    if (dst_site == "DAQ"):
        (dst_missing,dst_file_list,dst_file_size) = get_file_list_daq(run,year,daq_srv)
    elif (dst_site == "LNF"):
        (dst_missing,dst_file_list,dst_file_size) = get_file_list_lnf(run,year)
    elif (dst_site == "LNF2"):
        (dst_missing,dst_file_list,dst_file_size) = get_file_list_lnf2(run,year)
    elif (dst_site == "CNAF"):
        (dst_missing,dst_file_list,dst_file_size) = get_file_list_cnaf(run,year)
    elif (dst_site == "KLOE"):
        (dst_missing,dst_file_list,dst_file_size) = get_file_list_kloe(run,year)
    if verbose:
        if dst_missing:
            print "%s - at %-13s run %s is missing"%(now_str(),dst_string,run)
        else:
            print "%s - at %-13s run %s contains %d files"%(now_str(),dst_string,run,len(dst_file_list))

    if (src_missing or dst_missing):
        if (src_missing and dst_missing):
            print "=== WARNING: Run %s is missing at %s and %s ==="%(run,src_string,dst_string)
        elif src_missing:
            print "=== WARNING: Run %s is missing at %s ==="%(run,src_string)
        elif dst_missing:
            print "=== WARNING: Run %s is missing at %s ==="%(run,dst_string)
        sys.exit()

    if (len(src_file_list) != len(dst_file_list)):
        if verbose: print "%s - run %s has %d files at %s and %d files at %s"%(now_str(),run,len(src_file_list),src_string,len(dst_file_list),dst_string)

    # Merge source and destination lists removing duplicates
    file_list = list(set(src_file_list).union(set(dst_file_list)))
    file_list.sort()

    # Check file lists for differences
    if (verbose > 1): print "%s - Starting verification of run %s (%d files) between %s and %s"%(now_str(),run,len(file_list),src_string,dst_string)
    warnings = 0
    miss_at_src = 0
    miss_at_dst = 0
    wrong_size = 0
    miss_chksum_src = 0
    miss_chksum_dst = 0
    wrong_checksum = 0
    for rawfile in file_list:

        # Check if file is at source site
        if not rawfile in src_file_list:
            warnings += 1
            miss_at_src += 1
            if verbose: print "%s - not at %s"%(rawfile,src_string)

        # Check if file is at destination site
        elif not rawfile in dst_file_list:
            warnings += 1
            miss_at_dst += 1
            if verbose: print "%s - not at %s"%(rawfile,dst_string)

        # Check if files have the same size
        elif src_file_size[rawfile] != dst_file_size[rawfile]:
            warnings += 1
            wrong_size += 1
            if verbose: print "%s - file sizes are different: %d at %s vs. %d at %s"%(rawfile,src_file_size[rawfile],src_string,dst_file_size[rawfile],dst_string)

        else:

            # Verify checksum only if requested by the user
            if checksum:

                # Get checksum at source site
                src_checksum = ""
                if (src_site == "DAQ"):
                    src_checksum = get_checksum_daq("%s/%s"%(run,rawfile),year,daq_srv)
                elif (src_site == "LNF"):
                    src_checksum = get_checksum_lnf("%s/%s"%(run,rawfile),year)
                elif (src_site == "LNF2"):
                    src_checksum = get_checksum_lnf("%s/%s"%(run,rawfile),year)
                elif (src_site == "CNAF"):
                    src_checksum = get_checksum_cnaf("%s/%s"%(run,rawfile),year)

                # Get checksum at destination site
                dst_checksum = ""
                if (dst_site == "DAQ"):
                    dst_checksum = get_checksum_daq("%s/%s"%(run,rawfile),year,daq_srv)
                elif (dst_site == "LNF"):
                    dst_checksum = get_checksum_lnf("%s/%s"%(run,rawfile),year)
                elif (dst_site == "LNF2"):
                    dst_checksum = get_checksum_lnf("%s/%s"%(run,rawfile),year)
                elif (dst_site == "CNAF"):
                    dst_checksum = get_checksum_cnaf("%s/%s"%(run,rawfile),year)

                # Check if checksums are consistent
                if (src_checksum == "" and dst_checksum == ""):
                    miss_chksum_src += 1
                    miss_chksum_dst += 1
                    warnings += 1
                    if verbose: print "%s - unable to get checksum at %s and %s"%(rawfile,src_string,dst_string)
                elif (src_checksum == ""):
                    miss_chksum_src += 1
                    warnings += 1
                    if verbose: print "%s - unable to get checksum at %s - checksum at %s is %s"%(rawfile,src_string,dst_string,dst_checksum)
                elif (dst_checksum == ""):
                    miss_chksum_dst += 1
                    warnings += 1
                    if verbose: print "%s - unable to get checksum at %s - checksum at %s is %s"%(rawfile,dst_string,src_string,src_checksum)
                elif (src_checksum != dst_checksum):
                    wrong_checksum += 1
                    warnings += 1
                    if verbose: print "%s - checksums are different: %s at %s vs. %s at %s"%(rawfile,src_checksum,src_string,dst_checksum,dst_string)
                else:
                    if (verbose > 1): print "%s - OK - size %10d checksum %8s"%(rawfile,src_file_size[rawfile],src_checksum)

            else:
                if (verbose > 1): print "%s - OK - size %10d"%(rawfile,src_file_size[rawfile])

    if warnings:
        report = ""
        if  miss_at_src: report += " - %s: %d missing"%(src_string,miss_at_src)
        if  miss_at_dst: report += " - %s: %d missing"%(dst_string,miss_at_dst)
        if  wrong_size: report += " - %d wrong size"%wrong_size
        if checksum:
            if  miss_chksum_src: report += " - %s: %d no checksum"%(src_string,miss_chksum_src)
            if  miss_chksum_dst: report += " - %s: %d no checksum"%(dst_string,miss_chksum_dst)
            if  wrong_checksum: report += " - %d wrong checksum"%wrong_checksum
        print "=== WARNING: Run %s DOES NOT MATCH between %s and %s%s ==="%(run,src_string,dst_string,report)
    else:
        print "=== Run %s matches between %s and %s ==="%(run,src_string,dst_string)

def run_command(command):
    #print "> %s"%command
    p = subprocess.Popen(command,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,shell=True)
    return iter(p.stdout.readline, b'')

def now_str():
    return time.strftime("%Y-%m-%d %H:%M:%S",time.gmtime())

# Execution starts here
if __name__ == "__main__":
   main(sys.argv[1:])
