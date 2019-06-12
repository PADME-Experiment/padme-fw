#!/usr/bin/python

import os
import re
import sys
import time
import daemon
import getopt
import subprocess

from PadmeCDRServer import PadmeCDRServer
from PadmeCDRList import PadmeCDRList

# Define list of available data servers
data_servers_list = [ "l1padme3", "l1padme4" ]

# Define lists of source and destination sites
sites_list = [ "ALL", "DAQ", "LNF", "CNAF", "KLOE" ]
source_sites_list = [ "DAQ", "LNF", "CNAF" ]
destination_sites_list = [ "LNF", "CNAF", "KLOE" ]

# Define list of years of data taking
years_list = [ "2018", "2019" ]

def print_help():
    print 'PadmeCDR [-S src_site -D dst_site] [-L site] [-s data_srv] [-Y year] [-a after] [-b before] [-i] [-h]'
    print '  -S src_site     Source site %s'%source_sites_list
    print '  -D dst_site     Destination site %s'%destination_sites_list
    print '  -L site         Get list of files at site %s'%sites_list
    print '                  ALL will compare content of all sites (SLOW!)'
    print '  -s data_srv     Data server from which data are copied %s'%data_servers_list
    print '                  N.B. -s is only used when -S/-L is DAQ'
    print '  -Y year         Specify year of data taking to copy. Default: current year'
    print '  -a after_date   Only transfer runs collected after specified date (included). Format: yyyymmdd. Default: no limit'
    print '  -b before_date  Only transfer runs collected before specified date (included). Format: yyyymmdd. Default: no limit'
    print '  -i              Run the PadmeCDR server in interactive mode'
    print '  -h              Show this help message and exit'

def main(argv):

    # Get position of CDR main directory from PADME_CDR_DIR environment variable
    # Default to current dir if not set
    cdr_dir = os.getenv('PADME_CDR_DIR',".")

    try:
        opts,args = getopt.getopt(argv,"iS:D:L:s:Y:a:b:h")
    except getopt.GetoptError:
        print_help()
        sys.exit(2)

    year = time.strftime("%Y",time.gmtime())
    data_server = ""
    source_site = ""
    destination_site = ""
    list_site = ""
    date_after = ""
    date_before = ""
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
        elif opt == '-L':
            list_site = arg
        elif opt == '-Y':
            year = arg
        elif opt == '-a':
            date_after = arg
        elif opt == '-b':
            date_before = arg
        elif opt == '-i':
            serverInteractive = True

    # Check if year is permitted
    if (not year in years_list):
        print "ERROR - Requested year",year,"is not available. Use one of",years_list
        print_help()
        sys.exit(2)

    # Check if either -L or -S/-D was specified
    if ( list_site and (source_site or destination_site) ):
        print "ERROR - List site",list_site,"specified with source/destination sites",source_site,destination_site
        print_help()
        sys.exit(2)

    # When source/list is DAQ, check if data server was correctly specified
    if ( (source_site == "DAQ" or list_site == "DAQ") and not (data_server in data_servers_list) ):
        if (data_server):
            print "ERROR - Data server",data_server,"is unknown. Use one of",data_servers_list
        else:
            print "ERROR - You must specify one data server from",data_servers_list
        print_help()
        sys.exit(2)

    # Check if date interval was specified with the correct format
    if ( date_after != "" and not re.match("\d\d\d\d\d\d\d\d",date_after) ):
        print "ERROR - Start date (-a) has wrong format: %s",date_after
        print_help()
        sys.exit(2)
    if ( date_before != "" and not re.match("\d\d\d\d\d\d\d\d",date_before) ):
        print "ERROR - End date (-b) has wrong format: %s",date_before
        print_help()
        sys.exit(2)

    if ( list_site ):

        # Check if source site is valid
        if (not list_site in sites_list):
            print "ERROR - List site",list_site,"is unknown. Use one of",sites_list
            print_help()
            sys.exit(2)

        PadmeCDRList(list_site,data_server)

    else:

        # Check if source site is valid
        if (not source_site in source_sites_list):
            if (source_site):
                print "ERROR - Source site",source_site,"is unknown. Use one of",source_sites_list
            else:
                print "ERROR - You must specify one data server from",source_sites_list
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
            PadmeCDRServer(source_site,destination_site,data_server,year,date_after,date_before,"i")
        else:
            print "Starting PadmeCDRServer in background"
            with daemon.DaemonContext(working_directory="."): PadmeCDRServer(source_site,destination_site,data_server,year,date_after,date_before,"d")

# Execution starts here
if __name__ == "__main__":
   main(sys.argv[1:])
