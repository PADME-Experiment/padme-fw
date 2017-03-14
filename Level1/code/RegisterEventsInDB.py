#!/usr/bin/python

import os
import re
import sys
import getopt
import MySQLdb

# Directory with log files of DAQ runs
log_dir = "/home/daq/DAQ/log"

# Filename pattern for merge jobs log files
log_template = "merge_run_%s.log"

# Database connector handle
conn = None

def connect_db():

    global conn

    close_db()

    # Get DB connection parameters from environment variables
    DB_HOST   = os.getenv('PADME_DB_HOST'  ,'localhost')
    DB_PORT   = os.getenv('PADME_DB_PORT'  ,'5501')
    DB_USER   = os.getenv('PADME_DB_USER'  ,'padme')
    DB_PASSWD = os.getenv('PADME_DB_PASSWD','unknown')
    DB_NAME   = os.getenv('PADME_DB_NAME'  ,'PadmeDB')

    conn = MySQLdb.connect(host=DB_HOST,port=int(DB_PORT),user=DB_USER,passwd=DB_PASSWD,db=DB_NAME)

def close_db():

    global conn

    if (conn):
        conn.close()
        conn = None

def get_run_list():

    runs = []

    c = conn.cursor()
    c.execute("""SELECT number FROM run""")
    for (number,) in c:
        runs.append(number)
    c.close()
    conn.commit()

    return runs

def get_events_in_run(run):

    c = conn.cursor()
    q = ("SELECT total_events FROM run WHERE number = %s")
    c.execute(q,(run,))
    (total_events,) = c.fetchone()
    c.close()
    conn.commit()

    if (total_events == None):
        return -1
    else:
        return total_events

def update_run_events(run,events):

    #query = """UPDATE run SET total_events = %s WHERE number = %s"""%(events,run)
    #print query
    c = conn.cursor()
    q = ("UPDATE run SET total_events = %s WHERE number = %s")
    c.execute(q,(events,run))
    conn.commit()

def get_events_in_log(log_file):

    re_events = re.compile("^Total events written: (\d+) on (\d+) files$")
    found = False
    f = open(log_file)
    for l in f:
        m = re_events.search(l)
        if (m):
            found = True
            (n_events,n_files) = m.group(1,2)

    if found:
        return int(n_events)
    else:
        return -1

def print_help():
    print 'RegisterNEventInDB [-h] [-v]'

def main(argv):

    verbose = False

    try:
        opts,args = getopt.getopt(argv,"hv")
    except getopt.GetoptError:
        print_help()
        sys.exit(2)

    for opt,arg in opts:
        if opt == '-h':
            print_help()
            sys.exit()
        if opt == '-v':
            verbose = True

    connect_db()

    for run in get_run_list():
        nevtindb = get_events_in_run(run)
        if nevtindb == -1:
            print run,nevtindb,"*** ERROR *** no total_events field in DB (?)"
        else:
            log_file = log_dir + "/" + (log_template)%(run,)
            if os.path.isfile(log_file):
                nevtinlog = get_events_in_log(log_file)
                if (nevtinlog == -1):
                    if verbose: print run,log_file,nevtindb,nevtinlog,"No events in log file"
                elif (nevtindb == nevtinlog):
                    if verbose: print run,log_file,nevtindb,nevtinlog,"DB ok"
                elif (nevtindb == 0) and (nevtinlog != 0):
                    print run,log_file,nevtindb,nevtinlog,"DB update"
                    update_run_events(run,nevtinlog)
                else:
                    print run,log_file,nevtindb,nevtinlog,"*** ERROR ***"
            else:
                if verbose: print run,log_file,nevtindb,"No log file"

    close_db()

# Execution starts here
if __name__ == "__main__":

    main(sys.argv[1:])
