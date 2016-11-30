#!/bin/tcsh
# Find path to PADME software from position of this script
# assuming that this script is located in the Configure subdirectory
# of the main PADME software directory
set PADME=`realpath $0`
setenv PADME $PADME:h:h

# Define PADMEMC and Geant4 working dir
setenv PADMEMC ${PADME}/PadmeMC
setenv G4WORKDIR ${PADMEMC}

# Define PADMERECO
setenv PADMERECO ${PADME}/PadmeReco

# Define path to PadmeRoot and add it to LD_LIBRARY_PATH
setenv PADMEROOT ${PADME}/PadmeRoot
setenv LD_LIBRARY_PATH "${LD_LIBRARY_PATH}:${PADMEROOT}/lib"

# Configure PadmeDB access parameters fom convenitonal file
if ( -f ${HOME}/.padme-dbaccess.sh ) then
  source ${HOME}/.padme-dbaccess.sh
else
  echo "WARNING - File ${HOME}/.padme-dbaccess.sh not found: no DB access"
endif
