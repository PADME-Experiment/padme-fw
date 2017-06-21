#!/usr/bin/python

import os
import sys
import getopt
import time
import subprocess
import shutil
import re

from PadmeMCDB import PadmeMCDB

def now_str():

    return time.strftime("%Y-%m-%d %H:%M:%S",time.gmtime())

def print_help():

    print "PadmeMCProd [-h] [-n <name>] [-j <number_of_jobs>] [-c <config_file>] [-s submission_site] [-m padmemc_version]"

def create_storage_dir(sdir):

    gfal_mkdir_cmd = "gfal-mkdir -p srm://storm-fe-archive.cr.cnaf.infn.it:8444/srm/managerv2?SFN=/padmeTape%s"%sdir
    print "> %s"%gfal_mkdir_cmd
    rc = subprocess.call(gfal_mkdir_cmd.split())

def get_job_ce_status(ce_job_id):

    return "DONE-OK"

def finalize_job_ok(job_id,job_name,ce_job_id):

    # Save main directory, i.e. top production manager directory
    main_dir = os.getcwd()

    # Go to job working directory
    job_dir = db.get_job_dir(job_id)
    os.chdir(job_dir)

    # Recover output files from job
    getout_cmd = "glite-ce-job-output -N %s"%ce_job_id
    print "Recovering job output for job %s"%job_name
    print "> %s"%getout_cmd
    rc = subprocess.call(getout_cmd.split())

    # Purge job
    purge_job_cmd = "glite-ce-job-purge -N %s"%ce_job_id
    print "Purging job %s from CE"%job_name
    print "> %s"%purge_job_cmd
    rc = subprocess.call(purge_job_cmd.split())

    # Get name of dir where output files are stored from the ce_job_id
    out_dir = ce_job_id[8:]
    out_dir.replace(":","_")
    out_dir.replace("/","_")
    print "Output files should be stored in dir %s"%out_dir

    # Recover files from output directory and move them to job directory
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
    worker_node = ""
    file_name = ""
    file_size = ""
    file_adler32 = ""
    if os.path.exists("job.out"):
        jof = open("job.out","r")
        for line in jof:
            r = re.match("^Job starting at (.*) \(GMT\)$",line)
            if r: time_start = r.group(0)
            r = re.match("^Job ending at (.*) \(GMT\)$",line)
            if r: time_end = r.group(0)
            r = re.match("^Job running on node (.*) as user",line)
            if r: worker_node = r.group(0)
            r = re.match("^Data file (.*) with size (.*) and adler32 (.*) copied to CNAF$",line)
            if r:
                data_file_name = r.group(0)
                data_file_size = r.group(1)
                data_file_adler32 = r.group(2)
            r = re.match("^Hsto file (.*) with size (.*) and adler32 (.*) copied to CNAF$",line)
            if r:
                hsto_file_name = r.group(0)
                hsto_file_size = r.group(1)
                hsto_file_adler32 = r.group(2)
            # Need some reg exp to retrieve number of generated events
    if time_start:
        print "Job was started at %s (GMT)"%time_start
        db.set_job_time_start(job_id,time_start)
    if time_end:
        print "Job ended at %s (GMT)"%time_end
        db.set_job_time_end(job_id,time_end)
    if worker_node:
        print "Job run on worker node %s"%worker_node
        db.set_job_worker_node(job_id,worker_node)
    if data_file_name:
        print "Data file %s with size % adler32 %s"%data_file_name,data_file_size,data_file_adler32
        db.create_job_file(job_id,data_file_name,data_file_size,data_file_adler32)
    if hsto_file_name:
        print "Hsto file %s with size % adler32 %s"%hsto_file_name,hsto_file_size,hsto_file_adler32
        db.create_job_file(job_id,hsto_file_name,hsto_file_size,hsto_file_adler32)

    return True

def handle_jobs(job_list):

    active_jobs = 0

    for job_id in job_list:

        (name,status,ce_job_id) = db.get_job_info(job_id)

        print "Checking job %s %s %s %s"%(job_id,name,status,ce_job_id)

        # Nothing to do if job is already in end status
        if status == 3 or status == 4: continue

        # Last time job was still active: check if its status changed
        if status == 1 or status == 2:
            job_ce_status = get_job_ce_status(ce_job_id)
            if job_ce_status == "PENDING":
                active_jobs += 1
            elif job_ce_status == "RUNNING" or job_ce_status == "REALLY-RUNNING":
                active_jobs += 1
                if status == 1: db.set_job_status(job_id,2)
            elif job_ce_status == "DONE-OK":
                if finalize_job_ok(job_id,name,ce_job_id):
                    db.set_job_status(job_id,3)
                else:
                    db.set_job_status(job_id,4)
            
    return active_jobs

def submit_job(job_id,job_dir,ce_uri):

    # Save main directory, i.e. top production manager directory
    main_dir = os.getcwd()

    # Go to job working directory
    os.chdir(job_dir)

    # Submit job and log event to DB
    submit_cmd = "glite-ce-job-submit -a -r %s job.jdl"%ce_uri
    #ce_job_id = subprocess.check_output(submit_cmd.split())
    p1 = subprocess.Popen(submit_cmd.split(),stdin=None,stdout=subprocess.PIPE)
    ce_job_id = p1.communicate()[0]
    print "Job submitted as %s"%(ce_job_id)
    db.set_job_submitted(job_id,ce_job_id,now_str())

    # Go back to main directory before returning
    os.chdir(main_dir)

