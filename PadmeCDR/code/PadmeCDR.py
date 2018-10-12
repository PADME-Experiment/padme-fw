#!/usr/bin/python

import sys
import daemon
import getopt

from PadmeCDRServer import PadmeCDRServer

def print_help():
    print 'PadmeCDR [-c config_file] [-i] [-h]'
    print '  -c config_file  Read CDR configuration from file <config_file>. (Currently not supported)'
    print '  -i              Run the PadmeCDR server in interactive mode'
    print '  -h              Show this help message and exit'

def main(argv):

    try:
        opts,args = getopt.getopt(argv,"ic:h")
    except getopt.GetoptError:
        print_help()
        sys.exit(2)

    cfg_file = ""
    serverInteractive = False
    for opt,arg in opts:
        if opt == '-h':
            print_help()
            sys.exit()
        elif opt == '-c':
            cfg_file = arg
        elif opt == '-i':
            serverInteractive = True

    if serverInteractive:
        PadmeCDRServer(cfg_file,"i")
    else:
        print "Starting PadmeCDRServer in background"
        with daemon.DaemonContext(working_directory="."): PadmeCDRServer(cfg_file,"d")

# Execution starts here
if __name__ == "__main__":
   main(sys.argv[1:])
