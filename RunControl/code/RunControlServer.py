import os
import time
import socket
import sys
import re

from PadmeDB import PadmeDB

class RunControlServer:

    def __init__(self, run):

        self.run = run

        # Start in idle state
        self.current_state = "idle"

        # Create handler for PadmeDB
        self.db = PadmeDB()

        # Create useful regular expressions
        self.re_get_board_config = re.compile("^get_board_config (\d+)$")
        self.re_get_board_log_file = re.compile("^get_board_log_file (\d+)$")
        self.re_change_setup = re.compile("^change_setup (\w+)$")

        # Create a TCP/IP socket
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        # Bind the socket to the port
        server_address = ('localhost', 10000)
        self.write_log("Starting server socket on %s port %s"%server_address)
        try:
            self.sock.bind(server_address) # Listen for incoming connections
        except:
            self.write_log("Error binding to socket: "+str(sys.exc_info()[0]))
            return
        self.sock.listen(1)

        # Setup main interface
        self.main_loop()

        # Clean up before exiting
        self.sock.close()

    def main_loop(self):

        while True:

            # Wait for a connection
            self.write_log('waiting for a connection')
            (self.connection,client_address) = self.sock.accept()
            self.write_log('connection from '+str(client_address))

            while True:

                # Handle connection according to curren status of RunControl
                if self.current_state == "idle":
                    new_state = self.state_idle()
                elif self.current_state == "initialized":
                    new_state = self.state_initialized()
                elif self.current_state == "running":
                    new_state = self.state_running()
                elif self.current_state == "initfail":
                    new_state = self.state_initfail()
                else:
                    self.write_log("ERROR: unknown state %s - ABORTING"%self.current_state)
                    new_state = "exit"

            # See if status changed
                if new_state == "idle" or new_state == "initialized" or new_state == "running" or new_state == "initfail":
                    self.current_state = new_state
                elif new_state == "client_close":
                    self.connection.close()
                    break
                elif new_state == "exit":
                    self.connection.close()
                    return
                else:
                    self.write_log("ERROR: unknown new state %s - ABORTING"%new_state)
                    self.connection.close()
                    return

    def write_log(self,msg):
        print self.now_str()+" "+msg

    def now_str(self):
        return time.strftime("%Y-%m-%d %H:%M:%S",time.localtime())

    def state_idle(self):

        # Receive and process commands for "idle" state
        while True:

            cmd = self.get_command()
            self.write_log('received command '+cmd)
            if (cmd == "client_close"):
                return "client_close"
            elif (cmd == "get_state"):
                self.send_answer(self.current_state)
            elif (cmd == "get_setup"):
                self.send_answer(self.run.setup)
            elif (cmd == "get_setup_list"):
                self.send_answer(self.get_setup_list())
            elif (cmd == "get_board_list"):
                self.send_answer(str(self.run.boardid_list))
            elif (cmd == "get_last_run_number"):
                self.send_answer(str(self.db.get_last_run_in_db()))
            elif (cmd == "new_run"):
                res = self.new_run()
                if (res == "client_close"):
                    return "client_close"
                elif (res == "error"):
                    self.write_log('ERROR while initializing new run')
                elif (res == "initialized"):
                    return "initialized"
                elif (res == "initfail"):
                    return "initfail"
                else:
                    self.write_log("ERROR: new_run returned unknown answer "+res+" (?)")
            elif (cmd == "exit"):
                self.send_answer("exiting")
                return "exit"
            elif (cmd == "help"):
                msg = """Available commands:
help\t\tShow this help
get_state\tShow current state of RunControl
get_setup\tShow current setup name
get_setup_list\tShow list of available setups
get_board_list\tShow list of boards in use with current setup
get_board_config <b>\tShow current configuration of board <b>
get_last_run_number\tReturn last run number in DB
change_setup <setup>\tChange run setup to <setup>
new_run\t\tInitialize system for a new run
exit\t\tTell RunControl server to exit (use with extreme care!)"""
                self.send_answer(msg)
            else:

                # See if command can be handled by a regular expression
                found_re = False

                m = self.re_get_board_config.match(cmd)
                if (m):
                    self.send_answer(self.get_board_config(int(m.group(1))))
                    found_re = True

                m = self.re_change_setup.match(cmd)
                if (m):
                    self.send_answer(self.change_setup(m.group(1)))
                    found_re = True

                # No regular expression matched: command is unknown
                if not found_re:
                    self.send_answer("unknown command")
                    self.write_log('command '+cmd+' is unknown')

    def state_initialized(self):

        # Receive and process commands for "initialized" state
        while True:

            cmd = self.get_command()
            self.write_log('received command '+cmd)
            if (cmd == "client_close"):
                return "client_close"
            elif (cmd == "get_state"):
                self.send_answer(self.current_state)
            elif (cmd == "get_setup"):
                self.send_answer(self.run.setup)
            elif (cmd == "get_board_list"):
                self.send_answer(str(self.run.boardid_list))
            elif (cmd == "get_run_number"):
                self.send_answer(str(self.run.run_number))
            elif (cmd == "abort_run"):
                return self.abort_run()
            elif (cmd == "start_run"):
                return self.start_run()
            elif (cmd == "exit"):
                self.send_answer("exiting")
                return "exit"
            elif (cmd == "help"):
                msg = """Available commands:
help\t\tShow this help
get_state\tShow current state of RunControl
get_setup\tShow current setup name
get_board_list\tShow list of boards in use with current setup
get_board_config <b>\tShow current configuration of board <b>
get_board_log_file <b>\tGet name of log file for board <b>
get_run_number\tReturn current run number
start_run\t\tStart run
abort_run\t\tAbort run
exit\t\tTell RunControl server to exit (use with extreme care!)"""
                self.send_answer(msg)

            else:

                # See if command can be handled by a regular expression
                found_re = False

                m = self.re_get_board_config.match(cmd)
                if (m):
                    self.send_answer(self.get_board_config(int(m.group(1))))
                    found_re = True

                m = self.re_get_board_log_file.match(cmd)
                if (m):
                    self.send_answer(self.get_board_log_file(int(m.group(1))))
                    found_re = True

                # No regular expression matched: command is unknown
                if not found_re:
                    self.send_answer("unknown command")
                    self.write_log('command '+cmd+' is unknown')

    def state_running(self):

        # Receive and process commands for "running" state
        while True:

            cmd = self.get_command()
            self.write_log('received command '+cmd)
            if (cmd == "client_close"):
                return "client_close"
            elif (cmd == "get_state"):
                self.send_answer(self.current_state)
            elif (cmd == "get_setup"):
                self.send_answer(self.run.setup)
            elif (cmd == "get_board_list"):
                self.send_answer(str(self.run.boardid_list))
            elif (cmd == "get_run_number"):
                self.send_answer(str(self.run.run_number))
            elif (cmd == "stop_run"):
                return self.stop_run()
            elif (cmd == "exit"):
                self.send_answer("exiting")
                return "exit"
            elif (cmd == "help"):
                msg = """Available commands:
help\t\tShow this help
get_state\tShow current state of RunControl
get_setup\tShow current setup name
get_board_list\tShow list of boards in use with current setup
get_board_config <b>\tShow current configuration of board <b>
get_board_log_file <b>\tGet name of log file for board <b>
get_run_number\tReturn current run number
stop_run\t\tStop the run
exit\t\tTell RunControl server to exit (use with extreme care!)"""
                self.send_answer(msg)

            else:

                # See if command can be handled by a regular expression
                found_re = False

                m = self.re_get_board_config.match(cmd)
                if (m):
                    self.send_answer(self.get_board_config(int(m.group(1))))
                    found_re = True

                m = self.re_get_board_log_file.match(cmd)
                if (m):
                    self.send_answer(self.get_board_log_file(int(m.group(1))))
                    found_re = True

                # No regular expression matched: command is unknown
                if not found_re:
                    self.send_answer("unknown command")
                    self.write_log('command '+cmd+' is unknown')
        return "idle"

    def state_initfail(self):

        return "idle"

    def get_command(self):

        # First get length of string
        l = ""
        for i in range(5): # Max 99999 characters
            ch = self.connection.recv(1)
            if ch:
                l += ch
            else:
                self.write_log('no more data from client')
                return "client_close"
        ll = int(l)

        # Then read the right amount of characters from the socket
        cmd = ""
        for i in range(ll):
            ch = self.connection.recv(1)
            if ch:
                cmd += ch
            else:
                self.write_log('no more data from client')
                return "client_close"

        return cmd

    def send_answer(self,answer):

        if len(answer)<100000:
            self.write_log("Sending answer "+answer)
            self.connection.sendall("%5.5d"%len(answer)+answer)
        else:
            self.write_log('answer too long: cannot send')

    def get_board_config(self,brdid):
        if brdid in self.run.boardid_list:
            return self.run.adcboard_list[self.run.boardid_list.index(brdid)].format_config()
        else:
            return "ERROR: board id "+str(brdid)+" does not exist"

    def get_board_log_file(self,brdid):
        if brdid in self.run.boardid_list:
            return self.run.adcboard_list[self.run.boardid_list.index(brdid)].log_file
        else:
            return "ERROR: board id "+str(brdid)+" does not exist"

    def read_setup_list(self):

        # Get list of available setups
        setup_main_dir = "setup"
        setups = []
        for top,dirs,files in os.walk(setup_main_dir):
            if (top == setup_main_dir):
                for setup_dir in dirs: setups.append(setup_dir)
        setups.sort()
        return setups

    def get_setup_list(self):

        return str(self.read_setup_list())

    def change_setup(self,setup):

        # Check if requested setup is known
        if not (setup in self.read_setup_list()):
            self.write_log("change_setup - ERROR: request to set unknown setup "+setup)
            return "error"

        # Change (or reload) setup
        if (setup==self.run.setup):
            self.write_log("change_setup - reloading setup "+setup)
        else:
            self.write_log("change_setup - changing setup from "+self.run.setup+" to "+setup)
        self.run.change_setup(setup)

        return setup

    def new_run(self):

        # Retrieve run number - next=next run from DB, dummy=dummy run (i.e. run nr=0)
        # Return run number used (0 for dummy run) or "error" for invalid answer
        newrun_number = 0
        self.send_answer("run_number")
        ans = self.get_command()
        if (ans=="next"):
            newrun_number = self.db.get_last_run_in_db()+1
        elif (ans=="dummy"):
            newrun_number = 0
        elif (ans=="client_close"):
            return "client_close"
        else:
            self.write_log("run_number - invalid option "+ans+" received")
            self.send_answer("error")
            return "error"
        #self.send_answer(str(newrun_number))

        # Retrieve run type (TEST,DAQ,COSMIC)
        # Return run type used or "error" for invalid answer
        newrun_type = ""
        self.send_answer("run_type")
        ans = self.get_command()
        if (ans=="TEST" or ans=="DAQ" or ans=="COSMIC"):
            newrun_type = ans
        elif (ans=="client_close"):
            return "client_close"
        else:
            self.write_log("run_type - invalid option "+ans+" received")
            self.send_answer("error")
            return "error"
        #self.send_answer(newrun_type)

        newrun_user = ""
        self.send_answer("shift_crew")
        ans = self.get_command()
        if (ans=="client_close"): return "client_close"
        newrun_user = ans

        newrun_comment = ""
        self.send_answer("run_comment")
        ans = self.get_command()
        if (ans=="client_close"): return "client_close"
        newrun_comment = ans

        self.write_log("Run number: "+str(newrun_number))
        self.write_log("Run type: "+newrun_type)
        self.write_log("Run crew: "+newrun_user)
        self.write_log("Run comment: "+newrun_comment)

        # Set run configuration according to user's request
        self.run.change_run(newrun_number)
        self.run.run_type = newrun_type
        self.run.run_user = newrun_user
        self.run.run_comment = newrun_comment

        # Check if requested run number was not used before
        # Saves the day if more than one RunControl program is running at the same time (DON'T DO THAT!!!)
        if (self.run.run_number):
            run_is_in_db = self.db.is_run_in_db(self.run.run_number)
            if (run_is_in_db):
                self.write_log("ERROR - Run "+str(self.run.run_number)+" is already in the DB: cannot use it again")
                self.write_log("Please check if someone else is using this RunControl before retrying")
                self.send_answer("error_init")
                return "error"

        # Create run structure in the DB
        self.write_log("Initializing Run "+str(self.run.run_number))
        self.run.create_run()
        if (self.run.run_number): self.db.set_run_time_init(self.run.run_number,self.now_str())

        # Create directory to host log files
        self.write_log("Creating log directory "+self.run.log_dir)
        self.run.create_log_dir()

        # Write run and boards configuration files
        self.write_log("Writing configuration file "+self.run.config_file)
        self.run.write_config()
        for adc in (self.run.adcboard_list):
            self.write_log("Writing configuration file "+adc.config_file)
            adc.write_config()

        # Start DAQ for all boards
        self.send_answer("start_init")
        for adc in (self.run.adcboard_list):

            p_id = adc.start_daq()
            if p_id:
                self.write_log("ADC board %02d - Started DAQ with process id %d"%(adc.board_id,p_id))
                self.send_answer("adc "+str(adc.board_id)+" init")
                adc.status = "init"
            else:
                self.write_log("ADC board %02d - ERROR: could not start DAQ"%adc.board_id)
                self.send_answer("adc "+str(adc.board_id)+" fail")
                adc.status = "fail"

        # Wait for all boards to finish initialization
        n_try = 0
        while(1):
            all_boards_init = 1
            all_boards_ready = 1
            for adc in (self.run.adcboard_list):
                # Check if any board changed status
                if (adc.status == "init"):
                    if (os.path.exists(adc.initok_file)):
                        # Initialization ended OK
                        self.write_log("ADC board %02d - Initialized and ready for DAQ"%adc.board_id)
                        self.send_answer("adc "+str(adc.board_id)+" ready")
                        adc.status = "ready"
                    elif (os.path.exists(adc.initfail_file)):
                        # Problem during initialization
                        self.write_log("ADC board %02d - *** Initialization failed ***"%adc.board_id)
                        self.send_answer("adc "+str(adc.board_id)+" fail")
                        adc.status = "fail"
                    else:
                        # This board is still initializing
                        all_boards_init = 0
                # Check if any board is in fail status
                if (adc.status == "fail"): all_boards_ready = 0

            if (all_boards_init == 0):
                # Some boards are still initializing: keep waiting
                n_try += 1
                if (n_try>=10):
                    self.write_log("*** ERROR *** One or more boards did not initialize within 10sec. Cannot start run")
                    if (self.run.run_number): self.db.set_run_status(self.run.run_number,5) # Status 5: run with problems at initialization
                    self.send_answer("init_timeout")
                    return "error"
                time.sleep(1)
            elif (all_boards_ready):
                self.write_log("All boards completed initialization: DAQ run can be started")
                if (self.run.run_number): self.db.set_run_status(self.run.run_number,1) # Status 1: run correctly initialized
                self.send_answer("init_ready")
                return "initialized"
            else:
                self.write_log("*** ERROR *** One or more boards failed the initialization. Cannot start run")
                if (self.run.run_number): self.db.set_run_status(self.run.run_number,5) # Status 5: run with problems at initialization
                self.send_answer("init_fail")
                return "initfail"

    def start_run(self):

        self.write_log("Starting run")
        if (self.run.run_number):
            self.db.set_run_time_start(self.run.run_number,self.now_str())
            self.db.set_run_status(self.run.run_number,2) # Status 2: run started

        # Create "start the run" tag file
        open(self.run.start_file,'w').close()

        self.send_answer("run_started")

        # RunControl is now in "running" mode
        return "running"

    def stop_run(self):

        self.send_answer("run_comment_end")
        ans = self.get_command()
        if (ans=="client_close"): return "client_close"
        self.write_log("End of Run comment: "+ans)
        self.run.run_comment_end = ans

        self.write_log("Stopping run")
        if (self.run.run_number): self.db.set_run_status(self.run.run_number,3) # Status 3: run stopped normally

        return self.terminate_run()

    def abort_run(self):

        self.run.run_comment_end = "Run aborted"

        self.write_log("Aborting run")
        if (self.run.run_number): self.db.set_run_status(self.run.run_number,4) # Status 4: run aborted

        return self.terminate_run()

    def terminate_run(self):

        if (self.run.run_number):
            self.db.set_run_time_stop(self.run.run_number,self.now_str())
            self.db.set_run_comment_end(self.run.run_number,self.run.run_end_comment)

        # Create "stop the run" tag file
        open(self.run.quit_file,'w').close()

        # Run stop_daq procedure for each ADC board
        terminate_ok = True
        for adc in (self.run.adcboard_list):
            if adc.stop_daq():
                self.send_answer("adc %d terminate_ok"%adc.board_id)
                self.write_log("ADC board %02d - Terminated correctly"%adc.board_id)
            else:
                terminate_ok = False
                self.send_answer("adc %d terminate_error"%adc.board_id)
                self.write_log("ADC board %02d - WARNING: problems while terminating DAQ"%adc.board_id)
                if (self.run.run_number): self.db.set_run_status(self.run.run_number,6) # Status 6: run ended with errors

        # Clean up run directory
        for adc in (self.run.adcboard_list):
            if (os.path.exists(adc.initok_file)):   os.remove(adc.initok_file)
            if (os.path.exists(adc.initfail_file)): os.remove(adc.initfail_file)
        if(os.path.exists(self.run.start_file)): os.remove(self.run.start_file)
        if(os.path.exists(self.run.quit_file)):  os.remove(self.run.quit_file)

        if terminate_ok:
            self.send_answer("terminate_ok")
        else:
            self.send_answer("terminate_error")

        # At the end of this procedure RunControl is back to "idle" mode
        return "idle"

    def now_str(self): return time.strftime("%Y-%m-%d %H:%M:%S",time.localtime())
