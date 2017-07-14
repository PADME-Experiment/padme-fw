#!/usr/bin/python

import os
import sys
import getopt
import time
import subprocess
import shutil
import re
import daemon
import lockfile

from PadmeMCDB import PadmeMCDB
from Logger    import Logger

# ### Define PADME grid resources ###

# SRM to access PADME area on the CNAF tape library
PADME_CNAF_SRM = "srm://storm-fe-archive.cr.cnaf.infn.it:8444/srm/managerv2?SFN=/padmeTape"

# List of available submission sites and corresponding CE nodes
PADME_CE_NODE = {
    "LNF":  "atlasce1.lnf.infn.it:8443/cream-pbs-padme",
    "CNAF": "ce04-lcg.cr.cnaf.infn.it:8443/cream-lsf-padme",
    "SOFIA": "cream.grid.uni-sofia.bg:8443/cream-pbs-cms"
}

# Initialize global parameters and set some default values
PROD_NAME = ""
PROD_NJOBS = 0
PROD_CONFIG_FILE = ""
PROD_STORAGE_DIR = ""
PROD_DIR = ""
PROD_SCRIPT = "script/padmemc_prod.py"
PROD_CE = PADME_CE_NODE["LNF"]
PROD_MC_VERSION = "v0.4.0"
PROD_PROXY_FILE = ""
PROD_CHECK_DELAY = 60

def now_str():

    return time.strftime("%Y-%m-%d %H:%M:%S",time.gmtime())

def print_help():

    print "PadmeMCProd -n <prod_name> -j <number_of_jobs> [-c <config_file>] [-s <submission_site>] [-m <padmemc_version>] [-h]"
    print "  <prod_name>\t\tname for the production"
    print "  <number_of_jobs>\tnumber of production jobs to submit. Must be >0 and <=1000"
    print "  <config_file>\t\tname of configuration file to use. Default: cfg/<prod_name>.cfg"
    print "  <submission_site>\tsite where the jobs will be submitted. Allowed: LNF,CNAF. Default: LNF"
    print "  <padmemc_version>\tversion of PadmeMC to use for production. Must be installed on CVMFS. Default: v0.4.0"

