
source /opt/LCG_88_gcc_6.2.0_x86_64-centos7/setup.sh
source /opt/LCG_88_ROOT_6.08.06_x86_64-centos7-gcc62-opt/bin/thisroot.sh
source /opt/LCG_88_Geant4_10.02.p02_x86_64-centos7-gcc62-opt/bin/geant4.sh
source /opt/LCG_88_Geant4_10.02.p02_x86_64-centos7-gcc62-opt/share/Geant4-10.2.2/geant4make/geant4make.sh

source $HOME/git/padme-fw/Configure/padme_init.sh
export CPATH=/opt/LCG_88_clhep_2.3.1.1_x86_64-centos7-gcc62-opt/include:$CPATH
export LD_LIBRARY_PATH=/opt/LCG_88_clhep_2.3.1.1_x86_64-centos7-gcc62-opt/include:$LD_LIBRARY_PATH

export G4INSTALL=/opt/LCG_88_Geant4_10.02.p02_x86_64-centos7-gcc62-opt/share/Geant4-10.2.2/geant4make


export LD_LIBRARY_PATH=/opt/LCG_88_clhep_2.3.1.1_x86_64-centos7-gcc62-opt/lib/:/opt/LCG_88_XercesC_3.1.3_x86_64-centos7-gcc62-opt/lib:$LD_LIBRARY_PATH
export LIBRARY_PATH=/opt/LCG_88_clhep_2.3.1.1_x86_64-centos7-gcc62-opt/lib/:/opt/LCG_88_XercesC_3.1.3_x86_64-centos7-gcc62-opt/lib:$LIBRARY_PATH
export LD_LIBRARY_PATH=/usr/lib64:$LD_LIBRARY_PATH
