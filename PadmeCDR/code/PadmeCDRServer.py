#!/usr/bin/python

import os
import sys
import time
import subprocess

from Logger import Logger

class PadmeCDRServer:

    def __init__(self,cfg_file,mode):

        # Get position of CDR main directory from PADME_CDR_DIR environment variable
        # Default to current dir if not set
        self.cdr_dir = os.getenv('PADME_CDR_DIR',".")

        # Redefine print to send output to log file
        self.log_file = self.cdr_dir+"/log/PadmeCDRServer.log"
        sys.stdout = Logger(self.log_file)
        sys.stderr = sys.stdout
        if mode == "i": sys.stdout.interactive = True

        # Create lock file
        self.lock_file = self.cdr_dir+"/run/lock"
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

        # Here we will read the configuration file to override default settings

        # Read files currently on LNF and CNAF storage systems (will be replaced with DB)
        self.lnf_list  = self.get_file_list_lnf()
        self.cnaf_list = self.get_file_list_cnaf()

        # Start the main CDR loop
        self.main_loop()

    def create_lock_file(self):

        # Check if lock file exists
        if (os.path.exists(self.lock_file)):
            if (os.path.isfile(self.lock_file)):
                pid = 0
                with open(self.lock_file,"r") as lf:
                    for ll in lf: pid = ll

                print "Lock file %s found for pid %s - checking status"%(self.lock_file,pid)

                ppinfo = os.popen("ps -p %s"%pid)
                pinfo = ppinfo.readlines()
                ppinfo.close()
                if len(pinfo)==2:
                    if pinfo[1].find("<defunct>")>-1:
                        print "There is zombie process with this pid. The PadmeCDRServer is probably dead. Proceeding cautiously..."
                    else:
                        print "ERROR - there is already a PadmeCDRServer running with pid %s"%pid
                        return "error"
                else:
                    print "No PadmeCDRServer process found. As you were..."
            else:
                print "ERROR - Lock file %s found but it is not a file"%self.lock_file
                return "error"

        # Create our own lock file
        pid = os.getpid()
        with open(self.lock_file,"w") as lf:
            lf.write("%d"%pid)

        return "ok"

    def get_file_list_lnf(self):
        print "Getting list of raw data files for year %s on LNF disks"%self.year
        lnf_list = []
        for line in self.run_command("gfal-ls %s/%s"%(self.lnf_srm,self.data_dir)):
            run_dir = line.rstrip()
            for line2 in self.run_command("gfal-ls %s/%s/%s"%(self.lnf_srm,self.data_dir,run_dir)):
                lnf_list.append("%s/%s"%(run_dir,line2.rstrip()))
        return lnf_list

    def get_file_list_cnaf(self):
        print "Getting list of raw data files for year %s on CNAF tape library"%self.year
        cnaf_list = []
        for line in self.run_command("gfal-ls %s/%s"%(self.cnaf_srm,self.data_dir)):
            run_dir = line.rstrip()
            for line2 in self.run_command("gfal-ls %s/%s/%s"%(self.cnaf_srm,self.data_dir,run_dir)):
                cnaf_list.append("%s/%s"%(run_dir,line2.rstrip()))
        return cnaf_list

    def run_command(self,command):
        print("> %s"%command)
        p = subprocess.Popen(command,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,shell=True)
        return iter(p.stdout.readline, b'')

    def main_loop(self):

        # Loop forever waiting for new files to appear on data servers
        while True:

            # Define file copy counter. Used to check if DAQ is active
            n_files_copied = 0

            for daq_srv in self.daq_srv_list:

                print "=== Running on data server %s ==="%daq_srv

                # SFTP URL for rawdata on this data server
                daq_sftp = "sftp://%s%s"%(daq_srv,self.daq_path)

                # SSH syntax to execute a command on the DAQ data server
                daq_ssh = "ssh -i /home/%s/.ssh/%s -l %s %s"%(self.cdr_user,self.daq_keyfile,self.daq_user,daq_srv)

                print "Getting list of raw data files for year %s on %s"%(self.year,daq_srv)
                daq_list = []

                # Use find to get a quick list of all rawdata files on this data server
                for line in self.run_command("%s \'( cd %s/%s; find -type f | sed -e s+\./++ )\'"%(daq_ssh,self.daq_path,self.data_dir)):
                    daq_list.append(line.rstrip())

                # Make a full list by merging lists from CNAF, LNF and data server and removing duplicates
                full_list = []
                full_list.extend(daq_list)
                full_list.extend(self.lnf_list)
                full_list.extend(self.cnaf_list)
                full_list = sorted(set(full_list))

                # Show which file is located where
                #for rawfile in (full_list):
                #    print "%-30s %s%s%s"%(rawfile,
                #                          ("D" if rawfile in daq_list else "-"),
                #                          ("L" if rawfile in self.lnf_list else "-"),
                #                          ("C" if rawfile in self.cnaf_list else "-"))

                for rawfile in (full_list):

                    # Check if file must be copied from DAQ to LNF
                    if ( rawfile in daq_list and not rawfile in self.lnf_list ):

                        n_files_copied += 1

                        print "- File %s - Starting copy from DAQ to LNF"%rawfile
                        cmd = "gfal-copy -t 3600 -T 3600 -p -D\"SFTP PLUGIN:USER=%s\" -D\"SFTP PLUGIN:PRIVKEY=/home/%s/.ssh/%s\" %s/%s/%s %s/%s/%s"%(self.daq_user,self.cdr_user,self.daq_keyfile,daq_sftp,self.data_dir,rawfile,self.lnf_srm,self.data_dir,rawfile);
                        for line in self.run_command(cmd):
                            print(line.rstrip())
                        # Need to learn how to handle gfal_copy errors, timeouts, etc...

                        print "- File %s - Getting ADLER32 checksum at source"%rawfile
                        cmd = "%s %s %s/%s/%s"%(daq_ssh,self.daq_adler32_cmd,self.daq_path,self.data_dir,rawfile)
                        for line in self.run_command(cmd):
                            (a32_src,fdummy) = line.rstrip().split()

                        print "- File %s - Getting ADLER32 checksum at destination"%rawfile
                        cmd = "gfal-sum %s/%s/%s adler32"%(self.lnf_srm,self.data_dir,rawfile);
                        for line in self.run_command(cmd):
                            (fdummy,a32_dst) = line.rstrip().split()

                        print "- File %s - ADLER32 CRC - Source: %s - Destination: %s"%(rawfile,a32_src,a32_dst)

                        if ( a32_src == a32_dst ):
                            print "- File %s - Copy to LNF successful - Adding it to LNF list"%rawfile
                            self.lnf_list.append(rawfile)
                        else:
                            print "- File %s - ***ERROR*** while copying from DAQ to LNF"%rawfile

                    # Check if file must be copied from LNF to CNAF
                    if ( rawfile in self.lnf_list and not rawfile in self.cnaf_list ):

                        n_files_copied += 1

                        print "- File %s - Starting copy from LNF to CNAF"%rawfile
                        cmd = "gfal-copy -p -t 3600 -T 3600 --checksum ADLER32 %s/%s/%s %s/%s/%s"%(self.lnf_srm,self.data_dir,rawfile,self.cnaf_srm,self.data_dir,rawfile);
                        for line in self.run_command(cmd):
                            print(line.rstrip())
                        # Need to learn how to handle gfal_copy errors, timeouts, etc...

                        print "- File %s - Copy to CNAF successful - Adding it to CNAF list"%rawfile
                        self.cnaf_list.append(rawfile)

            # If no files were copied this means that DAQ is stopped: sleep for 1min before trying again
            if ( n_files_copied == 0 ): time.sleep(60)
