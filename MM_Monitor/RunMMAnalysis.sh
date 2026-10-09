#!/bin/bash

# ============================================================
# Parallel launcher for AnalysisTMM
# ============================================================

MACRO="AnalysisMM.C"
Path_to_Reco="/home/mancinima/BeamMonitorRun4/padme-fw/RecoMM"
Path_to_RecoData="/home/mancinima/BeamMonitorRun4/padme-fw/RecoMM/outputMM"
RUNS=(
    "${Path_to_Reco}/ProvaRecoMM.root"
)

mkdir -p logs

for INPUT in "${RUNS[@]}"; do

    BASENAME=$(basename "$INPUT" .root)
    LOGFILE="logs/${BASENAME}_AnalysisMM.log"
    PIDFILE="logs/${BASENAME}_AnalysisMM.pid"

    echo "Launching ${INPUT}"
    echo "  log: ${LOGFILE}"

    nohup root -l -b -q "${MACRO}(\"${INPUT}\")" \
        > "${LOGFILE}" 2>&1 &

    PID=$!
    echo "${PID}" > "${PIDFILE}"

    echo "  PID: ${PID}"
    echo
done

echo "All analyses launched."