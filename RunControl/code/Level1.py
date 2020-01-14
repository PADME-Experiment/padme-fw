import os
import re
import time
import shlex
import subprocess

from PadmeDB  import PadmeDB

class Level1:

    def __init__(self,l_id):

        self.level1_id = l_id

        # Get position of DAQ main directory from PADME_DAQ_DIR environment variable
        # Default to current dir if not set
        self.daq_dir = os.getenv('PADME_DAQ_DIR',".")

        # Define id file for passwordless ssh command execution
        self.ssh_id_file = "%s/.ssh/id_rsa_daq"%os.getenv('HOME',"~")

        self.db = PadmeDB()

        self.re_dbinfo_line = re.compile("^\s*DBINFO\s.*$")

        self.set_default_config()

    def set_default_config(self):

        self.node_id = 0
        self.node_ip = ""

        self.executable = os.getenv('PADME',".")+"/Level1/PadmeLevel1.exe"

        self.run_number = 0
        self.process_id = -1

        self.max_events = 10000

        self.config_file = "unset"
        self.log_file = "unset"

        self.input_stream = "unset"
        self.output_dir = "unset"
        self.output_header = "unset"

    def config_list(self):

        cfg_list = []

        cfg_list.append(["daq_dir",        self.daq_dir])
        cfg_list.append(["ssh_id_file",    self.ssh_id_file])
        cfg_list.append(["executable",     self.executable])

        cfg_list.append(["run_number",     str(self.run_number)])
        cfg_list.append(["level1_id",      str(self.level1_id)])
        if (self.run_number):
            cfg_list.append(["process_id", str(self.process_id)])

        cfg_list.append(["node_id",        str(self.node_id)])
        cfg_list.append(["node_ip",        self.node_ip])

        cfg_list.append(["config_file",    self.config_file])
        cfg_list.append(["log_file",       self.log_file])

        cfg_list.append(["input_stream",   self.input_stream])
        cfg_list.append(["output_dir",     self.output_dir])
        cfg_list.append(["output_header",  self.output_header])

        cfg_list.append(["max_events",     str(self.max_events)])

        return cfg_list

    def format_config(self):

        cfgstring = ""
        for cfg in self.config_list(): cfgstring += "%-30s %s\n"%(cfg[0],cfg[1])
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

    def create_level1(self):

        self.process_id = self.db.create_level1_process(self.run_number,self.node_id)
        if self.process_id == -1:
            print "Level1::create_level1 - ERROR: unable to create new Level1 process in DB"
            return "error"

        # Add all configuration parameters
        for cfg in self.config_list():
            self.db.add_cfg_para_proc(self.process_id,cfg[0],cfg[1])

        return "ok"

    def start_level1(self):

        command = "%s -r %d -i %s -o %s/%s -n %d"%(self.executable,self.run_number,self.input_stream,self.output_dir,self.output_header,self.max_events)

        # If DAQ process runs on a remote node then start it using passwordless ssh connection
        if self.node_id != 0:
            command = "ssh -i %s %s '( %s )'"%(self.ssh_id_file,self.node_ip,command)

        print "- Start Level1 process number %d"%self.level1_id
        print command
        print "  Log written to %s"%self.log_file

        # Open log file
        self.log_handle = open(self.log_file,"w")

        # Start Level1 process
        try:
            self.process = subprocess.Popen(shlex.split(command),stdout=self.log_handle,stderr=subprocess.STDOUT,bufsize=1)
        except OSError as e:
            print "Level1::start_level1 - ERROR: Execution failed: %s",e
            return 0

        # Tag start of process in DB
        if self.run_number:
            self.db.set_process_time_create(self.process_id,self.db.now_str())

        # Return process id
        return self.process.pid

    def stop_level1(self):

        # Tag stop process in DB

        # Wait up to 5 seconds for Level1 to stop
        for i in range(5):
            if self.process.poll() != None:
                # Process exited: clean up defunct process and close log file
                self.process.wait()
                self.log_handle.close()
                if self.run_number:
                    self.db.set_process_time_end(self.process_id,self.db.now_str())
                return True
            time.sleep(1)

        # Process did not stop smoothly: stop it
        print "Level1::stop_level1 - WARNING: Level1 process did not stop on its own. Terminating it"
        self.process.terminate()
        time.sleep(1)
        if self.process.poll() != None:
            self.process.wait()
            self.log_handle.close()

        if self.run_number:
            self.db.set_process_time_end(self.process_id,self.db.now_str())
        return False

    def parse_log(self):

        # Look for DBINFO lines in log file and send them to DBINFO line processor
        if os.path.exists(self.log_file) and os.path.isfile(self.log_file):
            with open(self.log_file,"r") as log:
                for line in log:
                    if self.re_dbinfo_line.match(line):
                        self.db.manage_dbinfo_entry(self.process_id,line)
        else:
            print "Level1::parse_log - WARNING: Level1 %d log file %s not found"%(self.level1_id,self.log_file)
