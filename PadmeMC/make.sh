#!/bin/zsh

source /home/padme/git/padme-fw/Configure/padme_init.zsh
incdirs=( $PWD/**/include /opt/*/include /opt/LCG_88_gcc_6.2.0_x86_64-centos7/include/c++/6.2.0//   $HOME/git/padme-fw/PadmeRoot/include  /opt/LCG_88_Geant4_10.02.p02_x86_64-centos7-gcc62-opt/include/Geant4/)
export CPATH=${(pj|:|)incdirs}
echo $CPATH
export CPATH=/opt/LCG_88_Geant4_10.02.p02_x86_64-centos7-gcc62-opt/include/Geant4:$CPATH

echo 
echo 
echo 
echo 
/opt/LCG_88_gcc_6.2.0_x86_64-centos7/bin/g++ -g -O0 -save-temps -fvisibility=hidden **/src/*.cc PADMEMC.cc -D G4MAJORVERSION=10 -L/opt/LCG_88_ROOT_6.08.06_x86_64-centos7-gcc62-opt/lib -lCore -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -lMultiProc -pthread -lm -ldl -rdynamic  -L/opt/LCG_88_Geant4_10.02.p02_x86_64-centos7-gcc62-opt/bin/../lib64 -lG4gl2ps -lG4FR -lG4GMocren -lG4visHepRep -lG4RayTracer -lG4VRML -lG4vis_management -lG4modeling -lG3toG4 -lG4interfaces -lG4persistency -lG4analysis -lG4error_propagation -lG4readout -lG4physicslists -lG4run -lG4event -lG4tracking -lG4parmodels -lG4processes -lG4digits_hits -lG4track -lG4particles -lG4geometry -lG4materials -lG4graphics_reps -lG4intercoms -lG4global -L/cvmfs/sft.cern.ch/lcg/releases/clhep/2.3.1.1-6ba0c/x86_64-centos7-gcc62-opt/lib -lCLHEP -lG4expat -lG4zlib

#-lG4OpenGL  -lG4Tree
