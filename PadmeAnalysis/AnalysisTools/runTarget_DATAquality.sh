#!/bin/bash

#basic example to run Data Quality Target starting from Padme Reco

#write the run directory
dir_run='/nfs/kloe/einstein3/padme/data/rawdata/run_0000000_20190228_131527';
#choose the rawdata in the directory
rawdata='run_0000000_20190228_131527_lvl1_00_001';


#do PADME Reco
cd ../../PadmeReco
./PadmeReco -n 10 -i $dir_run/$rawdata.root -o reco_${rawdata}.root
cd ..
#do PADME Analysis with Validation Target
cd PadmeAnalysis/
./PadmeAnalysis -m 2 -i ../PadmeReco/reco_${rawdata}.root -o analysis_DATAqualityTarget_${rawdata}.root

#do PADME AnalysisTools
cd AnalysisTools
ln -sf ../analysis_DATAqualityTarget_${rawdata}.root DATAqualityTarget.root
#root -l 'DATAqualityTarget.C("DATAqualityTarget.root")'
root -l   DATAqualityTarget.C
mv DATAqualityTarget.pdf DATAqualityTarget_${rawdata}.pdf
evince DATAqualityTarget_${rawdata}.pdf

