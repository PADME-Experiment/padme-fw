import MySQLdb
import os

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
        res = c.fetchone()
        if (res == None):
            print "PadmeDB - WARNING - Unknown run type selected: %s"%run_type
            run_type_id = 0
        else:
            (run_type_id,) = res
        self.conn.commit()

        # Create run
        c.execute("""INSERT INTO run (number,run_type_id,status,total_events,user) VALUES (%s,%s,%s,%s,%s)""",(run_nr,run_type_id,0,0,run_user))
        self.conn.commit()

        # Create start of run comment
        c.execute("""INSERT INTO log_entry (run_number,type,level,time,text) VALUES (%s,%s,%s,%s,%s)""",(run_nr,"SOR",0,self.now_str(),run_comment))
        self.conn.commit()

    def create_merger(self,run_number,node_id):

        self.check_db()
        c = self.conn.cursor()

        # Check if run exists
        c.execute("""SELECT number FROM run WHERE number = %s""",(run_number,))
        if (c.rowcount == 0):
            print "PadmeDB::create_merger - WARNING - Unknown run number: %d\n"%run_number
            return -1

        # Check if node exists
        c.execute("""SELECT id FROM node WHERE id = %s""",(node_id,))
        if (c.rowcount == 0):
            print "PadmeDB::create_merger - WARNING - Unknown node id: %d\n"%node_id
            return -1

        # Create merger and get its id
        c.execute("""INSERT INTO lvl1_process (run_number,node_id) VALUES (%s,%s)""",(run_number,node_id))
        return c.lastrowid

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

        self.check_db()
        c = self.conn.cursor()
        c.execute("""UPDATE run SET comment_end = %s WHERE number = %s""",(comment_end,run_nr))
        self.conn.commit()

    def add_cfg_para_run(self,run_nr,para_name,para_val):

        self.check_db()
        para_id = self.get_para_id(para_name)
        c = self.conn.cursor()
        c.execute("""INSERT INTO run_config_para (run_number,config_para_name_id,value) VALUES (%s,%s,%s)""",
                  (run_nr,para_id,para_val))
        self.conn.commit()

    def add_cfg_para_merger(self,merger_id,para_name,para_val):

        self.check_db()
        para_id = self.get_para_id(para_name)
        c = self.conn.cursor()
        c.execute("""INSERT INTO lvl1_proc_config_para (lvl1_process_id,config_para_name_id,value) VALUES (%s,%s,%s)""",
                  (merger_id,para_id,para_val))
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

        # First search by name
        c.execute("""SELECT id FROM node WHERE name=%s""",(node,))
        res = c.fetchone()
        self.conn.commit()
        if (res != None):
            (id,) = res
            return id

        # Then search by IP address
        c.execute("""SELECT id FROM node WHERE ip_addr_daq=%s""",(node,))
        res = c.fetchone()
        self.conn.commit()
        if (res != None):
            (id,) = res
            return id

        # If not found, return -1
        return -1

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
