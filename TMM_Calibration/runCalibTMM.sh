#!/bin/bash

set -uo pipefail

if [[ $# -lt 2 || $# -gt 4 ]]; then
  echo "Usage: source $0 PadmeRunID DetRunID [MaxEvents] [NevtBlock]"
  return 1 2>/dev/null || exit 1
fi

PadmeRunID="$1"
DetRunID="$2"
MaxEvents="${3:-0}"
NevtBlock="${4:-1000}"

CalibrationPath="/home/mancinima/BeamMonitorRun4/padme-fw/TMM_Calibration"
FileList="${CalibrationPath}/TMMFileList/run${PadmeRunID}.list"
OutputDir="${CalibrationPath}/outputTMM"
OutputFile="${OutputDir}/TMMCalibrationVoigt_run${PadmeRunID}"

echo
echo "Processing padmeRun ${PadmeRunID} - DetRun ${DetRunID}"
echo "Input list: ${FileList}"
echo "NEvt reco: ${MaxEvents} - block ${NevtBlock}"
echo "================================================================"
echo "Output ROOT file: ${OutputFile}"
echo

if [[ ! -s "${FileList}" ]]; then
  echo "ERROR: input file list does not exist or is empty: ${FileList}"
  return 1 2>/dev/null || exit 1
fi

echo "Generating ROOT dictionary..."

rm -f CalibTMMDict.cxx CalibTMMDict_rdict.pcm CalibTMMDict.rootmap

if ! rootcling -f CalibTMMDict.cxx -c CalibTMM.h CalibTMMLinkDef.h; then
  echo
  echo "ERROR: ROOT dictionary generation failed."
  return 1 2>/dev/null || exit 1
fi

echo
echo "Compiling..."

if ! g++ -g -O0 -std=c++17 -Wall -Wextra \
  RunCalibTMM.cpp CalibTMM.C CalibTMMDict.cxx \
  $(root-config --cflags --libs) \
  -o RunCalibTMM; then
  echo
  echo "ERROR: compilation failed. Reconstruction not started."
  return 1 2>/dev/null || exit 1
fi

echo
echo "Running reconstruction..."
echo

./RunCalibTMM \
  -l "${FileList}" \
  -r "${PadmeRunID}" \
  -d "${DetRunID}" \
  -n "${MaxEvents}" \
  -b "${NevtBlock}" \
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