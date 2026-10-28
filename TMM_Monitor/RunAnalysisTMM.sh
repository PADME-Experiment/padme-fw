#!/usr/bin/env bash
# Single-run ROOT analysis helper. The Python queue calls this once per run.
# The output is written transactionally: incomplete ROOT files are never published.
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
INPUT_DIR="${TMM_RECO_DIR:-/data9Vd1/padme/mancinima/RecoRun4_v0/TMM}"
OUTPUT_DIR="${TMM_ANALYSIS_DIR:-/data9Vd1/padme/mancinima/RecoRun4_v0/AnalysisTMM}"
MACRO="${TMM_ANALYSIS_MACRO:-${SCRIPT_DIR}/AnalysisTMM.C}"
INPUT=""
OUTPUT=""
RUN=""
EXPECTED_BINS=0

usage() {
  cat <<EOF
Usage:
  bash $0 -r 90032 [-b 100] [-o /path/test.root]
  bash $0 -i /path/RecoTMM_v0_0090032.root [-o /path/AnalysisTMM_test.root]

Options:
  -r RUN          PADME run ID; resolves RecoTMM_v0_%07d.root
  -i INPUT        Full input ROOT file (overrides -r)
  -o OUTPUT       Custom output ROOT path
  -b NBINS        Optional check that input has exactly this many time bins
  --input-dir DIR Default reconstruction folder
  --output-dir DIR Default analysis folder
  --macro FILE    AnalysisTMM.C path
  -h, --help      This help

Analysis reads nBins, tStartUnix, tStopUnix from reconstruction.
-b checks existing input binning; it does NOT rebin any histogram.
EOF
}

while (( $# )); do
  case "$1" in
    -r) [[ $# -ge 2 ]] || { usage >&2; exit 2; }; RUN="$2"; shift 2;;
    -i) [[ $# -ge 2 ]] || { usage >&2; exit 2; }; INPUT="$2"; shift 2;;
    -o) [[ $# -ge 2 ]] || { usage >&2; exit 2; }; OUTPUT="$2"; shift 2;;
    -b) [[ $# -ge 2 ]] || { usage >&2; exit 2; }; EXPECTED_BINS="$2"; shift 2;;
    --input-dir) [[ $# -ge 2 ]] || { usage >&2; exit 2; }; INPUT_DIR="$2"; shift 2;;
    --output-dir) [[ $# -ge 2 ]] || { usage >&2; exit 2; }; OUTPUT_DIR="$2"; shift 2;;
    --macro) [[ $# -ge 2 ]] || { usage >&2; exit 2; }; MACRO="$2"; shift 2;;
    -h|--help) usage; exit 0;;
    *) echo "ERROR: unknown argument: $1" >&2; usage >&2; exit 2;;
  esac
done

if [[ -n "$RUN" ]]; then
  [[ "$RUN" =~ ^[0-9]+$ ]] && (( 10#$RUN > 0 )) || { echo "ERROR: invalid PADME RunID: $RUN" >&2; exit 2; }
  printf -v PADDED_RUN '%07d' "$((10#$RUN))"
fi
if [[ -z "$INPUT" && -z "$RUN" ]]; then
  echo "ERROR: specify -r RUN or -i INPUT" >&2
  exit 2
fi
if [[ ! "$EXPECTED_BINS" =~ ^[0-9]+$ ]]; then
  echo "ERROR: -b must be a nonnegative integer" >&2
  exit 2
fi
if [[ -z "$INPUT" ]]; then
  INPUT="${INPUT_DIR}/RecoTMM_v0_${PADDED_RUN}.root"
fi
if [[ -z "$OUTPUT" ]]; then
  if [[ -z "$RUN" ]]; then
    base="$(basename -- "$INPUT" .root)"
    base="${base/#RecoTMM_/AnalysisTMM_}"
    [[ "$base" == AnalysisTMM_* ]] || base="AnalysisTMM_${base}"
    OUTPUT="${OUTPUT_DIR}/${base}.root"
  else
    OUTPUT="${OUTPUT_DIR}/AnalysisTMM_v0_${PADDED_RUN}.root"
  fi
fi
if [[ ! -s "$INPUT" ]]; then
  echo "ERROR: input ROOT file does not exist or is empty: $INPUT" >&2
  exit 1
fi
if [[ ! -f "$MACRO" ]]; then
  echo "ERROR: analysis macro missing: $MACRO" >&2
  exit 1
fi
if ! command -v root >/dev/null 2>&1; then
  echo "ERROR: ROOT executable 'root' not found in PATH" >&2
  exit 1
fi
mkdir -p -- "$(dirname -- "$OUTPUT")"
# Root writes the temporary file. Publish it only after successful analysis.
if [[ "$OUTPUT" == *.root ]]; then
  TEMP_OUTPUT="${OUTPUT%.root}.part-${BASHPID}.root"
else
  TEMP_OUTPUT="${OUTPUT}.part-${BASHPID}.root"
fi
TEMP_LOG="$(mktemp "${TMPDIR:-/tmp}/analysistmm.XXXXXXXX.log")"
trap 'rm -f -- "$TEMP_OUTPUT" "$TEMP_LOG"' EXIT
# Escape ROOT string literals (normal Linux paths are sufficient).
escape_root() { local v="$1"; v="${v//\\/\\\\}"; v="${v//\"/\\\"}"; printf '%s' "$v"; }
ROOT_CALL="$(escape_root "$MACRO")(\"$(escape_root "$INPUT")\",\"$(escape_root "$TEMP_OUTPUT")\",${EXPECTED_BINS})"
echo "[ANALYSIS] input  : $INPUT"
echo "[ANALYSIS] output : $OUTPUT"
echo "[ANALYSIS] bins check: $EXPECTED_BINS (0 = disabled)"
if ! root -l -b -q "$ROOT_CALL" 2>&1 | tee "$TEMP_LOG"; then
  echo "ERROR: ROOT analysis failed" >&2
  exit 1
fi
if [[ ! -s "$TEMP_OUTPUT" ]] || ! grep -Fq "ANALYSISTMM_SUCCESS: $TEMP_OUTPUT" "$TEMP_LOG"; then
  echo "ERROR: AnalysisTMM did not finish successfully; incomplete output will be discarded" >&2
  exit 1
fi
mv -f -- "$TEMP_OUTPUT" "$OUTPUT"
echo "ANALYSISTMM_FINISHED: $OUTPUT"
