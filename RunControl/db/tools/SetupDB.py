#!/usr/bin/python

import os
import re
import sqlite3

# Define regular expressions used in file parsing
re_empty = re.compile("^\s*$")
re_comment = re.compile("^\s*#")
re_board_type = re.compile("^\s*board_type\s+(\d+)\s+(\w+)\s+(\w+)\s+(\w+)\s+(\d+)\s*$")
re_board = re.compile("^\s*board\s+(\d+)\s+(\d+)\s+(\w+)\s*$")

def main():

    # Connect to new version of the DB
    conn = sqlite3.connect('./PadmeDAQ.db');
    c = conn.cursor()

    # Read DB setup file
    f = open("SetupDB.data","r")
    for l in f:

        if (re_empty.search(l) or re_comment.search(l)): continue

        # Process board types
        m = re_board_type.search(l)
        if (m):
            (id,type,manufacturer,model,n_channels) = m.group(1,2,3,4,5)
            n_channels = int(n_channels)
            c.execute("SELECT type,manufacturer,model,n_channels FROM board_type WHERE id=?",(id,))
            r = c.fetchone()
            if (r==None):
                # Board type does not exist: create it in the DB
                print "Creating board_type",id,type,manufacturer,model,n_channels
                c.execute("INSERT INTO board_type(id,type,manufacturer,model,n_channels) VALUES(?,?,?,?,?)",
                          (id,type,manufacturer,model,n_channels))
            else:
                # Board type exists: check if it is the same
                (old_type,old_manufacturer,old_model,old_n_channels) = r
                if (old_type         != type         or
                    old_manufacturer != manufacturer or
                    old_model        != model        or
                    old_n_channels   != n_channels):
                    print "ERROR: Found exisiting board_type",id,"with different characteristics"
                    print "  Old",old_type,old_manufacturer,old_model,old_n_channels
                    print "  New",type,manufacturer,model,n_channels
                else:
                    print "Found existing board_type",id,type,manufacturer,model,n_channels

        # Process boards
        m = re_board.search(l)
        if (m):
            (id,board_type_id,serial_number) = m.group(1,2,3)
            board_type_id = int(board_type_id)
            c.execute("SELECT board_type_id,serial_number FROM board WHERE id=?",(id,))
            r = c.fetchone()
            if (r==None):
                # Board does not exist: create it in the DB
                print "Creating board",id,board_type_id,serial_number
                c.execute("INSERT INTO board(id,board_type_id,serial_number) VALUES(?,?,?)",
                          (id,board_type_id,serial_number))
            else:
                # Board exists: check if it is the same
                (old_board_type_id,old_serial_number) = r
                if (old_board_type_id != board_type_id or
                    old_serial_number != serial_number):
                    print "ERROR: Found exisiting board",id,"with different characteristics"
                    print "  Old",old_board_type_id,old_serial_number
                    print "  New",board_type_id,serial_number
                else:
                    print "Found exisiting board",id,board_type_id,serial_number


    # Commit and close connection to DB
    conn.commit()
    conn.close()
      
# Execution starts here
if __name__ == "__main__":
   main()
