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

    # Top CVMFS direcotry for PadmeMC
    padmemc_cvmfs_dir = "/cvmfs/padme.roma1.infn.it/PadmeMC"

    # Top SRM path to CNAF tape library
    cnaf_padme_srm = "srm://storm-fe-archive.cr.cnaf.infn.it:8444/srm/managerv2?SFN=/padmeTape"

    (prod_name,job_name,mc_version,cfg_file,storage_dir,proxy_file) = argv

    job_dir = os.getcwd()

    print "=== PadmeMC Production %s Job %s ==="%(prod_name,job_name)
    print "Job starting at %s (UTC)"%now_str()
    print "Job running on node %s as user %s in dir %s"%(os.getenv('HOSTNAME'),os.getenv('USER'),job_dir)

    print "PadmeMC version",mc_version
    print "SRM server URI",cnaf_padme_srm
    print "Storage directory",storage_dir
    print "MC macro file",cfg_file
    print "Proxy file",proxy_file

    # Change permission rights for long-lived proxy (must be 600)
    os.chmod(proxy_file,0600)

    # Check if software directory for this version is available
    padmemc_version_dir = "%s/%s"%(padmemc_cvmfs_dir,mc_version)
    if not os.path.isdir(padmemc_version_dir):
        print "ERROR Directory %s not found"%padmemc_version_dir
        exit(2)

    # Check if padmemc configuration file is available
    padmemc_init_file = "%s/config/padme-configure.sh"%padmemc_version_dir
    if not os.path.exists(padmemc_init_file):
        print "ERROR File %s not found"%padmemc_init_file
        exit(2)

    # Check if PADMEMC executable is available
    padmemc_exe_file = "%s/PADMEMC"%padmemc_version_dir
    if not os.path.exists(padmemc_exe_file):
        print "ERROR File %s not found"%padmemc_exe_file
        exit(2)

    # Prepare shell script to run PadmeMC
    sf = open("job.sh","w")
    sf.write("#!/bin/bash\n")
    sf.write("echo \"--- Starting PADMEMC production ---\"\n")
    sf.write(". %s\n"%padmemc_init_file)
    sf.write("echo \"PADME = $PADME\"\n")
    sf.write("echo \"LD_LIBRARY_PATH = $LD_LIBRARY_PATH\"\n")
    sf.write("%s job.mac\n"%padmemc_exe_file)
    sf.write("pwd; ls -l\n")
    sf.close()

    # Run job script sending its output/error to stdout/stderr
    print "Production starting at %s (UTC)"%now_str()
    job_cmd = "/bin/bash job.sh"
    rc = subprocess.call(job_cmd.split())
    print "Production ending at %s (UTC)"%now_str()

    print "PADMEMC program ended with return code %s"%rc

    if rc == 0:

        print "--- Saving output files to CNAF tape library ---"

        # Obtain new VOMS proxy from long-lived proxy
        proxy_cmd = "voms-proxy-init --noregen --cert %s --key %s --voms vo.padme.org"%(proxy_file,proxy_file)
        print ">",proxy_cmd
        rc = subprocess.call(proxy_cmd.split())

        if os.path.exists("data.root"):

            data_src_file = "data.root"
            data_size = os.path.getsize(data_src_file)
            data_adler32 = get_adler32(data_src_file)
            data_src_uri = "file://%s/%s"%(job_dir,data_src_file)

            data_dst_file = "%s_%s_data.root"%(prod_name,job_name)
            data_dst_uri = "%s%s/%s"%(cnaf_padme_srm,storage_dir,data_dst_file)

            print "Copying",data_src_uri,"to",data_dst_uri
            data_copy_cmd = "gfal-copy %s %s"%(data_src_uri,data_dst_uri)
            print ">",data_copy_cmd
            rc = subprocess.call(data_copy_cmd.split())

            print "Data file %s with size %s and adler32 %s copied to CNAF"%(data_dst_file,data_size,data_adler32)

        else:

            print "WARNING File data.root does not exist in current directory"

        if os.path.exists("hsto.root"):

            hsto_src_file = "hsto.root"
            hsto_size = os.path.getsize(hsto_src_file)
            hsto_adler32 = get_adler32(hsto_src_file)
            hsto_src_uri = "file://%s/%s"%(job_dir,hsto_src_file)

            hsto_dst_file = "%s_%s_hsto.root"%(prod_name,job_name)
            hsto_dst_uri = "%s%s/%s"%(cnaf_padme_srm,storage_dir,hsto_dst_file)

            print "Copying %s to %s"%(hsto_src_uri,hsto_dst_uri)
            hsto_copy_cmd = "gfal-copy %s %s"%(hsto_src_uri,hsto_dst_uri)
            print ">",hsto_copy_cmd
            rc = subprocess.call(hsto_copy_cmd.split())

            print "Hsto file %s with size %s and adler32 %s copied to CNAF"%(hsto_dst_file,hsto_size,hsto_adler32)

        else:

            print "WARNING File hsto.root does not exist in current directory"

    else:

        print "ERROR Some errors occourred during simulation. Please check log."
        print "Output files will not be saved to tape storage."

    print "Job ending at %s (UTC)"%now_str()

# Execution starts here
if __name__ == "__main__":

    main(sys.argv[1:])
