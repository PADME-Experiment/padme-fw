#!/bin/bash

njob=$(expr $1 + 0)

source /afs/cern.ch/work/r/rgargiul/gpuenv/bin/activate

for i in $(seq 1 4); do
 if [ -d /eos/user/r/rgargiul/www/babatm/"$njob"_$i ]; then
    echo folder exist
    echo /eos/user/r/rgargiul/www/babatm/"$njob"_$i
    /afs/cern.ch/work/r/rgargiul/gpuenv/bin/python3.9 /afs/cern.ch/work/r/rgargiul/ritmo/gen/process_babayaga.py "$njob" $i
  fi

  if [ ! -d /eos/user/r/rgargiul/www/babatm/"$njob"_$i ]; then
    echo folder not exist
    if [ ! -f /eos/user/r/rgargiul/www/babatm/bhabha_reco_"$njob"_$i.root ]; then
      echo file not exist
      mkdir -p /eos/user/r/rgargiul/www/babatm/"$njob"_$i
      cd /eos/user/r/rgargiul/www/babatm/"$njob"_$i
      /afs/cern.ch/work/r/rgargiul/ritmo/gen/run_babayaga.sh $i
      /afs/cern.ch/work/r/rgargiul/gpuenv/bin/python3.9 /afs/cern.ch/work/r/rgargiul/ritmo/gen/process_babayaga.py "$njob" $i
    fi
  fi
done