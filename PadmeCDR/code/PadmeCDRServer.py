#!/usr/bin/python

import os
import sys
import time
import subprocess

from Logger import Logger

class PadmeCDRServer:

    def __init__(self):

        # Redefine print to send output to log file
        sys.stdout = Logger("log/PadmeCDRServer.log")

        # Create lock file
        self.lock_file = "run/lock"
        if (self.create_lock_file() == "error"): exit(1)

        self.cdr_user = os.environ['USER']

        self.year = time.strftime("%Y",time.localtime())

        self.data_dir = "%s/rawdata"%self.year

        self.daq_srv_list = [ "l1padme3", "l1padme4" ]
        self.daq_user = "daq"
        self.daq_keyfile = "id_rsa_cdr"
        self.daq_path = "/data/DAQ"

        self.daq_adler32_cmd = "/home/daq/DAQ/tools/adler32"

        self.lnf_srm = "srm://atlasse.lnf.infn.it:8446/srm/managerv2?SFN=/dpm/lnf.infn.it/home/vo.padme.org/daq"
        self.cnaf_srm = "srm://storm-fe-archive.cr.cnaf.infn.it:8444/srm/managerv2?SFN=/padmeTape/daq"

        self.lnf_list  = get_file_list_lnf()
        self.cnaf_list = get_file_list_cnaf()

    def create_lock_file(self):

        # Check if lock file exists
        if (os.path.exists(self.lock_file)):
            if (os.path.isfile(self.lock_file)):
                pid = 0
                with open(self.lock_file,"r") as lf:
                    for ll in lf: pid = ll

                print "Lock file %s found for pid %s - checking status"%(self.lock_file,pid)
                #self.write_log("ERROR - Lock file %s found for pid %s"%(self.lock_file,pid))

                ppinfo = os.popen("ps -p %s"%pid)
                pinfo = ppinfo.readlines()
                ppinfo.close()
                if len(pinfo)==2:
                    if pinfo[1].find("<defunct>")>-1:
                        print "There is zombie process with this pid. The RunControlServer is probably dead. Proceeding cautiously..."
                    else:
                        print "ERROR - there is already a RunControlServer running with pid %s"%pid
                        return "error"
                else:
                    print "No RunControlServer process found. As you were..."
            else:
                print "ERROR - Lock file %s found but it is not a file"%self.lock_file
                #self.write_log("ERROR - Lock file %s found but it is not a file"%self.lock_file)
                return "error"

        # Create our own lock file
        pid = os.getpid()
        with open(self.lock_file,"w") as lf:
            lf.write("%d"%pid)

        return "ok"

    def get_file_list_lnf(self):
        print "Getting list of raw data files for year %s on LNF disks"%self.year
        lnf_list = []
        for line in run_command("gfal-ls %s/%s"%(self.lnf_srm,self.data_dir)):
            run_dir = line.rstrip()
            for line2 in run_command("gfal-ls %s/%s/%s"%(self.lnf_srm,self.data_dir,run_dir)):
                lnf_list.append("%s/%s"%(run_dir,line2.rstrip()))
        return lnf_list

    def get_file_list_cnaf(self):
        print "Getting list of raw data files for year %s on CNAF tape library"%self.year
        cnaf_list = []
        for line in run_command("gfal-ls %s/%s"%(self.cnaf_srm,self.data_dir)):
            run_dir = line.rstrip()
            for line2 in run_command("gfal-ls %s/%s/%s"%(self.cnaf_srm,self.data_dir,run_dir)):
                cnaf_list.append("%s/%s"%(run_dir,line2.rstrip()))
        return cnaf_list

    def run_command(command):
        print("> %s"%command)
        p = subprocess.Popen(command,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,shell=True)
        return iter(p.stdout.readline, b'')

    def main_loop(self):

        # Loop forever waiting for new files to appear on data servers
        while True:

            for daq_srv in self.daq_srv_list:

                print "=== Running on data server %s ==="%daq_srv

                # SFTP URL for rawdata on this data server
                daq_sftp = "sftp://%s%s"%(daq_srv,self.daq_path)

                # SSH syntax to execute a command on the DAQ data server
                daq_ssh = "ssh -i /home/%s/.ssh/%s -l %s %s"%(self.cdr_user,self.daq_keyfile,self.daq_user,daq_srv)

                print "Getting list of raw data files for year %s on %s"%(self.year,daq_srv)
                daq_list = []

                # Use find to get a quick list of all rawdata files on this data server
                for line in run_command("%s \'( cd %s/%s; find -type f | sed -e s+\./++ )\'"%(daq_ssh,self.daq_path,self.daq_dir)):
                    daq_list.append(line.rstrip())

                # Make a full list by merging lists from CNAF, LNF and data server and removing duplicates
                full_list = []
                full_list.extend(daq_list)
                full_list.extend(lnf_list)
                full_list.extend(cnaf_list)
                full_list = sorted(set(full_list))

                # Show which file is located where
                #for rawfile in (full_list):
                #    print "%-30s %s%s%s"%(rawfile,
                #                          ("D" if rawfile in daq_list else "-"),
                #                          ("L" if rawfile in lnf_list else "-"),
                #                          ("C" if rawfile in cnaf_list else "-"))

                for rawfile in (full_list):

                    if ( rawfile in daq_list and not rawfile in lnf_list ):

                        print "--- Starting copy from DAQ to LNF of %s ---"%rawfile
                        cmd = "gfal-copy -t 3600 -T 3600 -p -D\"SFTP PLUGIN:USER=%s\" -D\"SFTP PLUGIN:PRIVKEY=/home/%s/.ssh/%s\" %s/%s/%s %s/%s/%s"%(self.daq_user,self.cdr_user,self.daq_keyfile,daq_sftp,self.data_dir,rawfile,self.lnf_srm,self.data_dir,rawfile);
                        for line in run_command(cmd):
                            print(line.rstrip())
                        # Need to learn how to handle gfal_copy errors, timeouts, etc...

                        print "--- Getting ADLER32 checksum of source file %s ---"%rawfile
                        cmd = "%s %s %s/%s/%s"%(daq_ssh,self.daq_adler32_cmd,self.daq_path,self.data_dir,rawfile)
                        for line in run_command(cmd):
                            (a32_src,fdummy) = line.rstrip().split()
                        print "Source file ADLER32 CRC: %s"%a32_src

                        print "--- Getting ADLER32 checksum of destination file %s ---"%rawfile
                        cmd = "gfal-sum %s/%s/%s adler32"%(self.lnf_srm,self.data_dir,rawfile);
                        for line in run_command(cmd):
                            (fdummy,a32_dst) = line.rstrip().split()
                        print "Destination file ADLER32 CRC: %s"%a32_dst

                        if ( a32_src == a32_dst ):
                            print("Copy to LNF was successful: adding %s to LNF list"%rawfile)
                            self.lnf_list.append(rawfile)
                        else:
                            print("ERROR while copying file from DAQ to LNF - File %s"%rawfile)

                    if ( rawfile in lnf_list and not rawfile in cnaf_list ):

                        print "--- Starting copy of %s from LNF to CNAF ---"%rawfile
                        cmd = "gfal-copy -p -t 3600 -T 3600 --checksum ADLER32 %s/%s/%s %s/%s/%s"%(self.lnf_srm,self.data_dir,rawfile,self.cnaf_srm,self.data_dir,rawfile);
                        for line in run_command(cmd):
                            print(line.rstrip())
                        # Need to learn how to handle gfal_copy errors, timeouts, etc...

                        print("Copy to CNAF was successful: adding %s to CNAF list"%rawfile)
                        cnaf_list.append(rawfile)

            # Sleep for 1min before trying again
            time.sleep(60)
