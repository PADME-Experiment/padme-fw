#!/bin/bash

runnumber="$1"

echo "Removing Events of run ${runnumber}"

rm /data9Vd1/padme/wnatale/VMMStudy/Events/*"${runnumber}"*

echo "Done"
