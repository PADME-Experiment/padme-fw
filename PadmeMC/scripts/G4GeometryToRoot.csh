#!/bin/tcsh
rm -f Padme.gdml Padme.root
./PADMEMC macros/GenerateGDML.mac
root -l << EOF
.x macros/GDML2Root.cc
.q
EOF
