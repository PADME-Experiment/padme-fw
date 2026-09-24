#!/bin/bash

# ============================================================
# Parallel launcher for AnalysisTMM
# ============================================================

MACRO="AnalysisTMM.C"

RUNS=(
    "/home/mancinima/BeamMonitorRun4/padme-fw/RecoTMM/outputTMM/TMMRecostructed_run677_Nevt-0_Nblk10000.root"
    # "/home/mancinima/BeamMonitorRun4/padme-fw/RecoTMM/outputTMM/TMMRecostructed_run677_Nevt-0_Nblk1000.root"
    "/home/mancinima/BeamMonitorRun4/padme-fw/RecoTMM/outputTMM/TMMRecostructed_run357_Nevt-0_Nblk10000.root"
    # "/home/mancinima/BeamMonitorRun4/padme-fw/RecoTMM/outputTMM/TMMRecostructed_run357_Nevt-0_Nblk1000.root"
    "/home/mancinima/BeamMonitorRun4/padme-fw/RecoTMM/outputTMM/TMMRecostructed_run534_Nevt-0_Nblk10000.root"
    # "/home/mancinima/BeamMonitorRun4/padme-fw/RecoTMM/outputTMM/TMMRecostructed_run534_Nevt-0_Nblk1000.root"
    "/home/mancinima/BeamMonitorRun4/padme-fw/RecoTMM/outputTMM/TMMRecostructed_run415_Nevt-0_Nblk10000.root"
    #"/home/mancinima/BeamMonitorRun4/padme-fw/RecoTMM/outputTMM/TMMRecostructed_run415_Nevt-0_Nblk1000.root"
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