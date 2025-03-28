#!/bin/bash
source /home/katerina/padme/reco/Configure/padme_init.sh

nevents=1000000

for input in run{318..370}.list; do
    #Check if the file exists
    if [ -f "$input" ]; then
	output="out_${input%.list}cut.root"

    #Filter events in some of the runs
    if [ "$input" == "run321.list" ]; then 
    nevents=10100 
    elif [ "$input" == "run322.list" ]; then 
    nevents=7100 
    elif [ "$input" == "run326.list" ]; then 
    nevents=14500 
    elif [ "$input" == "run328.list" ]; then 
    nevents=13600
    elif [ "$input" == "run333.list" ]; then 
    nevents=7800
    elif [ "$input" == "run337.list" ]; then 
    nevents=16000
    elif [ "$input" == "run339.list" ]; then 
    nevents=9000 
    elif [ "$input" == "run342.list" ]; then 
    nevents=5500 
    elif [ "$input" == "run348.list" ]; then 
    nevents=11000 
    elif [ "$input" == "run349.list" ]; then 
    nevents=17500 
    elif [ "$input" == "run352.list" ]; then 
    nevents=2100 
    elif [ "$input" == "run362.list" ]; then 
    nevents=2400
    elif [ "$input" == "run364.list" ]; then 
    nevents=6000
    elif [ "$input" == "run365.list" ]; then 
    nevents=500
    elif [ "$input" == "run367.list" ]; then 
    nevents=3300
    elif [ "$input" == "run369.list" ]; then 
    nevents=7000
    elif [ "$input" == "run370.list" ]; then 
    nevents=1000
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
