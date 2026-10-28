#!/usr/bin/env bash
# Compile RecoTMM once (--build-only), or compile and run a single reconstruction.
# For multi-run production use RunRecoTMM.py, which compiles once and queues jobs.
set -euo pipefail

RECO_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
LIST_DIR="${TMM_LIST_DIR:-${RECO_DIR}/TMMFileList}"
OUTPUT_DIR="${TMM_OUTPUT_DIR:-/data9Vd1/padme/mancinima/RecoRun4_v0/TMM}"

usage() {
  cat <<EOF
Usage:
  bash $0 --build-only
  bash $0 PadmeRunID DetRunID [MaxEvents]
  bash $0 PadmeRunID DetRunID MaxEvents tStartUnix tStopUnix [nBins]

Standalone helper. For multiple runs, use RunRecoTMM.py.
External time bounds, when supplied, are inclusive start / exclusive stop.
The default number of time bins is 100.
EOF
}

build_reco() {
  cd "$RECO_DIR"
  for source in RecoTMM.h RecoTMM.C RecoTMMLinkDef.h RunRecoTMM.cpp; do
    if [[ ! -f "$source" ]]; then
      echo "ERROR: missing source $RECO_DIR/$source" >&2
      return 1
    fi
  done
  for tool in rootcling root-config g++; do
    if ! command -v "$tool" >/dev/null 2>&1; then
      echo "ERROR: $tool not found in PATH" >&2
      return 1
    fi
  done

  echo "Generating ROOT dictionary (once)..."
  rootcling -f RecoTMMDict.cxx -c RecoTMM.h RecoTMMLinkDef.h

  local -a root_cflags root_libs
  read -r -a root_cflags <<< "$(root-config --cflags)"
  read -r -a root_libs <<< "$(root-config --libs)"
  echo "Compiling RunRecoTMM..."
  g++ -O2 -std=c++17 -Wall -Wextra \
    RunRecoTMM.cpp RecoTMM.C RecoTMMDict.cxx \
    "${root_cflags[@]}" "${root_libs[@]}" \
    -o RunRecoTMM
  echo "Compiled: $RECO_DIR/RunRecoTMM"
}

if [[ "${1:-}" == "--help" || "${1:-}" == "-h" ]]; then
  usage
  exit 0
fi
if [[ "${1:-}" == "--build-only" ]]; then
  [[ $# -eq 1 ]] || { usage >&2; exit 2; }
  build_reco
  exit $?
fi

if [[ $# -ne 2 && $# -ne 3 && $# -ne 5 && $# -ne 6 ]]; then
  usage >&2
  exit 2
fi

padme_run="$1"
det_run="$2"
max_events="${3:-0}"

if [[ ! "$padme_run" =~ ^[0-9]+$ || ! "$det_run" =~ ^[0-9]+$ || ! "$max_events" =~ ^[0-9]+$ ]]; then
  echo "ERROR: PadmeRunID, DetRunID and MaxEvents must be nonnegative integers" >&2
  exit 2
fi

# Prefer the original run<RunID>.list naming; also accept zero-padded lists.
padme_numeric="$((10#$padme_run))"
file_list="${LIST_DIR}/run${padme_numeric}.list"
if [[ ! -s "$file_list" ]]; then
  printf -v padded_run '%07d' "$padme_numeric"
  if [[ -s "${LIST_DIR}/run${padded_run}.list" ]]; then
    file_list="${LIST_DIR}/run${padded_run}.list"
  fi
fi
if [[ ! -s "$file_list" ]]; then
  echo "ERROR: missing/empty TMM input list: $file_list" >&2
  exit 1
fi

n_bins="${6:-100}"
if [[ ! "$n_bins" =~ ^[0-9]+$ ]] || (( n_bins < 1 )); then
  echo "ERROR: nBins must be a positive integer" >&2
  exit 2
fi

printf -v padded_run '%07d' "$padme_numeric"
mkdir -p "$OUTPUT_DIR"
# Production uses the same run-only naming convention as Target/LG.
# Limited-event tests receive a separate filename to avoid overwriting production.
if [[ "$max_events" == 0 ]]; then
  output_file="${OUTPUT_DIR}/RecoTMM_v0_${padded_run}.root"
else
  output_file="${OUTPUT_DIR}/RecoTMM_v0_${padded_run}_Nevt${max_events}.root"
fi

build_reco

cmd=("${RECO_DIR}/RunRecoTMM" -l "$file_list" -r "$padme_numeric" -d "$det_run"
     -n "$max_events" -b "$n_bins" -o "$output_file")
if (( $# >= 5 )); then
  cmd+=(-s "$4" -t "$5")
else
  echo "WARNING: no external UNIX bounds supplied; RecoTMM will use its legacy timestamp bounds" >&2
fi

printf 'Launching:'; printf ' %q' "${cmd[@]}"; printf '\n'
exec "${cmd[@]}"
