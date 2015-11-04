import os
from Tkinter import *
from PIL import Image, ImageTk

from PadmeDB import PadmeDB
from ADCBoardGUI import ADCBoardGUI

class RunControlGUI:

    def __init__(self, run):

        self.run = run

        # Connect to DB
        self.db = PadmeDB(self.run.db_file)

        # Initialize main graphic window
        self.root = Tk()
        self.root.geometry("943x750+300+300")
        #self.root.configure(background='white')
        self.root.configure(background='black')
        self.root.title("Padme DAQ")
        self.root.grid_columnconfigure(0,weight=0)
        self.root.grid_columnconfigure(1,weight=0)
        self.root.grid_rowconfigure(0,weight=0)
        self.root.grid_rowconfigure(1,weight=0)
        self.root.grid_rowconfigure(2,weight=1)

        # Show PADME logo and deathstar
        #logo = PhotoImage(file="pic/Deathstar.gif")
        pic = Image.open("pic/padme-big.png")
        #self.logo = ImageTk.PhotoImage(pic.resize((400,124),Image.ANTIALIAS))
        self.logo = ImageTk.PhotoImage(pic.resize((645,200),Image.ANTIALIAS))
        l_logo = Label(self.root,image=self.logo)
        l_logo.configure(borderwidth=0)
        #l_logo.grid(row=0,column=0,columnspan=2)
        l_logo.grid(row=0,column=1,sticky=W)
        pic2 = Image.open("pic/Deathstar.gif")
        self.logo2 = ImageTk.PhotoImage(pic2.resize((200,200),Image.ANTIALIAS))
        l_logo2 = Label(self.root,image=self.logo2)
        l_logo2.configure(borderwidth=0)
        l_logo2.grid(row=0,column=0,sticky=E)

        # Frame to host user interface
        self.f_main = Frame(self.root,bg="white")
        #self.f_main.grid(row=0,column=0,columnspan=2,rowspan=2,sticky=NW)
        self.f_main.grid(row=1,column=0)

        # Button to show/change run number
        self.b_set_runnr = Button(self.f_main)
        self.b_set_runnr.config(font=("Helvetica",16,"bold"))
        self.b_set_runnr.config(text="Run Number: %6d"%self.run.run_number)
        self.b_set_runnr.config(command=self.set_runnr)
        self.b_set_runnr.grid(row=0,column=0,columnspan=2,sticky=W+E)

        # Button to show/change run type
        self.b_set_runtype = Button(self.f_main)
        self.b_set_runtype.config(font=("Helvetica",16,"bold"))
        self.b_set_runtype.config(text="Run Type: "+self.run.run_type)
        self.b_set_runtype.config(command=self.cycle_runtype)
        self.b_set_runtype.grid(row=1,column=0,columnspan=2,sticky=W+E)

        # Button to show/change run comment
        self.b_set_runcmt = Button(self.f_main)
        self.b_set_runcmt.config(font=("Helvetica",14,"bold"))
        self.b_set_runcmt.config(text=self.run.run_comment)
        self.b_set_runcmt.config(command=self.set_runcomment)
        self.b_set_runcmt.grid(row=2,column=0,columnspan=2,sticky=W+E)

        # Buttons to handle run init,start,abort,stop
        self.b_init_run = Button(self.f_main,text="Init Run",command=self.init_run)
        self.b_init_run.grid(row=3,column=0,sticky=W+E)
        self.b_abort_run = Button(self.f_main,text="Abort Run",command=lambda mode="abort": self.terminate_run(mode))
        self.b_abort_run.grid(row=3,column=1,sticky=W+E)
        self.b_abort_run.config(state=DISABLED)
        self.b_start_run = Button(self.f_main,text="Start Run",command=self.start_run)
        self.b_start_run.grid(row=4,column=0,sticky=W+E)
        self.b_start_run.config(state=DISABLED)
        self.b_stop_run = Button(self.f_main,text="Stop Run",command=lambda mode="stop": self.terminate_run(mode))
        self.b_stop_run.grid(row=4,column=1,sticky=W+E)
        self.b_stop_run.config(state=DISABLED)

        # Button to quit Run Control
        self.b_quit_daq = Button(self.f_main,text="QUIT",fg="red",command=self.quit_daq)
        self.b_quit_daq.grid(row=5,columnspan=2,sticky=W+E)

        # Frame to hold buttons to access boards (6 buttons per row)
        self.n_board_buttons = 36
        n_buttons_per_row = 6
        self.f_boards = Frame(self.root,bg="black")
        self.f_boards.grid(row=1,column=1)

        # Create button to change RunControl setup
        self.b_setup = Button(self.f_boards)
        self.b_setup.config(font=("Helvetica",14,"bold"))
        self.b_setup.config(text="Setup: "+self.run.setup)
        self.b_setup.config(command=self.handle_setup)
        self.b_setup.grid(row=0,column=0,columnspan=n_buttons_per_row,sticky=W+E)
        # Create buttons for each board
        self.b_board = []
        for brd_id in range(0,self.n_board_buttons):
            self.b_board.insert(brd_id,Button(self.f_boards,text="ADC%02d"%brd_id,bg="white"))
            self.b_board[brd_id].grid(row=1+brd_id/n_buttons_per_row,column=brd_id%n_buttons_per_row,sticky=W+E)
        # Configure buttons according to current setup
        self.configure_board_buttons()

        # Frame for log messages
        self.f_log = Frame(self.root,bg="black")
        self.f_log.grid(row=2,column=0,columnspan=2,sticky=NSEW)

        self.s_log = Scrollbar(self.f_log)
        self.w_log = Text(self.f_log,height=20,width=132)
        self.s_log.pack(side=RIGHT,fill=Y)
        self.w_log.pack(side=LEFT,fill=Y)
        self.s_log.config(command=self.w_log.yview)
        self.w_log.config(yscrollcommand=self.s_log.set)
        self.w_log.insert(END,"=== Log window ===\n")

        # Start user interface
        self.root.mainloop()

    def show_board(self,board,mode):

        self.boardgui = ADCBoardGUI(board,mode)

    def show_log(self,text):

        print text
        self.w_log.insert(END,text+"\n")
        self.w_log.see(END)

    def wait_for_init(self):

        all_boards_init = 1
        all_boards_ready = 1
        for adc in (self.run.adcboard_list):
            # Check if any board changed status
            if (adc.status == "init"):
                if (os.path.exists(adc.initok_file)):
                    # Initialization ended OK
                    self.show_log("ADC board "+"%02d"%adc.board_id+" - Initialized and ready for DAQ")
                    self.b_board[adc.board_id].config(background="green")
                    adc.status = "ready"
                elif (os.path.exists(adc.initfail_file)):
                    # Problem during initialization
                    self.show_log("ADC board "+"%02d"%adc.board_id+" - *** Initialization failed ***")
                    self.b_board[adc.board_id].config(background="red")
                    adc.status = "fail"
                else:
                    # This board is still initializing
                    all_boards_init = 0
            # Check if any board is in fail status
            if (adc.status == "fail"): all_boards_ready = 0

        if (all_boards_init == 0):
            # Some boards are still initializing: keep waiting
            self.root.after(1000,self.wait_for_init)
        elif (all_boards_ready):
            # All boards correctly completed initialization
            self.show_log("All boards completed initialization: DAQ run can be started")
            self.b_start_run.config(state=NORMAL)
        else:
            self.show_log("*** ERROR *** One or more boards could not complete initialization. Cannot start run")

    def init_run(self):

        self.show_log("Initializing Run")

        # Check if requested run number was not used before
        if (self.db.is_run_in_db(self.run.run_number)):
            self.show_log("WARNING - Run "+str(self.run.run_number)+" is already in the DB: cannot use it again")
            return

        # Disable init_run button and enable abort_run buttons
        self.b_init_run.config(state=DISABLED)
        self.b_abort_run.config(state=NORMAL)

        self.show_log("Initializing Run")
        self.show_log("Run number\t"+str(self.run.run_number))
        self.show_log("Run type\t"+str(self.run.run_type))
        self.show_log("Run comment\t'"+self.run.run_comment+"'")

        self.show_log("Creating log directory "+self.run.log_dir)
        self.run.create_log_dir()

        self.show_log("Writing configuration file "+self.run.config_file)
        self.run.write_config()
        for adc in (self.run.adcboard_list):
            self.show_log("Writing configuration file "+adc.config_file)
            adc.write_config()

        # Create run structure in the DB
        self.run.create_run()

        # Start DAQ for all boards
        for adc in (self.run.adcboard_list):
            p_id = adc.start_daq()
            if p_id:
                self.show_log("ADC board "+"%02d"%adc.board_id+" - Started DAQ with process id "+str(p_id))
                self.b_board[adc.board_id].config(background="yellow")
                adc.status = "init"
                # Show log file instead of configuration
                self.b_board[adc.board_id].config(command=lambda brd=adc,mode="log": self.show_board(brd,mode))
            else:
                self.show_log("ADC board "+"%02d"%adc.board_id+" - ERROR: could not start DAQ")
                self.b_board[adc.board_id].config(background="red")
                adc.status = "fail"
                                                           
        # Wait for all boards to finish initialization
        self.wait_for_init()

    def start_run(self):

        self.show_log("Starting run")

        # Create "start the run" tag file
        open(self.run.start_file,'w').close()

        # Enable stop_run button and disable all the others
        self.b_init_run.config(state=DISABLED)
        self.b_abort_run.config(state=DISABLED)
        self.b_start_run.config(state=DISABLED)
        self.b_stop_run.config(state=NORMAL)

    def terminate_run(self,mode):

        if (mode=="abort"): self.show_log("Aborting Run")
        if (mode=="stop"):  self.show_log("Stopping Run")

        # Create "stop the run" tag file
        open(self.run.quit_file,'w').close()

        # Run stop_daq procedure for each ADC board
        for adc in (self.run.adcboard_list):
            if adc.stop_daq():
                self.show_log("ADC board "+"%02d"%adc.board_id+" - Terminated correctly")
            else:
                self.show_log("ADC board "+"%02d"%adc.board_id+" - WARNING: problems while terminating DAQ")

        # Clean up run directory
        for adc in (self.run.adcboard_list):
            if (os.path.exists(adc.initok_file)):   os.remove(adc.initok_file)
            if (os.path.exists(adc.initfail_file)): os.remove(adc.initfail_file)
        if(os.path.exists(self.run.start_file)): os.remove(self.run.start_file)
        if(os.path.exists(self.run.quit_file)):  os.remove(self.run.quit_file)

        # Reset ADC buttons color and functionality
        for adc in (self.run.adcboard_list):
            self.b_board[adc.board_id].config(bg="white")
            self.b_board[adc.board_id].config(command=lambda brd=adc,mode="cfg": self.show_board(brd,mode))

        # Enable init_run button and disable all the others
        self.b_init_run.config(state=NORMAL)
        self.b_abort_run.config(state=DISABLED)
        self.b_start_run.config(state=DISABLED)
        self.b_stop_run.config(state=DISABLED)

    def change_run(self,run):

        # Check if requested run number was not used before
        if (self.db.is_run_in_db(run)):
            self.show_log("WARNING - Run "+str(run)+" is already in the DB: cannot use it again")
            return

        self.run.change_run(run)
        self.b_set_runnr.config(text="Run Number:"+"%6d"%self.run.run_number)
        self.show_log("Run Number set to "+str(self.run.run_number))
        self.setrunnr.destroy()

    def dummy_run(self):
        self.change_run(0)

    def db_run(self):
        newrun = self.db.get_last_run_in_db()+1
        self.change_run(newrun)

    def manual_run(self,runnr):
        self.change_run(int(runnr.widget.get()))
    
    def set_runnr(self):
        self.setrunnr = Tk()
        self.setrunnr.geometry("223x105+400+400")
        self.setrunnr.title("Set Run Number")

        l_head = Label(self.setrunnr)
        l_head.config(text="Please choose a run number")
        l_head.config(font=("Helvetica",12,"bold"))
        l_head.config(justify=CENTER)
        l_head.grid(row=0,column=0,columnspan=2)

        b_dummy_run = Button(self.setrunnr,text="Dummy Run",command=self.dummy_run)
        b_dummy_run.grid(row=1,column=0,sticky=W+E)

        b_db_run = Button(self.setrunnr,text="Run from DB",command=self.db_run)
        b_db_run.grid(row=1,column=1,sticky=W+E)

        l_runnr = Label(self.setrunnr,text="Run number:")
        l_runnr.grid(row=2,column=0,sticky=E)
        e_runnr = Entry(self.setrunnr,width=10)
        e_runnr.bind('<Return>',self.manual_run)
        e_runnr.grid(row=2,column=1,sticky=W+E)

        b_close = Button(self.setrunnr,text="Close",command=self.setrunnr.destroy)
        b_close.grid(row=3,column=0,columnspan=2)

    def cycle_runtype(self):
        choices = ['TEST','DAQ','COSMIC']
        n_choices = len(choices)
        for c in range(n_choices):
            if (self.run.run_type == choices[c]):
                self.run.run_type = choices[(c+1)%n_choices]
                break
        self.b_set_runtype.config(text="Run Type: "+self.run.run_type)
        self.show_log("Run Type set to "+self.run.run_type)

    def set_runcomment(self):
        return

    def quit_daq(self):
        print "Quitting. Good bye"
        self.f_main.quit()

    def handle_setup(self):
        self.chgsetup = Tk()
        self.chgsetup.geometry("180x200+400+400")
        self.chgsetup.title("Change RunControl Setup")

        # Get list of available setups
        setup_main_dir = "setup"
        setups = []
        for top,dirs,files in os.walk(setup_main_dir):
            if (top == setup_main_dir):
                for setup_dir in dirs: setups.append(setup_dir)
        setups.sort()

        # Create Listbox to select desired setup
        self.f_setup = Frame(self.chgsetup)
        self.f_setup.grid(row=0,column=0,columnspan=2,sticky=NSEW)

        self.sb_setup = Scrollbar(self.f_setup)
        self.lb_setup = Listbox(self.f_setup)
        self.sb_setup.pack(side=RIGHT,fill=Y)
        self.lb_setup.pack(side=LEFT,fill=Y)
        self.sb_setup.config(command=self.lb_setup.yview)
        self.lb_setup.config(yscrollcommand=self.sb_setup.set)
        self.lb_setup.config(selectmode=BROWSE)
        for setup in setups: self.lb_setup.insert(END,setup)
        self.lb_setup.bind("<Double-Button-1>",self.change_setup_bind)

        # Set current setup as ACTIVE
        for idx in range(0,self.lb_setup.size()):
            if (self.lb_setup.get(idx)==self.run.setup): self.lb_setup.activate(idx)

        # Create button to confirm selection
        b_select = Button(self.chgsetup,text="Select",command=self.change_setup)
        b_select.grid(row=1,column=0)

        # Create button to close window
        b_close = Button(self.chgsetup,text="Close",command=self.chgsetup.destroy)
        b_close.grid(row=1,column=1)

    def change_setup(self):
        items = self.lb_setup.curselection()
        for item in items:
            setup = self.lb_setup.get(int(item))
            if (setup==self.run.setup):
                self.show_log("Reloading setup "+setup)
            else:
                self.show_log("Changing setup to "+setup)
            self.run.change_setup(setup)
            self.b_setup.config(text="Setup: "+self.run.setup)
            self.configure_board_buttons()
        self.chgsetup.destroy()

    def change_setup_bind(self,event): self.change_setup()

    def configure_board_buttons(self):
        # Configure board buttons according to current setup
        for brd_id in range(0,self.n_board_buttons):
            # Enable button only if corresponding board is active in this run
            self.b_board[brd_id].config(state=DISABLED)
            for adcboard in self.run.adcboard_list:
                if (adcboard.board_id == brd_id):
                    # Weird way to assign the same function to different buttons
                    self.b_board[brd_id].config(command=lambda brd=adcboard,mode="cfg": self.show_board(brd,mode))
                    self.b_board[brd_id].config(state=NORMAL)
