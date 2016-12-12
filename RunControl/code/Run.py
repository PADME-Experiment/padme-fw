import os
import re

from ADCBoard import ADCBoard
from PadmeDB import PadmeDB

class Run:

    def __init__(self):

        self.setup = "test"

        self.run_number = 0
        self.run_type = "TEST"
        self.run_user = "PADME crew"
        self.run_comment = "Generic run"
        self.run_end_comment = "Generic end of run"

        self.set_default_config()

    def change_run(self,run):

        self.run_number = run

        self.config_dir = "cfg/run_"+str(run)
        self.config_file = "run_"+str(run)+".cfg"
        self.config_file_head = "run_"+str(run)

        self.log_dir = "log/run_"+str(run)
        self.log_file_head = "run_"+str(run)

        for adcboard in self.adcboard_list:
            adcboard.run_number  = self.run_number
            s_bid = "b%02d"%adcboard.board_id
            adcboard.config_file = self.config_dir+"/"+self.config_file_head+"_"+s_bid+".cfg"
            adcboard.log_file    = self.log_dir+"/"+self.log_file_head+"_"+s_bid+".log"

    def set_default_config(self):

        self.adcboard_list = []
        self.boardid_list = []

        self.config_dir = "cfg/dummy_run"
        self.config_file = "dummy_run.cfg"
        self.config_file_head = "dummy_run"

        self.log_dir = "log/dummy_run"
        self.log_file_head = "dummy_run"

        self.start_file = "run/start"
        self.quit_file = "run/quit"
        self.initok_file_head = "run/initok"
        self.initfail_file_head = "run/initfail"
        self.lock_file_head = "run/lock"
        #self.db_file = "db/PadmeDAQ.db"

        self.data_dir = "data"
        self.data_file_head = "daq"

        self.total_daq_time = 0

    def read_setup(self):

        # Define regular expressions used in file parsing
        re_empty = re.compile("^\s*$")
        re_comment = re.compile("^\s*#")
        re_param = re.compile("^\s*(\w+)\s+(.+?)\s*$")
        re_boardid = re.compile("\d+")

        # Read default run configuration from file
        setup_file = "setup/%s/run.cfg"%self.setup
        if (not os.path.isfile(setup_file)):
            print "Run - ERROR: setup file %s not found"%setup_file
            return "error"
        f = open(setup_file)
        for l in f:
            if (re_empty.search(l) or re_comment.search(l)): continue
            m = re_param.search(l)
            if (m):
                (p_name,p_value) = m.group(1,2)
                if (p_name == "boardid_list"):
                    # Get sorted list of board ids while removing duplicates
                    self.s_boards = sorted(list(set(re_boardid.findall(p_value))))
                    # Convert to int as findall returns strings
                    self.boardid_list = []
                    for s_bid in self.s_boards: self.boardid_list.append(int(s_bid))
                else:
                    print "Run - WARNING: unknown parameter %s found while reading setup file %s"%(p_name,setup_file)
            else:
                print "Run - WARNING: unknown line format found while reading setup file %s"%(p_name,setup_file)
                print l
        f.close()
        return "ok"

    def format_config(self):

        cfgstring = ""

        cfgstring += "setup\t\t"+self.setup+"\n"

        s_board_list = ""
        for bid in self.boardid_list: s_board_list += " %d"%bid
        cfgstring += "board_list\t\t"+s_board_list+"\n"

        cfgstring += "config_dir\t\t"+self.config_dir+"\n"
        cfgstring += "config_file\t\t"+self.config_file+"\n"
        cfgstring += "config_file_head\t"+self.config_file_head+"\n"

        cfgstring += "log_dir\t\t\t"+self.log_dir+"\n"
        cfgstring += "log_file_head\t\t"+self.log_file_head+"\n"

        cfgstring += "run_number\t\t"+str(self.run_number)+"\n"
        cfgstring += "run_type\t\t"+self.run_type+"\n"
        cfgstring += "run_comment\t\t"+self.run_comment+"\n"

        cfgstring += "start_file\t\t"+self.start_file+"\n"
        cfgstring += "quit_file\t\t"+self.quit_file+"\n"
        cfgstring += "initok_file_head\t\t"+self.initok_file_head+"\n"
        cfgstring += "initfail_file_head\t\t"+self.initfail_file_head+"\n"
        cfgstring += "lock_file_head\t\t"+self.lock_file_head+"\n"
        #cfgstring += "db_file\t\t\t"+self.db_file+"\n"

        cfgstring += "data_dir\t\t"+self.data_dir+"\n"
        cfgstring += "data_file_head\t\t"+self.data_file_head+"\n"

        cfgstring += "total_daq_time\t\t"+str(self.total_daq_time)+"\n"

        return cfgstring

    def create_log_dir(self):

        if not os.path.exists(self.log_dir):
            os.mkdir(self.log_dir)

    def write_config(self):

        if not os.path.exists(self.config_dir):
            os.mkdir(self.config_dir)

        f = open(self.config_dir+"/"+self.config_file,"w")

        f.write(self.format_config())

        f.close()

    def print_config(self):

        print self.format_config()

    def create_run(self):

        if (self.run_number):

            # Connect to DB
            db = PadmeDB()

            db.create_run(self.run_number,self.run_type,self.run_user,self.run_comment)

            # Write run configuration parameters to DB
            db.add_cfg_para(self.run_number,"setup",              self.setup)
            s_board_list = ""
            for s_bid in self.s_boards: s_board_list += " "+s_bid
            db.add_cfg_para(self.run_number,"board_list",         s_board_list)
            db.add_cfg_para(self.run_number,"config_dir",         self.config_dir)
            db.add_cfg_para(self.run_number,"config_file",        self.config_file)
            db.add_cfg_para(self.run_number,"config_file_head",   self.config_file_head)
            db.add_cfg_para(self.run_number,"log_dir",            self.log_dir)
            db.add_cfg_para(self.run_number,"log_file_head",      self.log_file_head)
            db.add_cfg_para(self.run_number,"start_file",         self.start_file)
            db.add_cfg_para(self.run_number,"quit_file",          self.quit_file)
            db.add_cfg_para(self.run_number,"initok_file_head",   self.initok_file_head)
            db.add_cfg_para(self.run_number,"initfail_file_head", self.initfail_file_head)
            db.add_cfg_para(self.run_number,"lock_file_head",     self.lock_file_head)
            #db.add_cfg_para(self.run_number,"db_file",            self.db_file)
            db.add_cfg_para(self.run_number,"data_dir",           self.data_dir)
            db.add_cfg_para(self.run_number,"data_file_head",     self.data_file_head)
            db.add_cfg_para(self.run_number,"total_daq_time",     self.total_daq_time)

            del db

    def change_setup(self,setup):

        # Reset run configuration to its default values
        self.set_default_config()

        # Read new setup
        self.setup = setup
        if (self.read_setup() == "error"): return "error"

        # Create new set of ADC boards
        for b_id in self.boardid_list:
            print "Run - Configuring ADC board %d"%b_id
            adcboard = ADCBoard(b_id)
            self.configure_adcboard(adcboard)
            self.adcboard_list.append(adcboard)

        return setup

    def configure_adcboard(self,adcboard):

        # Reset ADC board to default configuration
        adcboard.set_default_config()

        # Configure board for current run
        s_bid = "b%02d"%adcboard.board_id
        adcboard.run_number     = self.run_number
        adcboard.config_file    = self.config_dir+"/"+self.config_file_head+"_"+s_bid+".cfg"
        adcboard.log_file       = self.log_dir+"/"+self.log_file_head+"_"+s_bid+".log"
        adcboard.start_file     = self.start_file
        adcboard.quit_file      = self.quit_file
        adcboard.initok_file    = self.initok_file_head+"_"+s_bid
        adcboard.initfail_file  = self.initfail_file_head+"_"+s_bid
        adcboard.lock_file      = self.lock_file_head+"_"+s_bid
        #adcboard.db_file        = self.db_file
        adcboard.data_file      = self.data_dir+"/"+self.data_file_head+"_"+s_bid
        adcboard.total_daq_time = self.total_daq_time

        # Read board setup
        adcboard.read_setup(self.setup)
