#!/usr/bin/python

import os
import sys
import daemon
import getopt
import subprocess

from PadmeCDRServer import PadmeCDRServer

#def print_help():
#    print 'PadmeCDR [-c config_file] [-i] [-h]'
#    print '  -c config_file  Read CDR configuration from file <config_file>. (Currently not supported)'
#    print '  -i              Run the PadmeCDR server in interactive mode'
#    print '  -h              Show this help message and exit'

def print_help():
    print 'PadmeCDR -s data_server [-i] [-h]'
    print '  -s              Define server from which data are copied'
    print '  -i              Run the PadmeCDR server in interactive mode'
    print '  -h              Show this help message and exit'

def main(argv):

    # Get position of CDR main directory from PADME_CDR_DIR environment variable
    # Default to current dir if not set
    cdr_dir = os.getenv('PADME_CDR_DIR',".")

    # Define list of available data servers
    data_servers_list = [ "l1padme3", "l1padme4" ]

    try:
        #opts,args = getopt.getopt(argv,"ic:h")
        opts,args = getopt.getopt(argv,"is:h")
    except getopt.GetoptError:
        print_help()
        sys.exit(2)

    cfg_file = ""
    data_server = ""
    serverInteractive = False
    for opt,arg in opts:
        if opt == '-h':
            print_help()
            sys.exit()
        #elif opt == '-c':
        #    cfg_file = arg
        elif opt == '-s':
            data_server = arg
        elif opt == '-i':
            serverInteractive = True

    # Check if data server is valid
    if (not data_server in data_servers_list):
        if (data_server):
            print "ERROR - Data server",data_server,"is unknown. Use one of",data_servers_list
        else:
            print "ERROR - You must specify one data server from",data_servers_list
        print_help()
        sys.exit(2)

    # Create long-lived proxy file (will ask user for password)
    long_proxy_file = "%s/run/long_proxy"%cdr_dir
    print "- Creating long-lived proxy file",long_proxy_file
    proxy_cmd = "voms-proxy-init --valid 480:00 --out %s"%long_proxy_file
    print ">",proxy_cmd
    if subprocess.call(proxy_cmd.split()):
        print "*** ERROR *** while generating long-lived proxy. Aborting"
        sys.exit(2)

    if serverInteractive:
        #PadmeCDRServer(cfg_file,"i")
        PadmeCDRServer(data_server,"i")
    else:
        print "Starting PadmeCDRServer in background"
        #with daemon.DaemonContext(working_directory="."): PadmeCDRServer(cfg_file,"d")
        with daemon.DaemonContext(working_directory="."): PadmeCDRServer(data_server,"d")

# Execution starts here
if __name__ == "__main__":
   main(sys.argv[1:])
