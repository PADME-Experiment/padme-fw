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
        opts,args = getopt.getopt(argv,"h",["no-gui","server","interactive"])
    except getopt.GetoptError:
        print 'RunControl [--gui|--no-gui] [--server] [--interactive] [-h|--help]'
        sys.exit(2)

    useGUI = False
    startServer = False
    serverInteractive = False
    for opt,arg in opts:
        if opt == '-h' or opt == '--help':
            print 'RunControl [--gui|--no-gui] [--server] [--interactive] [-h]'
            sys.exit()
        elif opt == '--gui':
            useGUI = True
        elif opt == '--no-gui':
            useGUI = False
        elif opt == '--server':
            startServer = True
        elif opt == '--interactive':
            serverInteractive = True

    if startServer:

        if serverInteractive:
            RunControlServer("i")
        else:
            print "Starting RunControlServer in background"
            with daemon.DaemonContext(working_directory="."): RunControlServer("d")

    elif useGUI:

        print "Sorry: RunControl GUI is currently not available. Please use --no-gui option to start the text version."
        #app = RunControlGUI()

    else:

        app = RunControlText()

# Execution starts here
if __name__ == "__main__":
   main(sys.argv[1:])
