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

# Functions to retrieve current and last run names
function get_current_run () {
    if test -f "$current_run_file"; then
	current_run=$( cat $current_run_file )
    else
	echo "ERROR - File $current_run_file with name of current run is missing! ABORTING"
	exit 1
    fi
}
function get_last_run () {
    if test -f "$last_run_file"; then
	last_run=$( cat $last_run_file )
    else
	echo "WARNING - File $last_run_file with name of last finished run is missing. Please check DAQ!"
	last_run=""
    fi
}

echo
echo "=== OnlineMonitor_wd program starting at $( date -u ) ==="

# Wait for an active run before doing anything
echo "- Waiting for an active run to appear..."
while :; do
    get_current_run
    get_last_run
    if [ "$current_run" != "$last_run" ]; then break; fi
    sleep $pause
done

echo "$( date -u ) - Active run $current_run found: starting monitoring"


while :; do 

    get_current_run

    # Check if the run has changed
    if [[ $current_run != $current_run_save ]]; then

	# If old OnlineMonitor process status is still RUNNING, tell it to stop and change its status to NOT RUNNING
	if [ "$om_running" -eq "1" ]; then
	    echo "$( date -u ) - Run name changed from $current_run_save to $current_run but OnlineMonitor process $om_pid is still active: telling it to stop"
	    touch $stopFile
	    om_running=0
	fi

	# Get node where data are written
	mergerNode=$( grep merger_node /home/daq/DAQ/runs/${current_run}/cfg/${current_run}.cfg | grep -v merger_node_list | awk '{ print $2 }' )

	# Directory where data are stored
	inputDir="/mnt/${mergerNode}/DAQ/${year}/rawdata"
	runDir="${inputDir}/${current_run}"

	# Wait for first file to appear
	while :; do
	    lastFile=$( ls -rt  $runDir | tail -1 )
	    if [ -s "${runDir}/${lastFile}" ]; then break; fi
	    sleep $pause
	done
	echo "$( date -u ) - Non-empty file $lastFile created in directory $runDir"

	# Log and error files for OnlineMOnitor of new run
	logFile="log/${current_run}.log"
	errFile="log/${current_run}.err"

	# File used to stop this OnlineMonitor
	stopFile="run/${current_run}.stop"

	echo
	echo "*** $( date -u ) - Starting OnlineMonitor ***"
	echo "  Run: $current_run"
	echo "  Merger node: $mergerNode"
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

    if [ "$om_running" -eq "1" ]; then

	# Check if run is still on-going and tell OnlineMonitor to stop if run has ended
	get_last_run
	if [ "$last_run" = "$current_run" ]; then
	    echo "$( date -u ) - Run $current_run has ended: telling OnlineMonitor process $om_pid to exit"
	    touch $stopFile
	    om_running=0
	    sleep $pause
	fi

	# Check if OnlineMonitor process is still running and restart it if it is dead
	kill -s 0 $om_pid 2>/dev/null
	if [ $? -ne 0 ]; then
	    # If process is dead because the run was stopped, do not restart the OnlineMonitor process
	    if [ "$last_run" = "$current_run" ]; then
	    	echo "$( date -u ) - OnlineMonitor process $om_pid exited because run $current_run has ended"
	    else
		echo "$( date -u ) - WARNING - OnlineMonitor process $om_pid is dead but run $current_run is still active: restart it"
		echo "> stdbuf -oL nohup ./OnlineMonitor -f -r -I -R $current_run -D $inputDir -c $configFile -o $watchDir -s $stopFile 1>>$logFile 2>>$errFile </dev/zero &"
		stdbuf -oL nohup ./OnlineMonitor -f -r -I -R $current_run -D $inputDir -c $configFile -o $watchDir -s $stopFile >>$logFile 2>>$errFile </dev/zero &
		om_pid=$!
	    fi
	fi

    fi

    # Wait a bit before checking again
    sleep $pause

done
