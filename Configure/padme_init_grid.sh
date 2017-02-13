#!/bin/bash

# Check if CVMFS is available with all needed software packages
if [ ! -d /cvmfs ]; then
    echo "ERROR - Directory /cvmfs does not exist on this node."
    exit 1
fi
if [ ! -d /cvmfs/sft.cern.ch ]; then
    echo "ERROR - Directory /cvmfs exists but sft.cern.ch is not available."
    exit 1
fi
if [ ! -d /cvmfs/geant4.cern.ch ]; then
    echo "ERROR - Directory /cvmfs exists but geant4.cern.ch is not available."
    exit 1
fi

# Execute gcc-4.9.3 configuration script
if [ -f /cvmfs/sft.cern.ch/lcg/releases/LCG_87/gcc/4.9.3/x86_64-slc6/setup.sh ]; then
    . /cvmfs/sft.cern.ch/lcg/releases/LCG_87/gcc/4.9.3/x86_64-slc6/setup.sh
else
    echo "ERROR - File /cvmfs/sft.cern.ch/lcg/releases/LCG_87/gcc/4.9.3/x86_64-slc6/setup.sh not found"
    exit 1
fi

# Execute ROOT-5.34.26 configuration script
if [ -f /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/5.34.36/x86_64-slc6-gcc49-opt/root/bin/thisroot.sh ]; then
    . /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/5.34.36/x86_64-slc6-gcc49-opt/root/bin/thisroot.sh
else
    echo "ERROR - File /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/5.34.36/x86_64-slc6-gcc49-opt/root/bin/thisroot.sh not found"
    exit 1
fi

# Execute GEANT4-10.3.ref01 configuration scripts
if [ -f /cvmfs/geant4.cern.ch/geant4/10.3.ref01/x86_64-slc6-gcc49-opt/bin/geant4.sh ]; then
    . /cvmfs/geant4.cern.ch/geant4/10.3.ref01/x86_64-slc6-gcc49-opt/bin/geant4.sh
else
    echo "ERROR - File /cvmfs/geant4.cern.ch/geant4/10.3.ref01/x86_64-slc6-gcc49-opt/bin/geant4.sh not found"
    exit 1
fi
if [ -f /cvmfs/geant4.cern.ch/geant4/10.3.ref01/x86_64-slc6-gcc49-opt/share/Geant4-10.3.0/geant4make/geant4make.sh ]; then
    . /cvmfs/geant4.cern.ch/geant4/10.3.ref01/x86_64-slc6-gcc49-opt/share/Geant4-10.3.0/geant4make/geant4make.sh
else
    echo "ERROR - File /cvmfs/geant4.cern.ch/geant4/10.3.ref01/x86_64-slc6-gcc49-opt/share/Geant4-10.3.0/geant4make/geant4make.sh not found"
    exit 1
fi

# Execute general PADME configuration script from the same dir as this script
THIS_DIR=$( dirname $( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd ) )
if [ -f ${THIS_DIR}/padme_init.sh ]; then
    . ${THIS_DIR}/padme_init.sh
else
    echo "ERROR - File ${THIS_DIR}/padme_init.sh not found."
fi
