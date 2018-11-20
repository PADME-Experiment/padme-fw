import os
import re
import time
import shlex
import subprocess

from PadmeDB  import PadmeDB

class Trigger:

    def __init__(self):

        # Get position of DAQ main directory from PADME_DAQ_DIR environment variable
        # Default to current dir if not set
        self.daq_dir = os.getenv('PADME_DAQ_DIR',".")

        # Define id file for passwordless ssh command execution
        self.ssh_id_file = "%s/.ssh/id_rsa_daq"%os.getenv('HOME',"~")

        self.db = PadmeDB()

        self.status = "idle"

        self.set_default_config()

    def set_default_config(self):

        self.node_id = 0
        self.node_ip = ""

        self.executable = os.getenv('PADME',".")+"/PadmeTrig/PadmeTrig.exe"

        self.run_number = 0

        self.process_mode = "DAQ"

        self.config_file = "unset"

        self.log_file = "unset"
        self.lock_file = "unset"
        self.initok_file = "unset"
        self.initfail_file = "unset"

        self.output_mode = "STREAM"
        self.output_stream = "unset"

        self.start_file = "unset"
        self.quit_file = "unset"

        self.total_daq_time = 0

        # Default trigger settings
        self.trigger_addr = "192.168.60.100"
        self.trigger_port = 7
        self.trigger_mask = int('0x01',0)

        # Default DAQ control parameters
        self.daq_loop_delay = 100000

    def read_setup(self,setup):

        # Define regular expressions used in file parsing
        re_empty = re.compile("^\s*$")
        re_comment = re.compile("^\s*#")
        re_param = re.compile("^\s*(\w+)\s+(.+?)\s*$")

        # Read default board configuration from file
        #setup_file = self.daq_dir+"/setup/"+setup+"/board_%02d.cfg"%self.board_id
        setup_file = "%s/setup/%s/trigger.cfg"%(self.daq_dir,setup)
        if (not os.path.isfile(setup_file)):
            print "Trigger - WARNING: setup file %s not found"%setup_file
            return
        f = open(setup_file)
        for l in f:
            if (re_empty.search(l) or re_comment.search(l)): continue
            m = re_param.search(l)
            if (m):
                (p_name,p_value) = m.group(1,2)
                if   (p_name == "trigger_mask"):      self.trigger_mask = int(p_value,0)
                elif (p_name == "trigger_addr"):      self.trigger_addr = p_value
                elif (p_name == "trigger_port"):      self.trigger_port = int(p_value,0)
                elif (p_name == "daq_loop_delay"):    self.daq_loop_delay = int(p_value,0)
                else:
                    print "Trigger - WARNING: unknown parameter found while reading config file: %s"%p_name
            else:
                print "Trigger - WARNING: unknown line format found while reading config file"
                print l
        f.close()

    def format_config(self):

        cfgstring = ""
        cfgstring += "daq_dir\t\t%s\n"%self.daq_dir
        cfgstring += "ssh_id_file\t\t%s\n"%self.ssh_id_file
        cfgstring += "executable\t\t%s\n"%self.executable
        cfgstring += "start_file\t\t%s\n"%self.start_file
        cfgstring += "quit_file\t\t%s\n"%self.quit_file

        cfgstring += "run_number\t\t%d\n"%self.run_number
        cfgstring += "process_mode\t\t%s\n"%self.process_mode
        if (self.run_number): cfgstring += "process_id\t\t%d\n"%self.process_id

        cfgstring += "node_id\t\t\t%d\n"%self.node_id
        cfgstring += "node_ip\t\t\t%s\n"%self.node_ip

        cfgstring += "config_file\t\t%s\n"%self.config_file
        cfgstring += "log_file\t\t%s\n"%self.log_file
        cfgstring += "lock_file\t\t%s\n"%self.lock_file
        cfgstring += "initok_file\t\t%s\n"%self.initok_file
        cfgstring += "initfail_file\t\t%s\n"%self.initfail_file

        cfgstring += "output_mode\t\t%s\n"%self.output_mode
        cfgstring += "output_stream\t\t%s\n"%self.output_stream

        cfgstring += "total_daq_time\t\t%d\n"%self.total_daq_time

        cfgstring += "trigger_mask\t\t%#02x\n"%self.trigger_mask
        cfgstring += "trigger_addr\t\t%s\n"%self.trigger_addr
        cfgstring += "trigger_port\t\t%d\n"%self.trigger_port

        cfgstring += "daq_loop_delay\t\t%d\n"%self.daq_loop_delay

        return cfgstring

    def write_config(self):

        if self.config_file == "unset":
            print "Trigger - ERROR: write_config() called but config_file variable not set!"
            return

        f = open(self.config_file,"w")
        f.write(self.format_config())
        f.close()

    def print_config(self):

        print self.format_config()

    def create_trigger(self):
    
        # Create Trigger process in DB
        self.process_id = self.db.create_trigger_process(self.run_number,self.node_id)
        if self.proc_daq_id == -1:
            print "ADCBoard::create_proc_daq - ERROR: unable to create new DAQ proces in DB"
            return "error"

        self.db.add_cfg_para_trigger(self.process_id,"daq_dir",            self.daq_dir)
        self.db.add_cfg_para_trigger(self.process_id,"ssh_id_file",        self.ssh_id_file)
        self.db.add_cfg_para_trigger(self.process_id,"executable",         self.executable)
        self.db.add_cfg_para_trigger(self.process_id,"start_file",         self.start_file)
        self.db.add_cfg_para_trigger(self.process_id,"quit_file",          self.quit_file)

        self.db.add_cfg_para_trigger(self.process_id,"run_number",         repr(self.run_number))
        self.db.add_cfg_para_trigger(self.process_id,"process_mode",       self.process_mode)

        self.db.add_cfg_para_trigger(self.process_id,"node_id",            repr(self.node_id))
        self.db.add_cfg_para_trigger(self.process_id,"node_ip",            self.node_ip)
                                                         
        self.db.add_cfg_para_trigger(self.process_id,"config_file",        self.config_file)
        self.db.add_cfg_para_trigger(self.process_id,"log_file",           self.log_file)
        self.db.add_cfg_para_trigger(self.process_id,"lock_file",          self.lock_file)
        self.db.add_cfg_para_trigger(self.process_id,"initok_file",        self.initok_file)
        self.db.add_cfg_para_trigger(self.process_id,"initfail_file",      self.initfail_file)
                                                                        
        self.db.add_cfg_para_trigger(self.process_id,"output_mode",        self.output_mode)
        self.db.add_cfg_para_trigger(self.process_id,"output_stream",      self.output_stream)

        self.db.add_cfg_para_trigger(self.process_id,"total_daq_time",     repr(self.total_daq_time))

        self.db.add_cfg_para_trigger(self.process_id,"trigger_mask",       "%#02x"%self.trigger_mask)
        self.db.add_cfg_para_trigger(self.process_id,"trigger_addr",       self.trigger_addr)
        self.db.add_cfg_para_trigger(self.process_id,"trigger_port",       repr(self.trigger_port))

        self.db.add_cfg_para_trigger(self.process_id,"daq_loop_delay",     repr(self.daq_loop_delay))

        return "ok"

    def start_trig(self):

        command = "%s -c %s"%(self.executable,self.config_file)

        # If Trigger process runs on a remote node then start it using passwordless ssh connection
        if self.node_id != 0:
            command = "ssh -i %s %s '( %s )'"%(self.ssh_id_file,self.node_ip,command)

        print "- Start Trigger process"
        print command
        print "  Log written to %s"%self.log_file

        # Open log file
        self.log_handle = open(self.log_file,"w")

        # Start Trigger process
        try:
            self.process = subprocess.Popen(shlex.split(command),stdout=self.log_handle,stderr=subprocess.STDOUT,bufsize=1)
        except OSError as e:
            print "Trigger::start_trig - ERROR: Execution failed: %s",e
            return 0

        # Return process id
        return self.process.pid

    def stop_trig(self):

        # Wait up to 5 seconds for DAQ to stop of its own (on quit file or on time elapsed)
        for i in range(10):

            if self.process.poll() != None:

                # Process exited: clean up defunct process and close log file
                self.process.wait()
                self.log_handle.close()
                return 1

            time.sleep(0.5)

        # Process did not stop: try sending and interrupt
        if self.node_id == 0:
            # If process is on local host, just send a kill signal
            command = "kill %d"%self.process.pid
        else:
            # If it is on a remote host, use ssh to send kill command.
            # PID on remote host is recovered from the lock file
            command = "ssh -i %s %s '( kill `cat %s` )'"%(self.ssh_id_file,self.node_ip,self.lock_file)
        print command
        os.system(command)

        # Wait up to 5 seconds for DAQ to stop on interrupt
        for i in range(10):

            if self.process.poll() != None:

                # Process exited: clean up defunct process and close log file
                self.process.wait()
                self.log_handle.close()
                return 1

            time.sleep(0.5)

        # Process did not stop smoothly: terminate it
        self.process.terminate()
        time.sleep(1)
        if self.process.poll() != None:
            self.process.wait()
            self.log_handle.close()
        return 0
