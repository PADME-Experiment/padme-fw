#!/bin/zsh

# Find path to PADME software from position of this script
# assuming that this script is located in the Configure subdirectory
# of the main PADME software directory
export PADME=$0:A:h:h

# Define PADMEMC and Geant4 working dir
export PADMEMC=${PADME}/PadmeMC
export G4WORKDIR=${PADMEMC}

# Define PADMERECO
export PADMERECO=${PADME}/PadmeReco


# Define path to PadmeRoot and add it to LD_LIBRARY_PATH
export PADMEROOT=${PADME}/PadmeRoot
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${PADMEROOT}/lib"

# Configure PadmeDB access parameters fom convenitonal file
if [ -f ${HOME}/.padme-dbaccess.sh ]
then
    source ${HOME}/.padme-dbaccess.sh
else
    echo "WARNING - File ${HOME}/.padme-dbaccess.sh not found: no DB access"
fi
export CPATH=${(pja:a)"${(f)$(ls -d $PADME/**/include)}"}:$CPATH
export LIBRARY_PATH=${(pja:a)"${(f)$(ls -d $PADMERECO/**/lib)}"}:$LIBRARY_PATH
export LIBRARY_PATH=${(pja:a)"${(f)$(ls -d $PADMEROOT/**/lib)}"}:$LIBRARY_PATH
export LD_LIBRARY_PATH=${(pja:a)"${(f)$(ls -d $PADMERECO/**/lib)}"}:$LD_LIBRARY_PATH
