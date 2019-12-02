import os
import re
import time
import shlex
import subprocess

from PadmeDB  import PadmeDB

class ADCBoard:

    def __init__(self,b_id):

        self.board_id = b_id

        # Get position of DAQ main directory from PADME_DAQ_DIR environment variable
        # Default to current dir if not set
        self.daq_dir = os.getenv('PADME_DAQ_DIR',".")

        # Define id file for passwordless ssh command execution
        self.ssh_id_file = "%s/.ssh/id_rsa_daq"%os.getenv('HOME',"~")

        self.db = PadmeDB()

        self.status = "idle"

        self.re_dbinfo_line = re.compile("^\s*DBINFO\s.*$")

        self.set_default_config()

    def set_default_config(self):

        self.node_id = 0
        self.node_ip = ""
        self.conet2_link = -1
        self.conet2_slot = -1

        self.executable = os.getenv('PADME',".")+"/PadmeDAQ/PadmeDAQ.exe"

        self.run_number = 0
        self.process_id = -1

        self.process_mode = "DAQ"

        self.config_file_daq = "unset"
        self.log_file_daq = "unset"
        self.lock_file_daq = "unset"
        self.initok_file_daq = "unset"
        self.initfail_file_daq = "unset"

        self.output_mode_daq = "STREAM"
        self.output_stream_daq = "unset"
        self.data_dir_daq = "unset"
        self.data_file_daq = "daq"

        self.config_file_zsup = "unset"
        self.log_file_zsup = "unset"
        self.lock_file_zsup = "unset"
        self.initok_file_zsup = "unset"
        self.initfail_file_zsup = "unset"

        self.input_stream_zsup = "unset"

        self.output_mode = "STREAM"
        self.output_stream_zsup = "unset"
        self.data_dir_zsup = "unset"
        self.data_file_zsup = "zsup"

        self.start_file = "unset"
        self.quit_file = "unset"

        self.total_daq_time = 0

        # Default V1742 ADC settings
        self.startdaq_mode = 0
        self.trigger_mode = 1
        self.trigger_iolevel = "NIM"
        self.group_enable_mask = int('0xf',0)
        self.channel_enable_mask = int('0xffffffff',0)
        self.offset_global = int('0x5600',0)
        self.offset_ch = []
        for ch in range(32): self.offset_ch.append(self.offset_global)
        self.post_trigger_size = 65
        self.max_num_events_blt = 128
        self.drs4_sampfreq = 2
        self.drs4corr_enable = 1

        # Autopass parameters (threshold in counts, duration in ns)
        self.auto_threshold = int('0x0400',0)
        self.auto_duration = 150

        # Default zero suppression settings
        self.zero_suppression = 0

        self.zs1_head = 80
        self.zs1_tail = 30
        self.zs1_nsigma = 3.
        self.zs1_nabovethr = 4
        self.zs1_badrmsthr = 15.

        self.zs2_tail = 30
        self.zs2_minrms = 4.6
        self.zs2_minrms_ch = []
        for ch in range(32): self.zs2_minrms_ch.append(self.zs2_minrms)

        # Default file parameters
        self.file_max_duration = 3600
        self.file_max_size     = 1024*1024*1024
        self.file_max_events   = 1000*1000

        # Default DAQ control parameters
        self.daq_loop_delay = 10000
        self.debug_scale = 100

    def read_setup(self,setup):

        if (self.board_id == -1):
            print "ADCBoard - ERROR: board id not set while reading setup"
            return

        # Define regular expressions used in file parsing
        re_empty = re.compile("^\s*$")
        re_comment = re.compile("^\s*#")
        re_param = re.compile("^\s*(\w+)\s+(.+?)\s*$")
        re_chvalue = re.compile("^\s*(\d+)\s+(\w+)\s*$")

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
                    mm = re_chvalue.search(p_value)
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
                elif (p_name == "debug_scale"): self.debug_scale = int(p_value,0)
                elif (p_name == "file_max_duration"): self.file_max_duration = int(p_value,0)
                elif (p_name == "file_max_size"): self.file_max_size = int(p_value,0)
                elif (p_name == "file_max_events"): self.file_max_events = int(p_value,0)
                elif (p_name == "zero_suppression"): self.zero_suppression = int(p_value,0)
                elif (p_name == "zs1_head"): self.zs1_head = int(p_value,0)
                elif (p_name == "zs1_tail"): self.zs1_tail = int(p_value,0)
                elif (p_name == "zs1_nsigma"): self.zs1_nsigma = float(p_value)
                elif (p_name == "zs1_nabovethr"): self.zs1_nabovethr = int(p_value,0)
                elif (p_name == "zs2_tail"): self.zs2_tail = int(p_value,0)
                elif (p_name == "zs2_minrms"):
                    self.zs2_minrms = float(p_value)
                    for ch in range(32): self.zs2_minrms_ch[ch] = self.zs2_minrms
                elif (p_name == "zs2_minrms_ch"):
                    mm = re_chvalue.search(p_value)
                    if (mm):
                        (ch,minrms) = mm.group(1,2)
                        self.zs2_minrms_ch[int(ch,0)] = int(minrms,0)
                    else:
                        print "ADCBoard - ERROR decoding channel zs2_minrms parameter in line:"
                        print l
                elif (p_name == "auto_threshold"): self.auto_threshold = int(p_value,0)
                elif (p_name == "auto_duration"): self.auto_duration = int(p_value,0)
                else:
                    print "ADCBoard - WARNNING: unknown parameter found while reading default config file: %s"%p_name
            else:
                print "ADCBoard WARNING: unknown line format found while reading default config file"
                print l
        f.close()

    def config_list_daq(self):

        cfg_list = []

        cfg_list.append(["daq_dir",              self.daq_dir])
        cfg_list.append(["ssh_id_file",          self.ssh_id_file])
        cfg_list.append(["executable",           self.executable])
        cfg_list.append(["start_file",           self.start_file])
        cfg_list.append(["quit_file",            self.quit_file])

        cfg_list.append(["run_number",           str(self.run_number)])
        cfg_list.append(["board_id",             str(self.board_id)])
        cfg_list.append(["process_mode",         self.process_mode])
        if (self.run_number):
            cfg_list.append(["process_id",       str(self.proc_daq_id)])

        cfg_list.append(["node_id",              str(self.node_id)])
        cfg_list.append(["node_ip",              self.node_ip])
        cfg_list.append(["conet2_link",          str(self.conet2_link)])
        cfg_list.append(["conet2_slot",          str(self.conet2_slot)])

        cfg_list.append(["config_file",          self.config_file_daq])
        cfg_list.append(["log_file",             self.log_file_daq])
        cfg_list.append(["lock_file",            self.lock_file_daq])
        cfg_list.append(["initok_file",          self.initok_file_daq])
        cfg_list.append(["initfail_file",        self.initfail_file_daq])

        cfg_list.append(["output_mode",          self.output_mode_daq])
        if self.output_mode_daq == "STREAM":
            cfg_list.append(["output_stream",    self.output_stream_daq])
        elif self.output_mode_daq == "FILE":
            cfg_list.append(["data_dir",         self.data_dir_daq])
            cfg_list.append(["data_file",        self.data_file_daq])
            cfg_list.append(["file_max_duration",str(self.file_max_duration)])
            cfg_list.append(["file_max_size",    str(self.file_max_size)])
            cfg_list.append(["file_max_events",  str(self.file_max_events)])

        cfg_list.append(["total_daq_time",       str(self.total_daq_time)])

        cfg_list.append(["startdaq_mode",        str(self.startdaq_mode)])
        cfg_list.append(["trigger_mode",         str(self.trigger_mode)])
        cfg_list.append(["trigger_iolevel",      self.trigger_iolevel])

        cfg_list.append(["group_enable_mask",    "%#1x"%self.group_enable_mask])
        cfg_list.append(["channel_enable_mask",  "%#08x"%self.channel_enable_mask])

        cfg_list.append(["offset_global",        "%#04x"%self.offset_global])
        for ch in range(32):
            if (self.offset_ch[ch] != self.offset_global):
                cfg_list.append(["offset_ch",    "%2d %#04x"%(ch,self.offset_ch[ch])])

        cfg_list.append(["post_trigger_size",    str(self.post_trigger_size)])
        cfg_list.append(["max_num_events_blt",   str(self.max_num_events_blt)])

        cfg_list.append(["drs4corr_enable",      str(self.drs4corr_enable)])
        cfg_list.append(["drs4_sampfreq",        str(self.drs4_sampfreq)])

        cfg_list.append(["auto_threshold",       "%#04x"%self.auto_threshold])
        cfg_list.append(["auto_duration",        str(self.auto_duration)])

        cfg_list.append(["daq_loop_delay",       str(self.daq_loop_delay)])

        cfg_list.append(["debug_scale",          str(self.debug_scale)])

        return cfg_list

    def config_list_zsup(self):

        cfg_list = []

        cfg_list.append(["daq_dir",                  self.daq_dir])
        cfg_list.append(["ssh_id_file",              self.ssh_id_file])
        cfg_list.append(["executable",               self.executable])

        cfg_list.append(["run_number",               str(self.run_number)])
        cfg_list.append(["board_id",                 str(self.board_id)])
        cfg_list.append(["process_mode",             "ZSUP"])
        if (self.run_number):
            cfg_list.append(["process_id",           str(self.proc_zsup_id)])

        cfg_list.append(["node_id",                  str(self.node_id)])
        cfg_list.append(["node_ip",                  self.node_ip])

        cfg_list.append(["config_file",              self.config_file_zsup])
        cfg_list.append(["log_file",                 self.log_file_zsup])
        cfg_list.append(["lock_file",                self.lock_file_zsup])
        cfg_list.append(["initok_file",              self.initok_file_zsup])
        cfg_list.append(["initfail_file",            self.initfail_file_zsup])

        cfg_list.append(["output_mode",              self.output_mode_zsup])
        if self.output_mode_zsup == "STREAM":
            cfg_list.append(["output_stream",        self.output_stream_zsup])
        elif self.output_mode_zsup == "FILE":
            cfg_list.append(["data_dir",             self.data_dir_zsup])
            cfg_list.append(["data_file",            self.data_file_zsup])
            cfg_list.append(["file_max_duration",    str(self.file_max_duration)])
            cfg_list.append(["file_max_size",        str(self.file_max_size)])
            cfg_list.append(["file_max_events",      str(self.file_max_events)])

        cfg_list.append(["input_stream",             self.input_stream_zsup])
                                                     
        cfg_list.append(["zero_suppression",         str(self.zero_suppression)])
        if (self.zero_suppression%100 == 1):

            cfg_list.append(["zs1_head",             str(self.zs1_head)])
            cfg_list.append(["zs1_tail",             str(self.zs1_tail)])
            cfg_list.append(["zs1_nsigma",           str(self.zs1_nsigma)])
            cfg_list.append(["zs1_nabovethr",        str(self.zs1_nabovethr)])
            cfg_list.append(["zs1_badrmsthr",        str(self.zs1_badrmsthr)])

        elif (self.zero_suppression%100 == 2):

            cfg_list.append(["zs2_tail",             str(self.zs2_tail)])
            cfg_list.append(["zs2_minrms",           str(self.zs2_minrms)])
            for ch in range(32):
                if (self.zs2_minrms_ch[ch] != self.zs2_minrms):
                    cfg_list.append(["zs2_minrms_ch","%2d %f"%(ch,self.zs2_minrms_ch[ch])])

        cfg_list.append(["debug_scale",              str(self.debug_scale)])

        return cfg_list

    def format_config_daq(self):

        cfgstring = ""
        for cfg in self.config_list_daq(): cfgstring += "%-30s %s\n"%cfg
        return cfgstring

        #cfgstring = ""
        #cfgstring += "daq_dir\t\t\t%s\n"%self.daq_dir
        #cfgstring += "ssh_id_file\t\t%s\n"%self.ssh_id_file
        #cfgstring += "executable\t\t%s\n"%self.executable
        #cfgstring += "start_file\t\t%s\n"%self.start_file
        #cfgstring += "quit_file\t\t%s\n"%self.quit_file
        #
        #cfgstring += "run_number\t\t%d\n"%self.run_number
        #cfgstring += "board_id\t\t%d\n"%self.board_id
        #cfgstring += "process_mode\t\t%s\n"%self.process_mode
        #if (self.run_number): cfgstring += "process_id\t\t%d\n"%self.proc_daq_id
        #
        #cfgstring += "node_id\t\t\t%d\n"%self.node_id
        #cfgstring += "node_ip\t\t\t%s\n"%self.node_ip
        #cfgstring += "conet2_link\t\t%d\n"%self.conet2_link
        #cfgstring += "conet2_slot\t\t%d\n"%self.conet2_slot
        #
        #cfgstring += "config_file\t\t%s\n"%self.config_file_daq
        #cfgstring += "log_file\t\t%s\n"%self.log_file_daq
        #cfgstring += "lock_file\t\t%s\n"%self.lock_file_daq
        #cfgstring += "initok_file\t\t%s\n"%self.initok_file_daq
        #cfgstring += "initfail_file\t\t%s\n"%self.initfail_file_daq
        #
        #cfgstring += "output_mode\t\t\t%s\n"%self.output_mode_daq
        #if self.output_mode_daq == "STREAM":
        #    cfgstring += "output_stream\t\t\t%s\n"%self.output_stream_daq
        #elif self.output_mode_daq == "FILE":
        #    cfgstring += "data_dir\t\t\t%s\n"%self.data_dir_daq
        #    cfgstring += "data_file\t\t\t%s\n"%self.data_file_daq
        #    cfgstring += "file_max_duration\t\t%d\n"%self.file_max_duration
        #    cfgstring += "file_max_size\t\t\t%d\n"%self.file_max_size
        #    cfgstring += "file_max_events\t\t%d\n"%self.file_max_events
        #
        #cfgstring += "total_daq_time\t\t%d\n"%self.total_daq_time
        #
        #cfgstring += "startdaq_mode\t\t%d\n"%self.startdaq_mode
        #cfgstring += "trigger_mode\t\t%d\n"%self.trigger_mode
        #cfgstring += "trigger_iolevel\t\t%s\n"%self.trigger_iolevel
        #
        #cfgstring += "group_enable_mask\t%#1x\n"%self.group_enable_mask
        #cfgstring += "channel_enable_mask\t%#08x\n"%self.channel_enable_mask
        #
        #cfgstring += "offset_global\t\t%#04x\n"%self.offset_global
        #for ch in range(32):
        #    if (self.offset_ch[ch] != self.offset_global):
        #        cfgstring += "offset_ch\t%d\t%#04x\n"%(ch,self.offset_ch[ch])
        #
        #cfgstring += "post_trigger_size\t%d\n"%self.post_trigger_size
        #cfgstring += "max_num_events_blt\t%d\n"%self.max_num_events_blt
        #
        #cfgstring += "drs4corr_enable\t\t%d\n"%self.drs4corr_enable
        #cfgstring += "drs4_sampfreq\t\t%d\n"%self.drs4_sampfreq
        #
        #cfgstring += "auto_threshold\t\t%#04x\n"%self.auto_threshold
        #cfgstring += "auto_duration\t\t%d\n"%self.auto_duration
        #
        #cfgstring += "daq_loop_delay\t\t%d\n"%self.daq_loop_delay
        #
        #cfgstring += "debug_scale\t\t\t%d\n"%self.debug_scale
        #
        #return cfgstring

    def format_config_zsup(self):

        cfgstring = ""
        for cfg in self.config_list_zsup(): cfgstring += "%-30s %s\n"%cfg
        return cfgstring

        #cfgstring = ""
        #cfgstring += "daq_dir\t\t\t%s\n"%self.daq_dir
        #cfgstring += "ssh_id_file\t\t%s\n"%self.ssh_id_file
        #cfgstring += "executable\t\t%s\n"%self.executable
        #
        #cfgstring += "run_number\t\t%d\n"%self.run_number
        #cfgstring += "board_id\t\t%d\n"%self.board_id
        #cfgstring += "process_mode\t\tZSUP\n"
        #if (self.run_number): cfgstring += "process_id\t\t%d\n"%self.proc_zsup_id
        #
        #cfgstring += "node_id\t\t\t%d\n"%self.node_id
        #cfgstring += "node_ip\t\t\t%s\n"%self.node_ip
        ##cfgstring += "conet2_link\t\t%d\n"%self.conet2_link
        ##cfgstring += "conet2_slot\t\t%d\n"%self.conet2_slot
        #
        #cfgstring += "config_file\t\t%s\n"%self.config_file_zsup
        #cfgstring += "log_file\t\t%s\n"%self.log_file_zsup
        #cfgstring += "lock_file\t\t%s\n"%self.lock_file_zsup
        #cfgstring += "initok_file\t\t%s\n"%self.initok_file_zsup
        #cfgstring += "initfail_file\t\t%s\n"%self.initfail_file_zsup
        #
        #cfgstring += "output_mode\t\t\t%s\n"%self.output_mode_zsup
        #if self.output_mode_zsup == "STREAM":
        #    cfgstring += "output_stream\t\t\t%s\n"%self.output_stream_zsup
        #elif self.output_mode_zsup == "FILE":
        #    cfgstring += "data_dir\t\t\t%s\n"%self.data_dir_zsup
        #    cfgstring += "data_file\t\t\t%s\n"%self.data_file_zsup
        #    cfgstring += "file_max_duration\t\t%d\n"%self.file_max_duration
        #    cfgstring += "file_max_size\t\t\t%d\n"%self.file_max_size
        #    cfgstring += "file_max_events\t\t%d\n"%self.file_max_events
        #
        #cfgstring += "input_stream\t\t%s\n"%self.input_stream_zsup
        #
        #cfgstring += "zero_suppression\t%d\n"%self.zero_suppression
        #if (self.zero_suppression%100 == 1):
        #    cfgstring += "zs1_head\t\t%d\n"%self.zs1_head
        #    cfgstring += "zs1_tail\t\t%d\n"%self.zs1_tail
        #    cfgstring += "zs1_nsigma\t\t%f\n"%self.zs1_nsigma
        #    cfgstring += "zs1_nabovethr\t\t%d\n"%self.zs1_nabovethr
        #    cfgstring += "zs1_badrmsthr\t\t%f\n"%self.zs1_badrmsthr
        #elif (self.zero_suppression%100 == 2):
        #    cfgstring += "zs2_tail\t\t%d\n"%self.zs2_tail
        #    cfgstring += "zs2_minrms\t\t%f\n"%self.zs2_minrms
        #    for ch in range(32):
        #        if (self.zs2_minrms_ch[ch] != self.zs2_minrms):
        #            cfgstring += "zs2_minrms_ch\t%d\t%f\n"%(ch,self.zs2_minrms_ch[ch])
        #
        #cfgstring += "debug_scale\t\t\t%d\n"%self.debug_scale
        #
        #return cfgstring

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
        self.proc_daq_id = self.db.create_daq_process(self.run_number,self.node_id)
        if self.proc_daq_id == -1:
            print "ADCBoard::create_proc_daq - ERROR: unable to create new DAQ process in DB"
            return "error"

        # Add info about optical link
        self.db.add_daq_process_optical_link(self.proc_daq_id,self.node_id,self.conet2_link,self.conet2_slot)

        # Add all configuration parameters
        for cfg in self.config_list_daq():
            self.db.add_cfg_para_proc(self.proc_daq_id,cfg[0],cfg[1])

        #self.db.add_cfg_para_proc(self.proc_daq_id,"daq_dir",            self.daq_dir)
        #self.db.add_cfg_para_proc(self.proc_daq_id,"ssh_id_file",        self.ssh_id_file)
        #self.db.add_cfg_para_proc(self.proc_daq_id,"executable",         self.executable)
        #self.db.add_cfg_para_proc(self.proc_daq_id,"start_file",         self.start_file)
        #self.db.add_cfg_para_proc(self.proc_daq_id,"quit_file",          self.quit_file)
        #                                                                
        ##self.db.add_cfg_para_proc(self.proc_daq_id,"run_number",         repr(self.run_number))
        #self.db.add_cfg_para_proc(self.proc_daq_id,"board_id",           repr(self.board_id))
        #self.db.add_cfg_para_proc(self.proc_daq_id,"process_mode",       self.process_mode)
        #                                                                
        ##self.db.add_cfg_para_proc(self.proc_daq_id,"node_id",            repr(self.node_id))
        #self.db.add_cfg_para_proc(self.proc_daq_id,"node_ip",            self.node_ip)
        #self.db.add_cfg_para_proc(self.proc_daq_id,"conet2_link",        repr(self.conet2_link))
        #self.db.add_cfg_para_proc(self.proc_daq_id,"conet2_slot",        repr(self.conet2_slot))
        #                                                                
        #self.db.add_cfg_para_proc(self.proc_daq_id,"config_file",        self.config_file_daq)
        #self.db.add_cfg_para_proc(self.proc_daq_id,"log_file",           self.log_file_daq)
        #self.db.add_cfg_para_proc(self.proc_daq_id,"lock_file",          self.lock_file_daq)
        #self.db.add_cfg_para_proc(self.proc_daq_id,"initok_file",        self.initok_file_daq)
        #self.db.add_cfg_para_proc(self.proc_daq_id,"initfail_file",      self.initfail_file_daq)
        #                                                                
        #self.db.add_cfg_para_proc(self.proc_daq_id,"output_mode",        self.output_mode_daq)
        #if self.output_mode_daq == "STREAM":
        #    self.db.add_cfg_para_proc(self.proc_daq_id,"output_stream",      self.output_stream_daq)
        #elif self.output_mode_daq == "FILE":
        #    self.db.add_cfg_para_proc(self.proc_daq_id,"data_dir",  self.data_dir_daq)
        #    self.db.add_cfg_para_proc(self.proc_daq_id,"data_file", self.data_file_daq)
        #    self.db.add_cfg_para_proc(self.proc_daq_id,"file_max_duration", self.file_max_duration)
        #    self.db.add_cfg_para_proc(self.proc_daq_id,"file_max_size",     self.file_max_size)
        #    self.db.add_cfg_para_proc(self.proc_daq_id,"file_max_events",   self.file_max_events)
        #                    
        #self.db.add_cfg_para_proc(self.proc_daq_id,"total_daq_time",     repr(self.total_daq_time))
        #                                                                
        #self.db.add_cfg_para_proc(self.proc_daq_id,"startdaq_mode",      repr(self.startdaq_mode))
        #self.db.add_cfg_para_proc(self.proc_daq_id,"trigger_mode",       repr(self.trigger_mode))
        #self.db.add_cfg_para_proc(self.proc_daq_id,"trigger_iolevel",    self.trigger_iolevel)
        #
        #self.db.add_cfg_para_proc(self.proc_daq_id,"group_enable_mask",  "%#1x"%self.group_enable_mask)
        #self.db.add_cfg_para_proc(self.proc_daq_id,"channel_enable_mask","%#08x"%self.channel_enable_mask)
        #
        #self.db.add_cfg_para_proc(self.proc_daq_id,"offset_global",      "%#04x"%self.proc_daq_id)
        #for ch in range(32):
        #    if (self.offset_ch[ch] != self.offset_global):
        #        self.db.add_cfg_para_proc(self.proc_daq_id,"offset_ch",  "%d %#04x"%(ch,self.offset_ch[ch]))
        #
        #self.db.add_cfg_para_proc(self.proc_daq_id,"post_trigger_size",  repr(self.post_trigger_size))
        #self.db.add_cfg_para_proc(self.proc_daq_id,"max_num_events_blt", repr(self.max_num_events_blt))
        #
        #self.db.add_cfg_para_proc(self.proc_daq_id,"drs4corr_enable",    repr(self.drs4corr_enable))
        #self.db.add_cfg_para_proc(self.proc_daq_id,"drs4_sampfreq",      repr(self.drs4_sampfreq))
        #
        #self.db.add_cfg_para_proc(self.proc_daq_id,"auto_threshold",     "%#04x"%self.auto_threshold)
        #self.db.add_cfg_para_proc(self.proc_daq_id,"auto_duration",      repr(self.auto_duration))
        #
        #self.db.add_cfg_para_proc(self.proc_daq_id,"daq_loop_delay",     repr(self.daq_loop_delay))
        #
        #self.db.add_cfg_para_proc(self.proc_daq_id,"debug_scale",        repr(self.debug_scale))

        return "ok"

    def create_proc_zsup(self):

        # Create ZSUP process in DB
        self.proc_zsup_id = self.db.create_zsup_process(self.run_number,self.node_id)
        if self.proc_zsup_id == -1:
            print "ADCBoard::create_proc_zsup - ERROR: unable to create new ZSUP proces in DB"
            return "error"

        # Add all configuration parameters
        for cfg in self.config_list_zsup():
            self.db.add_cfg_para_proc(self.proc_zsup_id,cfg[0],cfg[1])

        #self.db.add_cfg_para_proc(self.proc_zsup_id,"daq_dir",            self.daq_dir)
        #self.db.add_cfg_para_proc(self.proc_zsup_id,"ssh_id_file",        self.ssh_id_file)
        #self.db.add_cfg_para_proc(self.proc_zsup_id,"executable",         self.executable)
        #
        ##self.db.add_cfg_para_proc(self.proc_zsup_id,"run_number",         repr(self.run_number))
        #self.db.add_cfg_para_proc(self.proc_zsup_id,"board_id",           repr(self.board_id))
        #self.db.add_cfg_para_proc(self.proc_zsup_id,"process_mode",       "ZSUP")
        #
        ##self.db.add_cfg_para_proc(self.proc_zsup_id,"node_id",            repr(self.node_id))
        #self.db.add_cfg_para_proc(self.proc_zsup_id,"node_ip",            self.node_ip)
        #
        #self.db.add_cfg_para_proc(self.proc_zsup_id,"config_file",        self.config_file_zsup)
        #self.db.add_cfg_para_proc(self.proc_zsup_id,"log_file",           self.log_file_zsup)
        #self.db.add_cfg_para_proc(self.proc_zsup_id,"lock_file",          self.lock_file_zsup)
        #self.db.add_cfg_para_proc(self.proc_zsup_id,"initok_file",        self.initok_file_zsup)
        #self.db.add_cfg_para_proc(self.proc_zsup_id,"initfail_file",      self.initfail_file_zsup)
        #
        #self.db.add_cfg_para_proc(self.proc_zsup_id,"output_mode",        self.output_mode_zsup)
        #if self.output_mode_zsup == "STREAM":
        #    self.db.add_cfg_para_proc(self.proc_zsup_id,"output_stream", self.output_stream_zsup)
        #elif self.output_mode_zsup == "FILE":
        #    self.db.add_cfg_para_proc(self.proc_zsup_id,"data_dir",          self.data_dir_zsup)
        #    self.db.add_cfg_para_proc(self.proc_zsup_id,"data_file",         self.data_file_zsup)
        #    self.db.add_cfg_para_proc(self.proc_zsup_id,"file_max_duration", self.file_max_duration)
        #    self.db.add_cfg_para_proc(self.proc_zsup_id,"file_max_size",     self.file_max_size)
        #    self.db.add_cfg_para_proc(self.proc_zsup_id,"file_max_events",   self.file_max_events)
        #
        #self.db.add_cfg_para_proc(self.proc_zsup_id,"input_stream", self.input_stream_zsup)
        #
        #self.db.add_cfg_para_proc(self.proc_zsup_id,"zero_suppression",   repr(self.zero_suppression))
        #if (self.zero_suppression%100 == 1):
        #    self.db.add_cfg_para_proc(self.proc_zsup_id,"zs1_head",       repr(self.zs1_head))
        #    self.db.add_cfg_para_proc(self.proc_zsup_id,"zs1_tail",       repr(self.zs1_tail))
        #    self.db.add_cfg_para_proc(self.proc_zsup_id,"zs1_nsigma",     repr(self.zs1_nsigma))
        #    self.db.add_cfg_para_proc(self.proc_zsup_id,"zs1_nabovethr",  repr(self.zs1_nabovethr))
        #    self.db.add_cfg_para_proc(self.proc_zsup_id,"zs1_badrmsthr",  repr(self.zs1_badrmsthr))
        #elif (self.zero_suppression%100 == 2):
        #    self.db.add_cfg_para_proc(self.proc_zsup_id,"zs2_tail",       repr(self.zs2_tail))
        #    self.db.add_cfg_para_proc(self.proc_zsup_id,"zs2_minrms",     repr(self.zs2_minrms))
        #    for ch in range(32):
        #        if (self.zs2_minrms_ch[ch] != self.zs2_minrms):
        #            self.db.add_cfg_para_proc(self.proc_zsup_id,"zs2_minrms_ch","%d %d"%(ch,self.zs2_minrms_ch[ch]))
        #
        #self.db.add_cfg_para_proc(self.proc_zsup_id,"debug_scale",        repr(self.debug_scale))

        return "ok"

    #def get_link_id(self):
    #
    #    # Convert PadmeDAQ link description to link id from DB
    #    if (self.node_id == -1 or self.conet2_link == -1 or self.conet2_slot == -1): return -1
    #    return self.db.get_link_id(self.node_id,self.conet2_link/8,self.conet2_link%8,self.conet2_slot)

    def start_daq(self):

        command = "%s -c %s"%(self.executable,self.config_file_daq)

        # If DAQ process runs on a remote node then start it using passwordless ssh connection
        if self.node_id != 0:
            command = "ssh -i %s %s '( %s )'"%(self.ssh_id_file,self.node_ip,command)

        print "- Start DAQ process for board %d"%self.board_id
        print command
        print "  Log written to %s"%self.log_file_daq

        # Open log file
        self.log_handle_daq = open(self.log_file_daq,"w")

        # Start DAQ process
        try:
            self.process_daq = subprocess.Popen(shlex.split(command),stdout=self.log_handle_daq,stderr=subprocess.STDOUT,bufsize=1)
        except OSError as e:
            print "ADCBoard::start_daq - ERROR: Execution failed: %s",e
            return 0                

        # Tag start of process in DB
        if self.run_number:
            self.db.set_process_time_create(self.proc_daq_id,self.db.now_str())

        # Return process id
        return self.process_daq.pid

    def stop_daq(self):

        # Wait up to 5 seconds for DAQ to stop of its own (on quit file or on time elapsed)
        for i in range(10):
            if self.process_daq.poll() != None:
                # Process exited: clean up defunct process and close log file
                self.process_daq.wait()
                self.log_handle_daq.close()
                if self.run_number:
                    self.db.set_process_time_end(self.proc_daq_id,self.db.now_str())
                return True
            time.sleep(0.5)

        # Process did not stop: try sending and interrupt
        print "ADCBoard::stop_daq- WARNING: DAQ process did not stop on its own. Sending interrupt"
        if self.node_id == 0:
            # If process is on local host, just send a kill signal
            command = "kill %d"%self.process_daq.pid
        else:
            # If it is on a remote host, use ssh to send kill command.
            # PID on remote host is recovered from the lock file
            command = "ssh -i %s %s '( kill `cat %s` )'"%(self.ssh_id_file,self.node_ip,self.lock_file_daq)
        print command
        os.system(command)

        # Wait up to 5 seconds for DAQ to stop on interrupt
        for i in range(10):
            if self.process_daq.poll() != None:
                # Process exited: clean up defunct process and close log file
                self.process_daq.wait()
                self.log_handle_daq.close()
                if self.run_number:
                    self.db.set_process_time_end(self.proc_daq_id,self.db.now_str())
                return True
            time.sleep(0.5)

        # Process did not stop smoothly: terminate it
        print "ADCBoard::stop_daq - WARNING: DAQ process did not stop on interrupt. Terminating it"
        self.process_daq.terminate()
        time.sleep(1)
        if self.process_daq.poll() != None:
            self.process_daq.wait()
            self.log_handle_daq.close()
        if self.run_number: self.db.set_process_time_end(self.proc_daq_id,self.db.now_str())
        return False

    def start_zsup(self):

        command = "%s -c %s"%(self.executable,self.config_file_zsup)

        # If ZSUP process runs on a remote node then start it using passwordless ssh connection
        if self.node_id != 0:
            command = "ssh -i %s %s '( %s )'"%(self.ssh_id_file,self.node_ip,command)

        print "- Start ZSUP process for board %d"%self.board_id
        print command
        print "  Log written to %s"%self.log_file_zsup

        # Open log file
        self.log_handle_zsup = open(self.log_file_zsup,"w")

        # Start ZSUP process
        try:
            self.process_zsup = subprocess.Popen(shlex.split(command),stdout=self.log_handle_zsup,stderr=subprocess.STDOUT,bufsize=1)
        except OSError as e:
            print "ADCBoard::start_zsup - ERROR: Execution failed: %s",e
            return 0                

        # Tag start of process in DB
        if self.run_number:
            self.db.set_process_time_create(self.proc_zsup_id,self.db.now_str())

        # Return process id
        return self.process_zsup.pid

    def stop_zsup(self):

        # Tag stop process in DB

        # Wait up to 5 seconds for ZSUP to stop
        for i in range(10):
            if self.process_zsup.poll() != None:
                # Process exited: clean up defunct process and close log file
                self.process_zsup.wait()
                self.log_handle_zsup.close()
                if self.run_number:
                    self.db.set_process_time_end(self.proc_zsup_id,self.db.now_str())
                return True
            time.sleep(0.5)

        # Process did not stop smoothly: terminate it
        print "ADCBoard::stop_zsup - WARNING: ZSUP process did not stop on interrupt. Terminating it"
        self.process_zsup.terminate()
        time.sleep(1)
        if self.process_zsup.poll() != None:
            self.process_zsup.wait()
            self.log_handle_zsup.close()
        if self.run_number:
            self.db.set_process_time_end(self.proc_zsup_id,self.db.now_str())
        return False

    def parse_log_daq(self):

        # Look for DBINFO lines in log file and send them to DBINFO line processor
        if os.path.exists(self.log_file_daq) and os.path.isfile(self.log_file_daq):
            with open(self.log_file_daq,"r") as log:
                for line in log:
                    if self.re_dbinfo_line.match(line):
                        self.db.manage_dbinfo_entry(self.proc_daq_id,line)
        else:
            print "ADCBoard::parse_log_daq - WARNING: DAQ log file %s not found"%self.log_file_daq

    def parse_log_zsup(self):

        # Look for DBINFO lines in log file and send them to DBINFO line processor
        if os.path.exists(self.log_file_zsup) and os.path.isfile(self.log_file_zsup):
            with open(self.log_file_zsup,"r") as log:
                for line in log:
                    if self.re_dbinfo_line.match(line):
                        self.db.manage_dbinfo_entry(self.proc_zsup_id,line)
        else:
            print "ADCBoard::parse_log_zsup - WARNING: ZSUP log file %s not found"%self.log_file_zsup
