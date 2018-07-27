import MySQLdb
import os
import time
import re

class PadmeDB:

    def __init__(self):

        self.conn = None

    def __del__(self):

        self.close_db()

    def connect_db(self):

        self.close_db()

        # Get DB connection parameters from environment variables
        DB_HOST   = os.getenv('PADME_DB_HOST'  ,'localhost')
        DB_PORT   = os.getenv('PADME_DB_PORT'  ,'5501')
        DB_USER   = os.getenv('PADME_DB_USER'  ,'padme')
        DB_PASSWD = os.getenv('PADME_DB_PASSWD','unknown')
        DB_NAME   = os.getenv('PADME_DB_NAME'  ,'PadmeDAQ')

        self.conn = MySQLdb.connect(host=DB_HOST,port=int(DB_PORT),user=DB_USER,passwd=DB_PASSWD,db=DB_NAME)

    def close_db(self):

        if (self.conn):
            self.conn.close()
            self.conn = None

    def check_db(self):

        # This does not work
        #if (self.conn and self.conn.is_connected()): return
        if self.conn:
            try:
                self.conn.ping()
            except:
                self.connect_db()
        else:
            self.connect_db()

    def is_run_in_db(self,run_nr):

        self.check_db()
        c = self.conn.cursor()
        c.execute("""SELECT COUNT(number) FROM run WHERE number=%s""",(run_nr,))
        (n,) = c.fetchone()
        self.conn.commit()
        if (n): return 1

        return 0

    def get_last_run_in_db(self):

        self.check_db()
        c = self.conn.cursor()
        c.execute("""SELECT MAX(number) FROM run""")
        (maxrun,) = c.fetchone()
        self.conn.commit()
        if (maxrun == None):
            return 0
        else:
            return maxrun

    def create_run(self,run_nr,run_type,run_user,run_comment):

        self.check_db()
        c = self.conn.cursor()

        # Get run_type id
        c.execute("""SELECT id FROM run_type WHERE short_name = %s""",(run_type,))
        if (c.rowcount == 0):
            print "PadmeDB::create_run - WARNING - Unknown run type selected: %s - Defaulting to OTHER"%run_type
            c.execute("""SELECT id FROM run_type WHERE short_name = OTHER""")
        res = c.fetchone()
        (run_type_id,) = res
        self.conn.commit()

        # Create run
        c.execute("""INSERT INTO run (number,run_type_id,status,total_events,user) VALUES (%s,%s,%s,%s,%s)""",(run_nr,run_type_id,0,0,run_user))
        self.conn.commit()

        # Create start of run comment
        c.execute("""INSERT INTO log_entry (run_number,type,level,time,text) VALUES (%s,%s,%s,%s,%s)""",(run_nr,"SOR",0,self.now_str(),run_comment))
        self.conn.commit()

    def create_daq_process(self,mode,run_number,link_id):

        self.check_db()
        c = self.conn.cursor()

        # Check if run number exists
        c.execute("""SELECT number FROM run WHERE number = %s""",(run_number,))
        if c.rowcount == 0:
            print "PadmeDB::create_daq_process - ERROR - Unknown run number: %d\n"%run_number
            return -1

        # Check if link id exists
        op_link_exists = True
        c.execute("""SELECT id FROM optical_link WHERE id = %s""",(link_id,))
        if c.rowcount == 0:
            print "PadmeDB::create_daq_process - WARNING - Unknown optical_link id: %d\n"%link_id
            op_link_exists = False

        if op_link_exists:
            c.execute("""INSERT INTO daq_process (mode,run_number,optical_link_id,status) VALUES (%s,%s,%s,%s)""",(mode,run_number,link_id,0))
        else:
            # Accept processes with no associated optical link (optical_link_id is NULL)
            c.execute("""INSERT INTO daq_process (mode,run_number,status) VALUES (%s,%s,%s)""",(mode,run_number,0))

        process_id = c.lastrowid

        self.conn.commit()

        return process_id
        
    def create_trigger_process(self,run_number,node_id):

        self.check_db()
        c = self.conn.cursor()

        # Check if run exists
        c.execute("""SELECT number FROM run WHERE number = %s""",(run_number,))
        if (c.rowcount == 0):
            print "PadmeDB::create_trigger_process - ERROR - Unknown run number: %d\n"%run_number
            return -1

        # Check if node exists
        node_exists = True
        c.execute("""SELECT id FROM node WHERE id = %s""",(node_id,))
        if (c.rowcount == 0):
            print "PadmeDB::create_trigger_process - WARNING - Unknown node id: %d\n"%node_id
            node_exists = False

        # Create trigger process and get its id
        if node_exists:
            c.execute("""INSERT INTO trigger_process (run_number,node_id,status) VALUES (%s,%s,%s)""",(run_number,node_id,0))
        else:
            # Accept mergers with no associated node id (node_id is NULL)
            c.execute("""INSERT INTO trigger_process (run_number,status) VALUES (%s,%s)""",(run_number,0))
        process_id = c.lastrowid

        self.conn.commit()

        return process_id
        
    def create_merger_process(self,run_number,node_id):

        self.check_db()
        c = self.conn.cursor()

        # Check if run exists
        c.execute("""SELECT number FROM run WHERE number = %s""",(run_number,))
        if (c.rowcount == 0):
            print "PadmeDB::create_merger_process - ERROR - Unknown run number: %d\n"%run_number
            return -1

        # Check if node exists
        node_exists = True
        c.execute("""SELECT id FROM node WHERE id = %s""",(node_id,))
        if (c.rowcount == 0):
            print "PadmeDB::create_merger_process - WARNING - Unknown node id: %d\n"%node_id
            node_exists = False

        # Create merger and get its id
        if node_exists:
            c.execute("""INSERT INTO merger_process (run_number,node_id,status) VALUES (%s,%s,%s)""",(run_number,node_id,0))
        else:
            # Accept mergers with no associated node id (node_id is NULL)
            c.execute("""INSERT INTO merger_process (run_number,status) VALUES (%s,%s)""",(run_number,0))
        process_id = c.lastrowid

        self.conn.commit()

        return process_id
        
    def create_level1_process(self,run_number,node_id,number):

        self.check_db()
        c = self.conn.cursor()

        # Check if run exists
        c.execute("""SELECT number FROM run WHERE number = %s""",(run_number,))
        if (c.rowcount == 0):
            print "PadmeDB::create_level1_process - ERROR - Unknown run number: %d\n"%run_number
            return -1

        # Check if node exists
        node_exists = True
        c.execute("""SELECT id FROM node WHERE id = %s""",(node_id,))
        if (c.rowcount == 0):
            print "PadmeDB::create_level1_process - WARNING - Unknown node id: %d\n"%node_id
            node_exists = False

        # Create merger and get its id
        if node_exists:
            c.execute("""INSERT INTO level1_process (run_number,node_id,number,status) VALUES (%s,%s,%s,%s)""",(run_number,node_id,number,0))
        else:
            # Accept mergers with no associated node id (node_id is NULL)
            c.execute("""INSERT INTO level1_process (run_number,number,status) VALUES (%s,%s,%s)""",(run_number,number,0))
        process_id = c.lastrowid

        self.conn.commit()

        return process_id

    def set_run_status(self,run_nr,status):

        self.check_db()
        c = self.conn.cursor()
        c.execute("""UPDATE run SET status = %s WHERE number = %s""",(status,run_nr))
        self.conn.commit()

    def set_run_time_init(self,run_nr,time_init):

        self.check_db()
        c = self.conn.cursor()
        c.execute("""UPDATE run SET time_init = %s WHERE number = %s""",(time_init,run_nr))
        self.conn.commit()

    def set_run_time_start(self,run_nr,time_start):

        self.check_db()
        c = self.conn.cursor()
        c.execute("""UPDATE run SET time_start = %s WHERE number = %s""",(time_start,run_nr))
        self.conn.commit()

    def set_run_time_stop(self,run_nr,time_stop):

        self.check_db()
        c = self.conn.cursor()
        c.execute("""UPDATE run SET time_stop = %s WHERE number = %s""",(time_stop,run_nr))
        self.conn.commit()

    def set_run_comment_end(self,run_nr,comment_end):

        # Create end of run comment
        self.check_db()
        c = self.conn.cursor()
        c.execute("""INSERT INTO log_entry (run_number,type,level,time,text) VALUES (%s,%s,%s,%s,%s)""",(run_nr,"EOR",0,self.now_str(),comment_end))
        self.conn.commit()

    def set_run_total_events(self,run_nr,total_events):

        # Add total number of events info to run
        self.check_db()
        c = self.conn.cursor()
        c.execute("""UPDATE run SET total_events = %s WHERE number = %s""",(total_events,run_nr))
        self.conn.commit()

    def add_cfg_para_run(self,run_nr,para_name,para_val):

        self.check_db()
        para_id = self.get_para_id(para_name)
        c = self.conn.cursor()
        c.execute("""INSERT INTO run_config_para (run_number,config_para_name_id,value) VALUES (%s,%s,%s)""",
                  (run_nr,para_id,para_val))
        self.conn.commit()

    def add_cfg_para_daq(self,daq_id,para_name,para_val):

        self.check_db()
        para_id = self.get_para_id(para_name)
        c = self.conn.cursor()
        c.execute("""INSERT INTO daq_proc_config_para (daq_process_id,config_para_name_id,value) VALUES (%s,%s,%s)""",
                  (daq_id,para_id,para_val))
        self.conn.commit()

    def add_cfg_para_trigger(self,trigger_id,para_name,para_val):

        self.check_db()
        para_id = self.get_para_id(para_name)
        c = self.conn.cursor()
        c.execute("""INSERT INTO trigger_proc_config_para (trigger_process_id,config_para_name_id,value) VALUES (%s,%s,%s)""",
                  (trigger_id,para_id,para_val))
        self.conn.commit()

    def add_cfg_para_merger(self,merger_id,para_name,para_val):

        self.check_db()
        para_id = self.get_para_id(para_name)
        c = self.conn.cursor()
        c.execute("""INSERT INTO merger_proc_config_para (merger_process_id,config_para_name_id,value) VALUES (%s,%s,%s)""",
                  (merger_id,para_id,para_val))
        self.conn.commit()

    def add_cfg_para_level1(self,level1_id,para_name,para_val):

        self.check_db()
        para_id = self.get_para_id(para_name)
        c = self.conn.cursor()
        c.execute("""INSERT INTO level1_proc_config_para (level1_process_id,config_para_name_id,value) VALUES (%s,%s,%s)""",
                  (level1_id,para_id,para_val))
        self.conn.commit()

    def get_para_id(self,para_name):

        self.check_db()
        c = self.conn.cursor()
        c.execute("""SELECT id FROM config_para_name WHERE name=%s""",(para_name,))
        res = c.fetchone()
        self.conn.commit()

        if (res == None):
            c.execute("""INSERT INTO config_para_name (name) VALUES (%s)""",(para_name,))
            self.conn.commit()
            c.execute("""SELECT id FROM config_para_name WHERE name=%s""",(para_name,))
            res = c.fetchone()
            self.conn.commit()

        (id,) = res
        return id

    def now_str(self):
        return time.strftime("%Y-%m-%d %H:%M:%S",time.gmtime())

    def get_node_id(self,node):

        # Return DB id of node with given name/ip address (from DAQ VLAN)

        self.check_db()
        c = self.conn.cursor()

        # Check if this is an IP address
        res = re.match("^\d+\.\d+\.\d+\.\d+$",node)
        if (res != None):

            # Search by IP address on the DAQ VLAN
            c.execute("""SELECT id FROM node WHERE ip_addr_daq=%s""",(node,))
            res = c.fetchone()
            self.conn.commit()
            if (res != None):
                (id,) = res
                return id

        else:

            # If node name does not end with .lnf.infn.it, add it before searching (allow localhost)
            res = re.match("^\w*\.lnf\.infn\.it$",node)
            if (res == None and node != "localhost"): node += ".lnf.infn.it"

            # Search by name
            c.execute("""SELECT id FROM node WHERE name=%s""",(node,))
            res = c.fetchone()
            self.conn.commit()
            if (res != None):
                (id,) = res
                return id

        # If not found, return -1
        return -1

    def get_node_daq_ip(self,id):

        # Return IP address on DAQ VLAN (192.168.60.X) of node with given ID
 
        self.check_db()
        c = self.conn.cursor()

        c.execute("""SELECT ip_addr_daq FROM node WHERE id=%s""",(id,))
        res = c.fetchone()
        self.conn.commit()
        if (res != None):
            (ip,) = res
            if (ip != None):
                return ip

        # If not found, return null string
        return ""

    def get_run_types(self):

        # Return list of run types known to DB

        self.check_db()
        c = self.conn.cursor()

        c.execute("""SELECT short_name FROM run_type""")
        data = c.fetchall()
        type_list = []
        for row in data: type_list.append(row[0])

        self.conn.commit()

        return type_list

    def get_link_id(self,node_id,controller_id,channel_id,slot_id):

        # Return id of optical link given its description
        
        self.check_db()
        c = self.conn.cursor()

        c.execute("""SELECT id FROM optical_link WHERE node_id=%s AND controller_id=%s AND channel_id=%s AND slot_id=%s""",
                  (node_id,controller_id,channel_id,slot_id))
        if c.rowcount == 0: return -1
        ret = c.fetchone()
        return ret[0]

    def get_merger_final_info(self,merger_id):

        # Return final events and size info from merger
        
        self.check_db()
        c = self.conn.cursor()

        c.execute("""SELECT total_events,total_size FROM merger_process WHERE id=%s""",(merger_id,))
        if c.rowcount == 0: return (-1,-1)
        ret = c.fetchone()
        return ret
