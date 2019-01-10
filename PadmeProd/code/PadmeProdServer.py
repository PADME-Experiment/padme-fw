#!/usr/bin/python

import os
import sys
import time
import subprocess
import re

from PadmeMCDB import PadmeMCDB
from Logger import Logger
from ProxyHandler import ProxyHandler

class PadmeProdServer:

    def __init__(self,prod_name):

        self.db = PadmeMCDB()
        self.ph = ProxyHandler()
        self.prod_name = prod_name
        self.n_events = 0
        self.prod_check_delay = 60
        self.start_production()

    def start_production(self):

        # This is the daemonized code to start and handle the full production
        # It is structured to only depend on DB information

        # For the time being, enable terminal output (remove when demonizing)
        #sys.stdout.interactive = True
        #sys.stderr.interactive = True
        # Verify that production exists in DB and retrieve production id
        if not self.db.is_prod_in_db(self.prod_name):
            print "*** ERROR *** Production '%s' not found in DB"%self.prod_name
            sys.exit(2)
        prod_id = self.db.get_prod_id(self.prod_name)
    
        # Get some info about this prod
        (dummy,prod_ce,prod_dir,proxy_file,prod_njobs) = self.db.get_prod_info(prod_id)
    
        # Check if production dir exists
        if not os.path.isdir(prod_dir):
            print "*** ERROR *** Production directory '%s' not found"%prod_dir
            sys.exit(2)
    
        # Check if proxy file exists
        if not os.path.isfile(proxy_file):
            print "*** ERROR *** Long-lived proxy file '%s' not found"%proxy_file
            sys.exit(2)

        # Redirect stdout and stderr to files with automatic time logging
        log_file_name = "%s/%s.log"%(prod_dir,self.prod_name)
        sys.stdout = Logger(log_file_name)
        err_file_name = "%s/%s.err"%(prod_dir,self.prod_name)
        sys.stderr = Logger(err_file_name)
            
        # Get list of job ids for this production
        job_id_list = self.db.get_job_list(prod_id)
        if len(job_id_list) != prod_njobs:
            print "*** ERROR *** Number of jobs in DB and in production are different: %s != %s"%(len(job_id_list),prod_njobs)
            sys.exit(2)
    
        print "=== Starting Production %s ==="%self.prod_name
    
        # Main production loop
        while True:
    
            # Renew proxy if needed
            self.ph.renew_voms_proxy(proxy_file)
    
            # Call method to check jobs status and handle each job accordingly
            (jobs_submitted,jobs_idle,jobs_active,jobs_success,jobs_fail,jobs_undef) = self.handle_jobs(prod_ce,job_id_list)
    
            # If all jobs are in a final state, production is over
            if jobs_submitted+jobs_idle+jobs_active == 0:
                print "*** No unfinished jobs left: exiting ***"
                break
    
            # Show current production state and sleep for a while
            print "Jobs: submitted %d idle %d running %d success %d fail %d undef %d"%(jobs_submitted,jobs_idle,jobs_active,jobs_success,jobs_fail,jobs_undef)
            time.sleep(self.prod_check_delay)
    
        # Production is over: tag it as done and say bye bye
        self.db.close_prod(prod_id,self.now_str(),jobs_success,self.n_events)
    
        print "=== Ending Production %s ==="%self.prod_name
        sys.exit(0)
    
    def handle_jobs(self,prod_ce,job_id_list):
    
        jobs_submitted = 0
        jobs_idle = 0
        jobs_active = 0
        jobs_success = 0
        jobs_fail = 0
        jobs_undef = 0
    
        print "--- Checking status of production jobs ---"
    
        for job_id in job_id_list:
    
            (job_name,job_status,job_dir,ce_job_id) = self.db.get_job_info(job_id)
    
            # If status is 0, job was not submitted yet: do it now and get updated information
            if job_status == 0:
                ce_job_id = self.submit_job(job_id,job_dir,prod_ce)
                print "- %s %s SUBMITTED"%(job_name,ce_job_id)
                jobs_submitted += 1
                continue
    
            # If job is already in final status, just count it
            if job_status == 3:
                jobs_success += 1
                continue
            if job_status == 4 or job_status == 5 or job_status == 6:
                jobs_fail += 1
                continue
            if job_status == 7:
                jobs_undef += 1
                continue
    
            # Last time job was idle or active: check if its status changed
            job_ce_status = self.get_job_ce_status(ce_job_id)
            print "- %s %s %s"%(job_name,ce_job_id,job_ce_status)
            if job_ce_status == "PENDING" or job_ce_status == "REGISTERED" or job_ce_status == "IDLE":
                jobs_idle += 1
            elif job_ce_status == "RUNNING" or job_ce_status == "REALLY-RUNNING":
                jobs_active += 1
                if job_status == 1: self.db.set_job_status(job_id,2)
            elif job_ce_status == "DONE-OK":
                if self.finalize_job(job_id,job_name,ce_job_id):
                    self.db.set_job_status(job_id,3)
                    jobs_success += 1
                else:
                    self.db.set_job_status(job_id,5)
                    jobs_fail += 1
            elif job_ce_status == "DONE-FAILED":
                if self.finalize_job(job_id,job_name,ce_job_id):
                    self.db.set_job_status(job_id,4)
                else:
                    self.db.set_job_status(job_id,6)
                jobs_fail += 1
            elif job_ce_status == "UNDEF":
                self.db.set_job_status(job_id,7)
                jobs_undef += 1
           
        return (jobs_submitted,jobs_idle,jobs_active,jobs_success,jobs_fail,jobs_undef)
    
    def submit_job(self,job_id,job_dir,prod_ce):
    
        # Save main directory, i.e. top production manager directory
        main_dir = os.getcwd()
    
        # Go to job working directory
        os.chdir(job_dir)
    
        # Submit job and log event to DB
        submit_cmd = "glite-ce-job-submit -a -r %s job.jdl"%prod_ce
        p = subprocess.Popen(submit_cmd.split(),stdin=None,stdout=subprocess.PIPE)
        ce_job_id = p.communicate()[0].rstrip()
        self.db.set_job_submitted(job_id,ce_job_id,self.now_str())
    
        # Go back to main directory before returning
        os.chdir(main_dir)
    
        # Return submitted job identifier
        return ce_job_id

    def get_job_ce_status(self,ce_job_id):
    
        # Retrieve status of job
        job_status_cmd = "glite-ce-job-status %s"%ce_job_id
        p = subprocess.Popen(job_status_cmd.split(),stdin=None,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
        status_info = p.communicate()[0].split("\n")
        status = "UNDEF"
        for l in status_info:
            r = re.match("^\s*Status\s*= \[(.+)\].*$",l)
            if r: status = r.group(1)
        return status
    
    def finalize_job(self,job_id,job_name,ce_job_id):
    
        # Save main directory, i.e. top production manager directory
        main_dir = os.getcwd()
    
        # Go to job working directory
        job_dir = self.db.get_job_dir(job_id)
        os.chdir(job_dir)
    
        # Recover output files from job
        print "\tRecovering job output from CE"
        getout_cmd = "glite-ce-job-output -N %s"%ce_job_id
        print "\t> %s"%getout_cmd
        rc = subprocess.call(getout_cmd.split())
    
        # Purge job
        print "\tPurging job from CE"
        purge_job_cmd = "glite-ce-job-purge -N %s"%ce_job_id
        print "\t> %s"%purge_job_cmd
        rc = subprocess.call(purge_job_cmd.split())
    
        # Get name of dir where output files are stored from the ce_job_id
        out_dir = ce_job_id[8:].replace(":","_").replace("/","_")
    
        # Recover files from output directory and move them to job directory
        print "\tExtracting output files from dir %s"%out_dir
        if os.path.isdir(out_dir):
    
            out_file = "%s/job.out"%out_dir
            if os.path.exists(out_file):
                os.rename(out_file,"./job.out")
            else:
                print "WARNING File %s not found"%out_file
    
            err_file = "%s/job.err"%out_dir
            if os.path.exists(err_file):
                os.rename(err_file,"./job.err")
            else:
                print "WARNING File %s not found"%err_file
    
            sh_file = "%s/job.sh"%out_dir
            if os.path.exists(sh_file):
                os.rename(sh_file,"./job.sh")
            else:
                print "WARNING File %s not found"%sh_file
    
        # Scan log file looking for some information
        time_start = ""
        time_end = ""
        prog_start = ""
        prog_end = ""
        worker_node = ""
        file_list = []
        #data_file_name = ""
        #data_file_size = ""
        #data_file_adler32 = ""
        #hsto_file_name = ""
        #hsto_file_size = ""
        #hsto_file_adler32 = ""
        if os.path.exists("job.out"):
            jof = open("job.out","r")
            for line in jof:

                r = re.match("^Job starting at (.*) \(UTC\)$",line)
                if r: time_start = r.group(1)

                r = re.match("^Job ending at (.*) \(UTC\)$",line)
                if r: time_end = r.group(1)

                r = re.match("^Program starting at (.*) \(UTC\)$",line)
                if r: prog_start = r.group(1)

                r = re.match("^Program ending at (.*) \(UTC\)$",line)
                if r: prog_end = r.group(1)

                r = re.match("^Job running on node (.*) as user",line)
                if r: worker_node = r.group(1)

                r = re.match("^(.*) file (.*) with size (.*) and adler32 (.*) copied.*$",line)
                if r:
                    file_type = r.group(1)
                    file_name = r.group(2)
                    file_size = int(r.group(3))
                    file_adler32 = r.group(4)
                    file_list.append((file_type,file_name,file_size,file_adler32))

                #r = re.match("^Data file (.*) with size (.*) and adler32 (.*) copied to CNAF$",line)
                #if r:
                #    data_file_name = r.group(1)
                #    data_file_size = int(r.group(2))
                #    data_file_adler32 = r.group(3)
                #r = re.match("^Hsto file (.*) with size (.*) and adler32 (.*) copied to CNAF$",line)
                #if r:
                #    hsto_file_name = r.group(1)
                #    hsto_file_size = int(r.group(2))
                #    hsto_file_adler32 = r.group(3)

                # Need some method to retrieve number of generated events

        if time_start:
            print "\tJob started at %s (UTC)"%time_start
            self.db.set_job_time_start(job_id,time_start)

        if time_end:
            print "\tJob ended at %s (UTC)"%time_end
            self.db.set_job_time_end(job_id,time_end)

        if prog_start:
            print "\tProgram started at %s (UTC)"%prog_start
            self.db.set_run_time_start(job_id,prog_start)

        if prog_end:
            print "\tProgram ended at %s (UTC)"%prog_end
            self.db.set_run_time_end(job_id,prog_end)

        if worker_node:
            print "\tJob run on worker node %s"%worker_node
            self.db.set_job_worker_node(job_id,worker_node)

        if file_list:
            for (file_type,file_name,file_size,file_adler32) in file_list:
                print "\t%s file %s with size %s adler32 %s"%(file_type,file_name,file_size,file_adler32)
                self.db.create_job_file(job_id,file_name,file_type,0,0,file_size,file_adler32)

        #if data_file_name:
        #    print "\tData file %s with size %s adler32 %s"%(data_file_name,data_file_size,data_file_adler32)
        #    self.db.create_job_file(job_id,data_file_name,"MCDATA",0,0,data_file_size,data_file_adler32)
        #if hsto_file_name:
        #    print "\tHsto file %s with size %s adler32 %s"%(hsto_file_name,hsto_file_size,hsto_file_adler32)
        #    self.db.create_job_file(job_id,hsto_file_name,"MCHSTO",0,0,hsto_file_size,hsto_file_adler32)
    
        # Go back to top directory
        os.chdir(main_dir)
    
        # Need to define some error handling procedure
        return True

    def now_str(self): return time.strftime("%Y-%m-%d %H:%M:%S",time.gmtime())
