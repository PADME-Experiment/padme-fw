#!/usr/bin/python

import os
import sys
import time
import getopt
import subprocess

def main(argv):

    try:
        opts,args = getopt.getopt(argv,"y:h")
    except getopt.GetoptError:
        print 'transfer_files [-y year] [-h]'
        sys.exit(2)

    YEAR = time.strftime("%Y",time.localtime())
    for opt,arg in opts:
        if opt == '-h':
            print 'transfer_files [-y year] [-h]'
            sys.exit()
        elif opt == '-y':
            YEAR = arg

    DATA_DIR = "rawdata/%s"%YEAR

    DAQ_SRV = "padmesrv1"
    DAQ_USER = "daq"
    DAQ_PATH = "/home/daq/DAQ"

    DAQ_ADLER32_CMD = "/home/daq/DAQ/tools/adler32"

    DAQ_SFTP = "sftp://%s%s"%(DAQ_SRV,DAQ_PATH)
    LNF_SRM = "srm://atlasse.lnf.infn.it:8446/srm/managerv2?SFN=/dpm/lnf.infn.it/home/vo.padme.org/testbeam"
    CNAF_SRM = "srm://storm-fe-archive.cr.cnaf.infn.it:8444/srm/managerv2?SFN=/padmeTape/testbeam"

    print "Getting list of raw data files for year %s on %s"%(YEAR,DAQ_SRV)
    daq_list = []
    for line in run_command("ssh -l %s %s ls %s/%s"%(DAQ_USER,DAQ_SRV,DAQ_PATH,DATA_DIR)):
        daq_list.append(line.rstrip())

    print "Getting list of raw data files for year %s on LNF disks"%YEAR
    lnf_list = []
    for line in run_command("gfal-ls %s/%s"%(LNF_SRM,DATA_DIR)):
        lnf_list.append(line.rstrip())

    print "Getting list of raw data files for year %s on CNAF tape library"%YEAR
    cnaf_list = []
    for line in run_command("gfal-ls %s/%s"%(CNAF_SRM,DATA_DIR)):
        cnaf_list.append(line.rstrip())

    # Make a full list and show which file is located where
    full_list = []
    full_list.extend(daq_list)
    full_list.extend(lnf_list)
    full_list.extend(cnaf_list)
    full_list = sorted(set(full_list))
    for rawfile in (full_list):
        print "%-30s %s%s%s"%(rawfile,
                              ("D" if rawfile in daq_list else "-"),
                              ("L" if rawfile in lnf_list else "-"),
                              ("C" if rawfile in cnaf_list else "-"))

    for rawfile in (full_list):

        #print "=== Checking file %s ==="%rawfile

        if ( rawfile in daq_list and not rawfile in lnf_list ):

            print "--- Getting ADLER32 checksum of %s ---"%rawfile
            cmd = "ssh -l %s %s %s %s/%s/%s"%(DAQ_USER,DAQ_SRV,DAQ_ADLER32_CMD,DAQ_PATH,DATA_DIR,rawfile)
            print("> %s"%cmd)
            for line in run_command(cmd):
                (a32_daq,fdummy) = line.rstrip().split()
            print "Source file ADLER32 CRC: %s"%a32_daq

            print "--- Starting copy of %s from DAQ to LNF ---"%rawfile
            #cmd = "gfal-copy -t 3600 -T 3600 --checksum ADLER32:%s --checksum-mode target -D\"SFTP PLUGIN:USER=%s\" -D\"SFTP PLUGIN:PRIVKEY=/home/%s/.ssh/id_rsa\" %s/%s/%s %s/%s/%s"%(a32_daq,DAQ_USER,os.environ['USER'],DAQ_SFTP,DATA_DIR,rawfile,LNF_SRM,DATA_DIR,rawfile);
            #cmd = "gfal-copy -t 3600 -T 3600 --checksum ADLER32:%s -D\"SFTP PLUGIN:USER=%s\" -D\"SFTP PLUGIN:PRIVKEY=/home/%s/.ssh/id_rsa\" %s/%s/%s %s/%s/%s"%(a32_daq,DAQ_USER,os.environ['USER'],DAQ_SFTP,DATA_DIR,rawfile,LNF_SRM,DATA_DIR,rawfile);
            cmd = "gfal-copy -t 3600 -T 3600 -D\"SFTP PLUGIN:USER=%s\" -D\"SFTP PLUGIN:PRIVKEY=/home/%s/.ssh/id_rsa\" %s/%s/%s %s/%s/%s"%(DAQ_USER,os.environ['USER'],DAQ_SFTP,DATA_DIR,rawfile,LNF_SRM,DATA_DIR,rawfile);
            print("> %s"%cmd)
            for line in run_command(cmd):
                print(line.rstrip())
            # Need to learn how to handle gfal_copy errors, timeouts, etc...

            print("Copy to LNF was successful: adding %s to LNF list"%rawfile)
            lnf_list.append(rawfile)

        if ( rawfile in lnf_list and not rawfile in cnaf_list ):

            print "--- Starting copy of %s from LNF to CNAF ---"%rawfile
            cmd = "gfal-copy -t 3600 -T 3600 --checksum ADLER32 %s/%s/%s %s/%s/%s"%(LNF_SRM,DATA_DIR,rawfile,CNAF_SRM,DATA_DIR,rawfile);
            print("> %s"%cmd)
            for line in run_command(cmd):
                print(line.rstrip())
            # Need to learn how to handle gfal_copy errors, timeouts, etc...

            print("Copy to CNAF was successful: adding %s to CNAF list"%rawfile)
            cnaf_list.append(rawfile)

def run_command(command):
    p = subprocess.Popen(command,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,shell=True)
    return iter(p.stdout.readline, b'')

# Execution starts here
if __name__ == "__main__":
   main(sys.argv[1:])
