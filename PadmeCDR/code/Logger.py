import sys
import time

class Logger(object):

    def __init__(self,log_file):

        self.interactive = False

        self.terminal = sys.stdout
        self.log = open(log_file,"a")

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
