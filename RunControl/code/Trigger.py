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
        self.process_id = -1

        #self.process_mode = "DAQ"

        self.config_file = "unset"

        self.log_file = "unset"
        self.lock_file = "unset"
        self.initok_file = "unset"
        self.initfail_file = "unset"

        self.output_mode = "STREAM"
        self.output_stream = "unset"

        self.data_dir = "unset"
        self.data_file = "trig"

        self.start_file = "unset"
        self.quit_file = "unset"

        self.total_daq_time = 0

        # Default trigger settings
        self.trigger_addr = "192.168.60.100"
        self.trigger_port = 7
        self.trigger_mask = int('0x01',0)
        self.busy_mask = int('0x10',0)

        # BTF trigger settings (get all triggers, no autopass)
        #self.trig0_scale_global = 1
        self.trig0_scale_autopass = 0

        # External triggers settings (get all triggers, no autopass)
        self.trig1_scale_global = 1
        self.trig1_scale_autopass = 0
        self.trig2_scale_global = 1
        self.trig2_scale_autopass = 0
        self.trig3_scale_global = 1
        self.trig3_scale_autopass = 0
        self.trig4_scale_global = 1
        self.trig4_scale_autopass = 0
        self.trig5_scale_global = 1
        self.trig5_scale_autopass = 0

        # Software triggers settings (get all triggers, all autopass)
        self.trig6_scale_global = 1
        self.trig6_scale_autopass = 1
        self.trig7_scale_global = 1
        self.trig7_scale_autopass = 1

        # Delay for trigger 0 (BTF)
        self.trigger0_delay = int('0x7e',0)

        # Delay of correlated trigger wrt trigger 0
        self.correlated_trigger_delay = int('0x01f4 ',0)

        # Default timepix settings
        self.timepix_shutter_delay = int('0x02',0)
        self.timepix_shutter_width = int('0xff',0)

        # Default file parameters
        self.file_max_duration = 3600
        self.file_max_size     = 1024*1024*1024
        self.file_max_events   = 1000*1000

        # Default DAQ control parameters
        self.daq_loop_delay = 10000
        self.debug_scale = 100

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
                if   (p_name == "trigger_addr"):             self.trigger_addr = p_value
                elif (p_name == "trigger_port"):             self.trigger_port = int(p_value,0)
                elif (p_name == "trigger_mask"):             self.trigger_mask = int(p_value,0)
                elif (p_name == "busy_mask"):                self.busy_mask = int(p_value,0)
                elif (p_name == "trig0_scale_global"):       self.trig0_scale_global = int(p_value,0)
                elif (p_name == "trig0_scale_autopass"):     self.trig0_scale_autopass = int(p_value,0)
                elif (p_name == "trig1_scale_global"):       self.trig1_scale_global = int(p_value,0)
                elif (p_name == "trig1_scale_autopass"):     self.trig1_scale_autopass = int(p_value,0)
                elif (p_name == "trig2_scale_global"):       self.trig2_scale_global = int(p_value,0)
                elif (p_name == "trig2_scale_autopass"):     self.trig2_scale_autopass = int(p_value,0)
                elif (p_name == "trig3_scale_global"):       self.trig3_scale_global = int(p_value,0)
                elif (p_name == "trig3_scale_autopass"):     self.trig3_scale_autopass = int(p_value,0)
                elif (p_name == "trig4_scale_global"):       self.trig4_scale_global = int(p_value,0)
                elif (p_name == "trig4_scale_autopass"):     self.trig4_scale_autopass = int(p_value,0)
                elif (p_name == "trig5_scale_global"):       self.trig5_scale_global = int(p_value,0)
                elif (p_name == "trig5_scale_autopass"):     self.trig5_scale_autopass = int(p_value,0)
                elif (p_name == "trig6_scale_global"):       self.trig6_scale_global = int(p_value,0)
                elif (p_name == "trig6_scale_autopass"):     self.trig6_scale_autopass = int(p_value,0)
                elif (p_name == "trig7_scale_global"):       self.trig7_scale_global = int(p_value,0)
                elif (p_name == "trig7_scale_autopass"):     self.trig7_scale_autopass = int(p_value,0)
                elif (p_name == "trigger0_delay"):           self.trigger0_delay = int(p_value,0)
                elif (p_name == "correlated_trigger_delay"): self.correlated_trigger_delay = int(p_value,0)
                elif (p_name == "timepix_shutter_delay"):    self.timepix_shutter_delay = int(p_value,0)
                elif (p_name == "timepix_shutter_width"):    self.timepix_shutter_width = int(p_value,0)
                elif (p_name == "daq_loop_delay"):           self.daq_loop_delay = int(p_value,0)
                elif (p_name == "debug_scale"):              self.debug_scale = int(p_value,0)
                else:
                    print "Trigger - WARNING: unknown parameter found while reading config file: %s"%p_name
            else:
                print "Trigger - WARNING: unknown line format found while reading config file"
                print l
        f.close()

    def format_config(self):

        cfgstring = ""
        cfgstring += "daq_dir\t\t\t\t%s\n"%self.daq_dir
        cfgstring += "ssh_id_file\t\t\t%s\n"%self.ssh_id_file
        cfgstring += "executable\t\t\t%s\n"%self.executable
        cfgstring += "start_file\t\t\t%s\n"%self.start_file
        cfgstring += "quit_file\t\t\t%s\n"%self.quit_file

        cfgstring += "run_number\t\t\t%d\n"%self.run_number
        #cfgstring += "process_mode\t\t\t%s\n"%self.process_mode
        if (self.run_number): cfgstring += "process_id\t\t\t%d\n"%self.process_id

        cfgstring += "node_id\t\t\t\t%d\n"%self.node_id
        cfgstring += "node_ip\t\t\t\t%s\n"%self.node_ip

        cfgstring += "config_file\t\t\t%s\n"%self.config_file
        cfgstring += "log_file\t\t\t%s\n"%self.log_file
        cfgstring += "lock_file\t\t\t%s\n"%self.lock_file
        cfgstring += "initok_file\t\t\t%s\n"%self.initok_file
        cfgstring += "initfail_file\t\t\t%s\n"%self.initfail_file

        cfgstring += "output_mode\t\t\t%s\n"%self.output_mode
        if self.output_mode == "STREAM":
            cfgstring += "output_stream\t\t\t%s\n"%self.output_stream
        elif self.output_mode == "FILE":
            cfgstring += "data_dir\t\t\t%s\n"%self.data_dir
            cfgstring += "data_file\t\t\t%s\n"%self.data_file
            cfgstring += "file_max_duration\t\t%d\n"%self.file_max_duration
            cfgstring += "file_max_size\t\t\t%d\n"%self.file_max_size
            cfgstring += "file_max_events\t\t%d\n"%self.file_max_events

        cfgstring += "total_daq_time\t\t\t%d\n"%self.total_daq_time

        cfgstring += "trigger_addr\t\t\t%s\n"%self.trigger_addr
        cfgstring += "trigger_port\t\t\t%d\n"%self.trigger_port

        cfgstring += "trigger_mask\t\t\t0x%02x\n"%self.trigger_mask
        cfgstring += "busy_mask\t\t\t0x%02x\n"%self.busy_mask

        cfgstring += "trigger0_delay\t0x%02x\n"%self.trigger0_delay

        cfgstring += "correlated_trigger_delay\t0x%04x\n"%self.correlated_trigger_delay

        if (self.trigger_mask & 0x01):
            #cfgstring += "trig0_scale_global\t\t%d\n"%self.trig0_scale_global
            cfgstring += "trig0_scale_autopass\t\t%d\n"%self.trig0_scale_autopass

        if (self.trigger_mask & 0x02):
            cfgstring += "trig1_scale_global\t\t%d\n"%self.trig1_scale_global
            cfgstring += "trig1_scale_autopass\t\t%d\n"%self.trig1_scale_autopass

        if (self.trigger_mask & 0x04):
            cfgstring += "trig2_scale_global\t\t%d\n"%self.trig2_scale_global
            cfgstring += "trig2_scale_autopass\t\t%d\n"%self.trig2_scale_autopass

        if (self.trigger_mask & 0x08):
            cfgstring += "trig3_scale_global\t\t%d\n"%self.trig3_scale_global
            cfgstring += "trig3_scale_autopass\t\t%d\n"%self.trig3_scale_autopass

        if (self.trigger_mask & 0x10):
            cfgstring += "trig4_scale_global\t\t%d\n"%self.trig4_scale_global
            cfgstring += "trig4_scale_autopass\t\t%d\n"%self.trig4_scale_autopass

        if (self.trigger_mask & 0x20):
            cfgstring += "trig5_scale_global\t\t%d\n"%self.trig5_scale_global
            cfgstring += "trig5_scale_autopass\t\t%d\n"%self.trig5_scale_autopass

        if (self.trigger_mask & 0x40):
            cfgstring += "trig6_scale_global\t\t%d\n"%self.trig6_scale_global
            cfgstring += "trig6_scale_autopass\t\t%d\n"%self.trig6_scale_autopass

        if (self.trigger_mask & 0x80):
            cfgstring += "trig7_scale_global\t\t%d\n"%self.trig7_scale_global
            cfgstring += "trig7_scale_autopass\t\t%d\n"%self.trig7_scale_autopass

        cfgstring += "timepix_shutter_delay\t\t0x%02x\n"%self.timepix_shutter_delay
        cfgstring += "timepix_shutter_width\t\t0x%02x\n"%self.timepix_shutter_width

        cfgstring += "daq_loop_delay\t\t\t%d\n"%self.daq_loop_delay

        cfgstring += "debug_scale\t\t\t%d\n"%self.debug_scale

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
        if self.process_id == -1:
            print "Trigger::create_trigger - ERROR: unable to create new Trigger process in DB"
            return "error"

        self.db.add_cfg_para_proc(self.process_id,"daq_dir",            self.daq_dir)
        self.db.add_cfg_para_proc(self.process_id,"ssh_id_file",        self.ssh_id_file)
        self.db.add_cfg_para_proc(self.process_id,"executable",         self.executable)
        self.db.add_cfg_para_proc(self.process_id,"start_file",         self.start_file)
        self.db.add_cfg_para_proc(self.process_id,"quit_file",          self.quit_file)

        #self.db.add_cfg_para_proc(self.process_id,"run_number",         repr(self.run_number))
        #self.db.add_cfg_para_proc(self.process_id,"process_mode",       self.process_mode)

        #self.db.add_cfg_para_proc(self.process_id,"node_id",            repr(self.node_id))
        self.db.add_cfg_para_proc(self.process_id,"node_ip",            self.node_ip)
                                                         
        self.db.add_cfg_para_proc(self.process_id,"config_file",        self.config_file)
        self.db.add_cfg_para_proc(self.process_id,"log_file",           self.log_file)
        self.db.add_cfg_para_proc(self.process_id,"lock_file",          self.lock_file)
        self.db.add_cfg_para_proc(self.process_id,"initok_file",        self.initok_file)
        self.db.add_cfg_para_proc(self.process_id,"initfail_file",      self.initfail_file)

        self.db.add_cfg_para_proc(self.process_id,"output_mode", self.output_mode)
        if self.output_mode == "STREAM":
            self.db.add_cfg_para_proc(self.process_id,"output_stream", self.output_stream)
        elif self.output_mode == "FILE":
            self.db.add_cfg_para_proc(self.process_id,"data_dir",  self.data_dir)
            self.db.add_cfg_para_proc(self.process_id,"data_file", self.data_file)
            self.db.add_cfg_para_proc(self.process_id,"file_max_duration", self.file_max_duration)
            self.db.add_cfg_para_proc(self.process_id,"file_max_size",     self.file_max_size)
            self.db.add_cfg_para_proc(self.process_id,"file_max_events",   self.file_max_events)

        self.db.add_cfg_para_proc(self.process_id,"total_daq_time", repr(self.total_daq_time))

        self.db.add_cfg_para_proc(self.process_id,"trigger_addr", self.trigger_addr)
        self.db.add_cfg_para_proc(self.process_id,"trigger_port", repr(self.trigger_port))

        self.db.add_cfg_para_proc(self.process_id,"trigger_mask", "%#02x"%self.trigger_mask)
        self.db.add_cfg_para_proc(self.process_id,"busy_mask",    "%#02x"%self.busy_mask)

        self.db.add_cfg_para_proc(self.process_id,"trigger0_delay", "%#02x"%self.trigger0_delay)

        self.db.add_cfg_para_proc(self.process_id,"correlated_trigger_delay", "%#04x"%self.correlated_trigger_delay)

        if (self.trigger_mask & 0x01):
            #self.db.add_cfg_para_proc(self.process_id,"trig0_scale_global", repr(self.trig0_scale_global))
            self.db.add_cfg_para_proc(self.process_id,"trig0_scale_autopass", repr(self.trig0_scale_autopass))

        if (self.trigger_mask & 0x02):
            self.db.add_cfg_para_proc(self.process_id,"trig1_scale_global", repr(self.trig1_scale_global))
            self.db.add_cfg_para_proc(self.process_id,"trig1_scale_autopass", repr(self.trig1_scale_autopass))

        if (self.trigger_mask & 0x04):
            self.db.add_cfg_para_proc(self.process_id,"trig2_scale_global", repr(self.trig2_scale_global))
            self.db.add_cfg_para_proc(self.process_id,"trig2_scale_autopass", repr(self.trig2_scale_autopass))

        if (self.trigger_mask & 0x08):
            self.db.add_cfg_para_proc(self.process_id,"trig3_scale_global", repr(self.trig3_scale_global))
            self.db.add_cfg_para_proc(self.process_id,"trig3_scale_autopass", repr(self.trig3_scale_autopass))

        if (self.trigger_mask & 0x10):
            self.db.add_cfg_para_proc(self.process_id,"trig4_scale_global", repr(self.trig4_scale_global))
            self.db.add_cfg_para_proc(self.process_id,"trig4_scale_autopass", repr(self.trig4_scale_autopass))

        if (self.trigger_mask & 0x20):
            self.db.add_cfg_para_proc(self.process_id,"trig5_scale_global", repr(self.trig5_scale_global))
            self.db.add_cfg_para_proc(self.process_id,"trig5_scale_autopass", repr(self.trig5_scale_autopass))

        if (self.trigger_mask & 0x40):
            self.db.add_cfg_para_proc(self.process_id,"trig6_scale_global", repr(self.trig6_scale_global))
            self.db.add_cfg_para_proc(self.process_id,"trig6_scale_autopass", repr(self.trig6_scale_autopass))

        if (self.trigger_mask & 0x80):
            self.db.add_cfg_para_proc(self.process_id,"trig7_scale_global", repr(self.trig7_scale_global))
            self.db.add_cfg_para_proc(self.process_id,"trig7_scale_autopass", repr(self.trig7_scale_autopass))

        self.db.add_cfg_para_proc(self.process_id,"timepix_shutter_delay", "%#02x"%self.timepix_shutter_delay)
        self.db.add_cfg_para_proc(self.process_id,"timepix_shutter_width", "%#02x"%self.timepix_shutter_width)

        self.db.add_cfg_para_proc(self.process_id,"daq_loop_delay", repr(self.daq_loop_delay))

        self.db.add_cfg_para_proc(self.process_id,"debug_scale", repr(self.debug_scale))

        return "ok"

    def start_trig(self):

        command = "%s -c %s"%(self.executable,self.config_file)

        # If Trigger process runs on a remote node then start it using passwordless ssh connection
        if self.node_id != 0:
            command = "ssh -i %s %s '( %s )'"%(self.ssh_id_file,self.node_ip,command)

        print "- Starting Trigger process"
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

        # Tag start of process in DB
        if self.run_number: self.db.set_process_time_create(self.process_id)

        # Return process id
        return self.process.pid

    def stop_trig(self):

        # Wait up to 5 seconds for DAQ to stop of its own (on quit file or on time elapsed)
        for i in range(10):
            if self.process.poll() != None:
                # Process exited: clean up defunct process and close log file
                self.process.wait()
                self.log_handle.close()
                if self.run_number: self.db.set_process_time_end(self.process_id)
                return True
            time.sleep(0.5)

        # Process did not stop on its own: try sending and interrupt
        print "Trigger::stop_trig - WARNING: Trigger process did not stop on its own. Sending interrupt"
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
                if self.run_number: self.db.set_process_time_end(self.process_id)
                return True
            time.sleep(0.5)

        # Process did not stop smoothly: terminate it
        print "Trigger::stop_trig - WARNING: Trigger process did not stop on interrupt. Terminating it"
        self.process.terminate()
        time.sleep(1)
        if self.process.poll() != None:
            self.process.wait()
            self.log_handle.close()

        if self.run_number: self.db.set_process_time_end(self.process_id)
        return False
