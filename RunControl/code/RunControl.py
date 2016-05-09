#!/usr/bin/python

import os
import sys
import daemon
import getopt
from Run import Run
from RunControlGUI import RunControlGUI
from RunControlText import RunControlText
from RunControlServer import RunControlServer
from PadmeDB import PadmeDB

def main(argv):

    try:
        opts,args = getopt.getopt(argv,"h",["no-gui","server"])
    except getopt.GetoptError:
      print 'RunControl [--no-gui] [--server] [-h]'
      sys.exit(2)

    useGUI = True
    startServer = False
    for opt,arg in opts:
        if opt == '-h':
            print 'RunControl [--no-gui] [--server] [-h]'
            sys.exit()
        elif opt == '--no-gui':
            useGUI = False
        elif opt == '--server':
            startServer = True

    if startServer:

        # Check lock file and create our own
        lock_file = "run/lock"
        if (os.path.exists(lock_file)):
            if (os.path.isfile(lock_file)):
                pid = 0
                lf = open(lock_file,"r")
                for ll in lf: pid = ll
                lf.close()
                print "Lock file",lock_file,"found for pid",pid
            else:
                print "Lock file",lock_file,"found but it is not a file"
            exit(1)
        pid = os.getpid()
        lf = open(lock_file,"w")
        lf.write("%d"%pid)
        lf.close()

        # Get initial setup from last execution or use default
        setup = "test"
        lus = ""
        lus_file = "setup/last_used_setup"
        if (os.path.exists(lus_file)):
            if (os.path.isfile(lus_file)):
                lusf = open(lus_file,"r")
                lus = lusf.read().strip("\n")
                lusf.close()
                if (lus == ""):
                    print "File with last used setup",lus_file,"exists but it is empty. Using default setup:",setup
                else:
                    setup = lus
            else:
                print "ERROR - File with last used setup",lus_file,"exists but it is not a file"
                exit(1)
        else:
            print "Could not find file",lus_file,"- Using default setup:",setup

        # Create run
        #print "=== Starting PADME Run Control server with",setup,"setup"
        run = Run()
        run.change_setup(setup)

        #with daemon.DaemonContext(): app = RunControlServer(run)

        app = RunControlServer(run)

        print "=== Exit PADME Run Control ==="

        # Remove lock file
        os.remove(lock_file)

    elif useGUI:

        app = RunControlGUI()

    else:

        app = RunControlText()

# Execution starts here
if __name__ == "__main__":
   main(sys.argv[1:])
