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

RecoPath="/home/mancinima/BeamMonitorRun4/padme-fw/RecoMM"
CalibrationPath="/home/mancinima/BeamMonitorRun4/padme-fw/MM_Calibration"

# Reuse the raw-data list already maintained for MM calibration/reconstruction.
FileList="${CalibrationPath}/MMFileList/run${PadmeRunID}.list"

OutputDir="${RecoPath}/outputMM"
mkdir -p "${OutputDir}"

OutputFile="${RecoPath}/ProvaRecoMM.root"
# if [[ "${MaxEvents}" == "0" ]]; then
#   OutputFile="${OutputDir}/MMReco_run${PadmeRunID}_NevtAll_Nblk${NevtBlock}.root"
# else
#   OutputFile="${OutputDir}/MMReco_run${PadmeRunID}_Nevt${MaxEvents}_Nblk${NevtBlock}.root"
# fi

echo
echo "Processing PADME run ${PadmeRunID} - MM detector run ${DetRunID}"
echo "Input list: ${FileList}"
echo "N events: ${MaxEvents} - block: ${NevtBlock}"
echo "Output ROOT file: ${OutputFile}"
echo "================================================================"
echo

if [[ ! -s "${FileList}" ]]; then
  echo "ERROR: input file list does not exist or is empty: ${FileList}"
  return 1 2>/dev/null || exit 1
fi

echo "Generating ROOT dictionary..."

rm -f RecoMMDict.cxx RecoMMDict_rdict.pcm RecoMMDict.rootmap

if ! rootcling -f RecoMMDict.cxx -c RecoMM.h RecoMMLinkDef.h; then
  echo
  echo "ERROR: ROOT dictionary generation failed."
  return 1 2>/dev/null || exit 1
fi

echo
echo "Compiling..."

if ! g++ -g -O0 -std=c++17 -Wall -Wextra \
  RunRecoMM.cpp RecoMM.C RecoMMDict.cxx \
  $(root-config --cflags --libs) \
  -o RunRecoMM; then
  echo
  echo "ERROR: compilation failed. Reconstruction not started."
  return 1 2>/dev/null || exit 1
fi

echo
echo "Running MM reconstruction..."
echo

./RunRecoMM \
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
  echo "ERROR: reconstruction failed with exit code ${RecoStatus}"
  return ${RecoStatus} 2>/dev/null || exit ${RecoStatus}
fi

echo "Done!"
echo
