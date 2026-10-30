#!/bin/bash

runnumber="$1"

echo "Removing Reco of run ${runnumber}"

rm /data9Vd1/padme/wnatale/VMMStudy/RecoEvents/*"${runnumber}"*

echo "Done"
