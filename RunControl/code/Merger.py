import os
import re
import time
import shlex
import subprocess

from PadmeDB import PadmeDB

class Merger:

    def __init__(self):

        # Get position of DAQ main directory from PADME_DAQ_DIR environment variable
        # Default to current dir if not set
        self.daq_dir = os.getenv('PADME_DAQ_DIR',".")

        # Define id file for passwordless ssh command execution
        self.ssh_id_file = "%s/.ssh/id_rsa_daq"%os.getenv('HOME',"~")

        self.db = PadmeDB()

        self.set_default_config()

    def set_default_config(self):

        self.node_id = 0
        self.node_ip = ""

        self.executable = os.getenv('PADME',".")+"/Level1/PadmeMerger.exe"

        self.run_number = 0
        self.process_id = -1

        self.config_file = "undefined"
        self.log_file = "undefined"

        #self.output_mode = "STREAM"

        self.input_list = "undefined"
        self.output_list = "undefined"

    def format_config(self):

        cfgstring = ""
        cfgstring += "daq_dir\t\t\t%s\n"%self.daq_dir
        cfgstring += "ssh_id_file\t\t%s\n"%self.ssh_id_file
        cfgstring += "executable\t\t%s\n"%self.executable

        cfgstring += "run_number\t\t%d\n"%self.run_number
        if (self.run_number): cfgstring += "process_id\t\t%d\n"%self.process_id

        cfgstring += "node_id\t\t\t%d\n"%self.node_id
        cfgstring += "node_ip\t\t\t%s\n"%self.node_ip

        cfgstring += "config_file\t\t%s\n"%self.config_file
        cfgstring += "log_file\t\t%s\n"%self.log_file

        #cfgstring += "output_mode\t\t%s\n"%self.output_mode

        cfgstring += "input_list\t\t%s\n"%self.input_list
        cfgstring += "output_list\t\t%s\n"%self.output_list

        return cfgstring

    def write_config(self):

        if self.config_file == "unset":
            print "Merger::write_config - ERROR: config_file not set!"
            return

        f = open(self.config_file,"w")
        f.write(self.format_config())
        f.close()

    def print_config(self):

        print self.format_config()

    def create_merger(self):

        self.process_id = self.db.create_merger_process(self.run_number,self.node_id)
        if self.process_id == -1:
            print "Merger::create_merger - ERROR: unable to create new Merger process in DB"
            return "error"

        self.db.add_cfg_para_proc(self.process_id,"daq_dir",    self.daq_dir)
        self.db.add_cfg_para_proc(self.process_id,"ssh_id_file",self.ssh_id_file)
        self.db.add_cfg_para_proc(self.process_id,"executable", self.executable)

        #self.db.add_cfg_para_proc(self.process_id,"run_number", repr(self.run_number))

        #self.db.add_cfg_para_proc(self.process_id,"node_id",    repr(self.node_id))
        self.db.add_cfg_para_proc(self.process_id,"node_ip",    self.node_ip)
                                                         
        self.db.add_cfg_para_proc(self.process_id,"config_file",self.config_file)
        self.db.add_cfg_para_proc(self.process_id,"log_file",   self.log_file)

        #self.db.add_cfg_para_proc(self.process_id,"output_mode",self.output_mode)

        self.db.add_cfg_para_proc(self.process_id,"input_list", self.input_list)
        self.db.add_cfg_para_proc(self.process_id,"output_list",self.output_list)

        return "ok"

    def start_merger(self):

        command = "%s -r %d -i %s -o %s"%(self.executable,self.run_number,self.input_list,self.output_list)

        # If Merger process runs on a remote node then start it using passwordless ssh connection
        if self.node_id != 0:
            command = "ssh -i %s %s '( %s )'"%(self.ssh_id_file,self.node_ip,command)

        print "- Starting Merger process"
        print command
        print "  Log written to %s"%self.log_file

        # Open log file
        self.log_handle = open(self.log_file,"w")

        # Start Merger process
        try:
            self.process = subprocess.Popen(shlex.split(command),stdout=self.log_handle,stderr=subprocess.STDOUT,bufsize=1)
        except OSError as e:
            print "Merger::start_merger - ERROR: Execution failed: %s",e
            return 0                

        # Tag start of process in DB
        if self.run_number: self.db.set_process_time_create(self.process_id)

        # Return process id
        return self.process.pid

    def stop_merger(self):

        # Wait up to 60 seconds for Merger to stop
        for i in range(60):
            if self.process.poll() != None:
                # Process exited: clean up defunct process and close log file
                self.process.wait()
                self.log_handle.close()
                if self.run_number: self.db.set_process_time_end(self.process_id)
                return True
            time.sleep(1)

        # Process did not stop smoothly: terminate it
        print "Merger::stop_merger - WARNING: Merger process did not stop on its own. Terminating it"
        self.process.terminate()
        time.sleep(1)
        if self.process.poll() != None:
            self.process.wait()
            self.log_handle.close()

        if self.run_number: self.db.set_process_time_end(self.process_id)
        return False
