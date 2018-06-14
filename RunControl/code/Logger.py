import os
import sys
import time

class Logger(object):

    def __init__(self):

        # Get position of DAQ main directory from PADME_DAQ_DIR environment variable
        # Default to current dir if not set
        self.daq_dir = os.getenv('PADME_DAQ_DIR',".")

        self.log_file = self.daq_dir+"/log/RunControlServer.log"

        self.interactive = False

        self.terminal = sys.stdout
        self.log = open(self.log_file,"a")

    def write(self, message):

        if (message != "\n"):
            msg = self.now_str()+" "+message
        else:
            msg = message
        if self.interactive:
            self.terminal.write(msg)
            self.terminal.flush()
        self.log.write(msg)
        self.log.flush()

    def flush(self):

        #this flush method is needed for python 3 compatibility.
        #this handles the flush command by doing nothing.
        #you might want to specify some extra behavior here.
        pass

    def now_str(self):
        return time.strftime("%Y-%m-%d %H:%M:%S",time.gmtime())
