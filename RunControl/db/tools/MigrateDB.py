#!/usr/bin/python

import os
import sqlite3

def main():

    # Connect to old version of the DB
    connO = sqlite3.connect('/home/daq/DigiDaq/PadmeDAQ/db/PadmeDAQ.db');
    cO = connO.cursor()

    # Connect to new version of the DB
    #connN = sqlite3.connect('/home/daq/DigiDaq/PadmeDAQ.new/db/PadmeDAQ.db');
    connN = sqlite3.connect('./PadmeDAQ.db');
    cN = connN.cursor()

    # Create board in new DB (now done by SetupDB.py)
    #cN.execute("INSERT INTO board_type(type,model,n_channels) VALUES(?,?,?)",("ADC","CAEN V1742",32))
    #cN.execute("SELECT id FROM board_type WHERE model=?",("CAEN V1742",))
    #(board_type_id,) = cN.fetchone()
    #cN.execute("INSERT INTO board(serial_number,board_type_id) VALUES(?,?)",("XXXX",board_type_id))
    cN.execute("SELECT id FROM board WHERE serial_number=?",("39",))
    (board_id,) = cN.fetchone()

    # Get list of runs from the old DB and populate new DB
    cO.execute("SELECT * FROM run");
    resO = cO.fetchall()
    for rO in resO:

        # Get all run data
        (number,type,status,time_start,time_stop,n_raw_files,total_events,total_size,comment,configuration) = rO

        # Some go to the new run table
        cN.execute("INSERT INTO run VALUES(?,?,?,?,?,?,?)",
                   (number,type,status,time_start,time_stop,total_events,comment))

        # Some go to the new process table
        cN.execute("INSERT INTO process(run_number,board_id,status,time_start,time_stop,n_raw_files,total_events,total_size) VALUES(?,?,?,?,?,?,?,?)",(number,board_id,status,time_start,time_stop,n_raw_files,total_events,total_size))

        # Save process id for configuration parameters
        cN.execute("SELECT id FROM process WHERE run_number=?",(number,))
        (process_id,) = cN.fetchone()

        # Configuration must be parsed and saved as individual parameters
        for cl in configuration.split('\n'):
            if (cl!=""):
                print "configuration line",cl
                (para,val) = cl.split(" ",1)
            # See if parameter name exists or create it
                cN.execute("SELECT id FROM config_para_name WHERE name=?",(para,))
                res = cN.fetchone()
                if (res==None):
                    cN.execute("INSERT INTO config_para_name(name) VALUES(?)",(para,))
                    cN.execute("SELECT id FROM config_para_name WHERE name=?",(para,))
                    (para_id,) = cN.fetchone()
                else:
                    (para_id,) = res
                cN.execute("INSERT INTO proc_config_para(process_id,config_para_name_id,value) VALUES(?,?,?)",(process_id,para_id,val))

    # Get list of files from the old DB and populate raw_file in the new DB
    old_runnr = -1
    file_part = 0
    cO.execute("SELECT * FROM file ORDER BY time_open");
    resO = cO.fetchall()
    for rO in resO:
        (id,name,type,version,run_number,part,time_open,time_close,size,n_events) = rO

        # Get id of process corresponding to this run
        cN.execute("SELECT id FROM process WHERE run_number=?",(run_number,))
        (process_id,) = cN.fetchone()

        cN.execute("INSERT INTO raw_file(name,version,process_id,part,time_open,time_close,n_events,size) VALUES(?,?,?,?,?,?,?,?)",(name,version,process_id,part,time_open,time_close,n_events,size))

    connN.commit()
    connN.close()

    connO.commit()
    connO.close()

# Execution starts here
if __name__ == "__main__":
   main()
