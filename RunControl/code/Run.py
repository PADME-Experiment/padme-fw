import os
import re
import time

from Merger   import Merger
from ADCBoard import ADCBoard
from PadmeDB  import PadmeDB

class Run:

    def __init__(self):

        # Get position of DAQ main directory from PADME_DAQ_DIR environment variable
        # Default to current dir if not set
        self.daq_dir = os.getenv('PADME_DAQ_DIR',".")

        # Define name and position of control files
        self.control_dir = self.daq_dir+"/run"
        self.start_file = self.control_dir+"/start"
        self.quit_file = self.control_dir+"/quit"
        self.initok_file_head = self.control_dir+"/initok"
        self.initfail_file_head = self.control_dir+"/initfail"
        self.lock_file_head = self.control_dir+"/lock"

        self.db = PadmeDB()

        self.setup = "test"

        self.run_number = 0
        self.run_type = "TEST"
        self.run_user = "PADME crew"
        self.run_comment_start = "Generic run"
        self.run_comment_end = "Generic end of run"

        self.rawdata_dir = self.daq_dir+"/local/rawdata"

        self.set_default_config()

        self.merger = Merger()
        #self.merger_node_list = ["l1padme1","l1padme2"]
        #self.merger_node_list = ["l1padme1"]
        #self.merger_node_list = ["localhost"]
        self.merger_node_list = []
        self.merger_node = ""

    def change_run(self):

        if (self.run_number == 0):
            self.run_name = "run_0_"+time.strftime("%Y%m%d_%H%M%S",time.gmtime())
        else:
            self.run_name = "run_"+str(self.run_number)

        self.run_dir = self.daq_dir+"/runs/"+self.run_name

        #self.config_dir = "cfg/run_"+str(run)
        self.config_dir = self.run_dir+"/cfg"
        self.config_file = "run_"+str(self.run_number)+".cfg"
        self.config_file_head = "run_"+str(self.run_number)

        #self.log_dir = "runs/run_"+str(run)+"/log"
        self.log_dir = self.run_dir+"/log"
        self.log_file_head = "run_"+str(self.run_number)

        #self.stream_dir = self.run_dir+"/stream"
        self.stream_dir = self.daq_dir+"/local/streams/"+self.run_name
        self.stream_head = "run_"+str(self.run_number)

        # Make sure Merger runs on a different node after each run
        self.merger_node = self.next_merger_node()
        self.merger.node_id = self.db.get_node_id(self.merger_node)
        self.merger.node_ip = self.db.get_node_daq_ip(self.merger.node_id)

        self.merger.config_file = self.config_dir+"/"+self.config_file_head+"_merger.cfg"
        self.merger.log_file = self.log_dir+"/"+self.log_file_head+"_merger.log"
        self.merger.run_number = self.run_number
        self.merger.input_list = self.config_dir+"/"+self.config_file_head+"_merger.list"
        self.merger.output_dir = self.rawdata_dir+"/"+self.run_name
        self.merger.output_file = self.run_name

        for adcboard in self.adcboard_list:
            adcboard.run_number  = self.run_number
            if (self.run_type == "FAKE"):
                adcboard.process_mode = "FAKE"
            else:
                adcboard.process_mode = "DAQ"
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

        self.run_name = "dummy_run"

        self.run_dir = self.daq_dir+"/runs/"+self.run_name

        self.config_dir = self.run_dir+"/cfg"
        self.config_file = self.run_name+".cfg"
        self.config_file_head = self.run_name

        self.log_dir = self.run_dir+"/log"
        self.log_file_head = self.run_name

        #self.stream_dir = self.run_dir+"/stream"
        self.stream_dir = self.daq_dir+"/local/streams/"+self.run_name
        self.stream_head = self.run_name

        #self.start_file = self.daq_dir+"/run/start"
        #self.quit_file = self.daq_dir+"/run/quit"
        #self.initok_file_head = self.daq_dir+"/run/initok"
        #self.initfail_file_head = self.daq_dir+"/run/initfail"
        #self.lock_file_head = self.daq_dir+"/run/lock"

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
        setup_file = self.daq_dir+"/setup/%s/run.cfg"%self.setup
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
                elif (p_name == "merger_node_list"):
                    self.merger_node_list = p_value.split()
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

    def create_fifos(self):

        # Create stream directories and fifo files for ADCBoards
        for adc in (self.adcboard_list):
            if adc.node_id == 0:
                if not os.path.exists(self.stream_dir): os.makedirs(self.stream_dir,0755)
                os.mkfifo(adc.output_stream_daq)
                os.mkfifo(adc.output_stream_zsup)
            else:
                #node_ip = self.db.get_node_daq_ip(adc.node_id)
                #command = "ssh -i ~/.ssh/id_rsa_daq %s mkdir -p %s"%(adc.node_ip,self.stream_dir)
                #os.system(command)
                #command = "ssh -i ~/.ssh/id_rsa_daq %s mkfifo %s %s"%(adc.node_ip,adc.output_stream_daq,adc.output_stream_zsup)
                #os.system(command)
                command = "ssh -i ~/.ssh/id_rsa_daq %s '( mkdir -p %s ; mkfifo %s %s )'"%(adc.node_ip,self.stream_dir,adc.output_stream_daq,adc.output_stream_zsup)
                print command
                os.system(command)

        # Create stream directory on Merger
        if self.merger.node_id == 0:
            if not os.path.exists(self.stream_dir): os.makedirs(self.stream_dir,0755)
        else:
            #node_ip = self.db.get_node_daq_ip(self.merger.node_id)
            command = "ssh -i ~/.ssh/id_rsa_daq %s mkdir -p %s"%(self.merger.node_ip,self.stream_dir)
            os.system(command)

        # If an ADCBoard process runs on a node which is different from the Merger
        # we must replicate the ZSUP FIFO file on the Merger
        for adc in (self.adcboard_list):

            # Check if ADCBoard and Merger run on different nodes
            if adc.node_id != self.merger.node_id:

                # Duplicate ZSUP FIFO file on Merger node
                if self.merger.node_id == 0:
                    os.mkfifo(adc.output_stream_zsup)
                else:
                    command = "ssh -n -f -i /home/daq/.ssh/id_rsa_daq %s mkfifo %s"%(self.merger.node_ip,adc.output_stream_zsup)
                    os.system(command)
 
    def create_receivers(self):

        # If an ADCBoard process runs on a node which is different from the Merger
        # we create the receiving end of a network tunnel
        for adc in (self.adcboard_list):

            # Check if ADCBoard and Merger run on different nodes
            if adc.node_id != self.merger.node_id:

                # Define port for network tunnel
                port_number = 31400+adc.board_id

                # Open receiving end of tunnel on Merger node
                print "Creating receiving end of network tunnel for board %d on port %d"%(adc.board_id,port_number)
                command = "nc -l --recv-only %s %d > %s < /dev/zero &"%(self.merger.node_ip,port_number,adc.output_stream_zsup)
                if self.merger.node_id != 0:
                    command = "ssh -f -i /home/daq/.ssh/id_rsa_daq %s '( %s )'"%(self.merger.node_ip,command)
                print command
                os.system(command)
                time.sleep(1)
 
    def create_senders(self):

        # If an ADCBoard process runs on a node which is different from the Merger
        # we create the sending end of a network tunnel
        for adc in (self.adcboard_list):

            # Check if ADCBoard and Merger run on different nodes
            if adc.node_id != self.merger.node_id:

                # Define port for network tunnel
                port_number = 31400+adc.board_id

                # Open receiving end of tunnel on Merger node
                print "Creating sending end of network tunnel for board %d on port %d"%(adc.board_id,port_number)
                command = "nc --send-only %s %d < %s > /dev/null &"%(self.merger.node_ip,port_number,adc.output_stream_zsup)
                #command = "nc --send-only %s %d < %s &"%(self.merger.node_ip,port_number,adc.output_stream_zsup)
                if adc.node_id != 0:
                    command = "ssh -f -i /home/daq/.ssh/id_rsa_daq %s '( %s )'"%(adc.node_ip,command)
                print command
                os.system(command)
                time.sleep(1)

    #def create_streams(self):
    #
    #    # Create stream directory for Merger
    #    if self.merger.node_id == 0:
    #        if not os.path.exists(self.stream_dir): os.makedirs(self.stream_dir,0755)
    #    else:
    #        node_ip = self.db.get_node_daq_ip(self.merger.node_id)
    #        command = "ssh -i ~/.ssh/id_rsa_daq %s mkdir -p %s"%(node_ip,self.stream_dir)
    #        os.system(command)
    #
    #    # Create stream directories and fifo files for ADCBoards
    #    for adc in (self.adcboard_list):
    #        if adc.node_id == 0:
    #            if not os.path.exists(self.stream_dir): os.makedirs(self.stream_dir,0755)
    #            os.mkfifo(adc.output_stream_daq)
    #            os.mkfifo(adc.output_stream_zsup)
    #        else:
    #            node_ip = self.db.get_node_daq_ip(adc.node_id)
    #            command = "ssh -i ~/.ssh/id_rsa_daq %s mkdir -p %s"%(node_ip,self.stream_dir)
    #            os.system(command)
    #            command = "ssh -i ~/.ssh/id_rsa_daq %s mkfifo %s %s"%(node_ip,adc.output_stream_daq,adc.output_stream_zsup)
    #            os.system(command)
    #
    #    # If an ADCBoard process runs on a node which is different from the Merger
    #    # we must replicate the ZSUP FIFO file on the merger node
    #    # and create a network tunnel between the two FIFO files using the nc tool
    #    # Tunnel will use port number 31400+board_id
    #    for adc in (self.adcboard_list):
    #
    #        # Define host and port for network tunnel
    #        port_number = 31400+adc.board_id
    #        if self.merger.node_id == 0:
    #            host_ip = "192.168.0.2"
    #        else:
    #            host_ip = self.db.get_node_daq_ip(self.merger.node_id)
    #
    #        # Check if ADCBoard and Merger run on different nodes
    #        if adc.node_id != self.merger.node_id:
    #
    #            # Duplicate FIFO file and open receiving end of tunnel on Merger node
    #            if self.merger.node_id == 0:
    #                #os.mkfifo(adc.output_stream_zsup)
    #                ##command = "nc -l %s %d > %s &"%(host_ip,port_number,adc.output_stream_zsup)
    #                #command = "nc -l --recv-only %s %d > %s &"%(host_ip,port_number,adc.output_stream_zsup)
    #                #os.system(command)
    #                command = "%s -a %s -p %d -f %s"%(self.listener_script,host_ip,port_number,adc.output_stream_zsup)
    #                os.system(command)
    #            else:
    #                print "Creating receiving end of tunnel"
    #                #command = "ssh -n -f -i /home/daq/.ssh/id_rsa_daq %s mkfifo %s"%(host_ip,adc.output_stream_zsup)
    #                #print command
    #                #os.system(command)
    #                ##command = "ssh -i /home/daq/.ssh/id_rsa_daq %s '( nc -l %s %d > %s )' &"%(host_ip,host_ip,port_number,adc.output_stream_zsup)
    #                #command = "ssh -n -f -i /home/daq/.ssh/id_rsa_daq %s '( nc -l --recv-only -v -v %s %d > %s < /dev/zero )'"%(host_ip,host_ip,port_number,adc.output_stream_zsup)
    #                #print command
    #                #os.system(command)
    #                command = "ssh -n -i /home/daq/.ssh/id_rsa_daq %s '( %s -a %s -p %d -f %s )'"%(host_ip,self.listener_script,host_ip,port_number,adc.output_stream_zsup)
    #                os.system(command)
    #
    #            # Wait for listener to start
    #            #time.sleep(5)
    #
    #            # Open sending end of tunnel on node running DAQ/ZSUP for this ADCBoard
    #            if adc.node_id == 0:
    #                #command = "nc %s %d < %s &"
    #                command = "nc --send-only %s %d < %s &"
    #                os.system(command)
    #            else:
    #                node_ip = self.db.get_node_daq_ip(adc.node_id)
    #                print "Creating sending end of tunnel"
    #                #command = "ssh -n -i /home/daq/.ssh/id_rsa_daq %s '( nc %s %d < %s )' &"%(node_ip,host_ip,port_number,adc.output_stream_zsup)
    #                command = "ssh -n -f -i /home/daq/.ssh/id_rsa_daq %s '( nc --send-only -v -v %s %d < %s > /dev/null )'"%(node_ip,host_ip,port_number,adc.output_stream_zsup)
    #                print command
    #                os.system(command)
    #
    #    ## Create stream directory for this run (make sure the full tree is there)
    #    #if not os.path.exists(self.stream_dir): os.makedirs(self.stream_dir,0755)
    #    #
    #    ## Create named pipes to use for DAQ and ZSUP streams
    #    #for adc in (self.adcboard_list):
    #    #    print "Creating named pipes %s and %s for ADC board %d"%(adc.output_stream_daq,adc.output_stream_zsup,adc.board_id)
    #    #    os.mkfifo(adc.output_stream_daq)
    #    #    os.mkfifo(adc.output_stream_zsup)

    def create_merger_input_list(self):

        print "Creating merger input list file %s"%self.merger.input_list
        f = open(self.merger.input_list,"w")
        for adcboard in self.adcboard_list:
            f.write("%d %s\n"%(adcboard.board_id,adcboard.output_stream_zsup))
        f.close()

    #def create_merger_output_dir(self):
    #
    #    # Create directory where Merger will write its output files (make sure the full tree is there)
    #    print "Creating merger output files directory %s"%self.merger.output_dir
    #    if not os.path.exists(self.merger.output_dir): os.makedirs(self.merger.output_dir,0755)

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
        for link in self.board_link_list:
            (board,host,port,node) = link
            if (int(board) == adcboard.board_id):
                adcboard.node_id     = self.db.get_node_id(host)
                adcboard.node_ip     = self.db.get_node_daq_ip(adcboard.node_id)
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
