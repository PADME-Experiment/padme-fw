#!/bin/bash

# Pause between checks in seconds
pause=10

# Current year
year=$( date +%Y )

# Main configuration file
configFile="config/OnlineRecoMonitor.cfg"

# Watchdir directory (used by PadmeMonitor)
watchDir="/home/monitor/PadmeMonitor/watchdir"

# Node where reco files are written
recoFileNode="l1padme1"

# Trend file
trendFile="/home/monitor/DigiDaq/Trends/trend.root"

lastfile_processed=""
while true; do 

    lastrun=$( ls -rt /mnt/${recoFileNode}/DAQ/${year}/reco/ | tail -1 )
    lastfile=$( ls -rt /mnt/${recoFileNode}/DAQ/${year}/reco/${lastrun}/ | tail -1 )

    if [[ $lastfile != $lastfile_processed ]]; then

	inputFile="/mnt/${recoFileNode}/DAQ/${year}/reco/${lastrun}/${lastfile}"

	success=0
	for i in {1..5}; do
	    echo "> ./OnlineRecoMonitor -i $inputFile -t $trendFile -o $watchDir -c $configFile"
	    ./OnlineRecoMonitor -i $inputFile -t $trendFile -o $watchDir -c $configFile
	    if [ $? -eq 0 ]; then
		success=1
		break
	    fi
	    sleep $pause
	done
	if [ $success -eq 0 ]; then
	    echo "WARNING - Unable to run OnlineRecoMonitor on $inputFile"
	fi

	lastfile_processed=$lastfile

    fi

    sleep $pause

done
