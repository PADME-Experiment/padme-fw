#!/bin/bash

THIS_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

PROBLEMS=0

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
    if [ ! -f /cvmfs/sft.cern.ch/lcg/releases/LCG_87/gcc/4.9.3/x86_64-slc6/setup.sh ]; then
	echo "ERROR - File /cvmfs/sft.cern.ch/lcg/releases/LCG_87/gcc/4.9.3/x86_64-slc6/setup.sh not found"
	PROBLEMS=1
    fi

    # Look for ROOT-5.34.26 configuration script
    if [ ! -f /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/5.34.36/x86_64-slc6-gcc49-opt/root/bin/thisroot.sh ]; then
	echo "ERROR - File /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/5.34.36/x86_64-slc6-gcc49-opt/root/bin/thisroot.sh not found"
	PROBLEMS=1
    fi

    # Look for GEANT4-10.3.ref01 configuration scripts
    if [ ! -f /cvmfs/geant4.cern.ch/geant4/10.3.ref01/x86_64-slc6-gcc49-opt/bin/geant4.sh ]; then
	echo "ERROR - File /cvmfs/geant4.cern.ch/geant4/10.3.ref01/x86_64-slc6-gcc49-opt/bin/geant4.sh not found"
	PROBLEMS=1
    fi
    if [ ! -f /cvmfs/geant4.cern.ch/geant4/10.3.ref01/x86_64-slc6-gcc49-opt/share/Geant4-10.3.0/geant4make/geant4make.sh ]; then
	echo "ERROR - File /cvmfs/geant4.cern.ch/geant4/10.3.ref01/x86_64-slc6-gcc49-opt/share/Geant4-10.3.0/geant4make/geant4make.sh not found"
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
    . /cvmfs/sft.cern.ch/lcg/releases/LCG_87/gcc/4.9.3/x86_64-slc6/setup.sh
    . /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/5.34.36/x86_64-slc6-gcc49-opt/root/bin/thisroot.sh
    . /cvmfs/geant4.cern.ch/geant4/10.3.ref01/x86_64-slc6-gcc49-opt/bin/geant4.sh
    . /cvmfs/geant4.cern.ch/geant4/10.3.ref01/x86_64-slc6-gcc49-opt/share/Geant4-10.3.0/geant4make/geant4make.sh
    . ${THIS_DIR}/padme_init.sh
else
    echo "There were ERRORS: script aborting."
fi
