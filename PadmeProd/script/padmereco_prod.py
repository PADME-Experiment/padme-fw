#!/usr/bin/python

import os
import sys
import getopt
import time
import subprocess
import shutil

def now_str():

    return time.strftime("%Y-%m-%d %H:%M:%S",time.gmtime())

def get_adler32(outfile):

    adler_cmd = "adler32 %s"%outfile
    p = subprocess.Popen(adler_cmd.split(),stdin=None,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
    return p.communicate()[0].strip()

def main(argv):

    # Top CVMFS directory for PadmeReco
    padmereco_cvmfs_dir = "/cvmfs/padme.infn.it/PadmeReco"

    output_file = "data.root"

    (input_list,proxy_file,prod_name,job_name,reco_version,storage_dir,srm_uri) = argv

    job_dir = os.getcwd()

    print "=== PadmeReco Production %s Job %s ==="%(prod_name,job_name)
    print "Job starting at %s (UTC)"%now_str()
    print "Job running on node %s as user %s in dir %s"%(os.getenv('HOSTNAME'),os.getenv('USER'),job_dir)

    print "PadmeReco version",reco_version
    print "SRM server URI",srm_uri
    print "Storage directory",storage_dir
    print "Input file list",input_list
    print "Proxy file",proxy_file

    # Change permission rights for long-lived proxy (must be 600)
    os.chmod(proxy_file,0600)

    # Check if software directory for this version is available
    padmereco_version_dir = "%s/%s"%(padmereco_cvmfs_dir,reco_version)
    if not os.path.isdir(padmereco_version_dir):
        print "ERROR Directory %s not found"%padmereco_version_dir
        exit(2)

    # Create local soft link to the config directory
    config_dir = "%s/config"%padmereco_version_dir
    if not os.path.isdir(config_dir):
        print "ERROR Directory %s not found"%config_dir
        exit(2)
    os.symlink(config_dir,"config")

    # Check if PadmeReco configuration file is available
    padmereco_init_file = "%s/padme-configure.sh"%config_dir
    if not os.path.exists(padmereco_init_file):
        print "ERROR File %s not found"%padmereco_init_file
        exit(2)

    # Prepare shell script to run PadmeReco
    sf = open("job.sh","w")
    sf.write("#!/bin/bash\n")
    sf.write("echo \"--- Starting PADMERECO production ---\"\n")
    sf.write(". %s\n"%padmereco_init_file)
    sf.write("echo \"PADME = $PADME\"\n")
    sf.write("echo \"PADMERECO_EXE = $PADMERECO_EXE\"\n")
    sf.write("echo \"LD_LIBRARY_PATH = $LD_LIBRARY_PATH\"\n")
    sf.write("$PADMERECO_EXE -l %s -o %s -n 0\n"%(input_list,output_file))
    sf.write("pwd; ls -l\n")
    sf.close()

    # Run job script sending its output/error to stdout/stderr
    print "Program starting at %s (UTC)"%now_str()
    job_cmd = "/bin/bash job.sh"
    rc = subprocess.call(job_cmd.split())
    print "Program ending at %s (UTC)"%now_str()

    print "PADMERECO program ended with return code %s"%rc

    if rc == 0:

        print "--- Saving output files ---"

        # Obtain new VOMS proxy from long-lived proxy
        proxy_cmd = "voms-proxy-init --noregen --cert %s --key %s --voms vo.padme.org"%(proxy_file,proxy_file)
        print ">",proxy_cmd
        rc = subprocess.call(proxy_cmd.split())

        if os.path.exists(output_file):

            data_src_file = otuput_file
            data_size = os.path.getsize(data_src_file)
            data_adler32 = get_adler32(data_src_file)
            data_src_url = "file://%s/%s"%(job_dir,data_src_file)

            data_dst_file = "%s_%s_reco.root"%(prod_name,job_name)
            data_dst_url = "%s%s/%s"%(srm_uri,storage_dir,data_dst_file)

            print "Copying",data_src_url,"to",data_dst_url
            data_copy_cmd = "gfal-copy %s %s"%(data_src_url,data_dst_url)
            print ">",data_copy_cmd
            rc = subprocess.call(data_copy_cmd.split())

            print "RECODATA file %s with size %s and adler32 %s copied"%(data_dst_file,data_size,data_adler32)

        else:

            print "WARNING File data.root does not exist in current directory"

    else:

        print "ERROR Some errors occourred during reconstruction. Please check log."
        print "Output files will not be saved to tape storage."

    print "Job ending at %s (UTC)"%now_str()

# Execution starts here
if __name__ == "__main__":

    main(sys.argv[1:])
