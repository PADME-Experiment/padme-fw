#!/bin/bash

# Pause between checks in seconds
pause=10

# Current year
year=$( date +%Y )

# Rawdata directory
inputDir="/mnt/l0padme6/TMMchamber"

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

# Initialize status of SmallChamberMonitor process to NOT RUNNING
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

	# If old SmallChamberMonitor process status is RUNNING, tell it to stop and change the status to NOT RUNNING
	if [ "$om_running" -eq "1" ]; then
	    touch $stopFile
	    om_running=0
	fi

	# Log and error files for SmallChamberMonitor of new run
	logFile="log/TMM_${current_run}.log"
	errFile="log/TMM_${current_run}.err"

	# File used to stop this SmallOnlineMonitor
	stopFile="run/TMM_${current_run}.stop"

	# Tag file to create to signal that the run has ended
	endrunFile="run/TMM_${current_run}.endrun"

	# Check if at least one file has appeared
	lastfile=$( ls -rt  $inputDir/$current_run | tail -1 )
	if [[ ! -z "$lastfile" ]]; then

	    now=$( date -u )

	    # Extract chamber run name from file name
	    current_ch_run=$( echo $lastfile | sed -e "s/.root//" | sed -r -e "s/_[0-9]{1,4}$//" )

	    echo
	    echo "*** $now - Starting SmallChamberMonitor ***"
	    echo "  Run: $current_run"
	    echo "  Chamber Run: $current_ch_run"
	    echo "  Input rawdata directory: ${inputDir}/${current_run}"
	    echo "  Log file: $logFile"
	    echo "  Error file: $errFile"
	    echo "  Stop file: $stopFile"
	    echo "  EOR file: $endrunFile"

	    # Start ChamberMonitor for new run
	    echo "> stdbuf -oL nohup ./SmallChamberMonitor -f -r -R $current_ch_run -D ${inputDir}/${current_run} -c $configFile -o $watchDir -s $stopFile -e $endrunFile -v -v 1>>$logFile 2>>$errFile </dev/zero &"
	    stdbuf -oL nohup ./SmallChamberMonitor -f -r -R $current_ch_run -D ${inputDir}/${current_run} -c $configFile -o $watchDir -s $stopFile -e $endrunFile -v -v 1>>$logFile 2>>$errFile </dev/zero &
	    om_pid=$!

	    # Change status of SmallChamberMontior process to RUNNING
	    om_running=1

	    # Save name of current run
	    current_run_save=$current_run

	fi

    fi

    # Check if SmallChamberMonitor process is still running and restart it if it is dead
    if [ "$om_running" -eq "1" ]; then
	now=$( date -u )
	kill -s 0 $om_pid 2>/dev/null
	if [ $? -ne 0 ]; then
	    # If process is dead because the run was stopped, change status of OnlineMonitor process to NOT RUNNING and do not restart it
	    if test -f "$last_run_file"; then
		last_run=$( cat $last_run_file )
	    else
		echo "$now - WARNING - File $last_run_file with name of last finished run is missing. Please check DAQ!"
		last_run=""
	    fi
	    if [ "$last_run" = "$current_run" ]; then
	    	echo "$now - SmallChamberMonitor process $om_pid exited because the run has officially ended"
		om_running=0
	    #elif [ -f $endrunFile ]; then
	    #	echo "$now - SmallChamberMonitor process $om_pid exited because the run has ended"
	    #	om_running=0
	    else
		echo "$now - WARNING - SmallChamberMonitor process $om_pid is dead but run $current_run is still active: restart it"
		echo "> stdbuf -oL nohup ./SmallChamberMonitor -f -r -R $current_ch_run -D ${inputDir}/${current_run} -c $configFile -o $watchDir -s $stopFile -e $endrunFile -v -v 1>>$logFile 2>>$errFile </dev/zero &"
		stdbuf -oL nohup ./SmallChamberMonitor -f -r -R $current_ch_run -D ${inputDir}/${current_run} -c $configFile -o $watchDir -s $stopFile -e $endrunFile -v -v 1>>$logFile 2>>$errFile </dev/zero &
		om_pid=$!
	    fi
	fi
    fi

    sleep $pause

done
