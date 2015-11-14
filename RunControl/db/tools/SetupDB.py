#!/usr/bin/python

import os
import re
import MySQLdb

# Define regular expressions used in file parsing
re_empty = re.compile("^\s*$")
re_comment = re.compile("^\s*#")
re_board = re.compile("^\s*board\s+(\d+)\s+(\w+)\s+(\w+)\s+(\w+)\s+(\d+)\s+(\w+)\s+(\d+):(\d+):(\d+):(\d+)\s+(\d+)-(\d+)-(\d+)\s+(\d+):(\d+):(\d+)\s*$")

def main():

    # Get DB connection parameters from environment variables
    DB_HOST   = os.getenv('PADME_DB_HOST'  ,'localhost')
    DB_PORT   = os.getenv('PADME_DB_PORT'  ,'5501')
    DB_USER   = os.getenv('PADME_DB_USER'  ,'padme')
    DB_PASSWD = os.getenv('PADME_DB_PASSWD','unknown')
    DB_NAME   = os.getenv('PADME_DB_NAME'  ,'PadmeDB')
    conn = MySQLdb.connect(host=DB_HOST,port=int(DB_PORT),user=DB_USER,passwd=DB_PASSWD,db=DB_NAME)
    c = conn.cursor()

    # Read DB setup file
    f = open("SetupDB.data","r")
    for l in f:

        if (re_empty.search(l) or re_comment.search(l)): continue

        m = re_board.search(l)
        if (m):
            (board_id,type,manufacturer,model,n_channels,serial_number,node_id,controller_id,channel_id,slot_id,f_year,f_month,f_day,f_hour,f_min,f_sec) = m.group(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16)
            board_id = int(board_id)
            n_channels = int(n_channels)
            node_id = int(node_id)
            controller_id = int(controller_id)
            channel_id = int(channel_id)
            slot_id = int(slot_id)
            f_year = int(f_year)
            f_month = int(f_month)
            f_day = int(f_day)
            f_hour = int(f_hour)
            f_min = int(f_min)
            f_sec = int(f_sec)

            # Process board_type
            board_type_id = -1
            c.execute("""SELECT id FROM board_type WHERE manufacturer=%s AND model=%s""",(manufacturer,model))
            res =  c.fetchone()
            if (res == None):
                # Board type does not exist: create it in the DB
                print "Creating board_type",type,manufacturer,model,n_channels
                c.execute("""INSERT INTO board_type(type,manufacturer,model,n_channels) VALUES(%s,%s,%s,%s)""",
                          (type,manufacturer,model,n_channels))
                c.execute("""SELECT id FROM board_type WHERE manufacturer=%s AND model=%s""",(manufacturer,model))
                (board_type_id,) = c.fetchone()
            else:
                (board_type_id,) = res
                # Board type exists: check if it is the same
                c.execute("""SELECT type,n_channels FROM board_type WHERE id=%s""",(board_type_id,))
                (old_type,old_n_channels) = c.fetchone()
                if ( (old_type != type) or (old_n_channels != n_channels) ):
                    print "ERROR: Found exisiting board_type from",manufacturer,"model",model,"with different characteristics"
                    print "  Old",old_type,old_n_channels
                    print "  New",type,n_channels
                    return

            # Process board_phys
            board_phys_id = -1
            c.execute("""SELECT id FROM board_phys WHERE serial_number=%s AND board_type_id=%s""",(serial_number,board_type_id))
            res = c.fetchone()
            if (res == None):
                # Board_phys does not exist: create it in the DB
                print "Creating physical board with board_type_id",board_type_id,"and S/N",serial_number
                c.execute("""INSERT INTO board_phys(board_type_id,serial_number) VALUES(%s,%s)""",
                          (board_type_id,serial_number))
                c.execute("""SELECT id FROM board_phys WHERE serial_number=%s AND board_type_id=%s""",(serial_number,board_type_id))
                (board_phys_id,) = c.fetchone()
            else:
                (board_phys_id,) = res

            # Process optical_link
            optical_link_id = -1
            c.execute("""SELECT id FROM optical_link WHERE node_id=%s AND controller_id=%s AND channel_id=%s AND slot_id=%s""",(node_id,controller_id,channel_id,slot_id))
            res = c.fetchone()
            if (res == None):
                print "Creating optical link entry for",node_id,controller_id,channel_id,slot_id
                c.execute("""INSERT INTO optical_link(node_id,controller_id,channel_id,slot_id) VALUES(%s,%s,%s,%s)""",
                          (node_id,controller_id,channel_id,slot_id))
                c.execute("""SELECT id FROM optical_link WHERE node_id=%s AND controller_id=%s AND channel_id=%s AND slot_id=%s""",(node_id,controller_id,channel_id,slot_id))
                (optical_link_id,) = c.fetchone()
            else:
                (optical_link_id,) = res

            # Process board
            c.execute("""SELECT id FROM board WHERE id=%s""",(board_id,))
            res = c.fetchone()
            if (res == None):
                print "Creating board",board_id
                c.execute("""INSERT INTO board(id) VALUES(%s)""",(board_id,))

    # Commit and close connection to DB
    conn.commit()
    conn.close()
      
# Execution starts here
if __name__ == "__main__":
   main()
