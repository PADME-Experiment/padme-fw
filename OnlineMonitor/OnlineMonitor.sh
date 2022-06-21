#!/bin/bash

# Pause between checks in seconds
pause=10

# Current year
year=$( date +"%Y" )

# Main configuration file
configFile="config/OnlineMonitor.cfg"

# Watchdir directory (used by PadmeMonitor)
watchDir="/home/monitor/PadmeMonitor/watchdir"

# Variable to save last run started
lastrun_save=""

while true; do 

    # Get current run
    lastrun=$( cat /home/daq/DAQ/run/current_run )

    if [[ $lastrun != $lastrun_save ]]; then

	# Tell old OnlineMonitor to stop
	if [[ ! -z "$lastrun_save" ]]; then
	    touch $stopFile
	fi

	# Get node where data are written
	mergernode=$( grep merger_node /home/daq/DAQ/runs/${lastrun}/cfg/${lastrun}.cfg | grep -v merger_node_list | awk '{ print $2 }' )

	# Directory where data are stored
	inputDir="/mnt/${mergernode}/DAQ/${year}/rawdata"

	# Log and error files for OnlineMOnitor of new run
	logFile="log/${lastrun}.log"
	errFile="log/${lastrun}.err"

	# File used to stop this OnlineMonitor
	stopFile="run/${lastrun}.stop"

	# Check if at least one file has appeared
	lastfile=$( ls -rt  $inputDir/$lastrun | tail -1 )
	if [[ ! -z "$lastfile" ]]; then

	    now=$( date )

	    echo
	    echo "$now - Starting OnlineMonitor for run ${lastrun} on merger node ${mergernode} ***"
	    echo "  Input rawdata directory: $inputDir"

	    # Start OnlineMonitor for new run
	    echo "> stdbuf -oL nohup ./OnlineMonitor -f -R $lastrun -D $inputDir -c $configFile -o $watchDir -s $stopFile >>$logFile 2>$errFile </dev/zero &"
	    stdbuf -oL nohup ./OnlineMonitor -f -R $lastrun -D $inputDir -c $configFile -o $watchDir -s $stopFile >>$logFile 2>$errFile </dev/zero &

	    # Save name of last run
	    lastrun_save=$lastrun

	fi
    fi

    sleep $pause

done
