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

        # DB codes for run status
        self.DB_RUN_STATUS_INITIALIZED = 1
        self.DB_RUN_STATUS_RUNNING     = 2
        self.DB_RUN_STATUS_END_OK      = 3
        self.DB_RUN_STATUS_ABORTED     = 4
        self.DB_RUN_STATUS_INIT_ERROR  = 5
        self.DB_RUN_STATUS_END_ERROR   = 6
        self.DB_RUN_STATUS_UNKNOWN     = 7

        # Create regexps used to decode DBINFO lines
        self.init_dbinfo_regexp()

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

    def create_run(self,run_nr,run_name,run_type):

        # Run is always created with status 0
        status = 0

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
            c.execute("""INSERT INTO run (number,name,run_type_id,status) VALUES (%s,%s,%s,%s)""",(run_nr,run_name,run_type_id,status))
        except MySQLdb.Error as e:
            print "PadmeDB::create_run - MySQL Error:%d:%s"%(e.args[0],e.args[1])
        self.conn.commit()

    def set_run_status(self,run_nr,status):

        self.check_db()
        c = self.conn.cursor()
        try:
            c.execute("""UPDATE run SET status = %s WHERE number = %s""",(status,run_nr))
        except MySQLdb.Error as e:
            print "PadmeDB::set_run_status - MySQL Error:%d:%s"%(e.args[0],e.args[1])
        self.conn.commit()

    def set_run_time_create(self,run_nr,time_create):

        self.check_db()
        c = self.conn.cursor()
        try:
            c.execute("""UPDATE run SET time_create = %s WHERE number = %s""",(time_create,run_nr))
        except MySQLdb.Error as e:
            print "PadmeDB::set_run_time_create - MySQL Error:%d:%s"%(e.args[0],e.args[1])
        self.conn.commit()

    def set_run_time_init(self,run_nr,time_init):

        self.check_db()
        c = self.conn.cursor()
        try:
            c.execute("""UPDATE run SET time_init = %s WHERE number = %s""",(time_init,run_nr))
        except MySQLdb.Error as e:
            print "PadmeDB::set_run_time_init - MySQL Error:%d:%s"%(e.args[0],e.args[1])
        self.conn.commit()

    def set_run_time_start(self,run_nr,time_start):

        self.check_db()
        c = self.conn.cursor()
        try:
            c.execute("""UPDATE run SET time_start = %s WHERE number = %s""",(time_start,run_nr))
        except MySQLdb.Error as e:
            print "PadmeDB::set_run_time_start - MySQL Error:%d:%s"%(e.args[0],e.args[1])
        self.conn.commit()

    def set_run_time_stop(self,run_nr,time_stop):

        self.check_db()
        c = self.conn.cursor()
        try:
            c.execute("""UPDATE run SET time_stop = %s WHERE number = %s""",(time_stop,run_nr))
        except MySQLdb.Error as e:
            print "PadmeDB::set_run_time_stop - MySQL Error:%d:%s"%(e.args[0],e.args[1])
        self.conn.commit()

    def set_run_user(self,run_nr,user):

        # Create end of run comment
        self.check_db()
        c = self.conn.cursor()
        try:
            c.execute("""UPDATE run SET user = %s WHERE number = %s""",(user,run_nr))
        except MySQLdb.Error as e:
            print "PadmeDB::set_run_user - MySQL Error:%d:%s"%(e.args[0],e.args[1])
        self.conn.commit()

    def add_run_log_entry(self,run_nr,comm_type,comm_level,comm_time,comment):

        # Add log comment to run
        self.check_db()
        c = self.conn.cursor()
        try:
            c.execute("""INSERT INTO log_entry (run_number,type,level,time,text) VALUES (%s,%s,%s,%s,%s)""",
                      (run_nr,comm_type,comm_level,comm_time,comment))
        except MySQLdb.Error as e:
            print "PadmeDB::set_run_log_entry - MySQL Error:%d:%s"%(e.args[0],e.args[1])
        self.conn.commit()

    def set_run_comment_start(self,run_nr,comm_time,comment):

        # Add start-of-run comment to run
        self.add_run_log_entry(run_nr,"SOR",0,comm_time,comment)

    def set_run_comment_end(self,run_nr,comm_time,comment):

        # Add end-of-run comment to run
        self.add_run_log_entry(run_nr,"EOR",0,comm_time,comment)

    def set_run_total_events(self,run_nr,total_events):

        # Add total number of events info to run
        self.check_db()
        c = self.conn.cursor()
        try:
            c.execute("""UPDATE run SET total_events = %s WHERE number = %s""",(total_events,run_nr))
        except MySQLdb.Error as e:
            print "PadmeDB::set_run_total_events - MySQL Error:%d:%s"%(e.args[0],e.args[1])
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
            c.execute("""INSERT INTO process (run_number,process_type_id,status) VALUES (%s,%s,%s)""",(run_number,proc_type_id,0))
        except MySQLdb.Error as e:
            print "PadmeDB::create_process - MySQL Error:%d:%s"%(e.args[0],e.args[1])
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
                    print "PadmeDB::create_process - MySQL Error:%d:%s"%(e.args[0],e.args[1])

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

        # Add optical link to DAQ process
        self.check_db()
        c = self.conn.cursor()

        link_id = self.get_link_id(node_id,conet2_link/8,conet2_link%8,conet2_slot)
        if link_id == -1:
            print "PadmeDB::add_daq_process_optical_link - WARNING - Cannot get link for (process,node,link,slot)=(%d,%d,%d,%d)"%(proc_id,node_id,conet2_link,conet2_slot)
        else:
            try:
                c.execute("""INSERT INTO daq_link (process_id,optical_link_id) VALUES (%s,%s)""",(proc_id,link_id))
            except MySQLdb.Error as e:
                print "PadmeDB::add_daq_process_optical_link - MySQL Error:%d:%s"%(e.args[0],e.args[1])

    def add_proc_log_entry(self,proc_id,comm_type,comm_level,comm_time,comment):

        # Add log comment to run
        self.check_db()
        c = self.conn.cursor()
        try:
            c.execute("""INSERT INTO log_entry (run_number,type,level,time,text) VALUES (%s,%s,%s,%s,%s)""",
                      (run_nr,comm_type,comm_level,comm_time,comment))
        except MySQLdb.Error as e:
            print "PadmeDB::add_proc_log_entry - MySQL Error:%d:%s"%(e.args[0],e.args[1])
        self.conn.commit()

    def set_process_status(self,proc_id,status):

        self.check_db()
        c = self.conn.cursor()
        try:
            c.execute("""UPDATE process SET status = %s WHERE id = %s""",(status,proc_id))
        except MySQLdb.Error as e:
            print "PadmeDB::set_process_status - MySQL Error:%d:%s"%(e.args[0],e.args[1])
        self.conn.commit()

    def set_process_time_create(self,proc_id,time_create):

        self.check_db()
        c = self.conn.cursor()
        try:
            c.execute("""UPDATE process SET time_create = %s WHERE id = %s""",(time_create,proc_id))
        except MySQLdb.Error as e:
            print "PadmeDB::set_process_time_create - MySQL Error:%d:%s"%(e.args[0],e.args[1])
        self.conn.commit()

    def set_process_time_end(self,proc_id,time_end):

        self.check_db()
        c = self.conn.cursor()
        try:
            c.execute("""UPDATE process SET time_end = %s WHERE id = %s""",(time_end,proc_id))
        except MySQLdb.Error as e:
            print "PadmeDB::set_process_time_end - MySQL Error:%d:%s"%(e.args[0],e.args[1])
        self.conn.commit()

    def set_process_time_start(self,proc_id,time_start):

        self.check_db()
        c = self.conn.cursor()
        try:
            c.execute("""UPDATE process SET time_start = %s WHERE id = %s""",(time_start,proc_id))
        except MySQLdb.Error as e:
            print "PadmeDB::set_process_time_start - MySQL Error:%d:%s"%(e.args[0],e.args[1])
        self.conn.commit()

    def set_process_time_stop(self,proc_id,time_stop):

        self.check_db()
        c = self.conn.cursor()
        try:
            c.execute("""UPDATE process SET time_stop = %s WHERE id = %s""",(time_stop,proc_id))
        except MySQLdb.Error as e:
            print "PadmeDB::set_process_time_stop - MySQL Error:%d:%s"%(e.args[0],e.args[1])
        self.conn.commit()

    def set_process_total_events(self,proc_id,events):

        self.check_db()
        c = self.conn.cursor()
        try:
            c.execute("""UPDATE process SET total_events = %s WHERE id = %s""",(events,proc_id))
        except MySQLdb.Error as e:
            print "PadmeDB::set_process_total_events - MySQL Error:%d:%s"%(e.args[0],e.args[1])
        self.conn.commit()

    def set_process_total_size(self,proc_id,size):

        self.check_db()
        c = self.conn.cursor()
        try:
            c.execute("""UPDATE process SET total_size = %s WHERE id = %s""",(size,proc_id))
        except MySQLdb.Error as e:
            print "PadmeDB::set_process_total_size - MySQL Error:%d:%s"%(e.args[0],e.args[1])
        self.conn.commit()

    def set_process_n_files(self,proc_id,n_files):

        self.check_db()
        c = self.conn.cursor()
        try:
            c.execute("""UPDATE process SET n_files = %s WHERE id = %s""",(n_files,proc_id))
        except MySQLdb.Error as e:
            print "PadmeDB::set_process_n_files - MySQL Error:%d:%s"%(e.args[0],e.args[1])
        self.conn.commit()

    def create_file(self,file_name,file_type,fmt_version,proc_id,file_index):

        self.check_db()
        c = self.conn.cursor()

        # Check if process exists
        c.execute("""SELECT id FROM process WHERE id = %s""",(proc_id,))
        if c.rowcount == 0:
            print "PadmeDB::create_file - ERROR - Unknown process_id: %d\n"%proc_id
            return -1

        # Get file type id
        c.execute("""SELECT id FROM file_type WHERE type = %s""",(file_type,))
        if c.rowcount == 0:
            print "PadmeDB::create_file - ERROR - Unknown file type: %s\n"%file_type
            return -1
        (file_type_id,) = c.fetchone()

        # Create file and get its id
        try:
            c.execute("""INSERT INTO file (name,file_type_id,version,process_id,part,status) VALUES (%s,%s,%s,%s,%s,%s)""",(os.path.basename(file_name),file_type_id,fmt_version,proc_id,file_index,0))
        except MySQLdb.Error as e:
            print "PadmeDB::create_file - MySQL Error:%d:%s"%(e.args[0],e.args[1])
            return -1
        file_id = c.lastrowid

        self.conn.commit()

        return file_id

    def set_file_time_open(self,file_name,time_open):

        self.check_db()
        c = self.conn.cursor()
        try:
            c.execute("""UPDATE file SET time_open = %s WHERE name = %s""",(time_open,os.path.basename(file_name)))
        except MySQLdb.Error as e:
            print "PadmeDB::set_file_time_open - MySQL Error:%d:%s"%(e.args[0],e.args[1])
        self.conn.commit()

    def set_file_time_close(self,file_name,time_close):

        self.check_db()
        c = self.conn.cursor()
        try:
            c.execute("""UPDATE file SET time_close = %s WHERE name = %s""",(time_close,os.path.basename(file_name)))
        except MySQLdb.Error as e:
            print "PadmeDB::set_file_time_close - MySQL Error:%d:%s"%(e.args[0],e.args[1])
        self.conn.commit()

    def set_file_n_events(self,file_name,n_events):

        self.check_db()
        c = self.conn.cursor()
        try:
            c.execute("""UPDATE file SET n_events = %s WHERE name = %s""",(n_events,os.path.basename(file_name)))
        except MySQLdb.Error as e:
            print "PadmeDB::set_file_n_events - MySQL Error:%d:%s"%(e.args[0],e.args[1])
        self.conn.commit()

    def set_file_size(self,file_name,size):

        self.check_db()
        c = self.conn.cursor()
        try:
            c.execute("""UPDATE file SET size = %s WHERE name = %s""",(size,os.path.basename(file_name)))
        except MySQLdb.Error as e:
            print "PadmeDB::set_file_size - MySQL Error:%d:%s"%(e.args[0],e.args[1])
        self.conn.commit()

    def set_file_adler32(self,file_name,adler32):

        self.check_db()
        c = self.conn.cursor()
        try:
            c.execute("""UPDATE file SET adler32 = %s WHERE name = %s""",(adler32,os.path.basename(file_name)))
        except MySQLdb.Error as e:
            print "PadmeDB::set_file_adler32 - MySQL Error:%d:%s"%(e.args[0],e.args[1])
        self.conn.commit()

    def add_cfg_para_run(self,run_nr,para_name,para_val):

        self.check_db()
        para_id = self.get_para_id(para_name)
        c = self.conn.cursor()
        try:
            c.execute("""INSERT INTO run_config_para (run_number,config_para_name_id,value) VALUES (%s,%s,%s)""",
                      (run_nr,para_id,para_val))
        except MySQLdb.Error as e:
            print "PadmeDB::add_cfg_para_run - MySQL Error:%d:%s"%(e.args[0],e.args[1])
        self.conn.commit()

    def add_cfg_para_proc(self,proc_id,para_name,para_value):

        self.check_db()
        para_id = self.get_para_id(para_name)
        c = self.conn.cursor()
        try:
            c.execute("""INSERT INTO proc_config_para (process_id,config_para_name_id,value) VALUES (%s,%s,%s)""",
                      (proc_id,para_id,para_value))
        except MySQLdb.Error as e:
            print "PadmeDB::add_cfg_para_proc - MySQL Error:%d:%s"%(e.args[0],e.args[1])
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
                print "PadmeDB::get_para_id - MySQL Error:%d:%s"%(e.args[0],e.args[1])
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

    def init_dbinfo_regexp(self):

        # All DBINFO lines start with "DBINFO - <date> <time> - ", e.g. "DBINFO - 2019/10/08 12:32:44 - "
        dbinfo_head = "^\s*DBINFO\s+-\s+(\S+)\s+(\S+)\s+-\s+"

        # file_create <file_name> <file_type> <format_version> <process_id> <file_index>
        self.re_file_create = re.compile("%sfile_create\s+(\S+)\s+(\S+)\s+(\d+)\s+(\d+)\s*$"%dbinfo_head)

        # file_set_time_open <file_name> <date> <time>
        self.re_file_set_time_open = re.compile("%sfile_set_time_open\s+(\S+)\s+(\S+)\s+(\S+)\s*$"%dbinfo_head)

        # file_set_time_close <file_name> <date> <time>
        self.re_file_set_time_close = re.compile("%sfile_set_time_close\s+(\S+)\s+(\S+)\s+(\S+)\s*$"%dbinfo_head)

        # file_set_n_events <file_name> <n_events>
        self.re_file_set_n_events = re.compile("%sfile_set_n_events\s+(\S+)\s+(\d+)\s*$"%dbinfo_head)

        # file_set_size <file_name> <size>
        self.re_file_set_size = re.compile("%sfile_set_size\s+(\S+)\s+(\d+)\s*$"%dbinfo_head)

        # process_set_status <status>
        self.re_process_set_status = re.compile("%sprocess_set_status\s+(\d+)\s*$"%dbinfo_head)

        # process_set_time_start <date> <time>
        self.re_process_set_time_start = re.compile("%sprocess_set_time_start\s+(\S+)\s+(\S+)\s*$"%dbinfo_head)

        # process_set_time_stop <date> <time>
        self.re_process_set_time_stop = re.compile("%sprocess_set_time_stop\s+(\S+)\s+(\S+)\s*$"%dbinfo_head)

        # process_set_total_events <total_events>
        self.re_process_set_total_events = re.compile("%sprocess_set_total_events\s+(\d+)\s*$"%dbinfo_head)

        # process_set_total_size <total_size>
        self.re_process_set_total_size = re.compile("%sprocess_set_total_size\s+(\d+)\s*$"%dbinfo_head)

        # process_set_n_files <n_files>
        self.re_process_set_n_files = re.compile("%sprocess_set_n_files\s+(\d+)\s*$"%dbinfo_head)

        # add_proc_config_para <parameter_name> <parameter_value>
        self.re_add_proc_config_para = re.compile("%sadd_proc_config_para\s+(\S+)\s+(\S.*)$"%dbinfo_head)

        # add_proc_log_entry <type> <level> <text>
        self.re_add_proc_log_entry = re.compile("%sadd_proc_log_entry\s+(\S+)\s+(\d+)\s+(\S.*)$"%dbinfo_head)

    def manage_dbinfo_entry(self,proc_id,dbinfo_line):

        m = self.re_file_create.match(dbinfo_line)
        if m:
            dbinfo_date    = m.group(1)
            dbinfo_time    = m.group(2)
            file_name      = m.group(3)
            file_type      = m.group(4)
            format_version = int(m.group(5))
            file_index     = int(m.group(6))
            self.create_file(file_name,file_type,format_version,proc_id,file_index)
            return True

        m = self.re_file_set_time_open.match(dbinfo_line)
        if m:
            dbinfo_date = m.group(1)
            dbinfo_time = m.group(2)
            file_name   = m.group(3)
            open_date   = m.group(4)
            open_time   = m.group(5)
            self.set_file_time_open(file_name,"%s %s"%(open_date,open_time))
            return True

        m = self.re_file_set_time_close.match(dbinfo_line)
        if m:
            dbinfo_date = m.group(1)
            dbinfo_time = m.group(2)
            file_name   = m.group(3)
            close_date  = m.group(4)
            close_time  = m.group(5)
            self.set_file_time_close(file_name,"%s %s"%(close_date,close_time))
            return True

        m = self.re_file_set_n_events.match(dbinfo_line)
        if m:
            dbinfo_date = m.group(1)
            dbinfo_time = m.group(2)
            file_name   = m.group(3)
            n_events    = int(m.group(4))
            self.set_file_n_events(file_name,n_events)
            return True

        m = self.re_file_set_size.match(dbinfo_line)
        if m:
            dbinfo_date = m.group(1)
            dbinfo_time = m.group(2)
            file_name   = m.group(3)
            size        = int(m.group(4))
            self.set_file_size(file_name,size)
            return True

        m = self.re_process_set_status.match(dbinfo_line)
        if m:
            dbinfo_date = m.group(1)
            dbinfo_time = m.group(2)
            status      = int(m.group(3))
            self.set_process_status(proc_id,status)
            return True

        m = self.re_process_set_time_start.match(dbinfo_line)
        if m:
            dbinfo_date = m.group(1)
            dbinfo_time = m.group(2)
            start_date  = m.group(3)
            start_time  = m.group(4)
            self.set_process_time_start(proc_id,"%s %s"%(start_date,start_time))
            return True

        m = self.re_process_set_time_stop.match(dbinfo_line)
        if m:
            dbinfo_date = m.group(1)
            dbinfo_time = m.group(2)
            stop_date   = m.group(3)
            stop_time   = m.group(4)
            self.set_process_time_stop(proc_id,"%s %s"%(stop_date,stop_time))
            return True

        m = self.re_process_set_total_events.match(dbinfo_line)
        if m:
            dbinfo_date = m.group(1)
            dbinfo_time = m.group(2)
            tot_events  = int(m.group(3))
            self.set_process_total_events(proc_id,tot_events)
            return True

        m = self.re_process_set_total_size.match(dbinfo_line)
        if m:
            dbinfo_date = m.group(1)
            dbinfo_time = m.group(2)
            tot_size    = int(m.group(3))
            self.set_process_total_size(proc_id,tot_size)
            return True

        m = self.re_process_set_n_files.match(dbinfo_line)
        if m:
            dbinfo_date = m.group(1)
            dbinfo_time = m.group(2)
            n_files     = int(m.group(3))
            self.set_process_n_files(proc_id,n_files)
            return True

        m = self.re_add_proc_config_para.match(dbinfo_line)
        if m:
            dbinfo_date = m.group(1)
            dbinfo_time = m.group(2)
            para_name   = m.group(3)
            para_value  = m.group(4)
            self.add_cfg_para_proc(proc_id,para_name,para_value)
            return True

        m = self.re_add_proc_log_entry.match(dbinfo_line)
        if m:
            dbinfo_date = m.group(1)
            dbinfo_time = m.group(2)
            log_type    = m.group(3)
            log_level   = int(m.group(4))
            log_text    = m.group(5)
            self.add_proc_log_entry(proc_id,log_type,log_level,"%s %s"%(dbinfo_date,dbinfo_time),log_text)
            return True

        print "PadmeDB::manage_dbinfo_entry - WARNING - DBINFO line has unknown format\n%s"%dbinfo_line
        return False

    def now_str(self):
        return time.strftime("%Y-%m-%d %H:%M:%S",time.gmtime())
