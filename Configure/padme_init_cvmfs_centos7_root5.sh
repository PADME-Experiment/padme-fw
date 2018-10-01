#!/bin/bash

THIS_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

PROBLEMS=0

GCC_SETUP_SCRIPT=/cvmfs/sft.cern.ch/lcg/contrib/gcc/4.9/x86_64-centos7-gcc49-opt/setup.sh
ROOT_SETUP_SCRIPT=/cvmfs/sft.cern.ch/lcg/app/releases/ROOT/5.34.36/x86_64-centos7-gcc49-opt/root/bin/thisroot.sh

# Check if CVMFS is available with all needed software packages
if [ ! -d /cvmfs ]; then
    echo "ERROR - Directory /cvmfs does not exist on this node."
    PROBLEMS=1
else
    if [ ! -d /cvmfs/sft.cern.ch ]; then
	echo "ERROR - Directory /cvmfs exists but sft.cern.ch is not available."
	PROBLEMS=1
    fi
#    if [ ! -d /cvmfs/geant4.cern.ch ]; then
#	echo "ERROR - Directory /cvmfs exists but geant4.cern.ch is not available."
#	PROBLEMS=1
#    fi
fi

# Check if configuration scripts are present
if [ ${PROBLEMS} -eq 0 ]; then

    # Look for gcc-4.9.3 configuration script
    if [ ! -f ${GCC_SETUP_SCRIPT} ]; then
	echo "ERROR - File ${GCC_SETUP_SCRIPT} not found"
	PROBLEMS=1
    fi

    # Look for ROOT-5.34.26 configuration script
    if [ ! -f ${ROOT_SETUP_SCRIPT} ]; then
	echo "ERROR - File ${ROOT_SETUP_SCRIPT} not found"
	PROBLEMS=1
    fi

    # Look for general PADME configuration script in the same dir as this script
    if [ ! -f ${THIS_DIR}/padme_init.sh ]; then
	echo "ERROR - File ${THIS_DIR}/padme_init.sh not found."
	PROBLEMS=1
    fi

fi

# Execute all configuration scripts
if [ ${PROBLEMS} -eq 0 ]; then
    . ${GCC_SETUP_SCRIPT}
    . ${ROOT_SETUP_SCRIPT}
    . ${THIS_DIR}/padme_init.sh
else
    echo "There were ERRORS: script aborting."
fi

unset PROBLEMS
unset THIS_DIR
unset GCC_SETUP_SCRIPT
unset ROOT_SETUP_SCRIPT
