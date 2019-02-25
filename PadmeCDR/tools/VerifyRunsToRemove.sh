#!/bin/bash

usage() {
    echo "Usage: $0 [-D dst_site] [-y year] [-h]" 1>&2
    echo "Default: compare content of year 2019 on DAQ servers with CNAF" 1>&2
    exit 1
}

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
	./VerifyRun.py -R $run -S DAQ -s $srv -D $dst_site
    done
done
