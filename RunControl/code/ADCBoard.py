import os
import re
import subprocess
import time

class ADCBoard:

    def __init__(self,b_id):

        self.board_id = b_id

        self.status = "idle"

        self.set_default_config()

    def set_default_config(self):

        self.executable = os.getenv('PADME',".")+"/PadmeDAQ/PadmeDAQ.exe"

        self.run_number = 0

        self.config_file = "unset"
        self.log_file = "unset"

        self.start_file = "unset"
        self.quit_file = "unset"
        self.initok_file = "unset"
        self.initfail_file = "unset"
        self.lock_file = "unset"
        #self.db_file = "unset"

        self.data_file = "unset"

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
        self.zero_suppression = 1
        self.zs1_head = 80
        self.zs1_tail = 30
        self.zs1_nsigma = 3.
        self.zs1_nabovethr = 4

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
        setup_file = "setup/"+setup+"/board_%02d.cfg"%self.board_id
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
                else:
                    print "ADCBoard - WARNNING: unknown parameter found while reading default config file: %s"%p_name
            else:
                print "ADCBoard WARNING: unknown line format found while reading default config file"
                print l
        f.close()

    def set_board_id(self,b_id):

        self.board_id = b_id

    def format_config(self):

        cfgstring = ""
        cfgstring += "executable\t\t"+self.executable+"\n"
        cfgstring += "config_file\t\t"+self.config_file+"\n"
        cfgstring += "log_file\t\t"+self.log_file+"\n"

        cfgstring += "run_number\t\t"+str(self.run_number)+"\n"

        cfgstring += "board_id\t\t"+str(self.board_id)+"\n"

        cfgstring += "start_file\t\t"+self.start_file+"\n"
        cfgstring += "quit_file\t\t"+self.quit_file+"\n"
        cfgstring += "initok_file\t\t"+self.initok_file+"\n"
        cfgstring += "initfail_file\t\t"+self.initfail_file+"\n"
        cfgstring += "lock_file\t\t"+self.lock_file+"\n"
        #cfgstring += "db_file\t\t\t"+self.db_file+"\n"

        cfgstring += "data_file\t\t"+self.data_file+"\n"

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

        cfgstring += "file_max_duration\t"+repr(self.file_max_duration)+"\n"
        cfgstring += "file_max_size\t\t"+repr(self.file_max_size)+"\n"
        cfgstring += "file_max_events\t\t"+repr(self.file_max_events)+"\n"

        cfgstring += "zero_suppression\t"+repr(self.zero_suppression)+"\n"
        if (self.zero_suppression == 1):
            cfgstring += "zs1_head\t\t"+repr(self.zs1_head)+"\n"
            cfgstring += "zs1_tail\t\t"+repr(self.zs1_tail)+"\n"
            cfgstring += "zs1_nsigma\t\t"+repr(self.zs1_nsigma)+"\n"
            cfgstring += "zs1_nabovethr\t\t"+repr(self.zs1_nabovethr)+"\n"

        return cfgstring

    def write_config(self):

        if self.config_file == "unset":
            print "ADCBoard - ERROR: write_config() called but config_file not set!"
            return

        f = open(self.config_file,"w")
        f.write(self.format_config())
        f.close()

    def print_config(self):

        print self.format_config()

    def start_daq(self):

        # Open log file
        self.log_handle = open(self.log_file,"w")

        # Start DAQ process
        try:
            self.process = subprocess.Popen([self.executable,"-c",self.config_file],stdout=self.log_handle,stderr=subprocess.STDOUT,bufsize=1)
            #self.process = subprocess.Popen([self.executable,"-c",self.config_file],stdin=subprocess.DEVNULL,stdout=self.log_handle,stderr=subprocess.STDOUT,bufsize=1)
            #self.process = subprocess.Popen([self.executable,"-c",self.config_file],stdout=self.log_handle,stderr=subprocess.STDOUT,bufsize=1,creationflags=subprocess.DETACHED_PROCESS | subprocess.CREATE_NEW_PROCESS_GROUP)
            #self.process = subprocess.Popen(["/bin/sleep","60"],stdout=self.log_handle,stderr=subprocess.STDOUT,bufsize=1)
        except OSError as e:
            print "ADCBoard - ERROR: Execution failed: %s",e
            return 0

        # Return process id
        return self.process.pid

    def stop_daq(self):

        # Wait up to 5 seconds for DAQ to stop
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
