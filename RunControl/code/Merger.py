import os
import re
import time
import subprocess

from PadmeDB import PadmeDB

class Merger:

    def __init__(self):

        self.db = PadmeDB()

        self.set_default_config()

    def set_default_config(self):

        self.executable = os.getenv('PADME',".")+"/Level1/PadmeLevel1.exe"
        self.log_file = "undefined"
        self.config_file = "undefined"
        self.node_id = 0
        self.run_number = 0
        self.input_list = "undefined"
        self.output_dir = "."
        self.output_file = "rawdata"
        self.max_events = 10000

    def format_config(self):

        cfgstring = ""
        cfgstring += "executable\t\t%s\n"%self.executable
        cfgstring += "config_file\t\t%s\n"%self.config_file
        cfgstring += "log_file\t\t%s\n"%self.log_file
        cfgstring += "node_id\t\t\t%d\n"%self.node_id
        cfgstring += "run_number\t\t%d\n"%self.run_number
        cfgstring += "input_list\t\t%s\n"%self.input_list
        cfgstring += "output_dir\t\t%s\n"%self.output_dir
        cfgstring += "output_file\t\t%s\n"%self.output_file
        cfgstring += "max_events\t\t%d\n"%self.max_events
        return cfgstring

    def write_config(self):

        if self.config_file == "unset":
            print "Merger::write_config - ERROR: config_file not set!"
            return

        f = open(self.config_file,"w")
        f.write(self.format_config())
        f.close()

    def create_merger(self):

        self.merger_id = self.db.create_merger(self.run_number,self.node_id)
        if self.merger_id == -1: return "error"

        self.db.add_cfg_para_merger(self.merger_id,"input_list", self.input_list)
        self.db.add_cfg_para_merger(self.merger_id,"output_dir",self.output_dir)
        self.db.add_cfg_para_merger(self.merger_id,"output_file",self.output_file)
        self.db.add_cfg_para_merger(self.merger_id,"max_events", self.max_events)

        return "ok"

    def start_merger(self):

        # Open log file
        self.log_handle = open(self.log_file,"w")

        # Check if DB must be updated
        update_db = ""
        if (self.run_number): update_db = "-u"

        # Start DAQ process
        try:
            self.process = subprocess.Popen([self.executable,"-l",self.input_list,"-d",".","-o",self.output_dir+"/"+self.output_file,"-n",str(self.max_events),update_db],stdout=self.log_handle,stderr=subprocess.STDOUT,bufsize=1)
        except OSError as e:
            print "Merger::start_merger - ERROR: Execution failed: %s",e
            return 0

        # Return process id
        return self.process.pid


    def stop_merger(self):

        # Wait up to 5 seconds for Merger to stop
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