def main(argv):

    try:
        opts,args = getopt.getopt(argv,"hn:j:c:s:m:",[])
    except getopt.GetoptError:
        print_help()
        sys.exit(2)

    nJobs = 0
    configFile = ""
    prodName = ""
    storageDir = ""
    prodDir = ""
    prodScript = "script/padmemc_prod.py"
    ceURI = ""
    mcVersion = "v0.4.0"

    for opt,arg in opts:
        if opt == '-h':
            print_help()
            sys.exit()
        elif opt == '-n':
            prodName = arg
        elif opt == '-c':
            configFile = arg
        elif opt == '-m':
            mcVersion = arg
        elif opt == '-s':
            if arg == "CNAF":
                ceURI = "ce04-lcg.cr.cnaf.infn.it:8443/cream-lsf-padme"
            elif arg == "LNF":
                ceURI = "atlasce1.lnf.infn.it:8443/cream-pbs-padme"
            else:
                print "Invalid submission site %s. Valid: LNF,CNAF"%arg
                print_help()
                sys.exit(2)
        elif opt == '-j':
            try:
                nJobs = int(arg)
            except ValueError:
                print "Invalid number of jobs"
                print_help()
                sys.exit(2)

    if nJobs == 0:
        print "ERROR 0 jobs requested. Aborting"
        sys.exit(2)

    if ceURI == "":
        print "No CE URI specified: use LNF"
        ceURI = "atlasce1.lnf.infn.it:8443/cream-pbs-padme"

    if configFile == "":
        configFile = "cfg/%s.cfg"%prodName

    if storageDir == "":
        storageDir = "/mcprod/%s"%prodName

    if prodDir == "":
        prodDir = "prod/%s"%prodName

    print "prodName",prodName
    print "nJobs",nJobs
    print "configFile",configFile
    print "mcVersion",mcVersion
    print "prodDir",prodDir
    print "storageDir",storageDir
    print "prodScript",prodScript
    print "ceURI",ceURI

    # Check if prdouction dir already exists
    if os.path.exists(prodDir):
        print "ERROR Path %s already exists"%prodDir
        sys.exit(2)

    # Check configuration file and read it into a variable (will go to DB)
    if not os.path.isfile(configFile):
        print "ERROR Configuration file '%s' does not exist"%configFile
        sys.exit(2)
    prodConfig = open(configFile,"r").read()

    # Create handler for PadmeDB
    #db = PadmeMCDB()

    if (db.is_prod_in_db(prodName)):
        print "ERROR A production named '%s' already exists in DB"%prodName
        sys.exit(2)

    # Create new production in DB
    db.create_prod(prodName,ceURI,mcVersion,prodDir,prodConfig,storageDir,nJobs)
    prodId = db.get_prod_id(prodName)

    # Create production directory in the CNAF tape library
    create_storage_dir(storageDir)

    # Create empty array of job ids
    jobList = []

    # Create job structures
    os.mkdir(prodDir)
    for j in range(0,nJobs):

        jobName = "job%05d"%j

        # Create dir to hold individual job info
        jobDir = "%s/%s"%(prodDir,jobName)
        os.mkdir(jobDir)

        # Copy production script to job dir
        shutil.copyfile(prodScript,"%s/job.py"%jobDir)

        # Copy common configuration file to job dir
        shutil.copyfile(configFile,"%s/job.mac"%jobDir)

        # Create JDL file in job dir
        jf = open("%s/job.jdl"%jobDir,"w")
	jf.write("[\n")
	jf.write("Type = \"Job\";\n")
	jf.write("JobType = \"Normal\";\n")
	jf.write("Executable = \"/usr/bin/python\";\n")
	jf.write("Arguments = \"job.py %s %s %s job.mac %s\";\n"%(prodName,jobName,mcVersion,storageDir))
	jf.write("StdOutput = \"job.out\";\n")
	jf.write("StdError = \"job.err\";\n")
	jf.write("InputSandbox = {\"job.py\",\"job.mac\"};\n")
	jf.write("OutputSandbox = {\"job.out\", \"job.err\", \"job.sh\"};\n")
	jf.write("OutputSandboxBaseDestURI=\"gsiftp://localhost\";\n")
	jf.write("]\n")
        jf.close()

        # Create job entry in DB and register job
        db.create_job(prodId,jobName,jobDir)
        jobId = db.get_job_id(prodId,jobName)
        jobList.append(jobId)

        # Submit job
        submit_job(jobId,jobDir,ceURI)

    # Main loop: wake up every 5min and check jobs status
    while True:
        running_jobs = handle_jobs(jobList)
        if running_jobs == 0:
            print "No running jobs left: exiting"
            break
        print "%s jobs still running"%running_jobs
        time.sleep(300)

# Execution starts here
if __name__ == "__main__":

    # Create global handler to PadmeMCDB
    db = PadmeMCDB()

    main(sys.argv[1:])
