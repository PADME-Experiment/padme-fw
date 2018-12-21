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
#import lockfile

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
PROD_SCRIPT = "%s/PadmeProd/script/padmemc_prod.py"%PADME_FW
PROD_CE = PADME_CE_NODE["LNF"]
PROD_SRM = PADME_SRM_URI["CNAF"]
#PROD_MC_VERSION = "v0.4.0"
PROD_MC_VERSION = "develop"
PROD_PROXY_FILE = ""

def now_str(): return time.strftime("%Y-%m-%d %H:%M:%S",time.gmtime())

def print_help():

    print "PadmeMCProd -n <prod_name> -j <number_of_jobs> [-m <padmemc_version>] [-c <config_file>] [-s <submission_site>] [-d <storage_site>] [-h]"
    print "  <prod_name>\t\tname for the production"
    print "  <number_of_jobs>\tnumber of production jobs to submit. Must be >0 and <=1000"
    print "  <padmemc_version>\tversion of PadmeMC to use for production. Must be installed on CVMFS. Default: develop"
    print "  <config_file>\t\tname of configuration file to use. Default: cfg/<prod_name>.cfg"
    print "  <submission_site>\tsite where the jobs will be submitted. Allowed: LNF,CNAF. Default: LNF"
    print "  <storage_site>\tsite where the jobs output will be stored. Allowed: LNF,CNAF. Default: CNAF"

def main(argv):

    # Declare that here we can possibly modify these global variables
    global PROD_NAME
    global PROD_NJOBS
    global PROD_CONFIG_FILE
    global PROD_STORAGE_DIR
    global PROD_DIR
    global PROD_SCRIPT
    global PROD_CE
    global PROD_SRM
    global PROD_MC_VERSION
    global PROD_PROXY_FILE

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
        elif opt == '-d':
            if arg in PADME_SRM_URI.keys():
                PROD_SRM = PADME_SRT_URI[arg]
            else:
                print "*** ERROR *** Invalid storage site %s. Valid: %s"%(arg,",".join(PADME_SRM_URI.keys()))
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
    print "Storage SRM:",PROD_SRM
    print "Storage directory:",PROD_STORAGE_DIR

    # Check if production dir already exists
    if os.path.exists(PROD_DIR):
        print "*** ERROR *** Path %s already exists"%PROD_DIR
        sys.exit(2)

    # Check configuration file and read it into a variable (will go to DB)
    if not os.path.isfile(PROD_CONFIG_FILE):
        print "*** ERROR *** Configuration file '%s' does not exist"%PROD_CONFIG_FILE
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

    # Create new production in DB
    print "- Creating new production in DB"
    # This will improve when we have a web interface to handle production requests
    PROD_DESCRIPTION = "TEST"
    PROD_USER_REQ = "EL"
    PROD_NEVENTS_REQ = 0
    DB.create_mcprod(PROD_NAME,PROD_DESCRIPTION,PROD_USER_REQ,PROD_NEVENTS_REQ,PROD_CE,PROD_MC_VERSION,PROD_DIR,PROD_STORAGE_DIR,PROD_SRM,PROD_PROXY_FILE,now_str(),PROD_NJOBS)
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
    for j in range(0,PROD_NJOBS):

        jobName = "job%05d"%j

        # Create dir to hold individual job info
        jobDir = "%s/%s"%(PROD_DIR,jobName)
        os.mkdir(jobDir)

        # Copy production script to job dir
        shutil.copyfile(PROD_SCRIPT,"%s/job.py"%jobDir)

        # Copy common configuration file to job dir
        jobCfgFile = "%s/job.mac"%jobDir
        shutil.copyfile(PROD_CONFIG_FILE,jobCfgFile)
        with open(jobCfgFile,"r") as jcf: jobCfg=jcf.read()

        # Input file list is empty for MC
        jobList = ""

        # Copy long-lived proxy file to job dir
        shutil.copyfile(PROD_PROXY_FILE,"%s/job.proxy"%jobDir)

        # Create JDL file in job dir
        jf = open("%s/job.jdl"%jobDir,"w")
	jf.write("[\n")
	jf.write("Type = \"Job\";\n")
	jf.write("JobType = \"Normal\";\n")
	jf.write("Executable = \"/usr/bin/python\";\n")
	jf.write("Arguments = \"-u job.py job.mac job.proxy %s %s %s %s %s\";\n"%(PROD_NAME,jobName,PROD_MC_VERSION,PROD_STORAGE_DIR,PROD_SRM))
	jf.write("StdOutput = \"job.out\";\n")
	jf.write("StdError = \"job.err\";\n")
	jf.write("InputSandbox = {\"job.py\",\"job.mac\",\"job.proxy\"};\n")
	jf.write("OutputSandbox = {\"job.out\", \"job.err\", \"job.sh\"};\n")
	jf.write("OutputSandboxBaseDestURI=\"gsiftp://localhost\";\n")
	jf.write("]\n")
        jf.close()

        # Create job entry in DB and register job (no input file list)
        DB.create_job(prodId,jobName,jobDir,jobCfg,jobList)

    # Prepare daemon context

    # Assume that the current directory is the top level MC Production directory
    top_prod_dir = os.getcwd()
    print "Production working dir:",top_prod_dir

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

# Execution starts here
if __name__ == "__main__": main(sys.argv[1:])
