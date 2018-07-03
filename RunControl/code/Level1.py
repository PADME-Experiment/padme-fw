import os
import re
import time
import shlex
import subprocess

class Level1:

    def __init__(self,l_id):

        self.level1_id = l_id

        self.set_default_config()

    def set_default_config(self):

        self.node_id = 0
        self.node_ip = ""

        self.executable = os.getenv('PADME',".")+"/Level1/PadmeLevel1.exe"

        self.run_number = 0

        self.max_events = 10000

        self.config_file = "unset"
        self.log_file = "unset"
        self.lock_file = "unset"
        self.initok_file = "unset"
        self.initfail_file = "unset"

        self.input_stream = "unset"
        self.output_dir = "unset"
        self.output_header = "unset"

    def format_config(self):

        cfgstring = ""
        cfgstring += "executable\t\t%s\n"%self.executable

        cfgstring += "level1_id\t\t%d\n"%self.level1_id

        cfgstring += "config_file\t\t%s\n"%self.config_file
        cfgstring += "log_file\t\t%s\n"%self.log_file
        cfgstring += "lock_file\t\t%s\n"%self.lock_file

        cfgstring += "run_number\t\t%d\n"%self.run_number

        cfgstring += "max_events\t\t%d\n"%self.max_events

        cfgstring += "node_id\t\t\t%d\n"%self.node_id
        cfgstring += "node_ip\t\t\t%s\n"%self.node_ip

        cfgstring += "initok_file\t\t%s\n"%self.initok_file
        cfgstring += "initfail_file\t\t%s\n"%self.initfail_file

        cfgstring += "input_stream\t\t%s\n"%self.input_stream

        cfgstring += "output_dir\t\t%s\n"%self.output_dir
        cfgstring += "output_header\t\t%s\n"%self.output_header

        return cfgstring

    def write_config(self):

        if self.config_file == "unset":
            print "Level1 - ERROR: write_config() called but config_file not set!"
            return

        f = open(self.config_file,"w")
        f.write(self.format_config())
        f.close()

    def print_config(self):

        print self.format_config()

    def start_level1(self):

        command = "%s -r %d -i %s -o %s/%s -n %d"%(self.executable,self.run_number,self.input_stream,self.output_dir,self.output_header,self.max_events)

        # If DAQ process runs on a remote node then start it using passwordless ssh connection
        if self.node_id != 0:
            command = "ssh -i ~/.ssh/id_rsa_daq %s %s"%(self.node_ip,command)

        print "- Start Level1 process number %d"%self.level1_id
        print command
        print "  Log written to %s"%self.log_file

        # Open log file
        self.log_handle = open(self.log_file,"w")

        # Start Level1 process
        try:
            #self.process = subprocess.Popen(command.split(),stdout=self.log_handle,stderr=subprocess.STDOUT,bufsize=1)
            self.process = subprocess.Popen(shlex.split(command),stdout=self.log_handle,stderr=subprocess.STDOUT,bufsize=1)
        except OSError as e:
            print "Level1::start_level1 - ERROR: Execution failed: %s",e
            return 0

        # Return process id
        return self.process.pid

    def stop_level1(self):

        # Wait up to 5 seconds for Level1 to stop
        for i in range(5):

            if self.process.poll() != None:

                # Process exited: clean up defunct process and close log file
                self.process.wait()
                self.log_handle.close()
                return 1

            time.sleep(1)

        # Process did not stop smoothly: stop it
        self.process.terminate()
        time.sleep(1)
        if self.process.poll() != None:
            self.process.wait()
            self.log_handle.close()
        return 0
