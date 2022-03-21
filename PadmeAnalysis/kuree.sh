#!/usr/bin/bash

# works on my computer, biatches

TOP=$HOME/software/padme-fw/PadmeAnalysis

cd $TOP

/usr/bin/find $TOP -type d -name tmp -exec rm -rf {} \;
/usr/bin/find $TOP -type d -name lib -exec rm -rf {} \;

rm -f $TOP/PadmeAnalysis

cp $TOP/original.gmk $TOP/config/global.gmk

for v in AnalysisBase ECal EVeto HEPVeto PVeto SAC Target UserAnalysis; do
    cd $TOP/$v && make dirs lib
done


cp $TOP/kureee.gmk $TOP/config/global.gmk

cd $TOP && make
