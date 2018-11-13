#!/usr/bin/python

import os
import sys
import daemon
import getopt
import subprocess

from PadmeCDRServer import PadmeCDRServer

def print_help():
    print 'PadmeCDR -S src_site -D dst_site [-s data_srv] [-i] [-h]'
    print '  -S src_site     Source site [DAQ, LNF, CNAF]'
    print '  -D dst_site     Destination site [LNF, CNAF]'
    print '  -s data_srv     Data server from which data are copied. Only used when -S is DAQ'
    print '  -i              Run the PadmeCDR server in interactive mode'
    print '  -h              Show this help message and exit'

def main(argv):

    # Get position of CDR main directory from PADME_CDR_DIR environment variable
    # Default to current dir if not set
    cdr_dir = os.getenv('PADME_CDR_DIR',".")

    # Define list of available data servers
    data_servers_list = [ "l1padme3", "l1padme4" ]

    # Define lists of source and destination sites
    source_sites_list = [ "DAQ", "LNF", "CNAF" ]
    destination_sites_list = [ "LNF", "CNAF" ]

    try:
        opts,args = getopt.getopt(argv,"iS:D:s:h")
    except getopt.GetoptError:
        print_help()
        sys.exit(2)

    data_server = ""
    source_site = ""
    destination_site = ""
    serverInteractive = False
    for opt,arg in opts:
        if opt == '-h':
            print_help()
            sys.exit()
        elif opt == '-s':
            data_server = arg
        elif opt == '-S':
            source_site = arg
        elif opt == '-D':
            destination_site = arg
        elif opt == '-i':
            serverInteractive = True

    # Check if source site is valid
    if (not source_site in source_sites_list):
        if (source_site):
            print "ERROR - Source site",source_site,"is unknown. Use one of",source_sites_list
        else:
            print "ERROR - You must specify one data server from",source_sites_list
        print_help()
        sys.exit(2)

    # When source is DAQ, check if data server was correctly specified
    if ( (source_site == "DAQ") and not (data_server in data_servers_list) ):
        if (data_server):
            print "ERROR - Data server",data_server,"is unknown. Use one of",data_servers_list
        else:
            print "ERROR - You must specify one data server from",data_servers_list
        print_help()
        sys.exit(2)

    # Check if destination site is valid
    if (not destination_site in destination_sites_list):
        if (destination_site):
            print "ERROR - Destination site",destination_site,"is unknown. Use one of",destination_sites_list
        else:
            print "ERROR - You must specify one data server from",destination_sites_list
        print_help()
        sys.exit(2)

    if (source_site == "DAQ"):
        print "Starting PadmeCDRServer with source",source_site,"server",data_server,"and destination",destination_site
    else:
        print "Starting PadmeCDRServer with source",source_site,"and destination",destination_site

    # Create long-lived proxy file (will ask user for password)
    long_proxy_file = "%s/run/long_proxy"%cdr_dir
    print "- Creating long-lived proxy file",long_proxy_file
    proxy_cmd = "voms-proxy-init --valid 480:00 --out %s"%long_proxy_file
    print ">",proxy_cmd
    if subprocess.call(proxy_cmd.split()):
        print "*** ERROR *** while generating long-lived proxy. Aborting"
        sys.exit(2)

    if serverInteractive:
        PadmeCDRServer(source_site,destination_site,data_server,"i")
    else:
        print "Starting PadmeCDRServer in background"
        with daemon.DaemonContext(working_directory="."): PadmeCDRServer(source_site,destination_site,data_server,"d")


# Execution starts here
if __name__ == "__main__":
   main(sys.argv[1:])
