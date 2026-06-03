#!/bin/bash

padmerun="$1"
run="$2"

echo
echo "Processing run ${run}"
echo

root -l -b <<EOF
.L Reco.C++
Reco p(0,"${padmerun}","${run}")
p.Loop("${padmerun}","${run}")
.q
EOF

rm *_C*
rm *~

echo
echo "Done!"
echo