def main(argv):

    # Declare that here we can possibly modify these global variables
    global PROD_NAME
    global PROD_NJOBS
    global PROD_CONFIG_FILE
    global PROD_STORAGE_DIR
    global PROD_DIR
    global PROD_SCRIPT
    global PROD_CE
    global PROD_MC_VERSION
    global PROD_PROXY_FILE
    global PROD_CHECK_DELAY

    try:
        opts,args = getopt.getopt(argv,"hn:j:c:s:m:",[])
    except getopt.GetoptError:
        print_help()
        sys.exit(2)

    for opt,arg in opts:
        if opt == '-h':
            print_help()
            sys.exit(0)
        elif opt == '-n':
            PROD_NAME = arg
        elif opt == '-c':
            PROD_CONFIG_FILE = arg
        elif opt == '-m':
            PROD_MC_VERSION = arg
        elif opt == '-s':
            if arg in PADME_CE_NODE.keys():
                PROD_CE = PADME_CE_NODE[arg]
            else:
                print "*** ERROR *** Invalid submission site %s. Valid: %s"%(arg,",".join(PADME_CE_NODE.keys()))
                print_help()
                sys.exit(2)
        elif opt == '-j':
            try:
                PROD_NJOBS = int(arg)
            except ValueError:
                print "*** ERROR *** Invalid number of jobs: '%s'"%arg
                print_help()
                sys.exit(2)

    if PROD_NAME == "":
        print "*** ERROR *** No production name specified."
        print_help()
        sys.exit(2)

    if PROD_NJOBS == 0:
        print "*** ERROR *** Number of jobs was not specified."
        print_help()
        sys.exit(2)

    if PROD_NJOBS < 0 or PROD_NJOBS > 1000:
        print "*** ERROR *** Invalid number of jobs requested:",PROD_NJOBS,"Max allowed: 1000."
        print_help()
        sys.exit(2)

    if PROD_CONFIG_FILE == "":
        PROD_CONFIG_FILE = "cfg/%s.cfg"%PROD_NAME

    if PROD_STORAGE_DIR == "":
        PROD_STORAGE_DIR = "/mcprod/%s"%PROD_NAME

    if PROD_DIR == "":
        PROD_DIR = "prod/%s"%PROD_NAME

    # Show info about required production
    print "Starting production",PROD_NAME,"using PadmeMC version",PROD_MC_VERSION
    print "Submitting",PROD_NJOBS,"jobs to CE",PROD_CE
    print "Main production directory:",PROD_DIR
    print "Production script:",PROD_SCRIPT
    print "PadmeMC macro file:",PROD_CONFIG_FILE
    print "Storage directory:",PROD_STORAGE_DIR

    # Check if production dir already exists
    if os.path.exists(PROD_DIR):
        print "*** ERROR *** Path %s already exists"%PROD_DIR
        sys.exit(2)

    # Check configuration file and read it into a variable (will go to DB)
    if not os.path.isfile(PROD_CONFIG_FILE):
        print "*** ERROR *** Configuration file '%s' does not exist"%PROD_CONFIG_FILE
        sys.exit(2)
    prodConfig = open(PROD_CONFIG_FILE,"r").read()

    if (db.is_prod_in_db(PROD_NAME)):
        print "*** ERROR *** A production named '%s' already exists in DB"%PROD_NAME
        sys.exit(2)

    # Create production directory to host support dirs for all jobs
    print "- Creating production dir",PROD_DIR
    os.mkdir(PROD_DIR)

    # Create long-lived (20 days) proxy. Will ask user for password
    PROD_PROXY_FILE = "%s/job.proxy"%PROD_DIR
    print "- Creating long-lived proxy file",PROD_PROXY_FILE
    proxy_cmd = "voms-proxy-init --valid 480:0 --out %s"%PROD_PROXY_FILE
    print ">",proxy_cmd
    if subprocess.call(proxy_cmd.split()):
        print "*** ERROR *** while generating long-lived proxy. Aborting"
        shutil.rmtree(PROD_DIR)
        sys.exit(2)

    # Create new production in DB
    print "- Creating new production in DB"
    # This will improve when we have a web interface to handle production requests
    PROD_DESCRIPTION = "TEST"
    PROD_USER_REQ = "EL"
    PROD_NEVENTS_REQ = 100
    db.create_prod(PROD_NAME,PROD_DESCRIPTION,PROD_USER_REQ,PROD_NEVENTS_REQ,PROD_CE,PROD_MC_VERSION,PROD_DIR,PROD_STORAGE_DIR,PROD_PROXY_FILE,prodConfig,now_str(),PROD_NJOBS)
    prodId = db.get_prod_id(PROD_NAME)
    #db.set_prod_status(prodId,0)

    # Renew VOMS proxy using long-lived proxy if needed
    renew_proxy(PROD_PROXY_FILE)

    # Create production directory in the CNAF tape library
    print "- Creating dir",PROD_STORAGE_DIR,"in the CNAF tape library"
    gfal_mkdir_cmd = "gfal-mkdir -p %s%s"%(PADME_CNAF_SRM,PROD_STORAGE_DIR)
    print ">",gfal_mkdir_cmd
    rc = subprocess.call(gfal_mkdir_cmd.split())

    # Create job structures
    print "- Creating directory structure for production jobs"
    for j in range(0,PROD_NJOBS):

        jobName = "job%05d"%j

        # Create dir to hold individual job info
        jobDir = "%s/%s"%(PROD_DIR,jobName)
        os.mkdir(jobDir)

        # Copy production script to job dir
        shutil.copyfile(PROD_SCRIPT,"%s/job.py"%jobDir)

        # Copy common configuration file to job dir
        shutil.copyfile(PROD_CONFIG_FILE,"%s/job.mac"%jobDir)

        # Copy long-lived proxy file to job dir
        shutil.copyfile(PROD_PROXY_FILE,"%s/job.proxy"%jobDir)

        # Create JDL file in job dir
        jf = open("%s/job.jdl"%jobDir,"w")
	jf.write("[\n")
	jf.write("Type = \"Job\";\n")
	jf.write("JobType = \"Normal\";\n")
	jf.write("Executable = \"/usr/bin/python\";\n")
	jf.write("Arguments = \"-u job.py %s %s %s job.mac %s job.proxy\";\n"%(PROD_NAME,jobName,PROD_MC_VERSION,PROD_STORAGE_DIR))
	jf.write("StdOutput = \"job.out\";\n")
	jf.write("StdError = \"job.err\";\n")
	jf.write("InputSandbox = {\"job.py\",\"job.mac\",\"job.proxy\"};\n")
	jf.write("OutputSandbox = {\"job.out\", \"job.err\", \"job.sh\"};\n")
	jf.write("OutputSandboxBaseDestURI=\"gsiftp://localhost\";\n")
	jf.write("]\n")
        jf.close()

        # Create job entry in DB and register job
        db.create_job(prodId,jobName,jobDir)

    # Prepare daemon context

    # Assume that the current directory is the top level MC Production directory
    mcprod_dir = os.getcwd()

    # Lock file with daemon pid is located inside the production directory
    prod_lock = "%s/%s/%s.pid"%(mcprod_dir,PROD_DIR,PROD_NAME)

    print "Daemon working dir is",mcprod_dir
    print "Daemon lock file is",prod_lock

    context = daemon.DaemonContext()
    context.working_directory = mcprod_dir
    context.umask = 0o002
    context.pidfile = lockfile.FileLock(prod_lock)
    #context.signal_map = {
    #    signal.SIGTERM: program_cleanup,
    #    signal.SIGHUP: 'terminate',
    #    signal.SIGUSR1: reload_program_config,
    #    }

    # Become a daemon and start the production
    #with context: start_production(PROD_NAME)
    #with daemon.DaemonContext(): start_production(PROD_NAME)
    context.open()
    start_production(PROD_NAME)
    context.close()

