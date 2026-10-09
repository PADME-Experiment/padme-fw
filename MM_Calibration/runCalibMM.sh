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

FileList="${CalibrationPath}/MMFileList/run_${PadmeRunID}.list"
OutputDir="${CalibrationPath}/outputMM"
OutputFile="${OutputDir}/MMCalib${PadmeRunID}_det${DetRunID}"
# OutputFile="${CalibrationPath}/TestForBFieldOn"

# ============================================================
# PADME framework
# ============================================================
#
# CHANGE THESE TWO PATHS to the directories used by the working
# RecoTarget build.
#
# The first directory must contain:
#   TRawEvent.hh
#   TRawMergedEvent.hh
#
# The library directory must contain the PADME library/libraries
# providing those classes and their ROOT dictionaries.
#

PADME_INCLUDE="/home/mancinima/BeamMonitorRun4/padme-fw/PadmeRoot/include"
PADME_LIB="/home/mancinima/BeamMonitorRun4/padme-fw/PadmeRoot/lib"

# Replace this with the SAME library used by RecoTarget.
# Example only:
PADME_LIBS="-lPadmeRoot"

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

# Check that the custom framework headers are actually visible
if [[ ! -f "${PADME_INCLUDE}/TRawEvent.hh" ]]; then
  echo "ERROR: cannot find ${PADME_INCLUDE}/TRawEvent.hh"
  return 1 2>/dev/null || exit 1
fi

if [[ ! -f "${PADME_INCLUDE}/TRawMergedEvent.hh" ]]; then
  echo "ERROR: cannot find ${PADME_INCLUDE}/TRawMergedEvent.hh"
  return 1 2>/dev/null || exit 1
fi

echo "Generating ROOT dictionary..."

rm -f \
  CalibMMDict.cxx \
  CalibMMDict_rdict.pcm \
  CalibMMDict.rootmap

if ! rootcling \
  -f CalibMMDict.cxx \
  -I"${PADME_INCLUDE}" \
  -c \
  CalibMM.h \
  CalibMMLinkDef.h
then
  echo
  echo "ERROR: ROOT dictionary generation failed."
  return 1 2>/dev/null || exit 1
fi

echo
echo "Compiling..."

if ! g++ \
  -g \
  -O0 \
  -std=c++17 \
  -Wall \
  -Wextra \
  -I"${PADME_INCLUDE}" \
  RunCalibMM.cpp \
  CalibMM.C \
  CalibMMDict.cxx \
  $(root-config --cflags --libs) \
  -L"${PADME_LIB}" \
  ${PADME_LIBS} \
  -Wl,-rpath,"${PADME_LIB}" \
  -o RunCalibMM
then
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