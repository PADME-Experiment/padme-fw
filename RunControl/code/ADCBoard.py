import os
import re
import subprocess
import time

from PadmeDB  import PadmeDB

class ADCBoard:

    def __init__(self,b_id):

        self.board_id = b_id

        # Get position of DAQ main directory from PADME_DAQ_DIR environment variable
        # Default to current dir if not set
        self.daq_dir = os.getenv('PADME_DAQ_DIR',".")

        self.db = PadmeDB()

        self.status = "idle"

        self.set_default_config()

    def set_default_config(self):

        self.node_id = 0
        self.node_ip = ""
        self.conet2_link = -1
        self.conet2_slot = -1

        self.executable = os.getenv('PADME',".")+"/PadmeDAQ/PadmeDAQ.exe"

        self.run_number = 0

        self.process_mode = "DAQ"

        self.config_file_daq = "unset"
        self.log_file_daq = "unset"
        self.lock_file_daq = "unset"
        self.output_stream_daq = "unset"
        self.initok_file_daq = "unset"
        self.initfail_file_daq = "unset"

        self.config_file_zsup = "unset"
        self.log_file_zsup = "unset"
        self.lock_file_zsup = "unset"
        self.input_stream_zsup = "unset"
        self.output_stream_zsup = "unset"
        self.initok_file_zsup = "unset"
        self.initfail_file_zsup = "unset"

        self.start_file = "unset"
        self.quit_file = "unset"

        #self.data_dir = "unset"
        #self.data_file = "unset"

        self.total_daq_time = 0

        # Default V1742 ADC settings
        self.startdaq_mode = 0
        self.trigger_mode = 1
        self.trigger_iolevel = "NIM"
        self.group_enable_mask = int('0xf',0)
        self.channel_enable_mask = int('0xffffffff',0)
        self.offset_global = int('0x5600',0)
        self.offset_ch = []
        for ch in range(32):
            self.offset_ch.append(self.offset_global)
        self.post_trigger_size = 65
        self.max_num_events_blt = 128
        self.drs4_sampfreq = 2
        self.drs4corr_enable = 1

        # Default DAQ control parameters
        self.daq_loop_delay = 100000
        self.file_max_duration = 900
        self.file_max_size = 1024*1024*1024
        self.file_max_events = 100000

        # Default zero suppression settings
        self.zero_suppression = 0
        self.zs1_head = 80
        self.zs1_tail = 30
        self.zs1_nsigma = 3.
        self.zs1_nabovethr = 4
        self.zs2_minrms = 4.6
        self.zs2_tail = 30

    def read_setup(self,setup):

        if (self.board_id == -1):
            print "ADCBoard - ERROR: board id not set while reading setup"
            return

        # Define regular expressions used in file parsing
        re_empty = re.compile("^\s*$")
        re_comment = re.compile("^\s*#")
        re_param = re.compile("^\s*(\w+)\s+(.+?)\s*$")
        re_choffset = re.compile("^\s*(\d+)\s+(\w+)\s*$")

        # Read default board configuration from file
        setup_file = self.daq_dir+"/setup/"+setup+"/board_%02d.cfg"%self.board_id
        if (not os.path.isfile(setup_file)):
            print "ADCBoard - WARNING: setup file %s not found for board %d"%(setup_file,self.board_id)
            return
        f = open(setup_file)
        for l in f:
            if (re_empty.search(l) or re_comment.search(l)): continue
            m = re_param.search(l)
            if (m):
                (p_name,p_value) = m.group(1,2)
                if   (p_name == "startdaq_mode"): self.startdaq_mode = int(p_value,0)
                elif (p_name == "trigger_mode"): self.trigger_mode = int(p_value,0)
                elif (p_name == "trigger_iolevel"): self.trigger_iolevel = p_value
                elif (p_name == "group_enable_mask"): self.group_enable_mask = int(p_value,0)
                elif (p_name == "channel_enable_mask"): self.channel_enable_mask = int(p_value,0)
                elif (p_name == "offset_global"):
                    self.offset_global = int(p_value,0)
                    for ch in range(0,32): self.offset_ch[ch] = self.offset_global
                elif (p_name == "offset_ch"):
                    mm = re_choffset.search(p_value)
                    if (mm):
                        (ch,offset) = mm.group(1,2)
                        self.offset_ch[int(ch,0)] = int(offset,0)
                    else:
                        print "ADCBoard - ERROR decoding channel offset parameter in line:"
                        print l
                elif (p_name == "post_trigger_size"): self.post_trigger_size = int(p_value,0)
                elif (p_name == "max_num_events_blt"): self.max_num_events_blt = int(p_value,0)
                elif (p_name == "drs4corr_enable"): self.drs4corr_enable = int(p_value,0)
                elif (p_name == "drs4_sampfreq"): self.drs4_sampfreq = int(p_value,0)
                elif (p_name == "daq_loop_delay"): self.daq_loop_delay = int(p_value,0)
                elif (p_name == "file_max_duration"): self.file_max_duration = int(p_value,0)
                elif (p_name == "file_max_size"): self.file_max_size = int(p_value,0)
                elif (p_name == "file_max_events"): self.file_max_events = int(p_value,0)
                elif (p_name == "zero_suppression"): self.zero_suppression = int(p_value,0)
                elif (p_name == "zs1_head"): self.zs1_head = int(p_value,0)
                elif (p_name == "zs1_tail"): self.zs1_tail = int(p_value,0)
                elif (p_name == "zs1_nsigma"): self.zs1_nsigma = float(p_value)
                elif (p_name == "zs1_nabovethr"): self.zs1_nabovethr = int(p_value,0)
                elif (p_name == "zs2_tail"): self.zs2_tail = int(p_value,0)
                elif (p_name == "zs2_minrms"): self.zs2_minrms = float(p_value)
                else:
                    print "ADCBoard - WARNNING: unknown parameter found while reading default config file: %s"%p_name
            else:
                print "ADCBoard WARNING: unknown line format found while reading default config file"
                print l
        f.close()

    #def set_board_id(self,b_id):
    #
    #    self.board_id = b_id

    def format_config_daq(self):

        cfgstring = ""
        cfgstring += "executable\t\t"+self.executable+"\n"

        cfgstring += "config_file\t\t"+self.config_file_daq+"\n"
        cfgstring += "log_file\t\t"+self.log_file_daq+"\n"
        cfgstring += "lock_file\t\t"+self.lock_file_daq+"\n"

        if (self.run_number): cfgstring += "process_id\t\t%d\n"%self.proc_daq_id
        #cfgstring += "process_mode\t\tDAQ\n"
        cfgstring += "process_mode\t\t"+self.process_mode+"\n"

        cfgstring += "run_number\t\t"+str(self.run_number)+"\n"

        cfgstring += "board_id\t\t"+str(self.board_id)+"\n"

        cfgstring += "node_id\t\t\t"+str(self.node_id)+"\n"
        cfgstring += "node_ip\t\t\t"+self.node_ip+"\n"
        cfgstring += "conet2_link\t\t"+str(self.conet2_link)+"\n"
        cfgstring += "conet2_slot\t\t"+str(self.conet2_slot)+"\n"

        cfgstring += "start_file\t\t"+self.start_file+"\n"
        cfgstring += "quit_file\t\t"+self.quit_file+"\n"
        cfgstring += "initok_file\t\t"+self.initok_file_daq+"\n"
        cfgstring += "initfail_file\t\t"+self.initfail_file_daq+"\n"

        cfgstring += "output_mode\t\tSTREAM\n"
        cfgstring += "output_stream\t\t"+self.output_stream_daq+"\n"

        cfgstring += "total_daq_time\t\t"+repr(self.total_daq_time)+"\n"

        cfgstring += "startdaq_mode\t\t"+repr(self.startdaq_mode)+"\n"
        cfgstring += "trigger_mode\t\t"+repr(self.trigger_mode)+"\n"
        cfgstring += "trigger_iolevel\t\t"+self.trigger_iolevel+"\n"

        cfgstring += "group_enable_mask\t0x%1x\n"%self.group_enable_mask
        cfgstring += "channel_enable_mask\t0x%08x\n"%self.channel_enable_mask

        cfgstring += "offset_global\t\t0x%04x\n"%self.offset_global
        for ch in range(32):
            if (self.offset_ch[ch] != self.offset_global):
                cfgstring += "offset_ch\t%d\t0x%04x\n"%(ch,self.offset_ch[ch])

        cfgstring += "post_trigger_size\t"+repr(self.post_trigger_size)+"\n"
        cfgstring += "max_num_events_blt\t"+repr(self.max_num_events_blt)+"\n"

        cfgstring += "drs4corr_enable\t\t"+repr(self.drs4corr_enable)+"\n"
        cfgstring += "drs4_sampfreq\t\t"+repr(self.drs4_sampfreq)+"\n"

        cfgstring += "daq_loop_delay\t\t"+repr(self.daq_loop_delay)+"\n"

        return cfgstring

    def format_config_zsup(self):

        cfgstring = ""
        cfgstring += "executable\t\t"+self.executable+"\n"

        cfgstring += "config_file\t\t"+self.config_file_zsup+"\n"
        cfgstring += "log_file\t\t"+self.log_file_zsup+"\n"
        cfgstring += "lock_file\t\t"+self.lock_file_zsup+"\n"
        cfgstring += "initok_file\t\t"+self.initok_file_zsup+"\n"
        cfgstring += "initfail_file\t\t"+self.initfail_file_zsup+"\n"

        if (self.run_number): cfgstring += "process_id\t\t%d\n"%self.proc_zsup_id
        cfgstring += "process_mode\t\tZSUP\n"

        cfgstring += "run_number\t\t"+str(self.run_number)+"\n"

        cfgstring += "board_id\t\t"+str(self.board_id)+"\n"

        cfgstring += "node_id\t\t\t"+str(self.node_id)+"\n"
        cfgstring += "node_ip\t\t\t"+self.node_ip+"\n"
        cfgstring += "conet2_link\t\t"+str(self.conet2_link)+"\n"
        cfgstring += "conet2_slot\t\t"+str(self.conet2_slot)+"\n"

        cfgstring += "input_stream\t\t"+self.input_stream_zsup+"\n"

        cfgstring += "output_mode\t\tSTREAM\n"
        cfgstring += "output_stream\t\t"+self.output_stream_zsup+"\n"

        cfgstring += "zero_suppression\t"+repr(self.zero_suppression)+"\n"
        if (self.zero_suppression%100 == 1):
            cfgstring += "zs1_head\t\t"+repr(self.zs1_head)+"\n"
            cfgstring += "zs1_tail\t\t"+repr(self.zs1_tail)+"\n"
            cfgstring += "zs1_nsigma\t\t"+repr(self.zs1_nsigma)+"\n"
            cfgstring += "zs1_nabovethr\t\t"+repr(self.zs1_nabovethr)+"\n"
        elif (self.zero_suppression%100 == 2):
            cfgstring += "zs2_tail\t\t"+repr(self.zs2_tail)+"\n"
            cfgstring += "zs2_minrms\t\t"+repr(self.zs2_minrms)+"\n"

        return cfgstring

    def write_config(self):

        if self.config_file_daq == "unset":
            print "ADCBoard - ERROR: write_config() called but config_file_daq not set!"
            return

        f = open(self.config_file_daq,"w")
        f.write(self.format_config_daq())
        f.close()

        if self.config_file_zsup == "unset":
            print "ADCBoard - ERROR: write_config() called but config_file_zsup not set!"
            return

        f = open(self.config_file_zsup,"w")
        f.write(self.format_config_zsup())
        f.close()

    def print_config(self):

        print self.format_config_daq()
        print self.format_config_zsup()

    def create_proc_daq(self):

        # Create DAQ process in DB
        self.proc_daq_id = self.db.create_process("DAQ",self.run_number,self.get_link_id())
        if self.proc_daq_id == -1:
            print "ADCBoard::create_proc_daq - ERROR: unable to create new DAQ proces in DB"
            return "error"

        return "ok"

    def create_proc_zsup(self):

        # Create ZSUP process in DB
        self.proc_zsup_id = self.db.create_process("ZSUP",self.run_number,self.get_link_id())
        if self.proc_zsup_id == -1:
            print "ADCBoard::create_proc_zsup - ERROR: unable to create new ZSUP proces in DB"
            return "error"

        return "ok"

    def get_link_id(self):

        # Convert PadmeDAQ link description to link id from DB
        if (self.node_id == -1 or self.conet2_link == -1 or self.conet2_slot == -1): return -1
        return self.db.get_link_id(self.node_id,self.conet2_link/8,self.conet2_link%8,self.conet2_slot)

    def start_daq(self):

        command = "%s -c %s"%(self.executable,self.config_file_daq)

        # If DAQ process runs on a remote node then start it using passwordless ssh connection
        if self.node_id != 0:
            command = "ssh -i ~/.ssh/id_rsa_daq %s %s"%(self.node_ip,command)

        print "- Start DAQ process for board %d"%self.board_id
        print command
        print "  Log written to %s"%self.log_file_daq

        # Open log file
        self.log_handle_daq = open(self.log_file_daq,"w")

        # Start DAQ process
        #try:
        #    self.process_daq = subprocess.Popen([self.executable,"-c",self.config_file_daq],stdout=self.log_handle_daq,stderr=subprocess.STDOUT,bufsize=1)
        #except OSError as e:
        #    print "ADCBoard - ERROR: DAQ Execution failed: %s",e
        #    return 0

        # Start DAQ process
        try:
            self.process_daq = subprocess.Popen(command.split(),stdout=self.log_handle_daq,stderr=subprocess.STDOUT,bufsize=1)
        except OSError as e:
            print "ADCBoard::start_daq - ERROR: Execution failed: %s",e
            return 0                

        # Return process id
        return self.process_daq.pid

    def stop_daq(self):

        # Send an interrupt to DAQ process
        if self.node_id == 0:
            # If process is on local host, just send a kill signal
            command = "kill %d"%self.process_daq.pid
        else:
            # If it is on a remote host, use ssh to send kill command.
            # PID on remote host is recovered from the lock file
            command = "ssh -i ~/.ssh/id_rsa_daq %s '( kill `cat %s` )'"%(self.node_ip,self.lock_file_daq)
        os.system(command)

        # Wait up to 10 seconds for DAQ to stop
        for i in range(10):

            if self.process_daq.poll() != None:

                # Process exited: clean up defunct process and close log file
                self.process_daq.wait()
                self.log_handle_daq.close()
                return 1

            time.sleep(1)

        # Process did not stop smoothly: stop it
        self.process_daq.terminate()
        time.sleep(1)
        if self.process_daq.poll() != None:
            self.process_daq.wait()
            self.log_handle_daq.close()
        return 0

    def start_zsup(self):

        command = "%s -c %s"%(self.executable,self.config_file_zsup)

        # If ZSUP process runs on a remote node then start it using passwordless ssh connection
        if self.node_id != 0:
            command = "ssh -i ~/.ssh/id_rsa_daq %s %s"%(self.node_ip,command)

        print "- Start ZSUP process for board %d"%self.board_id
        print command
        print "  Log written to %s"%self.log_file_zsup

        # Open log file
        self.log_handle_zsup = open(self.log_file_zsup,"w")

        # Start ZSUP process
        #try:
        #    self.process_zsup = subprocess.Popen([self.executable,"-c",self.config_file_zsup],stdout=self.log_handle_zsup,stderr=subprocess.STDOUT,bufsize=1)
        #except OSError as e:
        #    print "ADCBoard - ERROR: ZSUP execution failed: %s",e
        #    return 0

        # Start ZSUP process
        try:
            self.process_zsup = subprocess.Popen(command.split(),stdout=self.log_handle_zsup,stderr=subprocess.STDOUT,bufsize=1)
        except OSError as e:
            print "ADCBoard::start_zsup - ERROR: Execution failed: %s",e
            return 0                

        # Return process id
        return self.process_zsup.pid

    def stop_zsup(self):

        # Wait up to 5 seconds for ZSUP to stop
        for i in range(5):

            if self.process_zsup.poll() != None:

                # Process exited: clean up defunct process and close log file
                self.process_zsup.wait()
                self.log_handle_zsup.close()
                return 1

            time.sleep(1)

        # Process did not stop smoothly: stop it
        self.process_zsup.terminate()
        time.sleep(1)
        if self.process_zsup.poll() != None:
            self.process_zsup.wait()
            self.log_handle_zsup.close()
        return 0
