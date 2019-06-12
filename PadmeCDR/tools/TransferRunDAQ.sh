#!/bin/bash

usage() {
    echo "Usage: $0 -r run [-S server] [-D dst_dir] [-y year] [-j jobs] [-h]" 1>&2
    echo "Default: copy from l1padme3 to current dir" 1>&2
    exit 1
}

now() {
    date -u +"%Y-%m-%d %H:%M:%S"
}
export -f now

# Function to handle the transfer of a single file
transfer() {
    server=$1
    src_file=$2
    dst_file=$3
    echo "$( now ) ($server) ${src_file} -> ${dst_file}"
    scp -i /home/leonardi/.ssh/id_rsa_cdr daq@${server}:${src_file} ${dst_file}
    rc=$?
    if [[ $rc -eq 0 ]]; then
	echo $( now ) - Copy was successful
    else
	echo $( now ) - WARNING - scp reported an error
    fi
    return $rc
}
export -f transfer

run=""
year=""
server="l1padme3"
dst_dir=""
src_file=""
dst_file=""
jobs=10
test=0
while getopts ":r:y:S:D:s:d:j:h" o; do
    case "${o}" in
	r)
	    run=${OPTARG}
	    ;;
        y)
            year=${OPTARG}
            ;;
        S)
            server=${OPTARG}
            ;;
        D)
            dst_dir=${OPTARG}
            ;;
        s)
            src_file=${OPTARG}
            ;;
        d)
            dst_file=${OPTARG}
            ;;
        j)
            jobs=${OPTARG}
            ;;
        *)
            usage
            ;;
    esac
done

# If source and destination files are specified, just do the copy and exit
if [[ -n $src_file && -n $dst_file ]]; then
    transfer $src_file $dst_file
    exit $?
fi

# Check if run was specified
if [[ -z $run ]]; then
    echo "ERROR - No run specified"
    usage
fi

# If no year was specified, try and extract it from run name
if [[ -z $year ]]; then
    regex="^run_[0-9]+_([0-9][0-9][0-9][0-9])[0-9]+_[0-9]+$"
    if [[ $run =~ $regex ]]; then
	year="${BASH_REMATCH[1]}"
    fi
    if [[ -z $year ]]; then
	echo "ERROR - Unable to extract year from run name and no year specified"
	usage
    fi
fi

# If destination is LOCAL, check if a directory was specified or set it to the current dir
if [[ -z $dst_dir ]]; then
    dst_dir=$( pwd )
fi
dst_dir=${dst_dir}/$run
mkdir -p $dst_dir

# Define path on DAQ server
src_path=/data/DAQ/${year}/rawdata/${run}

# Transfer all files from source to destination using parallel tool
echo $( now ) - Copying all files from $src_run_uri using $jobs parallel streams
ssh -i /home/leonardi/.ssh/id_rsa_cdr -l daq $server ls $src_path | sort | parallel -j $jobs transfer $server "${src_path}/"{} "${dst_dir}/"{}
echo $( now ) - All copy jobs completed
