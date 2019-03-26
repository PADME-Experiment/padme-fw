#!/bin/bash

#do PADME Reco
cd ../../PadmeReco
./PadmeReco -l /afs/le.infn.it/user/s/spagnolo/padme/PADMEsw/onDATA2018/prod/input_prova.list -n 1000 -o RecoFromRawOutputFile_ref.root
cd ..
#do PADME Analysis with Validation
cd PadmeAnalysis/
./PadmeAnalysis -w 1 -i /afs/le.infn.it/user/s/spagnolo/padme/PADMEsw/onDATA2018/dev/copyfromGC/RecoFromRawOutputFile_test.root -o RecoFromRawOutputFile_AnalysisTest.root
./PadmeAnalysis -w 1 -i ../PadmeReco/RecoFromRawOutputFile_ref.root -o RecoFromRawOutputFile_AnalysisRef.root

#do PADME AnalysisTools
cd AnalysisTools
ln -sf ../RecoFromRawOutputFile_AnalysisTest.root TestValidationSample.root
ln -sf ../RecoFromRawOutputFile_AnalysisRef.root RefValidationSample.root
root -l BasicExample_Padme.C
mv ValidationHisto.pdf ../..
cd ..
