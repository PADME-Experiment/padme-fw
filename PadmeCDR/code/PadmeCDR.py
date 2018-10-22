#!/usr/bin/python

import os
import sys
import daemon
import getopt
import subprocess

from PadmeCDRServer import PadmeCDRServer

def print_help():
    print 'PadmeCDR [-c config_file] [-i] [-h]'
    print '  -c config_file  Read CDR configuration from file <config_file>. (Currently not supported)'
    print '  -i              Run the PadmeCDR server in interactive mode'
    print '  -h              Show this help message and exit'

def main(argv):

    # Get position of CDR main directory from PADME_CDR_DIR environment variable
    # Default to current dir if not set
    cdr_dir = os.getenv('PADME_CDR_DIR',".")

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

    # Create long-lived proxy file (will ask user for password)
    long_proxy_file = "%s/run/long_proxy"%cdr_dir
    print "- Creating long-lived proxy file",long_proxy_file
    proxy_cmd = "voms-proxy-init --valid 480:0 --out %s"%long_proxy_file
    print ">",proxy_cmd
    if subprocess.call(proxy_cmd.split()):
        print "*** ERROR *** while generating long-lived proxy. Aborting"
        sys.exit(2)

    if serverInteractive:
        PadmeCDRServer(cfg_file,"i")
    else:
        print "Starting PadmeCDRServer in background"
        with daemon.DaemonContext(working_directory="."): PadmeCDRServer(cfg_file,"d")

# Execution starts here
if __name__ == "__main__":
   main(sys.argv[1:])
