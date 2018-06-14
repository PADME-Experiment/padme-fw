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
        self.node_ip = ""
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
        cfgstring += "node_ip\t\t\t%s\n"%self.node_ip
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

    def create_output_dir(self):

        if self.node_id == 0:

            # Create directory on local host
            print "Creating merger output files directory %s"%self.output_dir
            if not os.path.exists(self.output_dir): os.makedirs(self.output_dir,0755)

        else:

            # Create directory on remote node
            #node_ip = self.db.get_node_daq_ip(self.node_id)
            #if (node_ip == ""):
            #    print "Merger::create_output_dir - ERROR: node id %d has no address on DAQ VLAN",self.node_id
            #    return "error"
            print "Creating merger output files directory %s on %s"%(self.output_dir,self.node_ip)
            command = "ssh -i ~/.ssh/id_rsa_daq %s mkdir -p %s"%(self.node_ip,self.output_dir)
            print command
            subprocess.call(command.split())

        return "ok"

    def start_merger(self):

        command = "%s -r %d -l %s -o %s/%s -n %d"%(self.executable,self.run_number,self.input_list,self.output_dir,self.output_file,self.max_events)

        # Check if Merger runs on a remote node (node_id 0 is localhost)
        if self.node_id != 0:

            # Get IP address of merger node on DAQ VLAN (192.168.60.X)
            #node_ip = self.db.get_node_daq_ip(self.node_id)
            #if (node_ip == ""):
            #    print "Merger::start_merger - ERROR: node id %d has no address on DAQ VLAN",self.node_id
            #    return 0

            # Start Merger on remote node using passwordless ssh connection
            command = "ssh -i ~/.ssh/id_rsa_daq %s %s"%(self.node_ip,command)

        # Start Merger process on local node
        #try:
        #    self.process = subprocess.Popen([self.executable,"-r",str(self.run_number),"-l",self.input_list,"-o",self.output_dir+"/"+self.output_file,"-n",str(self.max_events)],stdout=self.log_handle,stderr=subprocess.STDOUT,bufsize=1)
        #except OSError as e:
        #    print "Merger::start_merger - ERROR: Execution failed: %s",e
        #    return 0

        print "- Starting Merger"
        print command
        print "  Log written to %s"%self.log_file

        # Open log file
        self.log_handle = open(self.log_file,"w")

        # Start Merger process
        try:
            self.process = subprocess.Popen(command.split(),stdout=self.log_handle,stderr=subprocess.STDOUT,bufsize=1)
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
