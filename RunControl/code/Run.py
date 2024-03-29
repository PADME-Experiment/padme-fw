import os
import re
import time
import shlex
import subprocess

from Level1   import Level1
from Merger   import Merger
from ADCBoard import ADCBoard
from Trigger  import Trigger
from PadmeDB  import PadmeDB

class Run:

    def __init__(self):

        # Get account under which the RunControl runs
        self.user_account = os.getenv('USER',"daq")

        # Get location of DAQ main directory from PADME_DAQ_DIR
        # Default to current dir if not set
        self.daq_dir = os.getenv('PADME_DAQ_DIR',".")

        # Get base port number for network tunnels from PADME_RC_TUNNEL_BASE_PORT
        self.base_port_number = int(os.getenv('PADME_RC_TUNNEL_BASE_PORT',"31400"))

        # Define id file for passwordless ssh command execution
        self.ssh_id_file = "%s/.ssh/id_rsa_daq"%os.getenv('HOME',"~")

        # Get location of padme-fw software from PADME
        # Default to PADME_DAQ_DIR/padme-fw if not set
        self.padme_fw = os.getenv('PADME',"%s/padme-fw"%self.daq_dir)

        # Define executables to use in production
        self.daq_executable = "%s/PadmeDAQ/PadmeDAQ.exe"%self.padme_fw
        self.merger_executable = "%s/Level1/PadmeMerger.exe"%self.padme_fw
        self.level1_executable = "%s/Level1/PadmeLevel1.exe"%self.padme_fw
        self.trigger_executable = "%s/PadmeTrig/PadmeTrig.exe"%self.padme_fw

        # Define directory containing setup subdirectories
        self.setup_root_dir = "%s/setup"%self.daq_dir

        # Define files where the current and last run name will be written
        self.current_run_file = "%s/run/current_run"%self.daq_dir
        self.last_run_file = "%s/run/last_run"%self.daq_dir

        # Define directory containing rawdata directories for each run
        self.rawdata_root_dir = self.daq_dir+"/local/rawdata"

        # Define name and position of control files
        self.control_dir = self.daq_dir+"/local/run"
        self.start_file = self.control_dir+"/start"
        self.quit_file = self.control_dir+"/quit"
        self.trig_start_file = self.control_dir+"/start_trig"
        self.trig_stop_file = self.control_dir+"/stop_trig"
        self.initok_file_head = self.control_dir+"/initok"
        self.initfail_file_head = self.control_dir+"/initfail"
        self.lock_file_head = self.control_dir+"/lock"

        # Connect to database services
        self.db = PadmeDB()

        # Do not define a default setup
        self.setup = ""

        # Run final status defaults to 3 (stopped normally)
        self.final_status = 3

        self.set_default_config()

    def change_run(self):

        # Check if requested run number was not used before
        # Saves the day if more than one RunControl program is running at the same time (DON'T DO THAT!!!)
        if (self.run_number):
            run_is_in_db = self.db.is_run_in_db(self.run_number)
            if (run_is_in_db):
                print "Run::change_run - ERROR - Run %d is already in the DB: cannot use it again"%self.run_number
                print "Please check if someone else is using this RunControl before retrying"
                #self.send_answer("error_init")
                return False

        # Define run name using run number and start time
        self.run_name = "run_%7.7d_%s"%(self.run_number,time.strftime("%Y%m%d_%H%M%S",time.gmtime()))

        # Write run name to current_run file for monitoring
        with open(self.current_run_file,"w") as lf: lf.write("%s\n"%self.run_name)

        self.run_dir = self.daq_dir+"/runs/"+self.run_name

        self.config_dir = "%s/cfg"%self.run_dir
        self.config_file = "%s.cfg"%self.run_name
        self.config_file_head = self.run_name

        self.log_dir = "%s/log"%self.run_dir
        self.log_file_head = self.run_name

        self.stream_dir = "%s/local/streams/%s"%(self.daq_dir,self.run_name)
        self.stream_head = self.run_name

        self.rawdata_dir = "%s/%s"%(self.rawdata_root_dir,self.run_name)
        self.rawdata_head = self.run_name

        # Make sure Merger runs on a different node after each run
        self.merger_node = self.next_merger_node()

        # Configure Merger for this run
        self.runconfig_merger(self.merger)

        # Configure Trigger for this run
        self.runconfig_trigger(self.trigger)

        # Configure ADC boards for this run
        for adcboard in self.adcboard_list:
            #print "--- Configuring for run board %d"%adcboard.board_id
            self.runconfig_adcboard(adcboard)

        # Configure Level1 processes for this run
        for level1 in self.level1_list:
            self.runconfig_level1(level1)

        # If this is a real run, create it in the DB
        if (self.run_number):
            print "Creating Run %d structure in DB"%self.run_number
            if self.create_run_in_db() == "error":
                print "Run::change_run - ERROR - Cannot create Run in the DB"
                return False

        return True

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

        self.trigger = None

        self.merger = None

        self.level1_list = []

        self.run_number = 0
        self.run_name = "run_%7.7d_%s"%(self.run_number,time.strftime("%Y%m%d_%H%M%S",time.gmtime()))
        self.run_type = "TEST"
        self.run_user = "PADME crew"
        self.run_comment_start = "Generic start of run"
        self.run_comment_end = "Generic end of run"

        self.run_dir = "%s/runs/%s"%(self.daq_dir,self.run_name)

        self.config_dir = "%s/cfg"%self.run_dir
        self.config_file = "%s.cfg"%self.run_name
        self.config_file_head = self.run_name

        self.log_dir = "%s/log"%self.run_dir
        self.log_file_head = self.run_name

        self.stream_dir = "%s/local/streams/%s"%(self.daq_dir,self.run_name)
        self.stream_head = self.run_name

        self.rawdata_dir = "%s/%s"%(self.rawdata_root_dir,self.run_name)
        self.rawdata_head = self.run_name

        self.trigger_node = "localhost"

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
                    s_boards = sorted(list(set(re_boardid.findall(p_value))),key=int)
                    for s_bid in s_boards:
                        # Convert to int as findall returns strings
                        self.boardid_list.append(int(s_bid))
                elif (p_name == "board_link"):
                    mm = re_board_link.search(p_value)
                    if (mm):
                        self.board_link_list.append(mm.group(1,2,3,4))
                    else:
                        print "Run - WARNING: unable to decode board_link parameter while reading setup file %s"%(setup_file,)
                        print l
                elif (p_name == "total_daq_time"):
                    self.total_daq_time = int(p_value)
                elif (p_name == "trigger_node"):
                    self.trigger_node = p_value
                #elif (p_name == "trigger_mask"):
                #    self.trigger_mask = p_value
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

    def config_list(self):

        cfg_list = []

        cfg_list.append(["user_account",      self.user_account])
        cfg_list.append(["daq_dir",           self.daq_dir])
        cfg_list.append(["base_port_number",  self.base_port_number])
        cfg_list.append(["ssh_id_file",       self.ssh_id_file])

        cfg_list.append(["daq_executable",    self.daq_executable])
        cfg_list.append(["trigger_executable",self.trigger_executable])
        cfg_list.append(["merger_executable", self.merger_executable])
        cfg_list.append(["level1_executable", self.level1_executable])

        cfg_list.append(["start_file",        self.start_file])
        cfg_list.append(["quit_file",         self.quit_file])
        cfg_list.append(["trig_start_file",   self.trig_start_file])
        cfg_list.append(["trig_stop_file",    self.trig_stop_file])
        cfg_list.append(["initok_file_head",  self.initok_file_head])
        cfg_list.append(["initfail_file_head",self.initfail_file_head])
        cfg_list.append(["lock_file_head",    self.lock_file_head])
        cfg_list.append(["rawdata_dir",       self.rawdata_dir])

        cfg_list.append(["run_number",        str(self.run_number)])
        cfg_list.append(["run_name",          self.run_name])
        cfg_list.append(["run_dir",           self.run_dir])
        cfg_list.append(["run_type",          self.run_type])
        cfg_list.append(["run_user",          self.run_user])
        cfg_list.append(["run_comment_start", self.run_comment_start])
        cfg_list.append(["setup",             self.setup])

        s_board_list = ""
        for b in self.boardid_list:
            if (s_board_list):
                s_board_list += " %d"%b
            else:
                s_board_list = "%d"%b
        cfg_list.append(["board_list",s_board_list])

        for b in self.boardid_list:
            for link in self.board_link_list:
                (board,host,port,node) = link
                if b == int(board):
                    board_link = "%s %s %s %s"%(board,host,port,node)
                    cfg_list.append(["board_link","%s %s %s %s"%(board,host,port,node)])

        cfg_list.append(["config_dir",      self.config_dir])
        cfg_list.append(["config_file",     self.config_file])
        cfg_list.append(["config_file_head",self.config_file_head])

        cfg_list.append(["log_dir",         self.log_dir])
        cfg_list.append(["log_file_head",   self.log_file_head])

        cfg_list.append(["stream_dir",      self.stream_dir])
        cfg_list.append(["stream_head",     self.stream_head])

        cfg_list.append(["rawdata_dir",     self.rawdata_dir])
        cfg_list.append(["rawdata_head",    self.rawdata_head])

        cfg_list.append(["trigger_node",    self.trigger_node])

        if self.merger_node:
            cfg_list.append(["merger_node", self.merger_node])

        if self.merger_node_list:
            cfg_list.append(["merger_node_list"," ".join(self.merger_node_list)])

        cfg_list.append(["level1_nproc",    str(self.level1_nproc)])
        cfg_list.append(["level1_maxevt",   str(self.level1_maxevt)])

        cfg_list.append(["total_daq_time",  str(self.total_daq_time)])

        return cfg_list

    def format_config(self):

        cfgstring = ""
        for cfg in self.config_list(): cfgstring += "%-30s %s\n"%(cfg[0],cfg[1])
        return cfgstring

    def create_run_in_db(self):

        # Create run in DB
        self.db.create_run(self.run_number,self.run_name,self.run_type)
        self.db.set_run_time_create(self.run_number,self.db.now_str())
        self.db.set_run_user(self.run_number,self.run_user)
        self.db.set_run_comment_start(self.run_number,self.db.now_str(),self.run_comment_start)

        # Add all configuration parameters
        #for cfg in self.config_list():
        #    self.db.add_cfg_para_run(self.run_number,cfg[0],cfg[1])

        # Create board structures in DB
        for adc in (self.adcboard_list):
            if adc.create_proc_daq() == "error":
                print "Run::create_run - ERROR - Cannot create DAQ process for board %d in the DB"%adc.board_id
                return "error"
            if adc.create_proc_zsup() == "error":
                print "Run::create_run - ERROR - Cannot create ZSUP process for board %d in the DB"%adc.board_id
                return "error"

        # Create Trigger structure in DB
        if self.trigger.create_trigger() == "error":
            print "Run::create_run - ERROR - Cannot create Trigger process in the DB"
            return "error"

        # Create Merger structure in DB
        if self.merger.create_merger() == "error":
            print "Run::create_run - ERROR - Cannot create Merger process in the DB"
            return "error"

        # Create Level1 structures in DB
        for lvl1 in (self.level1_list):
            if lvl1.create_level1() == "error":
                print "Run::create_run - ERROR - Cannot create Level1 process for level1 %d in the DB"%lvl1.level1_id
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

        print "Writing configuration file %s for Trigger"%self.trigger.config_file
        self.trigger.write_config()

        for adc in (self.adcboard_list):
            print "Writing configuration files %s and %s for ADC board %d"%(adc.config_file_daq,adc.config_file_zsup,adc.board_id)
            adc.write_config()

        for lvl1 in (self.level1_list):
            print "Writing configuration files %s for Level1 %d"%(lvl1.config_file,lvl1.level1_id)
            lvl1.write_config()

    def print_config(self):

        print self.format_config()

    def create_fifos(self):

        # Create stream directory and fifo file for Trigger
        if self.trigger.node_id == 0:
            if not os.path.exists(self.stream_dir): os.makedirs(self.stream_dir,0755)
            os.mkfifo(self.trigger.output_stream)
        else:
            command = "ssh -i %s %s '( mkdir -p %s ; mkfifo %s )'"%(self.ssh_id_file,self.trigger.node_ip,self.stream_dir,self.trigger.output_stream)
            print command
            os.system(command)

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
            if not os.path.exists(self.stream_dir): os.makedirs(self.stream_dir,0755)
        else:
            command = "ssh -i %s %s '( mkdir -p %s )'"%(self.ssh_id_file,self.merger.node_ip,self.stream_dir)
            print command
            os.system(command)

        # Create fifo files to connect Merger to Level1 processes
        for lvl1 in (self.level1_list):
            if lvl1.node_id == 0:
                os.mkfifo(lvl1.input_stream)
            else:
                command = "ssh -i %s %s '( mkfifo %s )'"%(self.ssh_id_file,lvl1.node_ip,lvl1.input_stream)
                print command
                os.system(command)

        # If Trigger process runs on a node which is different from the Merger
        # we must replicate the Trigger FIFO file on the Merger
        if self.trigger.node_id != self.merger.node_id:

            # Duplicate Trigger FIFO file on Merger node
            if self.merger.node_id == 0:
                os.mkfifo(self.trigger.output_stream)
            else:
                command = "ssh -n -i %s %s '( mkfifo %s )'"%(self.ssh_id_file,self.merger.node_ip,self.trigger.output_stream)
                print command
                os.system(command)

        # If an ADCBoard process runs on a node which is different from the Merger
        # we must replicate the ZSUP FIFO file on the Merger
        if self.merger.node_id == 0:

            for adc in (self.adcboard_list):
                if adc.node_id != self.merger.node_id: os.mkfifo(adc.output_stream_zsup)

        else:

            # More efficient if we give a single (long) ssh command
            stream_list = ""
            for adc in (self.adcboard_list):
                if adc.node_id != self.merger.node_id: stream_list += " %s"%adc.output_stream_zsup
            if stream_list:
                command = "ssh -n -i %s %s '( mkfifo %s )'"%(self.ssh_id_file,self.merger.node_ip,stream_list)
                print command
                os.system(command)
 
    def create_receivers(self):

        # Keep track of receiver processes and handles: needed for final celeanup
        self.proc_rcv = []
        self.hand_rcv = []

        # If an ADCBoard process runs on a node which is different from the Merger
        # we create the receiving end of a network tunnel
        for adc in (self.adcboard_list):

            # Check if ADCBoard and Merger run on different nodes
            if adc.node_id != self.merger.node_id:

                # Define port for network tunnel
                port_number = self.base_port_number+adc.board_id
                print "Creating receiving end of network tunnel for board %d on port %d"%(adc.board_id,port_number)

                # Define log file and open it
                log_file = "%s/%s_nc_%d_recv.log"%(self.log_dir,self.log_file_head,port_number)
                log_handle = open(log_file,"w")
                self.hand_rcv.append(log_handle)

                # Open receiving end of tunnel on Merger node
                command = "nc -l -k -v --recv-only %s %d > %s < /dev/zero"%(self.merger.node_ip,port_number,adc.output_stream_zsup)
                #command = "nc --udp -l -v --recv-only %s %d > %s < /dev/zero"%(self.merger.node_ip,port_number,adc.output_stream_zsup)
                if self.merger.node_id != 0:
                    command = "ssh -f -i %s %s '( %s )'"%(self.ssh_id_file,self.merger.node_ip,command)
                print command
                try:
                    proc = subprocess.Popen(shlex.split(command),stdout=log_handle,stderr=subprocess.STDOUT,bufsize=1)
                    self.proc_rcv.append(proc)
                except OSError as e:
                    print "Run::create_receivers - ERROR: Execution failed: %s",e
                time.sleep(0.5)

        # If the Trigger process runs on a node which is different from the Merger
        # we create the receiving end of a network tunnel
        if self.trigger.node_id != self.merger.node_id:

            # Define port for network tunnel
            port_number = self.base_port_number+99
            print "Creating receiving end of network tunnel for trigger on port %d"%port_number

            # Define log file and open it
            log_file = "%s/%s_nc_%d_recv.log"%(self.log_dir,self.log_file_head,port_number)
            log_handle = open(log_file,"w")
            self.hand_rcv.append(log_handle)

            # Open receiving end of tunnel on Merger node
            #command = "nc -l -k -v --recv-only %s %d > %s < /dev/zero"%(self.merger.node_ip,port_number,self.trigger.output_stream)
            command = "nc --udp -l -v --recv-only %s %d > %s < /dev/zero"%(self.merger.node_ip,port_number,self.trigger.output_stream)
            if self.merger.node_id != 0:
                command = "ssh -f -i %s %s '( %s )'"%(self.ssh_id_file,self.merger.node_ip,command)
            print command
            try:
                proc = subprocess.Popen(shlex.split(command),stdout=log_handle,stderr=subprocess.STDOUT,bufsize=1)
                self.proc_rcv.append(proc)
            except OSError as e:
                print "Run::create_receivers - ERROR: Execution failed: %s",e
            time.sleep(0.5)

    def create_senders(self):

        # Keep track of sender processes and handles: needed for final celeanup
        self.proc_snd = []
        self.hand_snd = []

        # If an ADCBoard process runs on a node which is different from the Merger
        # we create the sending end of a network tunnel
        for adc in (self.adcboard_list):

            # Check if ADCBoard and Merger run on different nodes
            if adc.node_id != self.merger.node_id:

                # Define port for network tunnel
                port_number = self.base_port_number+adc.board_id
                print "Creating sending end of network tunnel for board %d on port %d"%(adc.board_id,port_number)

                # Define log file and open it
                log_file = "%s/%s_nc_%d_send.log"%(self.log_dir,self.log_file_head,port_number)
                log_handle = open(log_file,"w")
                self.hand_snd.append(log_handle)

                # Open sending end of tunnel on DAQ node. Add some code to wait for receiving end to appear before proceeding.
                command = "while ! nc -z %s %d ; do sleep 1 ; done ; nc -v --send-only %s %d < %s > /dev/null"%(self.merger.node_ip,port_number,self.merger.node_ip,port_number,adc.output_stream_zsup)
                #command = "nc -v --udp --send-only %s %d < %s > /dev/null"%(self.merger.node_ip,port_number,adc.output_stream_zsup)
                if adc.node_id != 0:
                    command = "ssh -f -i %s %s '( %s )'"%(self.ssh_id_file,adc.node_ip,command)
                print command
                try:
                    proc = subprocess.Popen(shlex.split(command),stdout=log_handle,stderr=subprocess.STDOUT,bufsize=1)
                    self.proc_snd.append(proc)
                except OSError as e:
                    print "Run::create_senders - ERROR: Execution failed: %s",e
                time.sleep(0.5)

        # If the Trigger process runs on a node which is different from the Merger
        # we create the sending end of a network tunnel
        if self.trigger.node_id != self.merger.node_id:

            # Define port for network tunnel
            port_number = self.base_port_number+99
            print "Creating sending end of network tunnel for trigger on port %d"%port_number

            # Define log file and open it
            log_file = "%s/%s_nc_%d_send.log"%(self.log_dir,self.log_file_head,port_number)
            log_handle = open(log_file,"w")
            self.hand_snd.append(log_handle)

            # Open sending end of tunnel on Trigger node. Add some code to wait for receiving end to appear before proceeding.
            #command = "while ! nc -z %s %d ; do sleep 1 ; done ; nc -v --send-only %s %d < %s > /dev/null"%(self.merger.node_ip,port_number,self.merger.node_ip,port_number,self.trigger.output_stream)
            command = "nc -v --udp --send-only %s %d < %s > /dev/null"%(self.merger.node_ip,port_number,self.trigger.output_stream)
            if adc.node_id != 0:
                command = "ssh -f -i %s %s '( %s )'"%(self.ssh_id_file,self.trigger.node_ip,command)
            print command
            try:
                proc = subprocess.Popen(shlex.split(command),stdout=log_handle,stderr=subprocess.STDOUT,bufsize=1)
                self.proc_snd.append(proc)
            except OSError as e:
                print "Run::create_senders - ERROR: Execution failed: %s",e
            time.sleep(0.5)

    def create_merger_input_list(self):

        print "Creating merger input list file %s"%self.merger.input_list
        f = open(self.merger.input_list,"w")
        for adcboard in self.adcboard_list: f.write("%d %s\n"%(adcboard.board_id,adcboard.output_stream_zsup))
        f.write("%d %s\n"%(99,self.trigger.output_stream))
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
                if not os.path.exists(level1.output_dir): os.makedirs(level1.output_dir,0755)
            else:
                command = "ssh -i %s %s '( mkdir -p %s )'"%(self.ssh_id_file,level1.node_ip,level1.output_dir)
                print command
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

        # Create new Trigger process handler
        self.trigger = Trigger()
        self.configure_trigger(self.trigger)

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
        adcboard.initok_file_daq    = "%s_b%02d_daq"%(self.initok_file_head,adcboard.board_id)
        adcboard.initok_file_zsup   = "%s_b%02d_zsup"%(self.initok_file_head,adcboard.board_id)
        adcboard.initfail_file_daq  = "%s_b%02d_daq"%(self.initfail_file_head,adcboard.board_id)
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

    def configure_trigger(self,trigger):

        # Configure Trigger process after changing setup

        # Reset Trigger process handler to default configuration
        trigger.set_default_config()

        # Set executable
        trigger.executable = self.trigger_executable

        # Lock file (will contain PID of process)
        trigger.lock_file = "%s_trigger"%self.lock_file_head

        # Control files needed to start/stop trigger generation
        trigger.start_file = self.trig_start_file
        trigger.quit_file  = self.trig_stop_file

        # Status files for initialization
        trigger.initok_file = "%s_trigger"%self.initok_file_head
        trigger.initfail_file = "%s_trigger"%self.initfail_file_head

        # Set node where Trigger will run
        trigger.node_id = self.db.get_node_id(self.trigger_node)
        trigger.node_ip = self.db.get_node_daq_ip(trigger.node_id)

        # Define trigger mask to use for this setup
        #trigger.trigger_mask = self.trigger_mask

        # Define total DAQ time (default: 0, i.e. run forever)
        # In most cases the default is what you want
        trigger.total_daq_time = self.total_daq_time

        # Read Trigger settings from trigger setup file
        trigger.read_setup(self.setup)

    def configure_merger(self,merger):

        # Configure Merger process after changing setup

        # Reset Merger process handler to default configuration
        merger.set_default_config()

        # Set executable
        merger.executable = self.merger_executable

    def configure_level1(self,level1):

        # Configure Level1 processes after changing setup

        # Reset Level1 process handler to default configuration
        level1.set_default_config()

        # Set executable
        level1.executable = self.level1_executable

        # Set maximum number of events to write in a single file
        level1.max_events = self.level1_maxevt

    def runconfig_adcboard(self,adcboard):

        # Configure ADC board DAQ/ZSUP processes after changing run

        adcboard.run_number = self.run_number

        if (self.run_type == "FAKE"):
            adcboard.process_mode = "FAKE"
        else:
            adcboard.process_mode = "DAQ"

        s_bid = "b%02d"%adcboard.board_id
        adcboard.config_file_daq    = "%s/%s_%s_daq.cfg"%(self.config_dir,self.config_file_head,s_bid)
        adcboard.config_file_zsup   = "%s/%s_%s_zsup.cfg"%(self.config_dir,self.config_file_head,s_bid)
        adcboard.log_file_daq       = "%s/%s_%s_daq.log"%(self.log_dir,self.log_file_head,s_bid)
        adcboard.log_file_zsup      = "%s/%s_%s_zsup.log"%(self.log_dir,self.log_file_head,s_bid)
        adcboard.output_stream_daq  = "%s/%s_%s_daq"%(self.stream_dir,self.stream_head,s_bid)
        adcboard.input_stream_zsup  = adcboard.output_stream_daq
        adcboard.output_stream_zsup = "%s/%s_%s_zsup"%(self.stream_dir,self.stream_head,s_bid)

    def runconfig_trigger(self,trigger):

        # Configure Trigger process after changing run

        trigger.run_number = self.run_number

        trigger.config_file   = "%s/%s_trigger.cfg"%(self.config_dir,self.config_file_head)
        trigger.log_file      = "%s/%s_trigger.log"%(self.log_dir,self.log_file_head)
        trigger.output_stream = "%s/%s_trigger"%(self.stream_dir,self.stream_head)

    def runconfig_merger(self,merger):

        # Configure Merger process after changing run

        merger.run_number = self.run_number

        # Get node_id and node_ip from DB
        merger.node_id = self.db.get_node_id(self.merger_node)
        merger.node_ip = self.db.get_node_daq_ip(merger.node_id)

        merger.config_file = "%s/%s_merger.cfg"%(self.config_dir,self.config_file_head)
        merger.log_file    = "%s/%s_merger.log"%(self.log_dir,self.log_file_head)
        merger.input_list  = "%s/%s_merger_input.list"%(self.config_dir,self.config_file_head)
        merger.output_list = "%s/%s_merger_output.list"%(self.config_dir,self.config_file_head)

    def runconfig_level1(self,level1):

        # Configure Level1 processes after changing run

        level1.run_number = self.run_number

        # Get node_id and node_ip from DB using Merger node
        level1.node_id = self.db.get_node_id(self.merger_node)
        level1.node_ip = self.db.get_node_daq_ip(level1.node_id)

        s_lid = "lvl1_%02d"%level1.level1_id
        level1.config_file = "%s/%s_%s.cfg"%(self.config_dir,self.config_file_head,s_lid)
        level1.log_file    = "%s/%s_%s.log"%(self.log_dir,self.log_file_head,s_lid)
        level1.input_stream = "%s/%s_%s"%(self.stream_dir,self.stream_head,s_lid)

        level1.output_dir    = self.rawdata_dir
        level1.output_header = "%s_%s"%(self.rawdata_head,s_lid)

    def start(self):

        # Create the "start the run" tag file on all DAQ nodes
        print "Starting DAQs"
        for node_id in self.daq_nodes_id_list:
            if (node_id == 0):
                open(self.start_file,'w').close()
            else:
                command = "ssh -n -i %s %s '( touch %s )'"%(self.ssh_id_file,self.daq_nodes_ip_list[node_id],self.start_file)
                print command
                os.system(command)

        # Wait 5sec before enabling triggers
        time.sleep(5)

        # Enable triggers
        print "Enabling triggers"
        if (self.trigger.node_id) == 0:
            open(self.trig_start_file,'w').close()
        else:
            command = "ssh -n -i %s %s '( touch %s )'"%(self.ssh_id_file,self.trigger.node_ip,self.trig_start_file)
            print command
            os.system(command)

        # Update run status in DB
        #if (self.run_number):
        #    self.db.set_run_time_start(self.run_number,self.db.now_str())
        #    self.db.set_run_status(self.run_number,self.db.DB_RUN_STATUS_RUNNING)

    def stop(self):

        # Disable triggers
        print "Disabling triggers"
        if (self.trigger.node_id) == 0:
            open(self.trig_stop_file,'w').close()
        else:
            command = "ssh -n -i %s %s '( touch %s )'"%(self.ssh_id_file,self.trigger.node_ip,self.trig_stop_file)
            print command
            os.system(command)

        # Wait 1sec before telling all processes to stop
        time.sleep(1)

        # Create the "stop the run" tag file on all DAQ nodes
        for node_id in self.daq_nodes_id_list:
            if (node_id == 0):
                open(self.quit_file,'w').close()
            else:
                command = "ssh -n -i %s %s '( touch %s )'"%(self.ssh_id_file,self.daq_nodes_ip_list[node_id],self.quit_file)
                print command
                os.system(command)

        # Write run name to last_run file for monitoring
        with open(self.last_run_file,"w") as lf: lf.write("%s\n"%self.run_name)

        # Finalize run in DB
        #if (self.run_number):
        #    self.db.set_run_status(self.run_number,self.final_status)
        #    self.db.set_run_time_stop(self.run_number,self.db.now_str())
        #    self.db.set_run_comment_end(self.run_number,self.db.now_str(),self.run_comment_end)

    def clean_up(self):

        # Clean up control directories at end of run
        print "Cleaning up run directories"

        # Remove the "start/stop the run" tag files on all DAQ nodes
        for node_id in self.daq_nodes_id_list:
            if (node_id == 0):
                if (os.path.exists(self.start_file)): os.remove(self.start_file)
                if (os.path.exists(self.quit_file)): os.remove(self.quit_file)
            else:
                command = "ssh -n -i %s %s '( rm -f %s %s )'"%(self.ssh_id_file,self.daq_nodes_ip_list[node_id],self.start_file,self.quit_file)
                print command
                os.system(command)

        # Remove the "start/stop the triggers" tag files on Trigger node
        if (self.trigger.node_id == 0):
            if (os.path.exists(self.trig_start_file)): os.remove(self.trig_start_file)
            if (os.path.exists(self.trig_stop_file)): os.remove(self.trig_stop_file)
        else:
            command = "ssh -n -i %s %s '( rm -f %s %s )'"%(self.ssh_id_file,self.trigger.node_ip,self.trig_start_file,self.trig_stop_file)
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

        # Remove initok/initfail files on Trigger node
        if (self.trigger.node_id == 0):
            if (os.path.exists(self.trigger.initok_file)):   os.remove(self.trigger.initok_file)
            if (os.path.exists(self.trigger.initfail_file)): os.remove(self.trigger.initfail_file)
        else:
            command = "ssh -n -i %s %s '( rm -f %s %s)'"%(self.ssh_id_file,self.trigger.node_ip,self.trigger.initok_file,self.trigger.initfail_file)
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

        # Close all receiving/sending nc log files
        for handler in self.hand_rcv: handler.close()
        for handler in self.hand_snd: handler.close()
