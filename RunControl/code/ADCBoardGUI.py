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
        self.root.geometry("400x600")
        self.root.configure(background='grey')
        self.root.title(self.b_name)
        self.root.minsize(200,200)

        # Resizing only stretches the text window, i.e. (0,0) on the grid
        self.root.grid_columnconfigure(0,weight=1)
        self.root.grid_columnconfigure(1,weight=0)
        self.root.grid_rowconfigure(0,weight=1)
        self.root.grid_rowconfigure(1,weight=0)
        self.root.grid_rowconfigure(2,weight=0)

        # Button to quit board configuration window
        self.b_done = Button(self.root,text="Done",fg="red",command=self.close_gui)
        self.b_done.grid(row=2,column=0,sticky=W+E)

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

        self.w_cfg = Text(self.root,wrap=NONE)
        self.w_cfg.grid(row=0,column=0,sticky=NSEW)

        self.sx_cfg = Scrollbar(self.root,orient=HORIZONTAL,command=self.w_cfg.xview)
        self.sx_cfg.grid(row=1,column=0,sticky=EW)

        self.sy_cfg = Scrollbar(self.root,orient=VERTICAL,command=self.w_cfg.yview)
        self.sy_cfg.grid(row=0,column=1,sticky=NS)

        self.w_cfg.config(xscrollcommand=self.sx_cfg.set)
        self.w_cfg.config(yscrollcommand=self.sy_cfg.set)
        self.w_cfg.insert(END,"=== Board configuration ===\n")
        self.w_cfg.insert(END,self.config)

    def show_log(self):

        self.w_log = Text(self.root,wrap=NONE)
        self.w_log.grid(row=0,column=0,sticky=NSEW)

        self.sx_log = Scrollbar(self.root,orient=HORIZONTAL,command=self.w_log.xview)
        self.sx_log.grid(row=1,column=0,sticky=EW)

        self.sy_log = Scrollbar(self.root,orient=VERTICAL,command=self.w_log.yview)
        self.sy_log.grid(row=0,column=1,sticky=NS)

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
