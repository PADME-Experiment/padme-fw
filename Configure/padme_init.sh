#!/bin/bash

# Find path to PADME software from position of this script
# assuming that this script is located in the Configure subdirectory
# of the main PADME software directory
export PADME=$( dirname $( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd ) )

# Define PADMEMC and Geant4 working dir
export PADMEMC=${PADME}/PadmeMC
export G4WORKDIR=${PADMEMC}

# Define PADMERECO
export PADMERECO=${PADME}/PadmeReco

# Define PADMERECO3
export PADMERECO3=${PADME}/PadmeReco3

# Define PADMEANALYSIS
export PADMEANALYSIS=${PADME}/PadmeAnalysis

# Define path to PadmeRoot and add it to LD_LIBRARY_PATH
export PADMEROOT=${PADME}/PadmeRoot

# Add PadmeRoot to LD_LIBRARY_PATH (also remove previous definitions)
#export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${PADMEROOT}/lib"
clean_ld_library_path=`echo $LD_LIBRARY_PATH | tr ":" "\n" | grep -v PadmeRoot | paste -sd ":"`
export LD_LIBRARY_PATH="${clean_ld_library_path}:${PADMEROOT}/lib"
unset clean_ld_library_path

# Configure PadmeDB access parameters fom convenitonal file
if [ -f ${HOME}/.padme-dbaccess.sh ]
then
    source ${HOME}/.padme-dbaccess.sh
#else
#    echo "WARNING - File ${HOME}/.padme-dbaccess.sh not found: no DB access"
fi
