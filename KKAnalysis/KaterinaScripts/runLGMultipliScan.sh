#!/bin/bash
source /home/katerina/padme/reco/Configure/padme_init.sh

nevents=1000000

for input in run{385..395}.list; do
    #Check if the file exists
    if [ -f "$input" ]; then
	output="out/out_${input%.list}cut.root"

    #Filter events in some of the runs
    if [ "$input" == "run389.list" ]; then 
    nevents=14000 
    elif [ "$input" == "run390.list" ]; then 
    nevents=4500
    elif [ "$input" == "run395.list" ]; then 
    nevents=10500
    else
    nevents=1000000
    fi

    echo ""
	echo "Running leadglass.cpp for $input -> output: $output -- up to $nevents events"
	echo ""
	program_output=$(./LGRead -l "$input" -o "$output" -n "$nevents")
    #echo "$program_output"

    else
	echo "Warning: $input does not exist, skipping"
    fi
done
echo "Processing completed."
