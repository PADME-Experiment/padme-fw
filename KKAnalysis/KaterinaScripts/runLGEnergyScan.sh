#!/bin/bash

source /home/katerina/padme/reco/Configure/padme_init.sh

nevents=1000000

for input in run{311..316}.list; do
    #Check if the file exists
    if [ -f "$input" ]; then
	output="out_${input%.list}cut.root"

    #Filter events where needed
    #if [ "$input" == "run312.list" ]; then 
    #nevents=5500
    if [ "$input" == "run313.list" ]; then 
    nevents=21000
    elif [ "$input" == "run315.list" ]; then 
    nevents=15000
    else
    nevents=1000000
    fi

    echo ""
	echo "Running leadglass.cpp for $input -> output: $output -- up to $nevents events"
	echo ""
	program_output=$(./LGRead -l "$input" -o "$output" -n "$nevents")
    echo "$program_output"

    else
	echo "Warning: $input does not exist, skipping"
    fi
done
echo "Processing completed."
