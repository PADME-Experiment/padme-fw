#!/bin/bash

# cd /cvmfs/sft.cern.ch/lcg/releases/; ls -1d LCG_88/{gcc,ROOT,clhep,Geant4,XercesC}/*/*slc6*|grep -v "dbg$"

LCGPREF=/cvmfs/sft.cern.ch/lcg/releases/

PACKS=(
#LCG_88/clhep/2.3.1.1/x86_64-slc6-gcc49-opt
LCG_88/clhep/2.3.1.1/x86_64-slc6-gcc62-opt
#LCG_88/gcc/4.9.1/x86_64-slc6
#LCG_88/gcc/4.9.3/x86_64-slc6
LCG_88/gcc/6.2.0/x86_64-slc6
#LCG_88/Geant4/10.02.p02/x86_64-slc6-gcc49-opt
LCG_88/Geant4/10.02.p02/x86_64-slc6-gcc62-opt
#LCG_88/ROOT/6.08.06/x86_64-slc6-gcc49-opt
LCG_88/ROOT/6.08.06/x86_64-slc6-gcc62-opt
#LCG_88/XercesC/3.1.3/x86_64-slc6-gcc49-opt
LCG_88/XercesC/3.1.3/x86_64-slc6-gcc62-opt
)

for pak in ${PACKS[@]}; do 
echo
echo
echo rsync -auvvessh  ggeorge@lxplus.cern.ch:$LCGPREF/$pak/. /opt/${pak//\//_}
echo
rsync -auvvessh --del ggeorge@lxplus.cern.ch:$LCGPREF/$pak/. /opt/${pak//\//_}
done
