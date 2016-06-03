import os
import time
import re
import socket

from Tkinter import *
from PIL import Image, ImageTk

from ADCBoardGUI import ADCBoardGUI

class RunControlGUI:

    def __init__(self):

        # Create useful regular expressions
        self.re_board_list = re.compile("^\[((\d+\, )*\d+)\]$")
        #self.re_board_list = re.compile("^\[(.*)\]$")
        self.re_get_adc_init_status = re.compile("^adc (\d+) (\w+)$")
        self.re_get_adc_terminate_status = re.compile("^adc (\d+) (\w+)$")

        # Create a TCP/IP socket
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        # Connect the socket to the port where the server is listening
        server_address = ('localhost', 10000)
        print 'Connecting to RunControl server on host %s port %s' % server_address
        try:
            self.sock.connect(server_address)
        except:
            print "Error connecting to server: "+str(sys.exc_info()[0])
            return

        # Create main GUI window
        self.init_gui()

        # Ask server about current situation (state, run number and list of active boards)
        self.current_state = self.ask_server("get_state")
        self.current_run_nr = int(self.ask_server("get_run_number"))
        self.current_board_list = self.decode_board_list(self.ask_server("get_board_list"))
        self.current_setup = self.ask_server("get_setup")

        # Enable/disable buttons according to current state
        self.set_gui_state()
        self.configure_board_buttons()

        # Start user interface
        self.root.mainloop()

    def init_gui(self):

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
        pic_dir = os.getenv('PADME',".")+"/RunControl/pic/"
        #logo = PhotoImage(file="pic/Deathstar.gif")
        pic = Image.open(pic_dir+"padme-big.png")
        #self.logo = ImageTk.PhotoImage(pic.resize((400,124),Image.ANTIALIAS))
        self.logo = ImageTk.PhotoImage(pic.resize((645,200),Image.ANTIALIAS))
        l_logo = Label(self.root,image=self.logo)
        l_logo.configure(borderwidth=0)
        #l_logo.grid(row=0,column=0,columnspan=2)
        l_logo.grid(row=0,column=1,sticky=W)
        pic2 = Image.open(pic_dir+"Deathstar.gif")
        self.logo2 = ImageTk.PhotoImage(pic2.resize((200,200),Image.ANTIALIAS))
        l_logo2 = Label(self.root,image=self.logo2)
        l_logo2.configure(borderwidth=0)
        l_logo2.grid(row=0,column=0,sticky=E)

        # Frame to host user interface
        self.f_main = Frame(self.root,bg="white")
        #self.f_main.grid(row=0,column=0,columnspan=2,rowspan=2,sticky=NW)
        self.f_main.grid(row=1,column=0)

        # Button to show/change run number
        #self.b_set_runnr = Button(self.f_main)
        #self.b_set_runnr.config(font=("Helvetica",16,"bold"))
        #self.b_set_runnr.grid(row=0,column=0,columnspan=2,sticky=W+E)

        # Label to show run number
        self.l_run_number = Label(self.f_main)
        self.l_run_number.config(font=("Helvetica",16,"bold"))
        self.l_run_number.grid(row=0,column=0,columnspan=2,sticky=W+E)

        # Label to show run state
        self.l_run_state = Label(self.f_main)
        self.l_run_state.config(font=("Helvetica",16,"bold"))
        self.l_run_state.grid(row=1,column=0,columnspan=2,sticky=W+E)

        # Buttons to handle run init,start,abort,stop

        self.b_init_run = Button(self.f_main,text="New Run",command=self.set_run_config)
        self.b_init_run.config(font=("Helvetica",14,"bold"))
        self.b_init_run.grid(row=2,column=0,sticky=W+E)

        self.b_abort_run = Button(self.f_main,text="Abort Run",command=self.abort_run)
        self.b_abort_run.config(font=("Helvetica",14,"bold"))
        self.b_abort_run.grid(row=2,column=1,sticky=W+E)

        self.b_start_run = Button(self.f_main,text="Start Run",command=self.start_run)
        self.b_start_run.config(font=("Helvetica",14,"bold"))
        self.b_start_run.grid(row=3,column=0,sticky=W+E)

        self.b_stop_run = Button(self.f_main,text="Stop Run",command=self.set_end_of_run)
        self.b_stop_run.config(font=("Helvetica",14,"bold"))
        self.b_stop_run.grid(row=3,column=1,sticky=W+E)

        # Button to quit Run Control
        self.b_quit_daq = Button(self.f_main,text="QUIT",fg="red",command=self.quit_daq)
        self.b_quit_daq.grid(row=4,columnspan=2,sticky=W+E)
        self.b_quit_daq.config(state=NORMAL)

        # Frame to hold buttons to access boards (6 buttons per row)
        self.n_board_buttons = 36
        n_buttons_per_row = 6
        self.f_boards = Frame(self.root,bg="black")
        self.f_boards.grid(row=1,column=1)

        # Create button to change RunControl setup
        self.b_setup = Button(self.f_boards)
        self.b_setup.config(font=("Helvetica",14,"bold"))
        self.b_setup.config(command=self.handle_setup)
        self.b_setup.grid(row=0,column=0,columnspan=n_buttons_per_row,sticky=W+E)

        # Create buttons for each board
        self.b_board = []
        self.boardgui = []
        for brd_id in range(0,self.n_board_buttons):
            # Create board GUI handler
            self.boardgui.append(ADCBoardGUI(brd_id))
            # Create button
            self.b_board.append(Button(self.f_boards,text="ADC%02d"%brd_id,bg="white"))
            self.b_board[brd_id].grid(row=1+brd_id/n_buttons_per_row,column=brd_id%n_buttons_per_row,sticky=W+E)
            self.b_board[brd_id].config(command=self.boardgui[brd_id].change_status)

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

    def get_answer(self):

        # First get length of string
        l = ""
        for i in range(5): # Max 99999 characters
            ch = self.sock.recv(1)
            if ch:
                l += ch
            else:
                self.error_exit("Server closed connection")
        ll = int(l)

        # Then read the right amount of characters from the socket
        ans = ""
        for i in range(ll):
            ch = self.sock.recv(1)
            if ch:
                ans += ch
            else:
                self.error_exit("Server closed connection")

        return ans

    def send_command(self,cmd):

        if len(cmd)<100000:
            self.sock.sendall("%5.5d"%len(cmd)+cmd)
        else:
            self.error_exit("Command too long: cannot send")

    def ask_server(self,cmd):

        self.send_command(cmd)
        return self.get_answer()

    def show_log(self,text):

        txt = self.now_str()+" "+text
        print txt
        self.w_log.insert(END,txt+"\n")
        self.w_log.see(END)

    def error_exit(self,msg):

        errexit_root = Tk()
        errexit_root.geometry("600x350")
        errexit_root.title("Error Exit")

        l_errexit_head = Label(errexit_root)
        l_errexit_head.config(text="*** ERROR ***\n"+msg)
        l_errexit_head.config(font=("Helvetica",24,"bold"))
        l_errexit_head.config(justify=CENTER)
        l_errexit_head.grid(row=0,column=0)

        # Button to quit Run Control
        b_errexit_quit = Button(self.f_main,text="QUIT",fg="red",command=self.quit_daq)
        b_errexit_quit.grid(row=1,column=0)
        b_errexit_quit.config(state=NORMAL)

    def set_gui_state(self):

        self.l_run_number.config(text="Run Number: %s"%self.ask_server("get_run_number"))
        self.l_run_state.config(text="Run State: %s"%self.current_state)
        self.b_setup.config(text="Setup: %s"%self.current_setup)

        if (self.current_state == "idle"):

            self.b_init_run.config(state=NORMAL)
            self.b_abort_run.config(state=DISABLED)
            self.b_start_run.config(state=DISABLED)
            self.b_stop_run.config(state=DISABLED)
            self.b_quit_daq.config(state=NORMAL)
            self.b_setup.config(state=NORMAL)

        elif (self.current_state == "initializing"):

            self.b_init_run.config(state=DISABLED)
            self.b_abort_run.config(state=DISABLED)
            self.b_start_run.config(state=DISABLED)
            self.b_stop_run.config(state=DISABLED)
            self.b_quit_daq.config(state=DISABLED)
            self.b_setup.config(state=DISABLED)

        elif (self.current_state == "terminating"):

            self.b_init_run.config(state=DISABLED)
            self.b_abort_run.config(state=DISABLED)
            self.b_start_run.config(state=DISABLED)
            self.b_stop_run.config(state=DISABLED)
            self.b_quit_daq.config(state=DISABLED)
            self.b_setup.config(state=DISABLED)

        elif (self.current_state == "initialized"):

            self.b_init_run.config(state=DISABLED)
            self.b_abort_run.config(state=NORMAL)
            self.b_start_run.config(state=NORMAL)
            self.b_stop_run.config(state=DISABLED)
            self.b_quit_daq.config(state=NORMAL)
            self.b_setup.config(state=DISABLED)

        elif (self.current_state == "running"):

            self.b_init_run.config(state=DISABLED)
            self.b_abort_run.config(state=DISABLED)
            self.b_start_run.config(state=DISABLED)
            self.b_stop_run.config(state=NORMAL)
            self.b_quit_daq.config(state=NORMAL)
            self.b_setup.config(state=DISABLED)

        elif (self.current_state == "initfail"):

            self.b_init_run.config(state=NORMAL)
            self.b_abort_run.config(state=DISABLED)
            self.b_start_run.config(state=DISABLED)
            self.b_stop_run.config(state=DISABLED)
            self.b_quit_daq.config(state=NORMAL)
            self.b_setup.config(state=DISABLED)

        else:

            self.show_log("Server is in UNKNOWN state %s"%self.current_state)
            self.b_init_run.config(state=DISABLED)
            self.b_abort_run.config(state=DISABLED)
            self.b_start_run.config(state=DISABLED)
            self.b_stop_run.config(state=DISABLED)
            self.b_quit_daq.config(state=NORMAL)
            self.b_setup.config(state=DISABLED)

    def set_run_config(self):

        self.current_state = "initializing"
        self.set_gui_state()

        self.runcfg_root = Tk()
        self.runcfg_root.geometry("600x350")
        self.runcfg_root.title("Set Run Configuration")

        # Define default next run settings
        self.runcfg_nextrun = "dummy"
        self.runcfg_nextrunnr = 0
        self.runcfg_nexttype = "TEST"

        # Show current run number
        self.l_runcfg_head = Label(self.runcfg_root)
        self.l_runcfg_head.config(text="Run %s nr %d"%(self.runcfg_nextrun,self.runcfg_nextrunnr))
        self.l_runcfg_head.config(font=("Helvetica",24,"bold"))
        self.l_runcfg_head.config(justify=CENTER)
        self.l_runcfg_head.grid(row=0,column=0,columnspan=6)

        # Button to show/change run type
        self.b_set_runtype = Button(self.runcfg_root)
        self.b_set_runtype.config(font=("Helvetica",16,"bold"))
        self.b_set_runtype.config(text="Type: %s"%self.runcfg_nexttype)
        self.b_set_runtype.config(command=self.cycle_runtype)
        self.b_set_runtype.grid(row=1,column=0,columnspan=2,sticky=W+E)

        b_db_run = Button(self.runcfg_root,text="Run from DB",font=("Helvetica",16,"bold"),command=self.runcfg_db_run)
        b_db_run.grid(row=1,column=2,columnspan=2,sticky=W+E)

        b_dummy_run = Button(self.runcfg_root,text="Dummy Run",font=("Helvetica",16,"bold"),command=self.runcfg_dummy_run)
        b_dummy_run.grid(row=1,column=4,columnspan=2,sticky=W+E)

        l_runcrew = Label(self.runcfg_root,text="Shift crew:",font=("Helvetica",14,"bold"))
        l_runcrew.grid(row=2,column=0,sticky=E)
        self.e_runcfg_runcrew = Entry(self.runcfg_root,width=20)
        self.e_runcfg_runcrew.grid(row=2,column=1,columnspan=5,sticky=W+E)

        l_runcomment = Label(self.runcfg_root,text="Start of Run Comment",font=("Helvetica",16,"bold"))
        l_runcomment.grid(row=3,column=0,columnspan=6,sticky=W+E)
        self.e_runcfg_runcomment = Text(self.runcfg_root,height=8,width=80)
        self.e_runcfg_runcomment.grid(row=4,column=0,columnspan=6,sticky=W+E)

        b_init = Button(self.runcfg_root,text="Init Run",font=("Helvetica",16,"bold"),command=self.runcfg_initrun)
        b_init.grid(row=5,column=0,columnspan=3)

        b_cancel = Button(self.runcfg_root,text="Cancel",font=("Helvetica",16,"bold"),command=self.runcfg_cancel)
        b_cancel.grid(row=5,column=3,columnspan=3)

    def runcfg_initrun(self):

        # Tell server we want to initialize a new run
        self.send_command("new_run")

        ans = self.get_answer()
        if (ans != "run_number"):
            self.error_exit("runcfg_initrun - Protocol error\nExpected message run_number, received %s"%ans)

        # Tell server how to set run number (dummy or next)
        self.send_command(self.runcfg_nextrun)
        self.show_log("Run Number set to %s"%self.get_answer())
  
        ans = self.get_answer()
        if (ans != "run_type"):
            self.error_exit("runcfg_initrun - Protocol error\nExpected message run_type, received %s"%ans)
   
        # Tell server the run type (TEST,DAQ,COSMIC)
        self.send_command(self.runcfg_nexttype)
        self.show_log("Run Type set to %s"%self.get_answer())
  
        ans = self.get_answer()
        if (ans != "shift_crew"):
            self.error_exit("runcfg_initrun - Protocol error\nExpected message shift_crew, received %s"%ans)
     
        # Tell server the shift crew
        shift_crew = self.e_runcfg_runcrew.get()
        self.send_command(shift_crew)
        self.show_log("Run Crew set to %s"%shift_crew)
  
        ans = self.get_answer()
        if (ans != "run_comment"):
            self.error_exit("runcfg_initrun - Protocol error\nExpected message run_comment, received %s"%ans)

        # Tell server the start of run comment
        run_comment = self.e_runcfg_runcomment.get("1.0","end-1c")
        self.send_command(run_comment)
        self.show_log("Run Comment set to %s"%run_comment)

        self.runcfg_root.destroy()

        self.init_run()

    def runcfg_cancel(self):

        self.current_state = "idle"
        self.set_gui_state()

        self.runcfg_root.destroy()

    def runcfg_dummy_run(self):

        self.runcfg_nextrun = "dummy"
        self.runcfg_nextrunnr = 0
        self.l_runcfg_head.config(text="Run %s nr %d"%(self.runcfg_nextrun,self.runcfg_nextrunnr))

    def runcfg_db_run(self):

        self.runcfg_nextrun = "next"
        self.runcfg_nextrunnr = int(self.ask_server("get_run_number"))+1
        self.l_runcfg_head.config(text="Run %s nr %d"%(self.runcfg_nextrun,self.runcfg_nextrunnr))

    def set_end_of_run(self):

        self.show_log("Stopping run")

        self.current_state = "terminating"
        self.set_gui_state()

        self.endrun_root = Tk()
        self.endrun_root.geometry("600x350")
        self.endrun_root.title("Set End of Run Comment")

        self.l_endrun_head = Label(self.endrun_root)
        self.l_endrun_head.config(text="Run %s"%self.ask_server("get_run_number"))
        self.l_endrun_head.config(font=("Helvetica",24,"bold"))
        self.l_endrun_head.config(justify=CENTER)
        self.l_endrun_head.grid(row=0,column=0,columnspan=2)

        l_endcomment = Label(self.endrun_root,text="End of Run Comment",font=("Helvetica",16,"bold"))
        l_endcomment.grid(row=1,column=0,columnspan=2,sticky=W+E)
        self.e_endrun_comment = Text(self.endrun_root,height=8,width=80)
        self.e_endrun_comment.grid(row=2,column=0,columnspan=2,sticky=W+E)

        b_stop = Button(self.endrun_root,text="Stop Run",font=("Helvetica",16,"bold"),command=self.endrun_stoprun)
        b_stop.grid(row=3,column=0)

        b_cancel = Button(self.endrun_root,text="Cancel",font=("Helvetica",16,"bold"),command=self.endrun_cancel)
        b_cancel.grid(row=3,column=1)

    def endrun_stoprun(self):

        self.endrun_comment = self.e_endrun_comment.get("1.0","end-1c")
        self.show_log("End of Run Comment set to %s"%self.endrun_comment)

        self.endrun_root.destroy()

        self.stop_run()

    def endrun_cancel(self):

        self.current_state = "running"
        self.set_gui_state()

        self.endrun_root.destroy()

    def init_run(self):

        ans = self.get_answer()
        if (ans == "error_init"):
            self.error_exit("init_run - Initialization error\nAn error occourred while starting initialization")
        elif (ans != "start_init"):
            self.error_exit("init_run - Protocol error\nExpected message start_init or error_init, received %s"%ans)

        self.wait_for_init()

    def wait_for_init(self):

        ans = self.get_answer()
        if (ans == "init_timeout"):
            self.current_state = "initfail"
            self.set_gui_state()
            self.show_log("*** ERROR *** One or more boards timed out during initialization. Cannot start run")
        elif (ans == "init_fail"):
            self.current_state = "initfail"
            self.set_gui_state()
            self.show_log("*** ERROR *** One or more boards failed initialization. Cannot start run")
        elif (ans == "init_ready"):
            self.current_state = "initialized"
            self.set_gui_state()
            self.configure_board_buttons()
            self.show_log("All boards completed initialization: DAQ run can be started")
        else:
            m = self.re_get_adc_init_status.match(ans)
            if (m):
                brd_id = int(m.group(1))
                brd_status = m.group(2)
                if (brd_status == "init"):
                    self.show_log("ADC board %02d - Starting initialization"%brd_id)
                    self.b_board[brd_id].config(background="yellow")
                elif (brd_status == "ready"):
                    self.show_log("ADC board %02d - Initialized and ready for DAQ"%brd_id)
                    self.b_board[brd_id].config(background="green")
                elif (brd_status == "fail"):
                    self.show_log("ADC board %02d - *** Initialization failed ***"%brd_id)
                    self.b_board[brd_id].config(background="red")
                else:
                    self.error_exit("init_run - Protocol error\nReceived unknown initialization status %s for board %02d"%(brd_status,brd_id))
            else:
                self.error_exit("init_run - Protocol error\nExpected message \"init_<status>\" or \"adc <id> <status>\", received %s"%ans)
            self.root.after(100,self.wait_for_init)

    def start_run(self):

        self.show_log("Starting run")

        # Tell server to start the run
        self.send_command("start_run")
        ans = self.get_answer()
        if (ans != "run_started"):
            self.error_exit("start_run - Protocol error\nExpected message run_started, received %s"%ans)

        self.show_log("Run started")

        self.current_state = "running"
        self.set_gui_state()

    def abort_run(self):

        self.show_log("Aborting run")

        self.current_state = "terminating"
        self.set_gui_state()

        # Tell server we want to abort the run
        self.send_command("abort_run")
        self.wait_for_terminate()

    def stop_run(self):

        self.show_log("Stopping run")

        # Tell server we want to stop the run
        self.send_command("stop_run")

        ans = self.get_answer()
        if (ans != "run_comment_end"):
            self.error_exit("stop_run - Protocol error\nExpected message run_comment_end, received %s"%ans)

        # Tell server the end of run comment
        self.send_command(self.endrun_comment)

        # Handle the termination procedure
        self.wait_for_terminate()

    def wait_for_terminate(self):

        ans = self.get_answer()
        if ( (ans == "terminate_ok") or (ans == "terminate_error") ):

            if (ans == "terminate_ok"):
                self.show_log("Run terminated correctly")
            elif (ans == "terminate_error"):
                self.show_log("Run terminated with errors")

            # Reset idle state
            self.current_state = "idle"
            self.set_gui_state()
            self.configure_board_buttons()

        else:

            m = self.re_get_adc_terminate_status.match(ans)
            if (m):
                brd_id = int(m.group(1))
                brd_status = m.group(2)
                if (brd_status == "terminate_ok"):
                    self.show_log("ADC board %02d - Terminated correctly"%brd_id)
                    self.b_board[brd_id].config(background="white")
                elif (brd_status == "terminate_error"):
                    self.show_log("ADC board %02d - *** Termination ERROR ***"%brd_id)
                    self.b_board[brd_id].config(background="red")
                else:
                    self.error_exit("wait_for_terminate - Protocol error\nReceived unknown initialization status %s for board %02d"%(brd_status,brd_id))
            else:
                self.error_exit("""wait_for_terminate - Protocol error
Expected messages \"terminate_(ok|error)\" or "adc <b> <status>
Received message \"%s\" """%ans)
            self.root.after(100,self.wait_for_terminate)

    def change_run(self,run):

        # Check if requested run number was not used before
        if (self.db.is_run_in_db(run)):
            self.show_log("WARNING - Run "+str(run)+" is already in the DB: cannot use it again")
            return

        self.run.change_run(run)
        self.b_set_runnr.config(text="Run Number:"+"%6d"%self.run.run_number)
        self.show_log("Run Number set to "+str(self.run.run_number))
        self.setrunnr.destroy()

    def cycle_runtype(self):
        choices = ['TEST','DAQ','COSMIC']
        n_choices = len(choices)
        for c in range(n_choices):
            if (self.runcfg_nexttype == choices[c]):
                self.runcfg_nexttype = choices[(c+1)%n_choices]
                break
        self.b_set_runtype.config(text="Type: %s"%self.runcfg_nexttype)
        self.show_log("Next Run Type set to %s"%self.runcfg_nexttype)

    #def set_runcomment(self):
        #return

    def quit_daq(self):

        print "Closing socket"
        self.sock.close()
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
            if (self.lb_setup.get(idx)==self.current_setup): self.lb_setup.activate(idx)

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
            if (setup==self.current_setup):
                self.show_log("Reloading setup "+setup)
            else:
                self.show_log("Changing setup to "+setup)
            ans = self.ask_server("change_setup %s"%setup)
            if ( ans == setup ):
                self.current_setup = setup
                self.b_setup.config(text="Setup: %s"%self.current_setup)
                self.current_board_list = self.decode_board_list(self.ask_server("get_board_list"))
                self.configure_board_buttons()
                self.show_log("Setup is now %s"%self.current_setup)
            elif ( ans == "error" ):
                self.show_log("Error changing setup to %s - Still using setup %s"%(setup,self.current_setup))
            else:
                self.show_log("Server returned unexpected message %s - Still using setup %s"%(ans,self.current_setup))
                
        self.chgsetup.destroy()

    def change_setup_bind(self,event): self.change_setup()

    def configure_board_buttons(self):

        # Configure board buttons according to current setup
        for brd_id in range(0,self.n_board_buttons):
            # Make sure GUI window is closed
            self.boardgui[brd_id].close_gui()
            # Enable button only if corresponding board is active in this run
            self.b_board[brd_id].config(state=DISABLED)
            for curbrd_id in self.current_board_list:
                if (curbrd_id == brd_id):
                    self.b_board[brd_id].config(state=NORMAL)
                    if (self.current_state == "idle"):
                        self.boardgui[brd_id].mode = "cfg"
                        self.boardgui[brd_id].config = self.ask_server("get_board_config %d"%brd_id)
                    else:
                        self.boardgui[brd_id].mode = "log"
                        self.boardgui[brd_id].log_file = self.ask_server("get_board_log_file %d"%brd_id)
                        self.b_board[brd_id].config(background="green")

    def decode_board_list(self,list_str):

        b_list = ()
        m = self.re_board_list.match(list_str)
        if (m): b_list = map(int,m.group(1).split(", "))
        return b_list

    def now_str(self): return time.strftime("%Y-%m-%d %H:%M:%S",time.localtime())

