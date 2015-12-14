#!/bin/bash

# Find path to PADME software from position of this script
# assuming that this script is located in the Configure subdirectory
# of the main PADME software directory
export PADME=$( dirname $( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd ) )

# Define PADMEMC and Geant4 working dir
export PADMEMC=${PADME}/PadmeMC
export G4WORKDIR=${PADMEMC}

# Add path to PadmeRoot libraries to LD_LIBRARY_PATH
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${PADME}/PadmeRoot/lib"

# Configure PadmeDB access parameters fom convenitonal file
source ${HOME}/.padme-dbaccess.sh
