#!/usr/bin/python

import os
import sys
import subprocess

# Get position of CDR main directory from PADME_CDR_DIR environment variable
# Default to current dir if not set
cdr_dir = os.getenv('PADME_CDR_DIR',".")

# Define position of long proxy file
long_proxy_file = "%s/run/long_proxy"%cdr_dir
#long_proxy_file = "./proxy_test"

# Show info about existing proxy file
info_cmd = "voms-proxy-info --file %s"%long_proxy_file
print ">",info_cmd
if subprocess.call(info_cmd.split()):
    print "- Long-lived proxy does not exist or is not accessible. Trying to create one..."

# Create new long proxy file
print "- Creating long-lived proxy file",long_proxy_file
proxy_cmd = "voms-proxy-init --valid 480:00 --out %s"%long_proxy_file
print ">",proxy_cmd
if subprocess.call(proxy_cmd.split()):
    print "*** ERROR *** while generating long-lived proxy. Aborting"
    sys.exit(2)

# Show info about new proxy file
info_cmd = "voms-proxy-info --file %s"%long_proxy_file
print ">",info_cmd
if subprocess.call(info_cmd.split()):
    print "*** ERROR *** while querying info about long-lived proxy. Aborting"
    sys.exit(2)
