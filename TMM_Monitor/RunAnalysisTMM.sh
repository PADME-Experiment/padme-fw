#!/bin/bash

# ============================================================
# Parallel launcher for AnalysisTMM
# ============================================================

MACRO="AnalysisTMM.C"
Path_to_RecoData="/home/mancinima/BeamMonitorRun4/padme-fw/RecoTMM/outputTMM"
RUNS=(
    "${Path_to_RecoData}/TMMRecoTime_v1_run357_Nevt100000_Nblk1000.root"
    "${Path_to_RecoData}/TMMRecoTime_v1_run415_Nevt100000_Nblk1000.root"
    "${Path_to_RecoData}/TMMRecoTime_v1_run426_Nevt100000_Nblk1000.root"
    "${Path_to_RecoData}/TMMRecoTime_v1_run534_Nevt100000_Nblk1000.root"
    "${Path_to_RecoData}/TMMRecoTime_v1_run677_Nevt100000_Nblk1000.root"
)

mkdir -p logs

for INPUT in "${RUNS[@]}"; do

    BASENAME=$(basename "$INPUT" .root)
    LOGFILE="logs/${BASENAME}_AnalysisTMM.log"
    PIDFILE="logs/${BASENAME}_AnalysisTMM.pid"

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