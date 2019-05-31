#!/usr/bin/python

import os
import sys
import getopt
import time
import subprocess
import shutil
import re
import daemon
import daemon.pidfile

from PadmeProdServer import PadmeProdServer
from PadmeMCDB import PadmeMCDB
from ProxyHandler import ProxyHandler
#from Logger import Logger

# Get location of padme-fw software from PADME
# Default to HOME/padme-fw (or ~/padme-fw) if not set
PADME_FW = os.getenv('PADME',"%s/padme-fw"%os.getenv('HOME',"~"))

# Create global handler to PadmeMCDB
DB = PadmeMCDB()

# Create proxy handler
PH = ProxyHandler()

# ### Define PADME grid resources ###
        
# SRMs to access PADME area on the LNF and CNAF storage systems
PADME_SRM_URI = {
    "LNF":  "srm://atlasse.lnf.infn.it:8446/srm/managerv2?SFN=/dpm/lnf.infn.it/home/vo.padme.org",
    "CNAF": "srm://storm-fe-archive.cr.cnaf.infn.it:8444/srm/managerv2?SFN=/padmeTape"
}

PADME_ROOT_URI = {
    "LNF": "root://atlasse.lnf.infn.it:1094//vo.padme.org",
}

# List of available submission sites and corresponding CE nodes
PADME_CE_NODE = {
    "LNF":   "atlasce1.lnf.infn.it:8443/cream-pbs-padme",
    "CNAF":  "ce04-lcg.cr.cnaf.infn.it:8443/cream-lsf-padme",
    "SOFIA": "cream.grid.uni-sofia.bg:8443/cream-pbs-cms"
}

# Initialize global parameters and set some default values
PROD_RUN_NAME = ""
PROD_NAME = ""
PROD_FILES_PER_JOB = 100
PROD_STORAGE_DIR = ""
PROD_DIR = ""
PROD_SCRIPT = "%s/PadmeProd/script/padmereco_prod.py"%PADME_FW
PROD_CE = PADME_CE_NODE["LNF"]
PROD_SRM = PADME_SRM_URI["LNF"]
PROD_RECO_VERSION = "develop"
PROD_PROXY_FILE = ""
PROD_YEAR = "2018"

def now_str(): return time.strftime("%Y-%m-%d %H:%M:%S",time.gmtime())

def print_help():

    print "PadmeRecoProd -r <run_name> [-j <files_per_job>] [-v <padmereco_version>] [-n <prod_name>] [-s <submission_site>] [-d <storage_site>] [-h]"
    print "  <run_name>\t\tname of the run to process"
    print "  <padmereco_version>\tversion of PadmeReco to use for production. Must be installed on CVMFS. Default: develop"
    print "  <prod_name>\t\tname for the production. Default: <run_name>_<padmereco_version>"
    print "  <files_per_job>\tnumber of rawdata files to be reconstructed by each job. Default: %d"%PROD_FILES_PER_JOB
    print "  <submission_site>\tsite where the jobs will be submitted. Allowed: LNF,CNAF. Default: LNF"
    print "  <storage_site>\tsite where the jobs output will be stored. Allowed: LNF,CNAF. Default: LNF"

def run_command(command):
    print "> %s"%command
    p = subprocess.Popen(command,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,shell=True)
    return iter(p.stdout.readline,b'')

def rawfile_sort_key(f):
    r = re.match("^.*_(\d\d)_(\d\d\d).root$",f)
    if r:
        index = int(r.group(2))*100+int(r.group(1))
        #print f,r.group(1),r.group(2),index
        return index
    return 0

def get_run_file_list(run):

    run_file_list = []
    for line in run_command("gfal-ls %s/daq/%s/rawdata/%s"%(PADME_SRM_URI["LNF"],PROD_YEAR,run)):
        if re.match("^gfal-ls error: ",line):
            print "***ERROR*** gfal-ls returned error status while retrieving file list from run dir %s from LNF"%run_dir
            sys.exit(2)
        run_file_list.append(line.rstrip())
    run_file_list.sort(key=rawfile_sort_key)
    return run_file_list

