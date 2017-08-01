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
        DB_NAME   = os.getenv('PADME_DB_NAME'  ,'PadmeDB')

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
        #c.execute("""INSERT INTO run (number,type,status,time_init,time_start,time_stop,total_events,user,comment_start,comment_end) VALUES (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)""",(run_nr,run_type,0,0,0,0,0,run_user,run_comment,""))
        c.execute("""INSERT INTO run (number,type,status,total_events,user,comment_start) VALUES (%s,%s,%s,%s,%s,%s)""",(run_nr,run_type,0,0,run_user,run_comment))
        self.conn.commit()

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

    def add_cfg_para(self,run_nr,para_name,para_val):

        self.check_db()
        para_id = self.get_para_id(para_name)
        c = self.conn.cursor()
        c.execute("""INSERT INTO run_config_para (run_number,config_para_name_id,value) VALUES (%s,%s,%s)""",(run_nr,para_id,para_val))
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
