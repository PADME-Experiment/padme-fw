#import MySQLdb
import os

class PadmeMCDB:

    def __init__(self):

        self.conn = None

    def __del__(self):

        self.close_db()

    def connect_db(self):

        self.close_db()

        # Get DB connection parameters from environment variables
        DB_HOST   = os.getenv('PADME_MCDB_HOST'  ,'localhost')
        DB_PORT   = os.getenv('PADME_MCDB_PORT'  ,'5501')
        DB_USER   = os.getenv('PADME_MCDB_USER'  ,'padme')
        DB_PASSWD = os.getenv('PADME_MCDB_PASSWD','unknown')
        DB_NAME   = os.getenv('PADME_MCDB_NAME'  ,'PadmeMCDB')

        #self.conn = MySQLdb.connect(host=DB_HOST,port=int(DB_PORT),user=DB_USER,passwd=DB_PASSWD,db=DB_NAME)

    def close_db(self):

        if (self.conn):
            #self.conn.close()
            self.conn = None

    def check_db(self):

        # This does not work
        #if (self.conn and self.conn.is_connected()): return
        if self.conn:
            try:
                #self.conn.ping()
                pass
            except:
                self.connect_db()
        else:
            self.connect_db()

    def is_prod_in_db(self,prod_name):

        self.check_db()
        #c = self.conn.cursor()
        #c.execute("""SELECT COUNT(id) FROM production WHERE name=%s""",(prod_name,))
        #(n,) = c.fetchone()
        #self.conn.commit()
        #if (n): return 1

        return 0

    def create_prod(self,name,ce_uri,mc_version,prod_dir,config,storage_dir,n_jobs):

        self.check_db()
        #c = self.conn.cursor()
        #c.execute("""INSERT INTO production (name,description,user_req,n_events_req,ce_uri,mc_version,prod_dir,configuration,storage_dir,status,n_jobs) (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)""",(name,"TEST","EL",100,ce_uri,mc_version,prod_dir,config,storage_dir,0,n_jobs))
        #self.conn.commit()

    def get_prod_id(self,name):

        self.check_db()
        #c = self.conn.cursor()
        #c.execute("""SELECT id FROM production WHERE name=%s""",(name,))
        #self.conn.commit()
        #res = c.fetchone()
        #self.conn.commit()
        #if (res == None): return -1
        #(id,) = res
        #return id
        return 0

    def create_job(self,prod_id,name,job_dir):

        self.check_db()
        #c = self.conn.cursor()
        #c.execute("""INSERT INTO job (production_id,name,job_dir,random,status) (%s,%s,%s,%s,0)""",(prod_id,name,job_dir,""))
        #self.conn.commit()

    def get_job_id(self,prod_id,name):

        self.check_db()
        #c = self.conn.cursor()
        #c.execute("""SELECT id FROM job WHERE production_id=%s AND name=%s""",(prod_id,name,))
        #res = c.fetchone()
        #self.conn.commit()
        #if (res == None): return -1
        #(id,) = res
        #return id
        return 0

    def get_job_info(self,job_id):

        self.check_db()
        #c = self.conn.cursor()
        #c.execute("""SELECT name,status,ce_job_id FROM job WHERE id=%s""",(job_id,))
        #res = c.fetchone()
        #self.conn.commit()
        #if (res == None): return ("",-1,"")
        #return res
        return ("",-1,"")

    def set_job_status(self,job_id,status):

        self.check_db()
        #c = self.conn.cursor()
        #c.execute("""UPDATE job SET status = %s WHERE id = %s""",(status,job_id))
        #self.conn.commit()

    def set_job_submitted(self,job_id,ce_job_id,time_submit):

        self.check_db()
        #c = self.conn.cursor()
        #c.execute("""UPDATE job SET status = 1, ce_job_id = %s, time_submit = %s WHERE id = %s""",(ce_job_id,time_submit,job_id))
        #self.conn.commit()

    def get_job_dir(self,job_id):

        self.check_db()
        #c = self.conn.cursor()
        #c.execute("""SELECT job_dir FROM job WHERE id=%s""",(job_id,))
        #res = c.fetchone()
        #self.conn.commit()
        #if (res == None): return ""
        #(job_dir,) = res
        #return job_dir
        return ""

    def set_job_time_start(self,job_id,time_start):

        self.check_db()
        #c = self.conn.cursor()
        #c.execute("""UPDATE job SET time_start = %s WHERE id = %s""",(time_start,job_id))
        #self.conn.commit()

    def set_job_time_end(self,job_id,time_end):

        self.check_db()
        #c = self.conn.cursor()
        #c.execute("""UPDATE job SET time_end = %s WHERE id = %s""",(time_end,job_id))
        #self.conn.commit()

    def set_job_worker_node(self,job_id,worker_node):

        self.check_db()
        #c = self.conn.cursor()
        #c.execute("""UPDATE job SET worker_node = %s WHERE id = %s""",(worker_node,job_id))
        #self.conn.commit()

    def create_job_file(self,job_id,file_name,file_size,file_adler32):

        self.check_db()
        #c = self.conn.cursor()
        #c.execute("""INSERT INTO file (job_id,name,size,adler32) (%s,%s,%s,%s)""",(job_id,file_name,file_size,file_adler32))
        #self.conn.commit()
