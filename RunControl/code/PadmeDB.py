import MySQLdb
import os
import time
import re

class PadmeDB:

    def __init__(self):

        # Get DB connection parameters from environment variables
        self.DB_HOST   = os.getenv('PADME_DB_HOST'  ,'localhost')
        self.DB_PORT   = int(os.getenv('PADME_DB_PORT'  ,'5501'))
        self.DB_USER   = os.getenv('PADME_DB_USER'  ,'padme')
        self.DB_PASSWD = os.getenv('PADME_DB_PASSWD','unknown')
        self.DB_NAME   = os.getenv('PADME_DB_NAME'  ,'PadmeDAQ')

        self.conn = None

    def __del__(self):

        self.close_db()

    def connect_db(self):

        self.close_db()

        try:
            self.conn = MySQLdb.connect(host   = self.DB_HOST,
                                        port   = self.DB_PORT,
                                        user   = self.DB_USER,
                                        passwd = self.DB_PASSWD,
                                        db     = self.DB_NAME)
        except:
            print "*** PadmeDB ERROR *** Unable to connect to DB. Exception: %s"%sys.exc_info()[0]
            sys.exit(2)

    def close_db(self):

        if (self.conn):
            self.conn.close()
            self.conn = None

    def check_db(self):

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
        if (n): return True

        return False

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

    def create_run(self,run_nr,run_name,run_user,run_type,run_comment):

        self.check_db()
        c = self.conn.cursor()

        # Get run_type id
        c.execute("""SELECT id FROM run_type WHERE type = %s""",(run_type,))
        if (c.rowcount == 0):
            print "PadmeDB::create_run - WARNING - Unknown run type selected: %s - Defaulting to OTHER"%run_type
            c.execute("""SELECT id FROM run_type WHERE type = %s""",("OTHER",))
        res = c.fetchone()
        (run_type_id,) = res
        self.conn.commit()

        # Create run
        try:
            c.execute("""INSERT INTO run (number,name,user,run_type_id,status) VALUES (%s,%s,%s,%s,%s)""",(run_nr,run_name,run_user,run_type_id,0))
        except MySQLdb.Error as e:
            print "MySQL Error:%d:%s"%(e.args[0],e.args[1])
        self.conn.commit()

        # Create start of run comment
        try:
            c.execute("""INSERT INTO log_entry (run_number,type,level,time,text) VALUES (%s,%s,%s,%s,%s)""",(run_nr,"SOR",0,self.now_str(),run_comment))
        except MySQLdb.Error as e:
            print "MySQL Error:%d:%s"%(e.args[0],e.args[1])
        self.conn.commit()

    def create_process(self,run_number,proc_type,node_id):

        self.check_db()
        c = self.conn.cursor()

        # Check if run number exists
        c.execute("""SELECT number FROM run WHERE number = %s""",(run_number,))
        if c.rowcount == 0:
            print "PadmeDB::create_process - ERROR - Unknown run number: %d\n"%run_number
            return -1

        # Get process type id
        c.execute("""SELECT id FROM process_type WHERE type = %s""",(proc_type,))
        if c.rowcount == 0:
            print "PadmeDB::create_process - ERROR - Unknown process type: %s\n"%proc_type
            return -1
        (proc_type_id,) = c.fetchone()

        # Create process and get its id
        try:
            c.execute("""INSERT INTO process (run_number,proc_type_id,status) VALUES (%s,%s,%s)""",(run_number,proc_type_id,0))
        except MySQLdb.Error as e:
            print "MySQL Error:%d:%s"%(e.args[0],e.args[1])
            return -1
        process_id = c.lastrowid

        # Check if node exists and update process info
        if node_id:
            c.execute("""SELECT id FROM node WHERE id = %s""",(node_id,))
            if (c.rowcount == 0):
                print "PadmeDB::create_process - WARNING - Unknown node id: %d\n"%node_id
            else:
                try:
                    c.execute("""UPDATE process SET node_id = %s WHERE id = %s""",(node_id,process_id))
                except MySQLdb.Error as e:
                    print "MySQL Error:%d:%s"%(e.args[0],e.args[1])

        self.conn.commit()

        return process_id

   def create_daq_process(self,run_number,node_id):

        # Create process in database
        process_id = self.create_process(run_number,"ADCDAQ",node_id)

        return process_id
        
    def create_zsup_process(self,run_number,node_id):

        # Create process in database
        process_id = self.create_process(run_number,"ZEROSUP",node_id)

        return process_id
        
    def create_trigger_process(self,run_number,node_id):

        # Create process in database
        process_id = self.create_process(run_number,"TRIGGER",node_id)

        return process_id
        
    def create_merger_process(self,run_number,node_id):

        # Create process in database
        process_id = self.create_process(run_number,"MERGER",node_id)

        return process_id
        
    def create_level1_process(self,run_number,node_id):

        # Create process in database
        process_id = self.create_process(run_number,"LEVEL1",node_id)

        return process_id

    def add_daq_process_optical_link(self,proc_id,node_id,conet2_link,conet2_slot):

        link_id = self.get_link_id(node_id,conet2_link/8,conet2_link%8,conet2_slot)
        if link_id == -1:
            print "PadmeDB::add_daq_process_optical_link - WARNING - Cannot get link for (process,node,link,slot)=(%d,%d,%d,%d)"%(proc_id,node_id,conet2_link,conet2_slot)
        else:
            try:
                c.execute("""INSERT INTO daq_link (process_id,optical_link_id) VALUES (%s,%s)""",(proc_id,link_id))
            except MySQLdb.Error as e:
                print "MySQL Error:%d:%s"%(e.args[0],e.args[1])

    def set_run_status(self,run_nr,status):

        self.check_db()
        c = self.conn.cursor()
        try:
            c.execute("""UPDATE run SET status = %s WHERE number = %s""",(status,run_nr))
        except MySQLdb.Error as e:
            print "MySQL Error:%d:%s"%(e.args[0],e.args[1])
        self.conn.commit()

    def set_run_time_init(self,run_nr):

        self.check_db()
        c = self.conn.cursor()
        try:
            c.execute("""UPDATE run SET time_init = %s WHERE number = %s""",(self.now_str(),run_nr))
        except MySQLdb.Error as e:
            print "MySQL Error:%d:%s"%(e.args[0],e.args[1])
        self.conn.commit()

    def set_run_time_start(self,run_nr):

        self.check_db()
        c = self.conn.cursor()
        try:
            c.execute("""UPDATE run SET time_start = %s WHERE number = %s""",(self.now_str(),run_nr))
        except MySQLdb.Error as e:
            print "MySQL Error:%d:%s"%(e.args[0],e.args[1])
        self.conn.commit()

    def set_run_time_stop(self,run_nr):

        self.check_db()
        c = self.conn.cursor()
        try:
            c.execute("""UPDATE run SET time_stop = %s WHERE number = %s""",(self.now_str(),run_nr))
        except MySQLdb.Error as e:
            print "MySQL Error:%d:%s"%(e.args[0],e.args[1])
        self.conn.commit()

    def set_process_time_create(self,proc_id):

        self.check_db()
        c = self.conn.cursor()
        try:
            c.execute("""UPDATE process SET time_create = %s WHERE id = %s""",(self.now_str(),proc_id))
        except MySQLdb.Error as e:
            print "MySQL Error:%d:%s"%(e.args[0],e.args[1])
        self.conn.commit()

    def set_process_time_end(self,proc_id):

        self.check_db()
        c = self.conn.cursor()
        try:
            c.execute("""UPDATE process SET time_end = %s WHERE id = %s""",(self.now_str(),proc_id))
        except MySQLdb.Error as e:
            print "MySQL Error:%d:%s"%(e.args[0],e.args[1])
        self.conn.commit()

    def set_run_comment_end(self,run_nr,comment_end):

        # Create end of run comment
        self.check_db()
        c = self.conn.cursor()
        try:
            c.execute("""INSERT INTO log_entry (run_number,type,level,time,text) VALUES (%s,%s,%s,%s,%s)""",
                      (run_nr,"EOR",0,self.now_str(),comment_end))
        except MySQLdb.Error as e:
            print "MySQL Error:%d:%s"%(e.args[0],e.args[1])
        self.conn.commit()

    def set_run_total_events(self,run_nr,total_events):

        # Add total number of events info to run
        self.check_db()
        c = self.conn.cursor()
        try:
            c.execute("""UPDATE run SET total_events = %s WHERE number = %s""",(total_events,run_nr))
        except MySQLdb.Error as e:
            print "MySQL Error:%d:%s"%(e.args[0],e.args[1])
        self.conn.commit()

    def add_cfg_para_run(self,run_nr,para_name,para_val):

        self.check_db()
        para_id = self.get_para_id(para_name)
        c = self.conn.cursor()
        try:
            c.execute("""INSERT INTO run_config_para (run_number,config_para_name_id,value) VALUES (%s,%s,%s)""",
                      (run_nr,para_id,para_val))
        except MySQLdb.Error as e:
            print "MySQL Error:%d:%s"%(e.args[0],e.args[1])
        self.conn.commit()

    def add_cfg_para_proc(self,proc_id,para_name,para_val):

        self.check_db()
        para_id = self.get_para_id(para_name)
        c = self.conn.cursor()
        try:
            c.execute("""INSERT INTO proc_config_para (process_id,config_para_name_id,value) VALUES (%s,%s,%s)""",
                      (proc_id,para_id,para_val))
        except MySQLdb.Error as e:
            print "MySQL Error:%d:%s"%(e.args[0],e.args[1])
        self.conn.commit()

    def get_para_id(self,para_name):

        para_id = None
        self.check_db()
        c = self.conn.cursor()
        c.execute("""SELECT id FROM config_para_name WHERE name=%s""",(para_name,))
        res = c.fetchone()
        if (res == None):
            try:
                c.execute("""INSERT INTO config_para_name (name) VALUES (%s)""",(para_name,))
                para_id = c.lastrowid
            except MySQLdb.Error as e:
                print "MySQL Error:%d:%s"%(e.args[0],e.args[1])
        else:
            (para_id,) = res
        self.conn.commit()

        return para_id

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
                (node_id,) = res
                return node_id

        else:

            # If node name does not end with .lnf.infn.it, add it before searching (allow localhost)
            res = re.match("^\w*\.lnf\.infn\.it$",node)
            if (res == None and node != "localhost"): node += ".lnf.infn.it"

            # Search by name
            c.execute("""SELECT id FROM node WHERE name=%s""",(node,))
            res = c.fetchone()
            self.conn.commit()
            if (res != None):
                (node_id,) = res
                return node_id

        # If not found, return -1
        return -1

    def get_node_daq_ip(self,node_id):

        # Return IP address on DAQ VLAN (192.168.60.X) of node with given ID
 
        self.check_db()
        c = self.conn.cursor()

        c.execute("""SELECT ip_addr_daq FROM node WHERE id=%s""",(node_id,))
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

        c.execute("""SELECT type FROM run_type""")
        data = c.fetchall()
        self.conn.commit()

        type_list = []
        for row in data: type_list.append(row[0])

        return type_list

    def get_process_types(self):

        # Return list of process types known to DB

        self.check_db()
        c = self.conn.cursor()

        c.execute("""SELECT type FROM process_type""")
        data = c.fetchall()
        self.conn.commit()

        type_list = []
        for row in data: type_list.append(row[0])

        return type_list

    def get_file_types(self):

        # Return list of file types known to DB

        self.check_db()
        c = self.conn.cursor()

        c.execute("""SELECT type FROM file_type""")
        data = c.fetchall()
        self.conn.commit()

        type_list = []
        for row in data: type_list.append(row[0])

        return type_list

    def get_link_id(self,node_id,controller_id,channel_id,slot_id):

        # Return id of optical link given its description
        
        link_id = -1

        self.check_db()
        c = self.conn.cursor()

        c.execute("""SELECT id FROM optical_link WHERE node_id=%s AND controller_id=%s AND channel_id=%s AND slot_id=%s""",(node_id,controller_id,channel_id,slot_id))
        ret = c.fetchone()
        if ret != None: (link_id,) = ret

        self.conn.commit()

        return link_id

    def get_merger_final_info(self,merger_proc_id):

        # Return final events and size info from merger

        tot_evts = -1
        tot_size = -1
        
        self.check_db()
        c = self.conn.cursor()

        c.execute("""SELECT total_events,total_size FROM process WHERE id=%s""",(merger_proc_id,))
        ret = c.fetchone()
        if ret != None: (tot_evts,tot_size) = ret

        self.conn.commit()

        return (tot_evts,tot_size)

    def now_str(self):
        return time.strftime("%Y-%m-%d %H:%M:%S",time.gmtime())
