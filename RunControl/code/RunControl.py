#!/usr/bin/python

import os
import sys
import daemon
import getopt

from RunControlGUI import RunControlGUI
from RunControlText import RunControlText
from RunControlServer import RunControlServer

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

        # Use this only after Logger has been implemented
        #with daemon.DaemonContext(): app = RunControlServer()
        app = RunControlServer()

    elif useGUI:

        app = RunControlGUI()

    else:

        app = RunControlText()

# Execution starts here
if __name__ == "__main__":
   main(sys.argv[1:])
