import os
from Tkinter import *

from PadmeDB import PadmeDB

class ADCBoardGUI:

    def __init__(self,b_id):

        self.board_id = b_id
        self.b_name = "ADC%02d"%b_id
        self.config = ""
        self.log_file = ""
        self.mode = ""
        self.status = "off"

    def change_status(self):

        if   self.status == "on" : self.close_gui()
        elif self.status == "off": self.open_gui()

    def open_gui(self):

        if self.status == "on": return

        # Initialize main graphic window
        self.root = Tk()
        #self.root.geometry("579x485+500+500")
        self.root.geometry("579x485")
        self.root.configure(background='black')
        self.root.title(self.b_name)
        self.root.grid_columnconfigure(0,weight=1)
        self.root.grid_rowconfigure(0,weight=1)
        self.root.grid_rowconfigure(1,weight=0)

        # Button to quit board configuration window
        self.b_done = Button(self.root,text="Done",fg="red",command=self.close_gui)
        self.b_done.grid(row=1,column=0,sticky=W+E)

        if (self.mode=="cfg"): self.show_cfg()
        if (self.mode=="log"): self.show_log()

        self.status = "on"

    def close_gui(self):

        if self.status == "off": return

        if (self.mode=="log"):
            self.root.after_cancel(self.my_after)
            self.file.close()
        self.root.destroy()

        self.status = "off"

    def show_cfg(self):

        # Frame to show configuration
        #self.f_cfg = Frame(self.root,bg="black")
        #self.f_cfg.grid(row=0,column=0,sticky=NSEW)
        #self.s_cfg = Scrollbar(self.f_cfg)
        #self.w_cfg = Text(self.f_cfg,height=30,width=80)
        #self.s_cfg.pack(side=RIGHT,fill=Y)
        #self.w_cfg.pack(side=LEFT,fill=Y)
        #self.s_cfg.config(command=self.w_cfg.yview)
        #self.w_cfg.config(yscrollcommand=self.s_cfg.set)
        #self.w_cfg.insert(END,"=== Board configuration ===\n")
        #self.w_cfg.insert(END,self.config)

        self.f_cfg = Frame(self.root,bg="black")
        self.f_cfg.grid(row=0,column=0,sticky=NSEW)

        self.w_cfg = Text(self.f_cfg,height=30,width=80)
        self.w_cfg.grid(row=0,column=0)

        self.sx_cfg = Scrollbar(self.f_cfg,orient=HORIZONTAL,command=self.w_cfg.xview)
        self.sx_cfg.grid(row=1,column=0)

        self.sy_cfg = Scrollbar(self.f_cfg,orient=VERTICAL,command=self.w_cfg.yview)
        self.sy_cfg.grid(row=0,column=1)

        self.w_cfg.config(xscrollcommand=self.sx_cfg.set)
        self.w_cfg.config(yscrollcommand=self.sy_cfg.set)
        self.w_cfg.insert(END,"=== Board configuration ===\n")
        self.w_cfg.insert(END,self.config)

    def show_log(self):

        # Frame to show log file
        #self.f_log = Frame(self.root,bg="black")
        #self.f_log.grid(row=0,column=0,sticky=NSEW)
        #self.s_log = Scrollbar(self.f_log)
        #self.w_log = Text(self.f_log,height=30,width=80)
        #self.s_log.pack(side=RIGHT,fill=Y)
        #self.w_log.pack(side=LEFT,fill=Y)
        #self.s_log.config(command=self.w_log.yview)
        #self.w_log.config(yscrollcommand=self.s_log.set)
        #self.w_log.insert(END,"=== Board log file "+self.log_file+" ===\n")

        self.f_log = Frame(self.root,bg="black")
        self.f_log.grid(row=0,column=0,sticky=NSEW)

        self.w_log = Text(self.f_log,height=30,width=80)
        self.w_log.grid(row=0,column=0)

        self.sx_log = Scrollbar(self.f_log,orient=HORIZONTAL,command=self.w_log.xview)
        self.sx_log.grid(row=1,column=0)

        self.sy_log = Scrollbar(self.f_log,orient=VERTICAL,command=self.w_log.yview)
        self.sy_log.grid(row=0,column=1)

        self.w_log.config(xscrollcommand=self.sx_log.set)
        self.w_log.config(yscrollcommand=self.sy_log.set)
        self.w_log.insert(END,"=== Board log file "+self.log_file+" ===\n")

        # Open log file and read current content and size
        if self.log_file:
            self.file = open(self.log_file, 'r')
            data = self.file.read()
            self.size = len(data)
            self.w_log.insert(END, data)
            self.w_log.see(END)
            self.my_after = self.root.after(1000, self.update_log)

    def update_log(self):

        # If log file has grown, show the new content
        if (os.path.getsize(self.log_file) > self.size):
            data = self.file.read()
            self.size = self.size + len(data)
            self.w_log.insert(END, data)
            self.w_log.see(END)
        self.my_after = self.root.after(1000,self.update_log)
