#!/bin/bash

set -uo pipefail

if [[ $# -lt 2 || $# -gt 3 ]]; then
  echo "Usage: source $0 PadmeRunID DetRunID [MaxEvents]"
  return 1 2>/dev/null || exit 1
fi

PadmeRunID="$1"
DetRunID="$2"
MaxEvents="${3:-0}"

RecoPath="/home/mancinima/BeamMonitorRun4/padme-fw/RecoTMM"
FileList="${RecoPath}/TMMFileList/run${PadmeRunID}.list"
OutputDir="${RecoPath}/outputTMM"
# OutputFile="${OutputDir}/TMMRecostructed_run${PadmeRunID}_Nevt${MaxEvents}.root"
# OutputFile="${OutputDir}/Run680NoTarget_test.root"
# OutputFile="${RecoPath}/provaNuovaReco.root"
if [[ "${MaxEvents}" == "-0" ]]; then
  OutputFile="${OutputDir}/TMMRecoTime_TreeInd_v1_run${PadmeRunID}_NevtAll.root"
else
  OutputFile="${OutputDir}/TMMRecoTime_TreeInd_v1_run${PadmeRunID}_Nevt${MaxEvents}.root"
fi

echo
echo "Processing padmeRun ${PadmeRunID} - DetRun ${DetRunID}"
echo "Input list: ${FileList}"
echo "NEvt reco: ${MaxEvents} - Output ROOT file: ${OutputFile}"
echo "================================================================"
echo "Output ROOT file: ${OutputFile}"
echo

if [[ ! -s "${FileList}" ]]; then
  echo "ERROR: input file list does not exist or is empty: ${FileList}"
  return 1 2>/dev/null || exit 1
fi

echo "Generating ROOT dictionary..."

rm -f RecoTMMDict.cxx RecoTMMDict_rdict.pcm RecoTMMDict.rootmap

if ! rootcling -f RecoTMMDict.cxx -c RecoTMM.h RecoTMMLinkDef.h; then
  echo
  echo "ERROR: ROOT dictionary generation failed."
  return 1 2>/dev/null || exit 1
fi

echo
echo "Compiling..."

if ! g++ -g -O0 -std=c++17 -Wall -Wextra \
  RunRecoTMM.cpp RecoTMM.C RecoTMMDict.cxx \
  $(root-config --cflags --libs) \
  -o RunRecoTMM; then
  echo
  echo "ERROR: compilation failed. Reconstruction not started."
  return 1 2>/dev/null || exit 1
fi

echo
echo "Running reconstruction..."
echo

./RunRecoTMM \
  -l "${FileList}" \
  -r "${PadmeRunID}" \
  -d "${DetRunID}" \
  -n "${MaxEvents}" \
  -o "${OutputFile}"

RecoStatus=$?

echo
echo "Reconstruction completed with status ${RecoStatus}"
echo

if [[ ${RecoStatus} -ne 0 ]]; then
  echo
  echo "ERROR: reconstruction failed with exit code ${RecoStatus}"
  return ${RecoStatus} 2>/dev/null || exit ${RecoStatus}
fi

echo
echo "Done!"
echo