#!/bin/bash

usage() {
    echo "Usage: $0 [-D dst_site] [-y year] [-h]" 1>&2
    echo "Default: compare content of year 2019 on DAQ servers with CNAF" 1>&2
    exit 1
}

# Find where this script is really located: needed to find the corresponding VerifyRun.py script
SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  DIR="$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
DIR="$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )"
VERIFYRUN=$DIR/VerifyRun.py
if ! [[ -e $VERIFYRUN ]]; then
    echo "ERROR - $VERIFYRUN does not exist"
    usage
fi
if ! [[ -f $VERIFYRUN ]]; then
    echo "ERROR - $VERIFYRUN is not a regular file"
    usage
fi
if ! [[ -x $VERIFYRUN ]]; then
    echo "ERROR - $VERIFYRUN is not executable"
    usage
fi

# Define Storm access point to CNAF tape library and LNF storage system
srm_cnaf="srm://storm-fe-archive.cr.cnaf.infn.it:8444/srm/managerv2?SFN=/padmeTape"
srm_lnf="srm://atlasse.lnf.infn.it:8446/srm/managerv2?SFN=/dpm/lnf.infn.it/home/vo.padme.org"

srv_list=( "l1padme3" "l1padme4" )
daq_user="daq"
daq_keyfile="/home/${USER}/.ssh/id_rsa_cdr"
daq_path="/data/DAQ"
dst_site="CNAF"
year="2019"

while getopts ":D:y:h" o; do
    case "${o}" in
        D)
            dst_site=${OPTARG}
            ;;
        y)
            year=${OPTARG}
            ;;
        *)
            usage
            ;;
    esac
done

if [[ $dst_site != "CNAF" ]] && [[ $dst_site != "LNF" ]]; then
    echo "ERROR - Destination site can only be CNAF or LNF"
fi

if [[ $year != "2018" ]] && [[ $year != "2019" ]]; then
    echo "ERROR - DAQ year can only be 2018 or 2019"
fi

# Get list of runs on DAQ servers for given year and verify each of them
for srv in "${srv_list[@]}"; do
    for run in $( ssh -n -i $daq_keyfile -l $daq_user $srv ls ${daq_path}/${year}/rawdata ); do
	$VERIFYRUN -R $run -S DAQ -s $srv -D $dst_site
    done
done