def start_production(prod_name):

    # This is the daemonized code to start and handle the full production
    # It is structured to only depend on DB information

    # Verify that production exists in DB and retrieve production id
    if not db.is_prod_in_db(prod_name):
        print "*** ERROR *** Production '%s' not found in DB"%prod_name
        sys.exit(2)
    prod_id = db.get_prod_id(prod_name)

    # Get some info about this prod
    (dummy,prod_ce,prod_dir,proxy_file,prod_njobs) = db.get_prod_info(prod_id)

    # Check if production dir exists
    if not os.path.isdir(prod_dir):
        print "*** ERROR *** Production directory '%s' not found"%prod_dir
        sys.exit(2)

    # Check if proxy file exists
    if not os.path.isfile(proxy_file):
        print "*** ERROR *** Long-lived proxy file '%s' not found"%proxy_file
        sys.exit(2)
        
    # Get list of job ids for this production
    job_id_list = db.get_job_list(prod_id)
    if len(job_id_list) != prod_njobs:
        print "*** ERROR *** Number of jobs in DB and in production are different: %s != %s"%(len(job_id_list),prod_njobs)
        sys.exit(2)

    # Redirect stdout and stderr to files with automatic time logging
    log_file_name = "%s/%s.log"%(prod_dir,prod_name)
    sys.stdout = Logger(log_file_name)
    err_file_name = "%s/%s.err"%(prod_dir,prod_name)
    sys.stderr = Logger(err_file_name)

    # For the time being, enable terminal output (remove when demonizing)
    sys.stdout.interactive = True
    sys.stderr.interactive = True

    print "=== Starting MC Production %s ==="%prod_name

    # Main production loop
    while True:

        # Renew proxy if needed
        renew_proxy(proxy_file)

        # Call method to check jobs status and handle each job accordingly
        (jobs_submitted,jobs_idle,jobs_active,jobs_success,jobs_fail,jobs_undef) = handle_jobs(prod_ce,job_id_list)

        # If all jobs are in a final state, production is over
        if jobs_submitted+jobs_idle+jobs_active == 0:
            print "*** No unfinished jobs left: exiting ***"
            break

        # Show current production state and sleep for a while
        print "Jobs: submitted %d idle %d running %d success %d fail %d undef %d"%(jobs_submitted,jobs_idle,jobs_active,jobs_success,jobs_fail,jobs_undef)
        time.sleep(PROD_CHECK_DELAY)

    # Production is over: tag it as done and say bye bye
    n_events = 0
    db.close_prod(prod_id,now_str(),jobs_success,n_events)

    print "=== Ending MC Production %s ==="%prod_name
    sys.exit(0)

def handle_jobs(prod_ce,job_id_list):

    jobs_submitted = 0
    jobs_idle = 0
    jobs_active = 0
    jobs_success = 0
    jobs_fail = 0
    jobs_undef = 0

    print "--- Checking status of production jobs ---"

    for job_id in job_id_list:

        (job_name,job_status,job_dir,ce_job_id) = db.get_job_info(job_id)

        # If status is 0, job was not submitted yet: do it now and get updated information
        if job_status == 0:
            ce_job_id = submit_job(job_id,job_dir,prod_ce)
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
        job_ce_status = get_job_ce_status(ce_job_id)
        print "- %s %s %s"%(job_name,ce_job_id,job_ce_status)
        if job_ce_status == "PENDING" or job_ce_status == "REGISTERED" or job_ce_status == "IDLE":
            jobs_idle += 1
        elif job_ce_status == "RUNNING" or job_ce_status == "REALLY-RUNNING":
            jobs_active += 1
            if job_status == 1: db.set_job_status(job_id,2)
        elif job_ce_status == "DONE-OK":
            if finalize_job(job_id,job_name,ce_job_id):
                db.set_job_status(job_id,3)
                jobs_success += 1
            else:
                db.set_job_status(job_id,5)
                jobs_fail += 1
        elif job_ce_status == "DONE-FAILED":
            if finalize_job(job_id,job_name,ce_job_id):
                db.set_job_status(job_id,4)
            else:
                db.set_job_status(job_id,6)
            jobs_fail += 1
        elif job_ce_status == "UNDEF":
            db.set_job_status(job_id,7)
            jobs_undef += 1
       
    return (jobs_submitted,jobs_idle,jobs_active,jobs_success,jobs_fail,jobs_undef)

