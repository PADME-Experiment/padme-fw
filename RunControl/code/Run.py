import os
import re
import time
import shlex
import subprocess

from Level1   import Level1
from Merger   import Merger
from ADCBoard import ADCBoard
from PadmeDB  import PadmeDB

class Run:

    def __init__(self):

        # Get location of DAQ main directory from PADME_DAQ_DIR
        # Default to current dir if not set
        self.daq_dir = os.getenv('PADME_DAQ_DIR',".")

        # Define id file for passwordless ssh command execution
        self.ssh_id_file = "%s/.ssh/id_rsa_daq"%os.getenv('HOME',"~")

        # Get location of padme-fw software from PADME
        # Default to PADME_DAQ_DIR/padme-fw if not set
        self.padme_fw = os.getenv('PADME',"%s/padme-fw"%self.daq_dir)

        # Define executables to use in production
        self.daq_executable = "%s/PadmeDAQ/PadmeDAQ.exe"%self.padme_fw
        self.merger_executable = "%s/Level1/PadmeMerger.exe"%self.padme_fw
        self.level1_executable = "%s/Level1/PadmeLevel1.exe"%self.padme_fw

        # Define directory containing setup subdirectories
        self.setup_root_dir = "%s/setup"%self.daq_dir

        # Define directory containing rawdata directories for each run
        self.rawdata_root_dir = self.daq_dir+"/local/rawdata"

        # Define name and position of control files
        #self.control_dir = self.daq_dir+"/run"
        self.control_dir = self.daq_dir+"/local/run"
        self.start_file = self.control_dir+"/start"
        self.quit_file = self.control_dir+"/quit"
        self.initok_file_head = self.control_dir+"/initok"
        self.initfail_file_head = self.control_dir+"/initfail"
        self.lock_file_head = self.control_dir+"/lock"

        # Connect to database services
        self.db = PadmeDB()

        # Define base port number for network tunnels
        self.base_port_number = 31400

        # Do not define a default setup
        self.setup = ""

        self.set_default_config()

    def change_run(self):

        #print "--- Changing run"

        if (self.run_number == 0):
            self.run_name = "run_0_"+time.strftime("%Y%m%d_%H%M%S",time.gmtime())
        else:
            self.run_name = "run_%d"%self.run_number

        self.run_dir = self.daq_dir+"/runs/"+self.run_name

        self.config_dir = self.run_dir+"/cfg"
        self.config_file = "run_%d.cfg"%self.run_number
        self.config_file_head = "run_%d"%self.run_number

        self.log_dir = self.run_dir+"/log"
        self.log_file_head = "run_%d"%self.run_number

        self.stream_dir = self.daq_dir+"/local/streams/"+self.run_name
        self.stream_head = "run_%d"%self.run_number

        self.rawdata_dir = "%s/%s"%(self.rawdata_root_dir,self.run_name)
        self.rawdata_head = "run_%d"%self.run_number

        # Make sure Merger runs on a different node after each run
        self.merger_node = self.next_merger_node()

        # Configure Merger for this run
        self.runconfig_merger(self.merger)

        # Configure ADC boards for this run
        for adcboard in self.adcboard_list:
            #print "--- Configuring for run board %d"%adcboard.board_id
            self.runconfig_adcboard(adcboard)

        # Configure Level1 processes for this run
        for level1 in self.level1_list:
            #print "--- Configuring for run level1 %d"%level1.level1_id
            self.runconfig_level1(level1)

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

        # Clean up Run configuration and set all run parameters to default

        self.adcboard_list = []

        self.merger = None

        self.level1_list = []

        self.run_number = 0
        self.run_name = "run_0_"+time.strftime("%Y%m%d_%H%M%S",time.gmtime())
        self.run_type = "TEST"
        self.run_user = "PADME crew"
        self.run_comment_start = "Generic run"
        self.run_comment_end = "Generic end of run"

        self.run_dir = self.daq_dir+"/runs/"+self.run_name

        self.config_dir = self.run_dir+"/cfg"
        self.config_file = self.run_name+".cfg"
        self.config_file_head = self.run_name

        self.log_dir = self.run_dir+"/log"
        self.log_file_head = self.run_name

        self.stream_dir = self.daq_dir+"/local/streams/"+self.run_name
        self.stream_head = self.run_name

        self.merger_node = "localhost"
        self.merger_node_list = []

        self.level1_nproc = 1
        self.level1_maxevt = 10000

        self.total_daq_time = 0

    def read_setup(self):

        # Define regular expressions used in file parsing
        re_empty = re.compile("^\s*$")
        re_comment = re.compile("^\s*#")
        re_param = re.compile("^\s*(\w+)\s+(.+?)\s*$")
        re_boardid = re.compile("\d+")
        re_board_link = re.compile("^\s*(\d+)\s+(\S+)\s+(\d+)\s+(\d+)\s*$")

        # Read general run configuration from file
        setup_file = "%s/%s/run.cfg"%(self.setup_root_dir,self.setup)
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
                elif (p_name == "total_daq_time"):
                    self.total_daq_time = int(p_value)
                elif (p_name == "merger_node"):
                    self.merger_node = p_value
                elif (p_name == "merger_node_list"):
                    self.merger_node_list = p_value.split()
                elif (p_name == "level1_nproc"):
                    self.level1_nproc = int(p_value)
                elif (p_name == "level1_maxevt"):
                    self.level1_maxevt = int(p_value)
                else:
                    print "Run - WARNING: unknown parameter %s found while reading setup file %s"%(p_name,setup_file)
            else:
                print "Run - WARNING: unknown line format found while reading setup file %s"%(p_name,setup_file)
                print l
        f.close()

        return "ok"

    def format_config(self):

        cfgstring = ""

        cfgstring += "start_file\t\t%s\n"%self.start_file
        cfgstring += "quit_file\t\t%s\n"%self.quit_file
        cfgstring += "initok_file_head\t%s\n"%self.initok_file_head
        cfgstring += "initfail_file_head\t%s\n"%self.initfail_file_head
        cfgstring += "lock_file_head\t\t%s\n"%self.lock_file_head
        cfgstring += "rawdata_dir\t\t%s\n"%self.rawdata_dir

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
                    cfgstring += "board_link\t\t%s\n"%board_link

        cfgstring += "run_number\t\t%d\n"%self.run_number
        cfgstring += "run_name\t\t%s\n"%self.run_name
        cfgstring += "run_type\t\t%s\n"%self.run_type
        cfgstring += "run_user\t\t%s\n"%self.run_user
        cfgstring += "run_comment_start\t%s\n"%self.run_comment_start

        cfgstring += "config_dir\t\t%s\n"%self.config_dir
        cfgstring += "config_file\t\t%s\n"%self.config_file
        cfgstring += "config_file_head\t%s\n"%self.config_file_head

        cfgstring += "log_dir\t\t\t%s\n"%self.log_dir
        cfgstring += "log_file_head\t\t%s\n"%self.log_file_head

        cfgstring += "stream_dir\t\t%s\n"%self.stream_dir
        cfgstring += "stream_head\t\t%s\n"%self.stream_head

        if self.merger_node:
            cfgstring += "merger_node\t\t%s\n"%self.merger_node

        if self.merger_node_list:
            cfgstring += "merger_node_list\t\t%s\n"%" ".join(self.merger_node_list)
            

        cfgstring += "level1_nproc\t\t%d\n"%self.level1_nproc
        cfgstring += "level1_maxevt\t\t%d\n"%self.level1_maxevt

        cfgstring += "total_daq_time\t\t"+str(self.total_daq_time)+"\n"

        return cfgstring

    def create_run(self):

        # Create run in DB and save its configuration parameters

        self.db.create_run(self.run_number,self.run_type,self.run_user,self.run_comment_start)

        self.db.add_cfg_para_run(self.run_number,"start_file",         self.start_file)
        self.db.add_cfg_para_run(self.run_number,"quit_file",          self.quit_file)
        self.db.add_cfg_para_run(self.run_number,"initok_file_head",   self.initok_file_head)
        self.db.add_cfg_para_run(self.run_number,"initfail_file_head", self.initfail_file_head)
        self.db.add_cfg_para_run(self.run_number,"lock_file_head",     self.lock_file_head)

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

        self.db.add_cfg_para_run(self.run_number,"config_dir",         self.config_dir)
        self.db.add_cfg_para_run(self.run_number,"config_file",        self.config_file)
        self.db.add_cfg_para_run(self.run_number,"config_file_head",   self.config_file_head)

        self.db.add_cfg_para_run(self.run_number,"log_dir",            self.log_dir)
        self.db.add_cfg_para_run(self.run_number,"log_file_head",      self.log_file_head)

        self.db.add_cfg_para_run(self.run_number,"stream_dir",         self.stream_dir)
        self.db.add_cfg_para_run(self.run_number,"stream_head",        self.stream_head)

        if self.merger_node:
            self.db.add_cfg_para_run(self.run_number,"merger_node",    self.merger_node)

        if self.merger_node_list:
            self.db.add_cfg_para_run(self.run_number,"merger_node_list",    " ".join(self.merger_node_list))

        self.db.add_cfg_para_run(self.run_number,"level1_nproc",       str(self.level1_nproc))
        self.db.add_cfg_para_run(self.run_number,"level1_maxevt",       str(self.level1_maxevt))

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

        for lvl1 in (self.level1_list):
            print "Writing configuration files %s for Level1 %d"%(lvl1.config_file,lvl1.level1_id)
            lvl1.write_config()

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
                command = "ssh -i %s %s '( mkdir -p %s ; mkfifo %s %s )'"%(self.ssh_id_file,adc.node_ip,self.stream_dir,adc.output_stream_daq,adc.output_stream_zsup)
                print command
                os.system(command)

        # Create stream directory on Merger
        if self.merger.node_id == 0:
            if not os.path.exists(self.stream_dir):
                os.makedirs(self.stream_dir,0755)
        else:
            command = "ssh -i %s %s '( mkdir -p %s )'"%(self.ssh_id_file,self.merger.node_ip,self.stream_dir)
            os.system(command)

        # Create fifo files to connect Merger to Level1 processes
        for lvl1 in (self.level1_list):
            if lvl1.node_id == 0:
                os.mkfifo(lvl1.input_stream)
            else:
                #command = "ssh -i ~/.ssh/id_rsa_daq %s '( mkdir -p %s ; mkfifo %s )'"%(lvl1.node_ip,self.stream_dir,lvl1.input_stream)
                command = "ssh -i %s %s '( mkfifo %s )'"%(self.ssh_id_file,lvl1.node_ip,lvl1.input_stream)
                print command
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
                    command = "ssh -n -f -i %s %s '( mkfifo %s )'"%(self.ssh_id_file,self.merger.node_ip,adc.output_stream_zsup)
                    print command
                    os.system(command)
 
    def create_receivers(self):

        # If an ADCBoard process runs on a node which is different from the Merger
        # we create the receiving end of a network tunnel
        self.proc_rcv = []
        for adc in (self.adcboard_list):

            # Check if ADCBoard and Merger run on different nodes
            if adc.node_id != self.merger.node_id:

                # Define port for network tunnel
                port_number = self.base_port_number+adc.board_id
                log_file = "%s/%s_nc_%d_recv.log"%(self.log_dir,self.log_file_head,port_number)
                log_handle = open(log_file,"w")

                # Open receiving end of tunnel on Merger node
                print "Creating receiving end of network tunnel for board %d on port %d"%(adc.board_id,port_number)
                #command = "nc -l --recv-only %s %d > %s < /dev/zero &"%(self.merger.node_ip,port_number,adc.output_stream_zsup)
                command = "nc -l -k -v --recv-only %s %d > %s < /dev/zero"%(self.merger.node_ip,port_number,adc.output_stream_zsup)
                if self.merger.node_id != 0:
                    command = "ssh -f -i %s %s '( %s )'"%(self.ssh_id_file,self.merger.node_ip,command)
                print command
                #os.system(command)
                #time.sleep(1)
                try:
                    proc = subprocess.Popen(shlex.split(command),stdout=log_handle,stderr=subprocess.STDOUT,bufsize=1)
                    self.proc_rcv.append(proc)
                except OSError as e:
                    print "Run::create_receivers - ERROR: Execution failed: %s",e
                time.sleep(0.5)

    def create_senders(self):

        # If an ADCBoard process runs on a node which is different from the Merger
        # we create the sending end of a network tunnel
        self.proc_snd = []
        for adc in (self.adcboard_list):

            # Check if ADCBoard and Merger run on different nodes
            if adc.node_id != self.merger.node_id:

                # Define port for network tunnel
                port_number = self.base_port_number+adc.board_id
                log_file = "%s/%s_nc_%d_send.log"%(self.log_dir,self.log_file_head,port_number)
                log_handle = open(log_file,"w")

                # Open receiving end of tunnel on Merger node. Add some code to wait for receiving end to appear before proceeding.
                print "Creating sending end of network tunnel for board %d on port %d"%(adc.board_id,port_number)
                #command = "nc --send-only %s %d < %s > /dev/null &"%(self.merger.node_ip,port_number,adc.output_stream_zsup)
                command = "while ! nc -z %s %d ; do sleep 1 ; done ; nc -v --send-only %s %d < %s > /dev/null"%(self.merger.node_ip,port_number,self.merger.node_ip,port_number,adc.output_stream_zsup)
                if adc.node_id != 0:
                    command = "ssh -f -i %s %s '( %s )'"%(self.ssh_id_file,adc.node_ip,command)
                print command
                #os.system(command)
                #time.sleep(1)
                try:
                    proc = subprocess.Popen(shlex.split(command),stdout=log_handle,stderr=subprocess.STDOUT,bufsize=1)
                    self.proc_snd.append(proc)
                except OSError as e:
                    print "Run::create_senders - ERROR: Execution failed: %s",e
                time.sleep(0.5)

    def create_merger_input_list(self):

        print "Creating merger input list file %s"%self.merger.input_list
        f = open(self.merger.input_list,"w")
        for adcboard in self.adcboard_list:
            f.write("%d %s\n"%(adcboard.board_id,adcboard.output_stream_zsup))
        f.close()

    def create_merger_output_list(self):

        print "Creating merger output list file %s"%self.merger.output_list
        f = open(self.merger.output_list,"w")
        for lvl1 in self.level1_list:
            f.write("%s\n"%(lvl1.input_stream))
        f.close()

    def create_level1_output_dirs(self):

        for level1 in self.level1_list:
            print "Creating output dir %s for level1 %d"%(level1.output_dir,level1.level1_id)
            if level1.node_id == 0:
                if not os.path.exists(level1.output_dir):
                    os.makedirs(level1.output_dir,0755)
            else:
                command = "ssh -i %s %s '( mkdir -p %s )'"%(self.ssh_id_file,level1.node_ip,level1.output_dir)
                os.system(command)

    def change_setup(self,setup):

        # Reset run configuration to its default values
        self.set_default_config()

        # Read new setup
        self.setup = setup
        if (self.read_setup() == "error"): return "error"

        # Create new set of ADC board processes (DAQ and ZSUP) handlers
        self.daq_nodes_id_list = []
        for b in self.boardid_list:
            print "Run - Configuring ADC board %d"%b
            adcboard = ADCBoard(b)
            self.configure_adcboard(adcboard)
            self.adcboard_list.append(adcboard)
            self.daq_nodes_id_list.append(adcboard.node_id)

        # Get unique list of DAQ nodes (needed to create start/stop files)
        self.daq_nodes_id_list = list(set(self.daq_nodes_id_list))

        # Store ip addresses of DAQ nodes in a dictionary
        self.daq_nodes_ip_list = {}
        for node_id in self.daq_nodes_id_list:
            self.daq_nodes_ip_list[node_id] = self.db.get_node_daq_ip(node_id)

        # Create new Merger process handler
        self.merger = Merger()
        self.configure_merger(self.merger)

        # Create new set of Level1 process handlers
        for l in range(self.level1_nproc):
            print "Run - Configuring Level1 process %d"%l
            lvl1_proc = Level1(l)
            self.configure_level1(lvl1_proc)
            self.level1_list.append(lvl1_proc)

        return setup

    def configure_adcboard(self,adcboard):

        # Configure ADC board DAQ/ZSUP processes after changing setup

        # Reset ADC board to default configuration
        adcboard.set_default_config()

        # Set executable
        adcboard.executable = self.daq_executable

        # Lock files (will contain PID of processes)
        adcboard.lock_file_daq = "%s_b%02d_daq"%(self.lock_file_head,adcboard.board_id)
        adcboard.lock_file_zsup = "%s_b%02d_zsup"%(self.lock_file_head,adcboard.board_id)

        # Control files needed to start/stop DAQ (will disappear)
        adcboard.start_file = self.start_file
        adcboard.quit_file  = self.quit_file

        # Status files for initialization
        adcboard.initok_file_daq = "%s_b%02d_daq"%(self.initok_file_head,adcboard.board_id)
        adcboard.initok_file_zsup = "%s_b%02d_zsup"%(self.initok_file_head,adcboard.board_id)
        adcboard.initfail_file_daq = "%s_b%02d_daq"%(self.initfail_file_head,adcboard.board_id)
        adcboard.initfail_file_zsup = "%s_b%02d_zsup"%(self.initfail_file_head,adcboard.board_id)

        # Define board connection information
        for link in self.board_link_list:
            (board,host,port,node) = link
            if (int(board) == adcboard.board_id):
                adcboard.node_id = self.db.get_node_id(host)
                adcboard.node_ip = self.db.get_node_daq_ip(adcboard.node_id)
                adcboard.conet2_link = int(port)
                adcboard.conet2_slot = int(node)

        # Define total DAQ time (default: 0, i.e. run forever)
        # In most cases the default is what you want
        adcboard.total_daq_time     = self.total_daq_time

        # Read ADC settings from board setup file
        adcboard.read_setup(self.setup)

    def configure_level1(self,level1):

        # Configure Level1 processes after changing setup

        # Reset Level1 process handler to default configuration
        level1.set_default_config()

        # Set executable
        level1.executable = self.level1_executable

        # Set maximum number of events to write in a single file
        level1.max_events = self.level1_maxevt

    def configure_merger(self,merger):

        # Configure Merger process after changing setup

        # Reset Merger process handler to default configuration
        merger.set_default_config()

        # Set executable
        merger.executable = self.merger_executable

    def runconfig_adcboard(self,adcboard):

        # Configure ADC board DAQ/ZSUP processes after changing run

        adcboard.run_number = self.run_number

        if (self.run_type == "FAKE"):
            adcboard.process_mode = "FAKE"
        else:
            adcboard.process_mode = "DAQ"

        s_bid = "b%02d"%adcboard.board_id
        adcboard.config_file_daq = "%s/%s_%s_daq.cfg"%(self.config_dir,self.config_file_head,s_bid)
        adcboard.config_file_zsup = "%s/%s_%s_zsup.cfg"%(self.config_dir,self.config_file_head,s_bid)
        adcboard.log_file_daq = "%s/%s_%s_daq.log"%(self.log_dir,self.log_file_head,s_bid)
        adcboard.log_file_zsup = "%s/%s_%s_zsup.log"%(self.log_dir,self.log_file_head,s_bid)
        adcboard.output_stream_daq = "%s/%s_%s_daq"%(self.stream_dir,self.stream_head,s_bid)
        adcboard.input_stream_zsup = adcboard.output_stream_daq
        adcboard.output_stream_zsup = "%s/%s_%s_zsup"%(self.stream_dir,self.stream_head,s_bid)

    def runconfig_level1(self,level1):

        # Configure Level1 processes after changing run

        level1.run_number = self.run_number

        # Get node_id and node_ip from DB using Merger node
        level1.node_id = self.db.get_node_id(self.merger_node)
        level1.node_ip = self.db.get_node_daq_ip(level1.node_id)

        s_lid = "lvl1_%02d"%level1.level1_id
        level1.config_file = "%s/%s_%s.cfg"%(self.config_dir,self.config_file_head,s_lid)
        level1.log_file = "%s/%s_%s.log"%(self.log_dir,self.log_file_head,s_lid)

        level1.input_stream = "%s/%s_%s"%(self.stream_dir,self.stream_head,s_lid)
        level1.output_dir = self.rawdata_dir
        level1.output_header = "%s_%s"%(self.rawdata_head,s_lid)

    def runconfig_merger(self,merger):

        # Configure Merger process after changing run

        merger.run_number = self.run_number

        # Get node_id and node_ip from DB
        merger.node_id = self.db.get_node_id(self.merger_node)
        merger.node_ip = self.db.get_node_daq_ip(merger.node_id)

        merger.config_file = "%s/%s_merger.cfg"%(self.config_dir,self.config_file_head)
        merger.log_file = "%s/%s_merger.log"%(self.log_dir,self.log_file_head)
        merger.input_list = "%s/%s_merger_input.list"%(self.config_dir,self.config_file_head)
        merger.output_list = "%s/%s_merger_output.list"%(self.config_dir,self.config_file_head)

    def start(self):

        # Create the "start the run" tag file on all DAQ nodes
        for node_id in self.daq_nodes_id_list:
            if (node_id == 0):
                open(self.start_file,'w').close()
            else:
                #command = "ssh -n -i %s %s '( touch %s )'"%(self.ssh_id_file,self.db.get_node_daq_ip(node_id),self.start_file)
                command = "ssh -n -i %s %s '( touch %s )'"%(self.ssh_id_file,self.daq_nodes_ip_list[node_id],self.start_file)
                print command
                os.system(command)

    def stop(self):

        # Create the "stop the run" tag file on all DAQ nodes
        for node_id in self.daq_nodes_id_list:
            if (node_id == 0):
                open(self.quit_file,'w').close()
            else:
                #command = "ssh -n -i %s %s '( touch %s )'"%(self.ssh_id_file,self.db.get_node_daq_ip(node_id),self.quit_file)
                command = "ssh -n -i %s %s '( touch %s )'"%(self.ssh_id_file,self.daq_nodes_ip_list[node_id],self.quit_file)
                print command
                os.system(command)

    def clean_up(self):

        # Clean up control directories at end of run
        print "Cleaning up run directories"

        # Remove the "start the run" tag file on all DAQ nodes
        for node_id in self.daq_nodes_id_list:
            if (node_id == 0):
                if (os.path.exists(self.start_file)): os.remove(self.start_file)
            else:
                #command = "ssh -n -i %s %s '( rm -f %s )'"%(self.ssh_id_file,self.db.get_node_daq_ip(node_id),self.start_file)
                command = "ssh -n -i %s %s '( rm -f %s )'"%(self.ssh_id_file,self.daq_nodes_ip_list[node_id],self.start_file)
                print command
                os.system(command)

        # Remove the "end the run" tag file on all DAQ nodes
        for node_id in self.daq_nodes_id_list:
            if (node_id == 0):
                if (os.path.exists(self.quit_file)): os.remove(self.quit_file)
            else:
                #command = "ssh -n -i %s %s '( rm -f %s )'"%(self.ssh_id_file,self.db.get_node_daq_ip(node_id),self.quit_file)
                command = "ssh -n -i %s %s '( rm -f %s )'"%(self.ssh_id_file,self.daq_nodes_ip_list[node_id],self.quit_file)
                print command
                os.system(command)

        # Remove initok/initfail files for all ADC nodes
        for adc in (self.adcboard_list):
            if (adc.node_id == 0):
                if (os.path.exists(adc.initok_file_daq)):    os.remove(adc.initok_file_daq)
                if (os.path.exists(adc.initok_file_zsup)):   os.remove(adc.initok_file_zsup)
                if (os.path.exists(adc.initfail_file_daq)):  os.remove(adc.initfail_file_daq)
                if (os.path.exists(adc.initfail_file_zsup)): os.remove(adc.initfail_file_zsup)
            else:
                command = "ssh -n -i %s %s '( rm -f %s %s %s %s)'"%(self.ssh_id_file,adc.node_ip,adc.initok_file_daq,adc.initok_file_zsup,adc.initfail_file_daq,adc.initfail_file_zsup)
                print command
                os.system(command)

        # Stop all receiving nc processes on the merger nodes (clumsy but could not find another way)
        if (self.merger.node_id == 0):
            for proc in self.proc_rcv: proc.terminate()
        else:
            command = "ssh -i %s %s '( ps -fu %s | grep recv-only | grep -v bash | grep -v grep | awk \"{print \$2}\" | xargs kill )'"%(self.ssh_id_file,self.merger.node_ip,os.getenv('USER',"daq"))
            print command
            os.system(command)

        # Now we can clean up all nc processes
        for proc in self.proc_snd:
            if proc.poll() != None:
                proc.wait()
            else:
                print "Run::clean_up - Problem closing sending nc process"
        for proc in self.proc_rcv:
            if proc.poll() != None:
                proc.wait()
            else:
                print "Run::clean_up - Problem closing receiving nc process"
