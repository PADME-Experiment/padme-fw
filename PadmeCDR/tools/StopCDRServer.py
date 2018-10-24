#!/usr/bin/python

import os
import sys
import time

# Get position of CDR main directory from PADME_CDR_DIR environment variable
# Default to current dir if not set
cdr_dir = os.getenv('PADME_CDR_DIR',".")

# Path to stop_cdr file: when file appears, server will remove it and gently exit
stop_cdr_file = cdr_dir+"/run/stop_cdr"

# Check if stop_cdr file already exists
if (os.path.exists(stop_cdr_file)):
    print "WARNING: File %s already exists (?). Exiting"%stop_cdr_file
    sys.exit(1)

# Create stop_cdr file
print "Signaling PadmeCDR server to stop. May need a few seconds..."
open(stop_cdr_file,"w").close()

# Check if stop_cdr file disappears, i.e. the PadmeCDR server saw and deleted it
max_try = 60
n_try = 0
while(True):
    if (not os.path.exists(stop_cdr_file)): break
    n_try += 1
    if (n_try>max_try):
        print "WARNING: PadmeCDR did not react to stop_cdr file within 1 minute. Please investigate"
        break
    time.sleep(1)
