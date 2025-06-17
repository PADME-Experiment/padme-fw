#!/bin/bash

# Pause between checks in seconds
pause=10

# Current year
year=$( date +%Y )

# Main configuration file
configFile="config/OnlineMonitor.cfg"

# Watchdir directory (used by PadmeMonitor)
watchDir="/home/monitor/PadmeMonitor/watchdir"

# File with name of current run
current_run_file="/home/daq/DAQ/run/current_run"

# File with name of last finished run
last_run_file="/home/daq/DAQ/run/last_run"

# Variable to save last run started
current_run_save=""

# Initialize status of OnlineMonitor process to NOT RUNNING
om_running=0

while true; do 

    # Get current run
    if test -f "$current_run_file"; then
	current_run=$( cat $current_run_file )
    else
	echo "ERROR - File $current_run_file with name of current run is missing! ABORTING"
	exit 1
    fi

    # Check if the run has changed
    if [[ $current_run != $current_run_save ]]; then

	# If old OnlineMonitor process status is RUNNING, tell it to stop and change the status to NOT RUNNING
	if [ "$om_running" -eq "1" ]; then
	    touch $stopFile
	    om_running=0
	fi

	# Get node where data are written
	mergernode=$( grep merger_node /home/daq/DAQ/runs/${current_run}/cfg/${current_run}.cfg | grep -v merger_node_list | awk '{ print $2 }' )

	# Directory where data are stored
	inputDir="/mnt/${mergernode}/DAQ/${year}/rawdata"

	# Log and error files for OnlineMOnitor of new run
	logFile="log/${current_run}.log"
	errFile="log/${current_run}.err"

	# File used to stop this OnlineMonitor
	stopFile="run/${current_run}.stop"

	# Check if at least one file has appeared
	lastfile=$( ls -rt  $inputDir/$current_run | tail -1 )
	if [[ ! -z "$lastfile" ]]; then

	    now=$( date )

	    echo
	    echo "*** $now - Starting OnlineMonitor ***"
	    echo "  Run: $current_run"
	    echo "  Merger node: $mergernode"
	    echo "  Input rawdata directory: $inputDir"
	    echo "  Log file: $logFile"
	    echo "  Error file: $errFile"
	    echo "  Stop file: $stopFile"

	    # Start OnlineMonitor for new run
	    echo "> stdbuf -oL nohup ./OnlineMonitor -f -r -I -R $current_run -D $inputDir -c $configFile -o $watchDir -s $stopFile 1>>$logFile 2>>$errFile </dev/zero &"
	    stdbuf -oL nohup ./OnlineMonitor -f -r -I -R $current_run -D $inputDir -c $configFile -o $watchDir -s $stopFile 1>>$logFile 2>>$errFile </dev/zero &
	    om_pid=$!

	    # Change status of OnlineMontior process to RUNNING
	    om_running=1

	    # Save name of current run
	    current_run_save=$current_run

	fi
    fi

    # Check if OnlineMonitor process is still running and restart it if it is dead
    if [ "$om_running" -eq "1" ]; then
	kill -s 0 $om_pid 2>/dev/null
	if [ $? -ne 0 ]; then
	    # If process is dead because the run was stopped, change status of OnlineMonitor process to NOT RUNNING and do not restart it
	    if test -f "$last_run_file"; then
		last_run=$( cat $last_run_file )
	    else
		echo "WARNING - File $last_run_file with name of last finished run is missing. Please check DAQ!"
		last_run=""
	    fi
	    if [ "$last_run" = "$current_run" ]; then
		om_running=0
	    else
		now=$( date )
		echo "$now - WARNING - OnlineMonitor process $om_pid is dead but run $current_run is still active: restart it"
		echo "> stdbuf -oL nohup ./OnlineMonitor -f -r -I -R $current_run -D $inputDir -c $configFile -o $watchDir -s $stopFile 1>>$logFile 2>>$errFile </dev/zero &"
		stdbuf -oL nohup ./OnlineMonitor -f -r -I -R $current_run -D $inputDir -c $configFile -o $watchDir -s $stopFile 1>>$logFile 2>>$errFile </dev/zero &
		om_pid=$!
	    fi
	fi
    fi

    sleep $pause

done
