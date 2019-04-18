#!/bin/bash

usage() {
    echo "Usage: $0 -r run [-S src_site] [-D dst_site] [-L dst_dir] [-y year] [-j jobs] [-h]" 1>&2
    echo "Available source sites: CNAF LNF LNF2"
    echo "Available destination sites: CNAF LNF LNF2 LOCAL"
    echo "Default: copy from CNAF to LOCAL" 1>&2
    exit 1
}

now() {
    date -u +"%Y-%m-%d %H:%M:%S"
}
export -f now

# Function to handle the transfer of a single file
transfer() {
    src_file=$1
    dst_file=$2
    space_token=$3
    #echo "$( now ) ${src_file} -> ${dst_file}"
    #gfal-copy -p --checksum ADLER32 -t 3600 -T 3600 "${src_file}" "${dst_file}"
    echo "$( now ) gfal-copy -p --checksum ADLER32 -t 3600 -T 3600 ${space_token} ${src_file} ${dst_file}"
    gfal-copy -p --checksum ADLER32 -t 3600 -T 3600 ${space_token} ${src_file} ${dst_file}
    rc=$?
    if [[ $rc -eq 0 ]]; then
	echo $( now ) - Copy was successful
    else
	echo $( now ) - WARNING - gfal-copy reported an error
    fi
    return $rc
}
export -f transfer

# Define Storm access point to CNAF tape library and LNF/LNF2 storage system
srm_cnaf="srm://storm-fe-archive.cr.cnaf.infn.it:8444/srm/managerv2?SFN=/padmeTape"
srm_lnf="srm://atlasse.lnf.infn.it:8446/srm/managerv2?SFN=/dpm/lnf.infn.it/home/vo.padme.org"
srm_lnf2="srm://atlasse.lnf.infn.it:8446/srm/managerv2?SFN=/dpm/lnf.infn.it/home/vo.padme.org_scratch"

run=""
year=""
src_site="CNAF"
dst_site="LOCAL"
dst_dir=""
src_file=""
dst_file=""
jobs=10
test=0
while getopts ":r:y:S:D:L:s:d:j:th" o; do
    case "${o}" in
	r)
	    run=${OPTARG}
	    ;;
        y)
            year=${OPTARG}
            ;;
        S)
            src_site=${OPTARG}
            ;;
        D)
            dst_site=${OPTARG}
            ;;
        L)
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
        t)
            test=1
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
if [[ $dst_site = "LOCAL" && -z $dst_dir ]]; then
    dst_dir=$( pwd )
fi

# Define full URI of source run directory
if [[ $src_site = "CNAF" ]]; then
    src_run_uri="${srm_cnaf}/daq/${year}/rawdata/${run}"
elif [[ $src_site = "LNF" ]]; then
    src_run_uri="${srm_lnf}/daq/${year}/rawdata/${run}"
elif [[ $src_site = "LNF2" ]]; then
    src_run_uri="${srm_lnf2}/daq/${year}/rawdata/${run}"
else
    echo "ERROR - Source site ${src_site} is unknown. Please use CNAF, LNF, or LNF2"
    usage
fi

# Define full URI of destination run directory
space_token=""
if [[ $dst_site = "CNAF" ]]; then
    dst_run_uri="${srm_cnaf}/daq/${year}/rawdata/${run}"
elif [[ $dst_site = "LNF" ]]; then
    dst_run_uri="${srm_lnf}/daq/${year}/rawdata/${run}"
elif [[ $dst_site = "LNF2" ]]; then
    space_token="-S PADME_SCRATCH"
    dst_run_uri="${srm_lnf2}/daq/${year}/rawdata/${run}"
elif [[ $dst_site = "LOCAL" ]]; then
    dst_run_uri="file://${dst_dir}/${run}"
else
    echo "ERROR - Destination site ${dst_site} is unknown. Please use CNAF, LNF, LNF2, or LOCAL"
    usage
fi

# Transfer all files from source to destination using parallel tool
if [[ $test -eq 0 ]]; then
    echo $( now ) - Copying all files from $src_run_uri using $jobs parallel streams
    gfal-ls $src_run_uri | sort | parallel -j $jobs transfer "${src_run_uri}/"{} "${dst_run_uri}/"{} "\"${space_token}\""
    echo $( now ) - All copy jobs completed
else
    echo $( now ) - TEST MODE - Copying all files from $src_run_uri using $jobs parallel streams
    gfal-ls $src_run_uri | sort | parallel -j $jobs transfer "${src_run_uri}/"{} "file:///dev/null"
    echo $( now ) - TEST MODE - All copy jobs completed
fi
