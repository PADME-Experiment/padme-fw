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

CalibrationPath="/home/mancinima/BeamMonitorRun4/padme-fw/MM_Calibration"

# One MM ROOT input file per line. Keeping the same file-list mechanism
# used by the TMM calibration avoids changing the calibration I/O framework.
FileList="${CalibrationPath}/MMFileList/run${PadmeRunID}.list"
OutputDir="${CalibrationPath}/outputMM"
OutputFile="${OutputDir}/MMCalibrationVoigt_run${PadmeRunID}_det${DetRunID}"

mkdir -p "${OutputDir}"

echo
echo "Processing PADME run ${PadmeRunID} - MM detector run ${DetRunID}"
echo "Input list: ${FileList}"
echo "NEvt calibration: ${MaxEvents} - block ${NevtBlock}"
echo "================================================================"
echo "Output ROOT file: ${OutputFile}.root"
echo

if [[ ! -s "${FileList}" ]]; then

  echo "ERROR: input file list does not exist or is empty: ${FileList}"

  return 1 2>/dev/null || exit 1

fi

echo "Generating ROOT dictionary..."

rm -f CalibMMDict.cxx CalibMMDict_rdict.pcm CalibMMDict.rootmap

if ! rootcling -f CalibMMDict.cxx -c CalibMM.h CalibMMLinkDef.h; then

  echo
  echo "ERROR: ROOT dictionary generation failed."

  return 1 2>/dev/null || exit 1

fi

echo
echo "Compiling..."

if ! g++ -g -O0 -std=c++17 -Wall -Wextra \
  RunCalibMM.cpp CalibMM.C CalibMMDict.cxx \
  $(root-config --cflags --libs) \
  -o RunCalibMM; then

  echo
  echo "ERROR: compilation failed. Calibration not started."

  return 1 2>/dev/null || exit 1

fi

echo
echo "Running calibration..."
echo

./RunCalibMM \
  -l "${FileList}" \
  -r "${PadmeRunID}" \
  -d "${DetRunID}" \
  -n "${MaxEvents}" \
  -b "${NevtBlock}" \
  -o "${OutputFile}"

CalibStatus=$?

echo
echo "Calibration completed with status ${CalibStatus}"
echo

if [[ ${CalibStatus} -ne 0 ]]; then

  echo "ERROR: calibration failed with exit code ${CalibStatus}"

  return ${CalibStatus} 2>/dev/null || exit ${CalibStatus}

fi

echo "Done!"
echo
