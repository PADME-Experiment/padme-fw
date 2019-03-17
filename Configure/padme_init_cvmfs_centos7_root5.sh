#!/bin/bash

THIS_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

PROBLEMS=0

GCC_SETUP_SCRIPT=/cvmfs/sft.cern.ch/lcg/contrib/gcc/4.9/x86_64-centos7-gcc49-opt/setup.sh
ROOT_SETUP_SCRIPT=/cvmfs/sft.cern.ch/lcg/app/releases/ROOT/5.34.36/x86_64-centos7-gcc49-opt/root/bin/thisroot.sh
GEANT4_SETUP_SCRIPT=/cvmfs/geant4.cern.ch/geant4/10.3.ref01/x86_64-slc6-gcc49-opt/bin/geant4.sh
GEANT4MAKE_SETUP_SCRIPT=/cvmfs/geant4.cern.ch/geant4/10.3.ref01/x86_64-slc6-gcc49-opt/share/Geant4-10.3.0/geant4make/geant4make.sh

# Check if CVMFS is available with all needed software packages
if [ ! -d /cvmfs ]; then
    echo "ERROR - Directory /cvmfs does not exist on this node."
    PROBLEMS=1
else
    if [ ! -d /cvmfs/sft.cern.ch ]; then
	echo "ERROR - Directory /cvmfs exists but sft.cern.ch is not available."
	PROBLEMS=1
    fi
    if [ ! -d /cvmfs/geant4.cern.ch ]; then
	echo "ERROR - Directory /cvmfs exists but geant4.cern.ch is not available."
	PROBLEMS=1
    fi
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

    # Look for GEANT4-10.3.ref01 configuration scripts
    if [ ! -f ${GEANT4_SETUP_SCRIPT} ]; then
        echo "ERROR - File ${GEANT4_SETUP_SCRIPT} not found"
        PROBLEMS=1
    fi
    if [ ! -f ${GEANT4MAKE_SETUP_SCRIPT} ]; then
        echo "ERROR - File ${GEANT4MAKE_SETUP_SCRIPT} not found"
        PROBLEMS=1
    fi

    # Look for general PADME configuration script in the same dir as this script
    if [ ! -f ${THIS_DIR}/padme_init.sh ]; then
	echo "ERROR - File ${THIS_DIR}/padme_init.sh not found."
	PROBLEMS=1
    fi

fi

if [ ${PROBLEMS} -eq 0 ]; then

    # Execute all configuration scripts
    . ${GCC_SETUP_SCRIPT}
    . ${ROOT_SETUP_SCRIPT}
    . ${GEANT4_SETUP_SCRIPT}
    . ${GEANT4MAKE_SETUP_SCRIPT}
    . ${THIS_DIR}/padme_init.sh

    # Fix LD_LIBRARY_PATH to remove faulty Qt library and duplicated paths
    paths=$(echo $LD_LIBRARY_PATH | tr ":" "\n" | sort | uniq | grep -v "/qt/")
    LD_LIBRARY_PATH=""
    for path in $paths
    do
	if [ -z "$LD_LIBRARY_PATH" ]; then
	    LD_LIBRARY_PATH=${path}
	else
	    LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${path}
	fi
    done
    export LD_LIBRARY_PATH

else
    echo "There were ERRORS: script aborting."
fi

unset PROBLEMS
unset THIS_DIR
unset GCC_SETUP_SCRIPT
unset ROOT_SETUP_SCRIPT
unset GEANT4_SETUP_SCRIPT
unset GEANT4MAKE_SETUP_SCRIPT
