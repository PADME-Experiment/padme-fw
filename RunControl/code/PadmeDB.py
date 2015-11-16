import MySQLdb
import os

class PadmeDB:

    def __init__(self,db_file):

        self.conn = 0
        #self.set_db_file(db_file)
        self.conn = self.connect_db()

    def __del__(self):

        if (self.conn):
            self.conn.close()

    def connect_db(self):


        if (self.conn):
            self.conn.close()

        # Get DB connection parameters from environment variables
        DB_HOST   = os.getenv('PADME_DB_HOST'  ,'localhost')
        DB_PORT   = os.getenv('PADME_DB_PORT'  ,'5501')
        DB_USER   = os.getenv('PADME_DB_USER'  ,'padme')
        DB_PASSWD = os.getenv('PADME_DB_PASSWD','unknown')
        DB_NAME   = os.getenv('PADME_DB_NAME'  ,'PadmeDB')

        return MySQLdb.connect(host=DB_HOST,port=int(DB_PORT),user=DB_USER,passwd=DB_PASSWD,db=DB_NAME)

    def is_run_in_db(self,run_nr):

        c = self.conn.cursor()
        c.execute("""SELECT COUNT(number) FROM run WHERE number=%s""",(run_nr,))
        (n,) = c.fetchone()
        self.conn.commit()
        if (n): return 1

        return 0

    def get_last_run_in_db(self):

        c = self.conn.cursor()
        c.execute("""SELECT MAX(number) FROM run""")
        (maxrun,) = c.fetchone()
        self.conn.commit()
        if (maxrun == None):
            return 0
        else:
            return maxrun

    def create_run(self,run_nr,run_type,run_user,run_comment):

        if (run_nr!=0):
            c = self.conn.cursor()
            c.execute("""INSERT INTO run (number,type,status,time_init,time_start,time_stop,total_events,user,comment_start,comment_end) VALUES (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)""",(run_nr,run_type,0,0,0,0,0,run_user,run_comment,""))
            self.conn.commit()

    def set_run_status(self,run_nr,status):

        if (run_nr!=0):
            c = self.conn.cursor()
            c.execute("""UPDATE run SET status = %s WHERE number = %s""",(status,run_nr))
            self.conn.commit()

    def set_run_time_init(self,run_nr,time_init):

        if (run_nr!=0):
            c = self.conn.cursor()
            c.execute("""UPDATE run SET time_init = %s WHERE number = %s""",(time_init,run_nr))
            self.conn.commit()

    def set_run_time_start(self,run_nr,time_start):

        if (run_nr!=0):
            c = self.conn.cursor()
            c.execute("""UPDATE run SET time_start = %s WHERE number = %s""",(time_start,run_nr))
            self.conn.commit()

    def set_run_time_stop(self,run_nr,time_stop):

        if (run_nr!=0):
            c = self.conn.cursor()
            c.execute("""UPDATE run SET time_stop = %s WHERE number = %s""",(time_stop,run_nr))
            self.conn.commit()

    def set_run_comment_end(self,run_nr,comment_end):

        if (run_nr!=0):
            c = self.conn.cursor()
            c.execute("""UPDATE run SET comment_end = %s WHERE number = %s""",(comment_end,run_nr))
            self.conn.commit()

    def add_cfg_para(self,run_nr,para_name,para_val):

        if (run_nr!=0):
            para_id = self.get_para_id(para_name)
            c = self.conn.cursor()
            c.execute("""INSERT INTO run_config_para (run_number,config_para_name_id,value) VALUES (%s,%s,%s)""",(run_nr,para_id,para_val))
            self.conn.commit()

    def get_para_id(self,para_name):

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
