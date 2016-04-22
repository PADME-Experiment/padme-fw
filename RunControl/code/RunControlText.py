import os
import time

from PadmeDB import PadmeDB

class RunControlText:

    def __init__(self, run):

        self.run = run

        # Connect to PadmeDB
        self.db = PadmeDB(self.run.db_file)

        # Show main interface
        self.main_loop()

    def main_loop(self):

        while(1):

            # Show current status
            print "Last Run Number:",self.db.get_last_run_in_db()
            print "Current Setup:",self.run.setup
            print "Active boards:",self.run.boardid_list

            cmd = raw_input('Command:')

            if (cmd=='help'): self.show_help()
            elif (cmd=='quit' or cmd=='exit'): return
            elif (cmd=='change_setup'): self.change_setup()
            elif (cmd=='show_board'): self.show_board()
            elif (cmd=='new_run'): self.new_run()
            elif (cmd=='stop_run'): self.stop_run()
            else: print "Unknown command",cmd

    def show_help(self):

        print """
List of available commands
change_setup    change current setup
show_board      show board configuration
new_run         start a new run
stop_run        stop current run
quit/exit       exit this program
help            show this help
"""

    def show_board(self):

        s_brd = raw_input('Board id ('+str(self.run.boardid_list)+'):')
        if (s_brd==''): return
        brdid = int(s_brd)
        if brdid in self.run.boardid_list:
            self.run.adcboard_list[self.run.boardid_list.index(brdid)].print_config()
        else:
            print "ERROR: board id",s_brd,"does not exist"

    def new_run(self):

        while(1):
            newrun_number = self.db.get_last_run_in_db()+1
            ans = raw_input("Next run is "+str(self.newrun_number)+". OK to confim, D for dummy run:")
            if (ans=="D"):
                print "Run will be dummy, i.e. not logged into DB"
                self.newrun_number = 0
                break
            elif (ans=="OK"):
                print "Run number set to",newrun_number
                break
            elif (ans==""): return
            else: print "Invalid option",ans,"- Try again (RETURN to exit)"

        while(1):
            ans = raw_input("Run type - T(EST),D(AQ),C(OSMIC):")
            if (ans=="T"):
                newrun_type = "TEST"
                break
            elif (ans=="D"):
                newrun_type = "DAQ"
                break
            elif (ans=="C"):
                newrun_type = "COSMIC"
                break
            elif (ans==""): return
            else: print "Invalid option",ans,"- Try again (RETURN to exit)"

        newrun_user = raw_input("Shift crew:")
        newrun_comment = raw_input("Start of run comment:")

        print "Run number:",newrun_number
        print "Run type:",newrun_type
        print "Run crew:",newrun_user
        print "Run comment:",newrun_comment
        while(1):
            ans = raw_input("OK to confirm or RETURN to exit:")
            if (ans==""): return
            elif (ans=="OK"): break
            else: print "Invalid option",ans,"- Try again"

        # Set run configuration according to user's request
        self.run.change_run(newrun_number)
        self.run.run_type = newrun_type
        self.run.run_user = newrun_user
        self.run.run_comment = newrun_comment

        # Check if requested run number was not used before
        # Saves the day if someone is using this program from somewhere else (DON'T DO THAT!!!)
        if (self.db.is_run_in_db(self.run.run_number)):
            print "WARNING - Run",self.run.run_number,"is already in the DB: cannot use it again"
            print "Please check if someone else is using the DAQ before retrying"
            return

        # Create run structure in the DB
        print "Initializing Run",self.run.run_number
        self.run.create_run()
        self.db.set_run_time_init(self.run.run_number,self.now_str())

        # Create directory to host log files
        print "Creating log directory",self.run.log_dir
        self.run.create_log_dir()

        # Write run and boards configuration files
        print "Writing configuration file",self.run.config_file
        self.run.write_config()
        for adc in (self.run.adcboard_list):
            print "Writing configuration file ",adc.config_file
            adc.write_config()

        # Start DAQ for all boards
        for adc in (self.run.adcboard_list):

            p_id = adc.start_daq()
            if p_id:
                print "ADC board %02d - Started DAQ with process id"%adc.board_id,p_id
                adc.status = "init"
            else:
                print "ADC board %02d - ERROR: could not start DAQ"%adc.board_id
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
                        print "ADC board %02d - Initialized and ready for DAQ"%adc.board_id
                        adc.status = "ready"
                    elif (os.path.exists(adc.initfail_file)):
                        # Problem during initialization
                        print "ADC board %02d - *** Initialization failed ***"%adc.board_id
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
                    self.show_log("*** ERROR *** One or more boards did not initialize within 10sec. Cannot start run")
                    self.db.set_run_status(self.run.run_number,5) # Status 5: run with problems at initialization
                    return
                sleep(1)
            elif (all_boards_ready):
                # All boards correctly completed initialization
                self.show_log("All boards completed initialization: DAQ run can be started")
                self.db.set_run_status(self.run.run_number,1) # Status 1: run correctly initialized
                break
            else:
                self.show_log("*** ERROR *** One or more boards failed the initialization. Cannot start run")
                self.db.set_run_status(self.run.run_number,5) # Status 5: run with problems at initialization
                return

        # Ask user to confirm start of run
        while(1):
            ans = raw_input("GO to start run, ABORT to abort:")
            if (ans=="ABORT"):
                self.terminate_run("abort")
            elif (ans!="GO"):
                print "Invalid option",ans,"- Try again"

        print "Starting run"
        self.db.set_run_time_start(self.run.run_number,self.now_str())
        self.db.set_run_status(self.run.run_number,2) # Status 2: run started

        # Create "start the run" tag file
        open(self.run.start_file,'w').close()

    def stop_run(self):

        self.run.run_end_comment = raw_input("End of run comment:")
        self.terminate_run("stop")

    def terminate_run(self,mode):

        if (mode=="abort"):
            print "Aborting run"
            self.db.set_run_status(self.run.run_number,4) # Status 4: run aborted
        if (mode=="stop"):
            print "Stopping run"
            self.db.set_run_status(self.run.run_number,3) # Status 3: run stopped normally

        self.db.set_run_time_stop(self.run.run_number,self.now_str())
        self.db.set_run_comment_end(self.run.run_number,self.run.run_end_comment)

        # Create "stop the run" tag file
        open(self.run.quit_file,'w').close()

        # Run stop_daq procedure for each ADC board
        for adc in (self.run.adcboard_list):
            if adc.stop_daq():
                print "ADC board %02d - Terminated correctly"%adc.board_id
            else:
                print "ADC board %02d - WARNING: problems while terminating DAQ"%adc.board_id
                self.db.set_run_status(self.run.run_number,6) # Status 6: run ended with errors

        # Clean up run directory
        for adc in (self.run.adcboard_list):
            if (os.path.exists(adc.initok_file)):   os.remove(adc.initok_file)
            if (os.path.exists(adc.initfail_file)): os.remove(adc.initfail_file)
        if(os.path.exists(self.run.start_file)): os.remove(self.run.start_file)
        if(os.path.exists(self.run.quit_file)):  os.remove(self.run.quit_file)

    def change_setup(self):

        # Get list of available setups
        setup_main_dir = "setup"
        setups = []
        for top,dirs,files in os.walk(setup_main_dir):
            if (top == setup_main_dir):
                for setup_dir in dirs: setups.append(setup_dir)
        setups.sort()

        while(1):
            setup = raw_input('Setup name (\'show\' for available setups):')
            if (setup==''): break
            elif (setup=='show'):
                print "Available setups:"
                for s in setups: print s
            elif (setup in setups):
                if (setup==self.run.setup):
                    print "Reloading setup",setup
                else:
                    print "Changing setup to",setup
                self.run.change_setup(setup)
            else:
                print "Setup '"+setup+"' is not available"

    def now_str(self): return time.strftime("%Y-%m-%d %H:%M:%S",time.localtime())