def submit_job(job_id,job_dir,prod_ce):

    # Save main directory, i.e. top production manager directory
    main_dir = os.getcwd()

    # Go to job working directory
    os.chdir(job_dir)

    # Submit job and log event to DB
    submit_cmd = "glite-ce-job-submit -a -r %s job.jdl"%prod_ce
    p = subprocess.Popen(submit_cmd.split(),stdin=None,stdout=subprocess.PIPE)
    ce_job_id = p.communicate()[0].rstrip()
    db.set_job_submitted(job_id,ce_job_id,now_str())

    # Go back to main directory before returning
    os.chdir(main_dir)

    # Return submitted job identifier
    return ce_job_id

def renew_proxy(proxy_file):

    renew = True

    # Check if current proxy is valid and has at least 2 hours left
    check_proxy_cmd = "voms-proxy-info"
    p = subprocess.Popen(check_proxy_cmd.split(),stdin=None,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
    proxy_info = p.communicate()[0].split("\n")
    for l in proxy_info:
        r = re.match("^timeleft  \: (\d+)\:.*$",l)
        if r and int(r.group(1))>=2: renew = False

    # Proxy needs renewal: create a new one using the long-lived proxy
    if renew:
        print "--- Renewing proxy ---"
        renew_proxy_cmd = "voms-proxy-init --noregen --cert %s --key %s --voms vo.padme.org --valid 96:0"%(proxy_file,proxy_file)
        print "> %s"%renew_proxy_cmd
        rc = subprocess.call(renew_proxy_cmd.split())

def get_job_ce_status(ce_job_id):

    # Retrieve status of job
    job_status_cmd = "glite-ce-job-status %s"%ce_job_id
    p = subprocess.Popen(job_status_cmd.split(),stdin=None,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
    status_info = p.communicate()[0].split("\n")
    status = "UNDEF"
    for l in status_info:
        r = re.match("^\s*Status\s*= \[(.+)\].*$",l)
        if r: status = r.group(1)
    return status

def finalize_job(job_id,job_name,ce_job_id):

    # Save main directory, i.e. top production manager directory
    main_dir = os.getcwd()

    # Go to job working directory
    job_dir = db.get_job_dir(job_id)
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
    worker_node = ""
    data_file_name = ""
    data_file_size = ""
    data_file_adler32 = ""
    hsto_file_name = ""
    hsto_file_size = ""
    hsto_file_adler32 = ""
    if os.path.exists("job.out"):
        jof = open("job.out","r")
        for line in jof:
            r = re.match("^Job starting at (.*) \(UTC\)$",line)
            if r: time_start = r.group(1)
            r = re.match("^Job ending at (.*) \(UTC\)$",line)
            if r: time_end = r.group(1)
            r = re.match("^Job running on node (.*) as user",line)
            if r: worker_node = r.group(1)
            r = re.match("^Data file (.*) with size (.*) and adler32 (.*) copied to CNAF$",line)
            if r:
                data_file_name = r.group(1)
                data_file_size = int(r.group(2))
                data_file_adler32 = r.group(3)
            r = re.match("^Hsto file (.*) with size (.*) and adler32 (.*) copied to CNAF$",line)
            if r:
                hsto_file_name = r.group(1)
                hsto_file_size = int(r.group(2))
                hsto_file_adler32 = r.group(3)
            # Need some reg exp to retrieve number of generated events
    if time_start:
        print "\tJob started at %s (UTC)"%time_start
        db.set_job_time_start(job_id,time_start)
    if time_end:
        print "\tJob ended at %s (UTC)"%time_end
        db.set_job_time_end(job_id,time_end)
    if worker_node:
        print "\tJob run on worker node %s"%worker_node
        db.set_job_worker_node(job_id,worker_node)
    if data_file_name:
        print "\tData file %s with size %s adler32 %s"%(data_file_name,data_file_size,data_file_adler32)
        db.create_job_file(job_id,data_file_name,"MCDATA",0,0,data_file_size,data_file_adler32)
    if hsto_file_name:
        print "\tHsto file %s with size %s adler32 %s"%(hsto_file_name,hsto_file_size,hsto_file_adler32)
        db.create_job_file(job_id,hsto_file_name,"MCHSTO",0,0,hsto_file_size,hsto_file_adler32)

    # Go back to top directory
    os.chdir(main_dir)

    # Need to define some error handling procedure
    return True

# Execution starts here
if __name__ == "__main__":

    # Create global handler to PadmeMCDB
    db = PadmeMCDB()

    main(sys.argv[1:])
