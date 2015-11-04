import os
from Tkinter import *

from PadmeDB import PadmeDB

class ADCBoardGUI:

    def __init__(self,board,mode):

        self.board = board
        self.b_name = "ADC%02d" % (board.board_id,)
        self.mode = mode

        self.main_window()

        if (self.mode=="cfg"): self.show_cfg()
        if (self.mode=="log"): self.show_log()

    def main_window(self):

        # Initialize main graphic window
        self.root = Tk()
        self.root.geometry("579x485+500+500")
        self.root.configure(background='black')
        self.root.title(self.b_name)
        self.root.grid_columnconfigure(0,weight=1)
        self.root.grid_rowconfigure(0,weight=1)
        self.root.grid_rowconfigure(1,weight=0)

        # Button to quit board configuration window
        self.b_done = Button(self.root,text="Done",fg="red",command=self.done)
        self.b_done.grid(row=1,column=0,sticky=W+E)

    def show_cfg(self):

        # Frame to show configuration
        self.f_cfg = Frame(self.root,bg="black")
        self.f_cfg.grid(row=0,column=0,sticky=NSEW)
        self.s_cfg = Scrollbar(self.f_cfg)
        self.w_cfg = Text(self.f_cfg,height=30,width=80)
        self.s_cfg.pack(side=RIGHT,fill=Y)
        self.w_cfg.pack(side=LEFT,fill=Y)
        self.s_cfg.config(command=self.w_cfg.yview)
        self.w_cfg.config(yscrollcommand=self.s_cfg.set)
        self.w_cfg.insert(END,"=== Board configuration ===\n")
        self.w_cfg.insert(END,self.board.format_config())

    def show_log(self):

        # Frame to show log file
        self.f_log = Frame(self.root,bg="black")
        self.f_log.grid(row=0,column=0,sticky=NSEW)
        self.s_log = Scrollbar(self.f_log)
        self.w_log = Text(self.f_log,height=30,width=80)
        self.s_log.pack(side=RIGHT,fill=Y)
        self.w_log.pack(side=LEFT,fill=Y)
        self.s_log.config(command=self.w_log.yview)
        self.w_log.config(yscrollcommand=self.s_log.set)
        self.w_log.insert(END,"=== Board log file "+self.board.log_file+" ===\n")

        # Open log file and read current content and size
        self.file = open(self.board.log_file, 'r')
        data = self.file.read()
        self.size = len(data)
        self.w_log.insert(END, data)
        self.w_log.see(END)
        self.my_after = self.root.after(1000, self.update_log)

    def update_log(self):
        # If log fiel has grown, show the new content
        if (os.path.getsize(self.board.log_file) > self.size):
            data = self.file.read()
            self.size = self.size + len(data)
            self.w_log.insert(END, data)
            self.w_log.see(END)
        self.my_after = self.root.after(1000,self.update_log)

    def done(self):

        if (self.mode=="log"):
            self.root.after_cancel(self.my_after)
            self.file.close()
        self.root.destroy()
