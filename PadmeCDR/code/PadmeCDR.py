#!/usr/bin/python

import daemon
import getopt

def print_help():
    print 'PadmeCDR [-c config_file] [-h]'

def main(argv):

    try:
        opts,args = getopt.getopt(argv,"c:h")
    except getopt.GetoptError:
        print_help()
        sys.exit(2)

    cfg_file = ""
    for opt,arg in opts:
        if opt == '-h':
            print_help()
            sys.exit()
        elif opt == '-c':
            cfg_file = arg

    # Here we read the configuration file

    with daemon.DaemonContext():
        cdr = RunControlServer()
        cdr.main_loop()

# Execution starts here
if __name__ == "__main__":
   main(sys.argv[1:])
