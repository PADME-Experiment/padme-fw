#!/usr/bin/python

import os
import time
import sqlite3
import MySQLdb

def main():

    # Connect to old version of the DB
    connO = sqlite3.connect('/home/daq/DigiDaq/PadmeDAQ/db/PadmeDAQ.db');
    cO = connO.cursor()

    # Get DB connection parameters from environment variables
    DB_HOST   = os.getenv('PADME_DB_HOST'  ,'localhost')
    DB_PORT   = os.getenv('PADME_DB_PORT'  ,'5501')
    DB_USER   = os.getenv('PADME_DB_USER'  ,'padme')
    DB_PASSWD = os.getenv('PADME_DB_PASSWD','unknown')
    DB_NAME   = os.getenv('PADME_DB_NAME'  ,'PadmeDB')

    connN = MySQLdb.connect(host=DB_HOST,port=int(DB_PORT),user=DB_USER,passwd=DB_PASSWD,db=DB_NAME)
    cN = connN.cursor()

    # Old runs only used board 0
    board_id = 0

    # The crew was always the same
    run_user = "Leonardi, Raggi, Valente"

    # All runs were succesfully closed
    run_status = 3
    proc_status = 2

    # End comment default
    comment_end = "Normal end of run"

    # Get list of runs from the old DB and populate new DB
    cO.execute("SELECT * FROM run");
    resO = cO.fetchall()
    for rO in resO:

        # Get all run data
        (number,run_type,status,time_start,time_stop,n_daq_files,total_events,total_size,comment_start,configuration) = rO

        time_start = time.strftime("%Y-%m-%d %H:%M:%S",time.localtime(time_start))
        time_stop = time.strftime("%Y-%m-%d %H:%M:%S",time.localtime(time_stop))

        # Some go to the new run table
        cN.execute("""INSERT INTO run VALUES(%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)""",
                   (number,run_type,run_status,time_start,time_start,time_stop,total_events,run_user,comment_start,comment_end))

        # Some go to the new process table
        cN.execute("""INSERT INTO process(run_number,board_id,status,time_start,time_stop,n_daq_files,total_events,total_size) VALUES(%s,%s,%s,%s,%s,%s,%s,%s)""",(number,board_id,proc_status,time_start,time_stop,n_daq_files,total_events,total_size))

        # Save process id for configuration parameters
        cN.execute("""SELECT id FROM process WHERE run_number = %s""",(number,))
        (process_id,) = cN.fetchone()

        # Configuration must be parsed and saved as individual parameters
        for cl in configuration.split('\n'):
            if (cl!=""):
                print "configuration line",cl
                (para,val) = cl.split(" ",1)
            # See if parameter name exists or create it
                cN.execute("""SELECT id FROM config_para_name WHERE name = %s""",(para,))
                res = cN.fetchone()
                if (res==None):
                    cN.execute("""INSERT INTO config_para_name(name) VALUES(%s)""",(para,))
                    cN.execute("""SELECT id FROM config_para_name WHERE name = %s""",(para,))
                    res = cN.fetchone()
                (para_id,) = res
                cN.execute("""INSERT INTO proc_config_para(process_id,config_para_name_id,value) VALUES(%s,%s,%s)""",(process_id,para_id,val))

    # Get list of files from the old DB and populate raw_file in the new DB
    old_runnr = -1
    file_part = 0
    cO.execute("SELECT * FROM file ORDER BY time_open");
    resO = cO.fetchall()
    for rO in resO:
        (id,name,type,version,run_number,part,time_open,time_close,size,n_events) = rO

        time_open = time.strftime("%Y-%m-%d %H:%M:%S",time.localtime(time_open))
        time_close = time.strftime("%Y-%m-%d %H:%M:%S",time.localtime(time_close))

        # Get id of process corresponding to this run
        cN.execute("""SELECT id FROM process WHERE run_number = %s""",(run_number,))
        (process_id,) = cN.fetchone()

        cN.execute("""INSERT INTO daq_file(name,version,process_id,part,time_open,time_close,n_events,size) VALUES(%s,%s,%s,%s,%s,%s,%s,%s)""",(name,version,process_id,part,time_open,time_close,n_events,size))

    connN.commit()
    connN.close()

    connO.commit()
    connO.close()

# Execution starts here
if __name__ == "__main__":
   main()
