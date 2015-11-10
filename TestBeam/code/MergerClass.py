#!/usr/bin/python

import sys
import signal
import getopt
import os
import re
import time

class MergerClass:

    def __init__(self):

        self.run_name = ""
        self.log_path = ""
        self.daq_data_dir = ""
        self.merged_list = ""
        self.merged_file_template = ""
        self.merged_last_file = ""
        self.padme_merge = ""

    def Start(self):

        # Get list of log files and boards for this run
        print 'Looking for log files in',self.log_path
        re_board = re.compile("^"+self.run_name+"_b(\d\d).log")
        log_list = []
        boardid_list = []
        for top,dirs,files in os.walk(self.log_path):
            if (top == self.log_path):
                for f in files:
                    m = re_board.search(f)
                    if (m):
                        boardid = int(m.group(1))
                        print "Found board",boardid
                        log_list.append(self.log_path+"/"+f)
                        boardid_list.append(boardid)

        # Show user what we found
        for l in range(len(log_list)):
            print "Board",boardid_list[l],"Log",l,log_list[l]

        # Loop forever (use CTRL-C to kill)
        last_merged_slice = 0
        disaligned_counter = 0
        re_fileclose = re.compile("Closed file \'(.+)\' after")
        while(1):

            # Look for data files for each board
            daqfiles = []
            for l in range(len(log_list)):
                daqfiles.append([])
                f = open(log_list[l])
                for r in f:
                    m = re_fileclose.search(r)
                    if (m): daqfiles[l].append(m.group(1))

            # Verify if number of files corresponds
            all_ok = 1
            for l in range(len(log_list)):
                print "Board",boardid_list[l],"has",len(daqfiles[l]),"DAQ files"
                if (len(daqfiles[l]) != len(daqfiles[0])): all_ok = 0
            if (all_ok):
                disaligned_counter = 0
            else:
                disaligned_counter += 1
                if (disaligned_counter<5):
                    time.sleep(1)
                    continue
                else:
                    print "ERROR: files disaligned for more than 5 seeconds"
                    sys.exit(1)

            n_data_files = len(daqfiles[0])

            # Check if new files have appeared and merge the last one
            if (n_data_files>last_merged_slice):

                # Create file with list of files to merge
                if os.path.exists(self.merged_list): os.remove(self.merged_list)
                f = open(self.merged_list,"w")
                for l in range(len(log_list)): f.write(str(l)+" "+daqfiles[l][-1]+"\n")
                f.close()

                # Merge list of files
                merged_file = self.merged_file_template+"_"+str(n_data_files)+".root"
                if os.path.exists(merged_file): os.remove(merged_file)
                cmd = self.padme_merge+" -d "+self.daq_data_dir+" -l "+self.merged_list+" -o "+merged_file+" -n 0"
                os.system(cmd)

                # Tell Monitor name of last merged file
                if os.path.exists(self.merged_last_file): os.remove(self.merged_last_file)
                f = open(self.merged_last_file,"w")
                f.write(merged_file)
                f.close()

                # Update merged slices counter
                last_merged_slice = n_data_files

                # Sleep a small time (remove?)
                time.sleep(1)

            else:

                # Sleep for a while
                print "Nothing new to show: sleeping for a while"
                time.sleep(10)
