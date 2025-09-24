#!/bin/bash

# Pause between checks in seconds
pause=10

# Current year
year=$( date +%Y )

# Rawdata directory
inputDir="/mnt/l0padme2/MMchamber"

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

# Initialize status of ChamberMonitor process to NOT RUNNING
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
echo "=== ChamberMonitor_wd program starting at $( date -u ) ==="

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

	# If old ChamberMonitor process status is still RUNNING, tell it to stop and change its status to NOT RUNNING
	if [ "$om_running" -eq "1" ]; then
	    echo "$( date -u ) - Run name changed from $current_run_save to $current_run but ChamberMonitor process $om_pid is still active: telling it to stop"
	    touch $stopFile
	    om_running=0
	fi

	# Wait for run directory to appear
	while :; do
	    # Directory where rawdata is stored
	    runDir="${inputDir}/${current_run}"
	    if [ -d "$runDir" ]; then break; fi
	    sleep $pause
	    # Take into account that run can change before chamber daq is started (e.g. cosmics run)
	    get_current_run
	done
	echo "$( date -u ) - Directory $runDir was created"

	# Wait for first file to appear
	while :; do
	    lastFile=$( ls -rt  $runDir | tail -1 )
	    if [ -s "${runDir}/${lastFile}" ]; then break; fi
	    sleep $pause
	done
	echo "$( date -u ) - Non-empty file $lastFile created in directory $runDir"

	# Log and error files for ChamberMonitor of new run
	logFile="log/MM_${current_run}.log"
	errFile="log/MM_${current_run}.err"

	# File used to stop this OnlineMonitor
	stopFile="run/MM_${current_run}.stop"

	# Tag file to create to signal that the run has ended
	endrunFile="run/MM_${current_run}.endrun"

	# Extract chamber run name from file name
	current_ch_run=$( echo $lastFile | sed -e "s/.root//" | sed -r -e "s/_[0-9]{1,4}$//" )

	echo
	echo "*** $( date -u ) - Starting ChamberMonitor ***"
	echo "  Run: $current_run"
	echo "  Chamber Run: $current_ch_run"
	echo "  Input rawdata directory: $runDir"
	echo "  Log file: $logFile"
	echo "  Error file: $errFile"
	echo "  Stop file: $stopFile"
	echo "  EOR file: $endrunFile"

	# Start ChamberMonitor for new run
	echo "> stdbuf -oL nohup ./ChamberMonitor -f -r -R $current_ch_run -D $runDir -c $configFile -o $watchDir -s $stopFile -e $endrunFile -v -v 1>>$logFile 2>>$errFile </dev/zero &"
	stdbuf -oL nohup ./ChamberMonitor -f -r -R $current_ch_run -D $runDir -c $configFile -o $watchDir -s $stopFile -e $endrunFile -v -v 1>>$logFile 2>>$errFile </dev/zero &
	om_pid=$!

	# Change status of ChamberMontior process to RUNNING
	om_running=1

	# Save name of current run
	current_run_save=$current_run

    fi

    if [ "$om_running" -eq "1" ]; then

	# Check if run is still on-going and tell ChamberMonitor to stop if run has ended
	get_last_run
	if [ "$last_run" = "$current_run" ]; then
	    echo "$( date -u ) - Run $current_run has ended: telling ChamberMonitor process $om_pid to exit"
	    touch $stopFile
	    om_running=0
	    sleep $pause
	fi

	# Check if ChamberMonitor process is still running and restart it if it is dead
	kill -s 0 $om_pid 2>/dev/null
	if [ $? -ne 0 ]; then
	    # If process is dead because the run was stopped, do not restart the ChamberMonitor process
	    if [ "$last_run" = "$current_run" ]; then
	    	echo "$( date -u ) - ChamberMonitor process $om_pid exited because run $current_run has ended"
	    #elif [ -f $endrunFile ]; then
	    #	echo "$now - ChamberMonitor process $om_pid exited because the run has ended"
	    #	om_running=0
	    else
		echo "$( date -u ) - WARNING - ChamberMonitor process $om_pid is dead but run $current_run is still active: restart it"
		echo "> stdbuf -oL nohup ./ChamberMonitor -f -r -R $current_ch_run -D $runDir -c $configFile -o $watchDir -s $stopFile -e $endrunFile -v -v 1>>$logFile 2>>$errFile </dev/zero &"
		stdbuf -oL nohup ./ChamberMonitor -f -r -R $current_ch_run -D $runDir -c $configFile -o $watchDir -s $stopFile -e $endrunFile -v -v 1>>$logFile 2>>$errFile </dev/zero &
		om_pid=$!
	    fi
	fi

    fi

    # Wait a bit before checking again
    sleep $pause

done
