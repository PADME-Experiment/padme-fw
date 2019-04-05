#!/bin/bash

usage() { echo "Usage: $0 -r run [-y year] [-j jobs] [-h]" 1>&2; exit 1; }

now() {
    date -u +"%Y-%m-%d %H:%M:%S"
}
export -f now

# Function to handle the prestage of a single file
prestage() {
    uri=$1
    status=$( gfal-xattr $uri user.status )
    echo $( now ) - $uri $status
    if [ "$status" == "NEARLINE " ]; then
	echo $( now ) - Prestaging $uri
	gfal-legacy-bringonline $uri
	rc=$?
	if [[ $rc -ne 0 ]]; then
	    echo $( now ) - WARNING - gfal-legacy-bringonline reported an error
	    return $rc
	fi
	status=$( gfal-xattr $uri user.status )
	echo $( now ) - $uri $status
	if [ "$status" == "NEARLINE " ]; then
	    echo $( now ) - WARNING - Prestaging failed
	    return 1
	fi
    fi
    return 0
}
export -f prestage

# Define Storm access point to CNAF tape library
cnaf_srm="srm://storm-fe-archive.cr.cnaf.infn.it:8444/srm/managerv2?SFN=/padmeTape"

run=""
year=""
#uri=""
jobs="200"
while getopts ":r:y:j:h" o; do
    case "${o}" in
	r)
	    run=${OPTARG}
	    ;;
        y)
            year=${OPTARG}
            ;;
        j)
            jobs=${OPTARG}
            ;;
        *)
            usage
            ;;
    esac
done

# Check if run was specified
if [ -z "${run}" ]; then
    echo "ERROR - No run specified"
    usage
    exit 1
fi

# If no year was specified, try and extract it from run name
if [ -z "${year}" ]; then
    regex="^run_[0-9]+_([0-9][0-9][0-9][0-9])[0-9]+_[0-9]+$"
    if [[ $run =~ $regex ]]; then
	year="${BASH_REMATCH[1]}"
    fi
    if [ -z "${year}" ]; then
	echo "ERROR - Unable to extract year from run name and no year specified"
	usage
	exit 1
    fi
fi

# Run this script in "prestage a single file" mode using parallel to speed it up
run_uri="$cnaf_srm/daq/$year/rawdata/$run"
gfal-ls $run_uri | sort | parallel -j $jobs prestage "$run_uri/"{}
