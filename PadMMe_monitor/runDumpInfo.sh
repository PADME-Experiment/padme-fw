#!/bin/bash

roofilepath="$1"
padmerun="$2"

echo
echo "Processing run ${padmerun}"
echo

root -l -b -q 'DumpInfo.C("${rootfilepath}","${padmerun}")'

rm *_C*
rm *~

echo
echo "Done!"
echo


