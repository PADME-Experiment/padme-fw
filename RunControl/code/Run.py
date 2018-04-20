import os
import re
import time

from Merger   import Merger
from ADCBoard import ADCBoard
from PadmeDB  import PadmeDB

class Run:

    def __init__(self):

        self.db = PadmeDB()

        self.setup = "test"

        self.run_number = 0
        self.run_type = "TEST"
        self.run_user = "PADME crew"
        self.run_comment_start = "Generic run"
        self.run_comment_end = "Generic end of run"

        self.rawdata_dir = "rawdata"

        self.set_default_config()

        self.merger = Merger()
        #self.merger_node_list = ["l1padme1","l1padme2"]
        self.merger_node_list = ["localhost"]
        self.merger_node = ""

    def change_run(self,run):

        self.run_number = run

        if (run == 0):
            self.run_name = "run_0_"+time.strftime("%Y%m%d_%H%M%S",time.gmtime())
        else:
            self.run_name = "run_"+str(run)

        self.run_dir = "runs/"+self.run_name

        #self.config_dir = "cfg/run_"+str(run)
        self.config_dir = self.run_dir+"/cfg"
        self.config_file = "run_"+str(run)+".cfg"
        self.config_file_head = "run_"+str(run)

        #self.log_dir = "runs/run_"+str(run)+"/log"
        self.log_dir = self.run_dir+"/log"
        self.log_file_head = "run_"+str(run)

        self.stream_dir = self.run_dir+"/stream"
        self.stream_head = "run_"+str(run)

        self.merger_node = self.next_merger_node()

        self.merger.config_file = self.config_dir+"/"+self.config_file_head+"_merger.cfg"
        self.merger.log_file = self.log_dir+"/"+self.log_file_head+"_merger.log"
        self.merger.run_number = self.run_number
        self.merger.node_id = self.db.get_node_id(self.merger_node)
        self.merger.input_list = self.config_dir+"/"+self.config_file_head+"_merger.list"
        self.merger.output_dir = self.rawdata_dir+"/"+self.run_name
        self.merger.output_file = self.run_name
        

        for adcboard in self.adcboard_list:
            adcboard.run_number  = self.run_number
            s_bid = "b%02d"%adcboard.board_id
            adcboard.config_file_daq = self.config_dir+"/"+self.config_file_head+"_"+s_bid+"_daq.cfg"
            adcboard.config_file_zsup = self.config_dir+"/"+self.config_file_head+"_"+s_bid+"_zsup.cfg"
            adcboard.log_file_daq = self.log_dir+"/"+self.log_file_head+"_"+s_bid+"_daq.log"
            adcboard.log_file_zsup = self.log_dir+"/"+self.log_file_head+"_"+s_bid+"_zsup.log"
            adcboard.output_stream_daq = self.stream_dir+"/"+self.stream_head+"_"+s_bid+"_daq"
            adcboard.output_stream_zsup = self.stream_dir+"/"+self.stream_head+"_"+s_bid+"_zsup"
            adcboard.input_stream_zsup = adcboard.output_stream_daq

    def next_merger_node(self):

        # If merger node is currently not defined, set it to first in list or localhost
        if not self.merger_node:
            if self.merger_node_list:
                return self.merger_node_list[0]
            else:
                return "localhost"

        # If the list of nodes was not defined, keep using always the same node
        if not self.merger_node_list: return self.merger_node

        # Find current node in node list and use next one
        use_next = False
        for node in self.merger_node_list:
            if use_next: return node
            if node == self.merger_node:
                use_next = True

        # Current node was the last in the list, so we restart with the first node in the list
        if use_next: return self.merger_node_list[0]

        # Handle misconfigurations (should never happen)
        print "Run::next_merger_node - WARNING: current merger host %s is not in the merger node list %s"%(self.merger_node,self.merger_node_list)
        print "                        Will use first node in list: %s"%self.merger_node_list[0]
        return self.merger_node_list[0]

    def set_default_config(self):

        self.adcboard_list = []
        self.boardid_list = []

        self.run_dir = "runs/dummy_run"

        self.config_dir = self.run_dir+"/cfg"
        self.config_file = "dummy_run.cfg"
        self.config_file_head = "dummy_run"

        self.log_dir = self.run_dir+"/log"
        self.log_file_head = "dummy_run"

        self.stream_dir = self.run_dir+"/stream"
        self.stream_head = "dummy_run"

        self.start_file = "run/start"
        self.quit_file = "run/quit"
        self.initok_file_head = "run/initok"
        self.initfail_file_head = "run/initfail"
        self.lock_file_head = "run/lock"

        #self.data_dir = "data"
        #self.data_file_head = "daq"

        self.total_daq_time = 0

    def read_setup(self):

        # Define regular expressions used in file parsing
        re_empty = re.compile("^\s*$")
        re_comment = re.compile("^\s*#")
        re_param = re.compile("^\s*(\w+)\s+(.+?)\s*$")
        re_boardid = re.compile("\d+")
        #re_board_link = re.compile("^\s*(\d+)\s+(\w+)\s+(\d+)\s+(\d+)\s*$")
        re_board_link = re.compile("^\s*(\d+)\s+(\S+)\s+(\d+)\s+(\d+)\s*$")

        # Read default run configuration from file
        setup_file = "setup/%s/run.cfg"%self.setup
        if (not os.path.isfile(setup_file)):
            print "Run - ERROR: setup file %s not found"%setup_file
            return "error"
        f = open(setup_file)
        self.boardid_list = []
        self.board_link_list = []
        for l in f:
            if (re_empty.search(l) or re_comment.search(l)): continue
            m = re_param.search(l)
            if (m):
                (p_name,p_value) = m.group(1,2)
                if (p_name == "boardid_list"):
                    # Get sorted list of board ids while removing duplicates
                    s_boards = sorted(list(set(re_boardid.findall(p_value))))
                    for s_bid in s_boards:
                        # Convert to int as findall returns strings
                        b = int(s_bid)
                        self.boardid_list.append(b)
                elif (p_name == "board_link"):
                    mm = re_board_link.search(p_value)
                    if (mm):
                        self.board_link_list.append(mm.group(1,2,3,4))
                    else:
                        print "Run - WARNING: unable to decode board_link parameter while reading setup file %s"%(setup_file,)
                        print l
                elif (p_name == "merger_node"):
                    self.merger_node = p_value
                else:
                    print "Run - WARNING: unknown parameter %s found while reading setup file %s"%(p_name,setup_file)
            else:
                print "Run - WARNING: unknown line format found while reading setup file %s"%(p_name,setup_file)
                print l
        f.close()

        return "ok"

    def format_config(self):

        cfgstring = ""

        cfgstring += "setup\t\t\t%s\n"%self.setup

        s_board_list = ""
        for b in self.boardid_list:
            if (s_board_list):
                s_board_list += " %d"%b
            else:
                s_board_list = "%d"%b
        cfgstring += "board_list\t\t%s\n"%s_board_list

        for b in self.boardid_list:
            for link in self.board_link_list:
                (board,host,port,node) = link
                if b == int(board):
                    board_link = "%s %s %s %s"%(board,host,port,node)
                    cfgstring += "board_link\t\t"+board_link+"\n"

        cfgstring += "merger_node\t\t%s\n"%self.merger_node

        cfgstring += "config_dir\t\t"+self.config_dir+"\n"
        cfgstring += "config_file\t\t"+self.config_file+"\n"
        cfgstring += "config_file_head\t"+self.config_file_head+"\n"

        cfgstring += "log_dir\t\t\t"+self.log_dir+"\n"
        cfgstring += "log_file_head\t\t"+self.log_file_head+"\n"

        cfgstring += "stream_dir\t\t"+self.stream_dir+"\n"
        cfgstring += "stream_head\t\t"+self.stream_head+"\n"

        cfgstring += "run_number\t\t"+str(self.run_number)+"\n"
        cfgstring += "run_type\t\t"+self.run_type+"\n"
        cfgstring += "run_user\t\t"+self.run_user+"\n"
        cfgstring += "run_comment_start\t"+self.run_comment_start+"\n"

        cfgstring += "start_file\t\t"+self.start_file+"\n"
        cfgstring += "quit_file\t\t"+self.quit_file+"\n"
        cfgstring += "initok_file_head\t"+self.initok_file_head+"\n"
        cfgstring += "initfail_file_head\t"+self.initfail_file_head+"\n"
        cfgstring += "lock_file_head\t\t"+self.lock_file_head+"\n"

        #cfgstring += "data_dir\t\t"+self.data_dir+"\n"
        #cfgstring += "data_file_head\t\t"+self.data_file_head+"\n"

        cfgstring += "total_daq_time\t\t"+str(self.total_daq_time)+"\n"

        return cfgstring

    def create_run(self):

        # Create run in DB and save its configuration parameters

        self.db.create_run(self.run_number,self.run_type,self.run_user,self.run_comment_start)

        self.db.add_cfg_para_run(self.run_number,"setup",self.setup)

        s_board_list = ""
        for b in self.boardid_list:
            if (s_board_list):
                s_board_list += " %d"%b
            else:
                s_board_list = "%d"%b
        self.db.add_cfg_para_run(self.run_number,"board_list",s_board_list)

        for b in self.boardid_list:
            for link in self.board_link_list:
                (board,host,port,node) = link
                if b == int(board):
                    board_link = "%s %s %s %s"%(board,host,port,node)
                    self.db.add_cfg_para_run(self.run_number,"board_link",board_link)

        self.db.add_cfg_para_run(self.run_number,"merger_node",        self.merger_node)
        self.db.add_cfg_para_run(self.run_number,"config_dir",         self.config_dir)
        self.db.add_cfg_para_run(self.run_number,"config_file",        self.config_file)
        self.db.add_cfg_para_run(self.run_number,"config_file_head",   self.config_file_head)
        self.db.add_cfg_para_run(self.run_number,"log_dir",            self.log_dir)
        self.db.add_cfg_para_run(self.run_number,"log_file_head",      self.log_file_head)
        self.db.add_cfg_para_run(self.run_number,"start_file",         self.start_file)
        self.db.add_cfg_para_run(self.run_number,"quit_file",          self.quit_file)
        self.db.add_cfg_para_run(self.run_number,"initok_file_head",   self.initok_file_head)
        self.db.add_cfg_para_run(self.run_number,"initfail_file_head", self.initfail_file_head)
        self.db.add_cfg_para_run(self.run_number,"lock_file_head",     self.lock_file_head)
        #self.db.add_cfg_para_run(self.run_number,"data_dir",           self.data_dir)
        #self.db.add_cfg_para_run(self.run_number,"data_file_head",     self.data_file_head)
        self.db.add_cfg_para_run(self.run_number,"total_daq_time",     self.total_daq_time)

        # Create Merger structure in DB
        if self.merger.create_merger() == "error":
            print "ERROR - Cannot create MERGER process in the DB"
            return "error"

        # Create board structures in DB
        for adc in (self.adcboard_list):
            if adc.create_proc_daq() == "error":
                print "ERROR - Cannot create DAQ process in the DB"
                return "error"
            if adc.create_proc_zsup() == "error":
                print "ERROR - Cannot create ZSUP process in the DB"
                return "error"

        return "ok"

    def create_log_dir(self):

        # Create log directory for this run (make sure the full tree is there)
        if not os.path.exists(self.log_dir): os.makedirs(self.log_dir,0755)

    def write_config(self):

        # Create config directory for this run (make sure the full tree is there)
        if not os.path.exists(self.config_dir): os.makedirs(self.config_dir,0755)

        print "Writing configuration file %s"%self.config_file
        f = open(self.config_dir+"/"+self.config_file,"w")
        f.write(self.format_config())
        f.close()

        print "Writing configuration file %s for Merger"%self.merger.config_file
        self.merger.write_config()

        for adc in (self.adcboard_list):
            print "Writing configuration files %s and %s for ADC board %d"%(adc.config_file_daq,adc.config_file_zsup,adc.board_id)
            adc.write_config()

    def print_config(self):

        print self.format_config()

    def create_streams(self):

        # Create stream directory for this run (make sure the full tree is there)
        if not os.path.exists(self.stream_dir): os.makedirs(self.stream_dir,0755)

        # Create named pipes to use for DAQ and ZSUP streams
        for adc in (self.adcboard_list):
            print "Creating named pipes %s and %s for ADC board %d"%(adc.output_stream_daq,adc.output_stream_zsup,adc.board_id)
            os.mkfifo(adc.output_stream_daq)
            os.mkfifo(adc.output_stream_zsup)

    def create_merger_input_list(self):

        print "Creating merger input list file %s"%self.merger.input_list
        f = open(self.merger.input_list,"w")
        for adcboard in self.adcboard_list:
            f.write("%d %s\n"%(adcboard.board_id,adcboard.output_stream_zsup))
        f.close()

    def create_merger_output_dir(self):

        # Create directory where Merger will write its output files (make sure the full tree is there)
        print "Creating merger output files directory %s"%self.merger.output_dir
        if not os.path.exists(self.merger.output_dir): os.makedirs(self.merger.output_dir,0755)

    def change_setup(self,setup):

        # Reset run configuration to its default values
        self.set_default_config()

        # Read new setup
        self.setup = setup
        if (self.read_setup() == "error"): return "error"

        # Create new set of ADC boards
        for b in self.boardid_list:
            print "Run - Configuring ADC board %d"%b
            adcboard = ADCBoard(b)
            self.configure_adcboard(adcboard)
            self.adcboard_list.append(adcboard)

        return setup

    def configure_adcboard(self,adcboard):

        # Reset ADC board to default configuration
        adcboard.set_default_config()

        # Configure board for current run
        adcboard.run_number = self.run_number

        # Get board connection information
        adcboard.node_id     = -1
        adcboard.conet2_link = -1
        adcboard.conet2_slot = -1
        for link in self.board_link_list:
            (board,host,port,node) = link
            if (int(board) == adcboard.board_id):
                adcboard.node_id     = self.db.get_node_id(host)
                adcboard.conet2_link = int(port)
                adcboard.conet2_slot = int(node)

        s_bid = "b%02d"%adcboard.board_id
        adcboard.config_file_daq    = self.config_dir+"/"+self.config_file_head+"_"+s_bid+"_daq.cfg"
        adcboard.config_file_zsup   = self.config_dir+"/"+self.config_file_head+"_"+s_bid+"_zsup.cfg"
        adcboard.log_file_daq       = self.log_dir+"/"+self.log_file_head+"_"+s_bid+"_daq.log"
        adcboard.log_file_zsup      = self.log_dir+"/"+self.log_file_head+"_"+s_bid+"_zsup.log"
        adcboard.lock_file_daq      = self.lock_file_head+"_"+s_bid+"_daq"
        adcboard.lock_file_zsup     = self.lock_file_head+"_"+s_bid+"_zsup"
        adcboard.start_file         = self.start_file
        adcboard.quit_file          = self.quit_file
        adcboard.initok_file_daq    = self.initok_file_head+"_"+s_bid+"_daq"
        adcboard.initfail_file_daq  = self.initfail_file_head+"_"+s_bid+"_daq"
        adcboard.initok_file_zsup   = self.initok_file_head+"_"+s_bid+"_zsup"
        adcboard.initfail_file_zsup = self.initfail_file_head+"_"+s_bid+"_zsup"
        #adcboard.data_file          = self.data_dir+"/"+self.data_file_head+"_"+s_bid
        adcboard.total_daq_time     = self.total_daq_time

        # Read board setup
        adcboard.read_setup(self.setup)
