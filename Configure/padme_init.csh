#!/bin/tcsh
# Find path to PADME software from position of this script
# assuming that this script is located in the Configure subdirectory
# of the main PADME software directory

set PADME=($_)
set PADME=`realpath -e $PADME[2]`
set PADME=$PADME:h:h
setenv PADME $PADME

# Define PADMEMC and Geant4 working dir
setenv PADMEMC ${PADME}/PadmeMC
setenv G4WORKDIR ${PADMEMC}

# Define PADMERECO
setenv PADMERECO ${PADME}/PadmeReco

# Define path to PadmeRoot and add it to LD_LIBRARY_PATH
setenv PADMEROOT ${PADME}/PadmeRoot
setenv LD_LIBRARY_PATH "${LD_LIBRARY_PATH}:${PADMEROOT}/lib"

# Configure PadmeDB access parameters fom convenitonal file
if ( -f ${HOME}/.padme-dbaccess.csh ) then
  source ${HOME}/.padme-dbaccess.csh
else
  echo "WARNING - File ${HOME}/.padme-dbaccess.csh not found: no DB access"
endif
