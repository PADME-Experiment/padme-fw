#!/bin/bash

source /home/katerina/padme/reco/Configure/padme_init.sh

#nevents=1000000

for input in run{301..310}.list; do
    #Check if the file exists
    if [ -f "$input" ]; then
	    output="out_${input%.list}.root"
        echo ""
	    echo "Running leadglass.cpp for $input -> output: $output"
	    echo ""
	    program_output=$(./LGRead -l "$input" -o "$output")
        echo "$program_output"
    else
	    echo "Warning: $input does not exist, skipping"
    fi
done
echo "Processing completed."
