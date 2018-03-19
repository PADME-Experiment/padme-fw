#!/usr/bin/python

import os
import re
import MySQLdb

# Define regular expressions used in file parsing
re_empty = re.compile("^\s*$")
re_comment = re.compile("^\s*#")
#re_board = re.compile("^\s*board\s+(\d+)\s+(\w+)\s+(\w+)\s+(\w+)\s+(\d+)\s+(\w+)\s+(\d+):(\d+):(\d+):(\d+)\s+(\d+)-(\d+)-(\d+)\s+(\d+):(\d+):(\d+)\s*$")
re_board = re.compile("^\s*board\s+(\d+)\s+(\w+)\s+(\w+)\s+(\w+)\s+(\d+)\s+(\w+)\s*$")
re_link = re.compile("^\s*link\s+(\d+):(\d+):(\d+):(\d+)\s+(\d+)\s+(\d+-\d+-\d+)\s+(\d+:\d+:\d+)\s*$")

max_datetime = "2049-12-31 23:59:59"

def main():

    # Get DB connection parameters from environment variables
    DB_HOST   = os.getenv('PADME_DB_HOST'  ,'localhost')
    DB_PORT   = os.getenv('PADME_DB_PORT'  ,'5501')
    DB_USER   = os.getenv('PADME_DB_USER'  ,'padme')
    DB_PASSWD = os.getenv('PADME_DB_PASSWD','unknown')
    DB_NAME   = os.getenv('PADME_DB_NAME'  ,'PadmeDAQ')
    conn = MySQLdb.connect(host=DB_HOST,port=int(DB_PORT),user=DB_USER,passwd=DB_PASSWD,db=DB_NAME)
    c = conn.cursor()

    # Read DB setup file
    f = open("PadmeDAQ_data.txt","r")
    for l in f:

        if (re_empty.search(l) or re_comment.search(l)): continue

        # Check boards
        m = re_board.search(l)
        if (m):

            (board_id,type,manufacturer,model,n_channels,serial_number) = m.group(1,2,3,4,5,6)
            board_id = int(board_id)
            n_channels = int(n_channels)
            #print "Read",board_id,type,manufacturer,model,n_channels,serial_number

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

            # Check if board with given id exists
            c.execute("""SELECT serial_number,board_type_id FROM board WHERE id=%s""",(board_id,))
            res = c.fetchone()
            if (res == None):
                # board_id does not exist: check if another board with same type/sn exists
                c.execute("""SELECT id FROM board WHERE serial_number=%s AND board_type_id=%s""",(serial_number,board_type_id))
                res = c.fetchone()
                if (res == None):
                    # Board does not exist: create it in the DB
                    print "Creating board with board_type_id",board_type_id,"and S/N",serial_number
                    c.execute("""INSERT INTO board(id,board_type_id,serial_number) VALUES(%s,%s,%s)""",
                          (board_id,board_type_id,serial_number))
                else:
                    # Another board with same type/sn exists: error!
                    (old_board_id,) = res;
                    print "ERROR: Found exisiting board with board_type_id",board_type_id,"and S/N",serial_number,"but different id - Old id",old_board_id,"New id",board_id
                    return
            else:
                # board_id exists: check if type/sn correspond
                (old_serial_number,old_board_type_id) = res
                if ( (old_board_type_id != board_type_id) or (old_serial_number != serial_number) ):
                    print "ERROR: Found exisiting board with id",board_id,"but different board_type_id/serial_numer"
                    print "  Old",old_board_type_id,old_serial_number
                    print "  New",board_type_id,serial_number
                    return


        # Check links
        m = re_link.search(l)
        if (m):

            (node_id,controller_id,channel_id,slot_id,board_id,f_date,f_time) = m.group(1,2,3,4,5,6,7)
            board_id = int(board_id)
            opt_link = node_id+":"+controller_id+":"+channel_id+":"+slot_id
            node_id = int(node_id)
            controller_id = int(controller_id)
            channel_id = int(channel_id)
            slot_id = int(slot_id)
            f_datetime = f_date+" "+f_time

            # Get id of optical link or create it
            optical_link_id = -1
            c.execute("""SELECT id FROM optical_link WHERE node_id=%s AND controller_id=%s AND channel_id=%s AND slot_id=%s""",(node_id,controller_id,channel_id,slot_id))
            res = c.fetchone()
            if (res == None):
                # Link does not exist: create it in the DB
                print "Creating optical link for node",node_id,"controller",controller_id,"channel",channel_id,"slot",slot_id
                c.execute("""INSERT INTO optical_link(node_id,controller_id,channel_id,slot_id) VALUES(%s,%s,%s,%s)""",(node_id,controller_id,channel_id,slot_id))
                c.execute("""SELECT id FROM optical_link WHERE node_id=%s AND controller_id=%s AND channel_id=%s AND slot_id=%s""",(node_id,controller_id,channel_id,slot_id))
                (optical_link_id,) = c.fetchone()
            else:
                (optical_link_id,) = res

            # Check if board connected to link changed
            c.execute("""SELECT id,board_id FROM l_board_optical_link WHERE optical_link_id=%s AND time_start<=%s AND time_stop>%s""",(optical_link_id,f_datetime,f_datetime))
            res = c.fetchone()
            if (res == None):
                print "Creating connection of board",board_id,"to link",opt_link,"starting from",f_datetime
                c.execute("""INSERT INTO l_board_optical_link(board_id,optical_link_id,time_start,time_stop) VALUES(%s,%s,%s,%s)""",(board_id,optical_link_id,f_datetime,max_datetime))
            else:
                (old_link_id,old_board_id) = res
                old_link_id = int(old_link_id)
                old_board_id = int(old_board_id)
                if (old_board_id != board_id):
                    print "Link",opt_link,"changed board from",old_board_id,"to",board_id,"at",f_datetime
                    c.execute("""UPDATE l_board_optical_link SET time_stop=%s WHERE id=%s""",(f_datetime,old_link_id))
                    c.execute("""INSERT INTO l_board_optical_link(board_id,optical_link_id,time_start,time_stop) VALUES(%s,%s,%s,%s)""",(board_id,optical_link_id,f_datetime,max_datetime))

    # Commit and close connection to DB
    conn.commit()
    conn.close()
      
# Execution starts here
if __name__ == "__main__":
   main()