def main(argv):

    # Declare that here we can possibly modify these global variables
    global PROD_RUN_NAME
    global PROD_NAME
    global PROD_FILES_PER_JOB
    global PROD_STORAGE_DIR
    global PROD_DIR
    global PROD_SCRIPT
    global PROD_CE
    global PROD_SRM
    global PROD_RECO_VERSION
    global PROD_PROXY_FILE

    try:
        opts,args = getopt.getopt(argv,"hr:n:j:s:v:",[])
    except getopt.GetoptError:
        print_help()
        sys.exit(2)

    for opt,arg in opts:
        if opt == '-h':
            print_help()
            sys.exit(0)
        elif opt == '-r':
            PROD_RUN_NAME = arg
        elif opt == '-n':
            PROD_NAME = arg
        elif opt == '-v':
            PROD_RECO_VERSION = arg
        elif opt == '-s':
            if arg in PADME_CE_NODE.keys():
                PROD_CE = PADME_CE_NODE[arg]
            else:
                print "*** ERROR *** Invalid submission site %s. Valid: %s"%(arg,",".join(PADME_CE_NODE.keys()))
                print_help()
                sys.exit(2)
        elif opt == '-d':
            if arg in PADME_SRM_URI.keys():
                PROD_SRM = PADME_SRT_URI[arg]
            else:
                print "*** ERROR *** Invalid storage site %s. Valid: %s"%(arg,",".join(PADME_SRM_URI.keys()))
                print_help()
                sys.exit(2)
        elif opt == '-j':
            try:
                PROD_FILES_PER_JOB = int(arg)
            except ValueError:
                print "*** ERROR *** Invalid number of files per job: '%s'"%arg
                print_help()
                sys.exit(2)

    if PROD_RUN_NAME == "":
        print "*** ERROR *** No run name specified."
        print_help()
        sys.exit(2)

    r = re.match("^run_\d+_(\d\d\d\d)\d\d\d\d_\d\d\d\d\d\d$",PROD_RUN_NAME)
    if r:
        PROD_YEAR = r.group(1)
    else:
        print "WARNING: run name \'%s\' has unrecognized format: assuming year %s"%(PROD_RUN_NAME,PROD_YEAR)

    if PROD_NAME == "":
        PROD_NAME = "%s_%s"%(PROD_RUN_NAME,PROD_RECO_VERSION)
        print "WARNING: No Production Name was specified. Using %s"%PROD_NAME

    if PROD_FILES_PER_JOB < 0 or PROD_FILES_PER_JOB > 1000:
        print "*** ERROR *** Invalid number of files per job requested:",PROD_FILES_PER_JOB,"- Max allowed: 1000."
        print_help()
        sys.exit(2)

    if PROD_STORAGE_DIR == "":
        PROD_STORAGE_DIR = "/daq/%s/recodata/%s"%(PROD_YEAR,PROD_NAME)

    if PROD_DIR == "":
        PROD_DIR = "prod/%s"%PROD_NAME

    # Show info about required production
    print "Starting production",PROD_NAME,"for run",PROD_RUN_NAME,"using PadmeReco version",PROD_RECO_VERSION
    print "Submitting jobs of",PROD_FILES_PER_JOB,"files to CE",PROD_CE
    print "Main production directory:",PROD_DIR
    print "Production script:",PROD_SCRIPT
    print "Storage SRM:",PROD_SRM
    print "Storage directory:",PROD_STORAGE_DIR

    # Check if production dir already exists
    if os.path.exists(PROD_DIR):
        print "*** ERROR *** Path %s already exists"%PROD_DIR
        sys.exit(2)

    if (DB.is_prod_in_db(PROD_NAME)):
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

    # Get list of files for run to reconstruct
    # All files are assumed to be on the LNF SE and available via the ROOTX protocol
    file_list = get_run_file_list(PROD_RUN_NAME)
    job_file_lists = []
    job_file_list = []
    for f in file_list:
        if len(job_file_list) == PROD_FILES_PER_JOB:
            job_file_lists.append(job_file_list)
            job_file_list = []
        file_url = "%s/daq/%s/rawdata/%s/%s"%(PADME_ROOT_URI["LNF"],PROD_YEAR,PROD_RUN_NAME,f)
        job_file_list.append(file_url)
    if job_file_list: job_file_lists.append(job_file_list)

    # Create new production in DB
    print "- Creating new production in DB"
    # This will improve when we have a web interface to handle production requests
    PROD_DESCRIPTION = "TEST"
    DB.create_recoprod(PROD_NAME,PROD_RUN_NAME,PROD_DESCRIPTION,PROD_CE,PROD_RECO_VERSION,PROD_DIR,PROD_STORAGE_DIR,PROD_SRM,PROD_PROXY_FILE,now_str(),len(job_file_lists))
    prodId = DB.get_prod_id(PROD_NAME)
    #DB.set_prod_status(prodId,0)

    # Renew VOMS proxy using long-lived proxy if needed
    PH.renew_voms_proxy(PROD_PROXY_FILE)

    # Create production directory in the storage SRM
    print "- Creating dir",PROD_STORAGE_DIR,"in",PROD_SRM
    gfal_mkdir_cmd = "gfal-mkdir -p %s%s"%(PROD_SRM,PROD_STORAGE_DIR)
    print ">",gfal_mkdir_cmd
    rc = subprocess.call(gfal_mkdir_cmd.split())

    # Create job structures
    print "- Creating directory structure for production jobs"
    for j in range(0,len(job_file_lists)):

        jobName = "job%05d"%j

        # Create dir to hold individual job info
        jobDir = "%s/%s"%(PROD_DIR,jobName)
        os.mkdir(jobDir)

        # Copy production script to job dir
        shutil.copyfile(PROD_SCRIPT,"%s/job.py"%jobDir)

        # Configuration file is empty for Reco
        jobCfg = ""

        # Create list with files to process
        jobListFile = "%s/job.list"%jobDir
        with open(jobListFile,"w") as jlf:
            for f in job_file_lists[j]: jlf.write("%s\n"%f)
        with open(jobListFile,"r") as jlf: jobList = jlf.read()

        # Copy long-lived proxy file to job dir
        shutil.copyfile(PROD_PROXY_FILE,"%s/job.proxy"%jobDir)

        # Create JDL file in job dir
        jf = open("%s/job.jdl"%jobDir,"w")
	jf.write("[\n")
	jf.write("Type = \"Job\";\n")
	jf.write("JobType = \"Normal\";\n")
	jf.write("Executable = \"/usr/bin/python\";\n")
	jf.write("Arguments = \"-u job.py job.list job.proxy %s %s %s %s %s\";\n"%(PROD_NAME,jobName,PROD_RECO_VERSION,PROD_STORAGE_DIR,PROD_SRM))
	jf.write("StdOutput = \"job.out\";\n")
	jf.write("StdError = \"job.err\";\n")
	jf.write("InputSandbox = {\"job.py\",\"job.list\",\"job.proxy\"};\n")
	jf.write("OutputSandbox = {\"job.out\", \"job.err\", \"job.sh\"};\n")
	jf.write("OutputSandboxBaseDestURI=\"gsiftp://localhost\";\n")
	jf.write("]\n")
        jf.close()

        # Create job entry in DB and register job
        DB.create_job(prodId,jobName,jobDir,jobCfg,jobList)

    # Prepare daemon context

    # Assume that the current directory is the top level Production directory
    top_prod_dir = os.getcwd()
    print "Production top working dir:",top_prod_dir

    # Lock file with daemon pid is located inside the production directory
    prod_lock = "%s/%s/%s.pid"%(top_prod_dir,PROD_DIR,PROD_NAME)
    print "Production lock file:",prod_lock

    # Redirect stdout and stderr to log/err files inside the production directory
    prod_log_file = "%s/%s/%s.log"%(top_prod_dir,PROD_DIR,PROD_NAME)
    prod_err_file = "%s/%s/%s.err"%(top_prod_dir,PROD_DIR,PROD_NAME)
    print "Production log file:",prod_log_file
    print "Production err file:",prod_err_file
    #sys.stdout = Logger(prod_log_file)
    #sys.stderr = Logger(prod_err_file)

    # Start Padme Production Server as a daemon
    context = daemon.DaemonContext()
    context.working_directory = top_prod_dir
    context.umask = 0o002
    context.pidfile = daemon.pidfile.PIDLockFile(prod_lock)
    context.open()
    PadmeProdServer(PROD_NAME)
    context.close()
    #PadmeProdServer(PROD_NAME)

# Execution starts here
if __name__ == "__main__": main(sys.argv[1:])
