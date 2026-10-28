#!/usr/bin/env python3

import argparse
import csv
import json
import math
import os
import re
import shlex
import subprocess
import sys
import time
from collections import deque
from datetime import datetime
from pathlib import Path


SCRIPT_DIR = Path(__file__).resolve().parent
PROJECT_ROOT = Path('/home/mancinima/BeamMonitorRun4/padme-fw/TargetReco')
OFFLINE_DB = Path('/home/mancinima/BeamMonitorRun4/padme-fw/UserAnalysis/config/run_level_offline_db_2025_full_withTMM.txt')
TIMES_DB = Path('/home/mancinima/BeamMonitorRun4/padme-fw/RunIV_Times.txt')
SOURCE = SCRIPT_DIR / 'ChannelChi2.cpp'
EXECUTABLE = SCRIPT_DIR / 'ChannelChi2'
DEFAULT_RECO_DIR = Path("/data9Vd1/padme/mancinima/RecoRun4_v0/TargetLG")
DEFAULT_OUTPUT_DIR = Path('/data9Vd1/padme/mancinima/RecoRun4_v0/Analysis_TargetLG')
DEFAULT_CALIB = PROJECT_ROOT / 'CalibTarget/outputCalibration/TargetCalibrationConst_NewCharge2.txt'
DEFAULT_INPUT_TEMPLATE = 'RecoTarget_v0_run{run:07d}.root'
DEFAULT_OUTPUT_PREFIX = 'AnalysisTarget_v0_run'


def run_id(value):
    value = str(value).strip()
    if not value.isdigit() or int(value) <= 0:
        raise ValueError(f'Invalid run number: {value!r}')
    return int(value)

def clean_lines(path):
    if not path.is_file():
        raise FileNotFoundError(f'Database missing: {path}')
    with path.open(encoding='utf-8-sig') as stream:
        for number, raw in enumerate(stream, 1):
            stripped = raw.strip()
            if stripped and not stripped.startswith('#'):
                yield number, stripped

def normalized_header(value):
    return re.sub(r'[^a-z0-9]', '', value.lower())

def guess_energy_column(header):
    # Do not assume an undocumented index: only accept recognizable names.
    matches = []
    #preferred = {'ebeam', 'ebeammev', 'beamenergy', 'beamenergymev', 'energymev', 'positronenergy', 'positronenergymev', 'eplusenergy', 'eplusenergymev', 'encurr'}
    preferred = 'En_Curr'
    for index, label in enumerate(header):
        normalized = normalized_header(label)
        if normalized in preferred:
            matches.append(index)
    if len(matches) != 1:
        raise ValueError('Cannot uniquely identify the beam-energy column in the offline database header. Header={header!r}; specify --energy-column INDEX (0-based), or --energy-file.')
    return matches[0]

def read_offline_db(path, explicit_column):
    rows = list(clean_lines(path))
    if not rows:
        raise ValueError(f'Empty offline database: {path}')
    
    header = rows[0][1].split()
    has_header = not header[0].isdigit()

    if explicit_column is None:
        if not has_header:
            raise ValueError('Offline database lacks a recognizable header. Specify --energy-column INDEX (0-based) or --energy-file.')
        energy_column = guess_energy_column(header)
    else:
        energy_column = explicit_column
        if energy_column < 1:
            raise ValueError('--energy-column must be >= 1 (column 0 contains RunID)')
    
    if has_header and energy_column == 4 and (len(header) <= 4 or normalized_header(header[4]) != 'encurr'):
        raise ValueError(f'Expected En_Curr in fifth column, found {header[4] if len(header) > 4 else '<missing>'!r}; check the database layout or set --energy-column')
    records = {}

    for line_number, row in (rows[1:] if has_header else rows):
        fields = row.split()
        try:
            rid = run_id(fields[0])
        except ValueError as exc:
            raise ValueError(f'{path}:{line_number}: {exc}') from exc
        if rid in records:
            raise ValueError(f'{path}:{line_number}: duplicate RunID {rid}')

        # Run selection uses ONLY column 0. Beam energy is a separate, explicit input.
        records[rid] = (fields, line_number)
    return records, energy_column

def read_energy_file(path):
    """Optional 2-column override: RunID Ebeam_MeV; no other columns used."""
    result = {}

    for line_number, row in clean_lines(path):
        fields = row.split()
        if normalized_header(fields[0]) in {'run', 'runid', 'runnumber'}:
            continue
        if len(fields) < 2:
            raise ValueError(f'{path}:{line_number}: expected RUN ENERGY_MEV')
        rid = run_id(fields[0])
        energy = float(fields[1])
        if not math.isfinite(energy) or energy <= 0 or rid in result:
            raise ValueError(f'{path}:{line_number}: invalid/duplicate energy for run {rid}')
        result[rid] = energy
    return result

def read_timing_run_ids(path):
    selected = set()

    for number, row in clean_lines(path):
        fields = row.split()

        if normalized_header(fields[0]) in {'runid', 'run', 'runnumber'}:
            continue
        rid = run_id(fields[0])

        if rid in selected:
            raise ValueError(f'{path}:{number}: duplicate run ID {rid}')
        selected.add(rid)
    return selected

def compile_analysis(source, executable):
    
    if not source.is_file():
        raise FileNotFoundError(f'ChannelChi2 source missing: {source}')
    flags = shlex.split(subprocess.check_output(['root-config', '--cflags', '--libs'], text=True))
    cmd = ['g++', '-std=c++17', '-O2', '-Wall', '-Wextra', str(source), '-o', str(executable)] + flags + ['-lMinuit']
    print('[COMPILE]', shlex.join(cmd), flush=True)
    subprocess.run(cmd, check=True, cwd=source.parent)

def parser():
    p = argparse.ArgumentParser(description='Analyse all reconstructed and approved Target/LG Run IV runs.')
    p.add_argument('runs', nargs='*', help='Optional RunIDs; must belong to offline DB (default: all)')
    p.add_argument('--offline-db', type=Path, default=OFFLINE_DB)
    p.add_argument('--times-file', type=Path, default=TIMES_DB)
    p.add_argument('--energy-column', type=int, default=4, help='0-based beam-energy column in offline DB (default: 4 = fifth column, En_Curr)')
    p.add_argument('--energy-file', type=Path, help='Optional 2-column RunID Ebeam_MeV table, overriding energy column')
    p.add_argument('--reco-dir', type=Path, default=DEFAULT_RECO_DIR)
    p.add_argument('--input-template', default=DEFAULT_INPUT_TEMPLATE, help='Filename with {run} or {run:07d}')
    p.add_argument('--calibration-file', type=Path, default=DEFAULT_CALIB)
    p.add_argument('--output-dir', type=Path, default=DEFAULT_OUTPUT_DIR)
    p.add_argument('-j', '--jobs', type=int, default=30)
    p.add_argument('--background', action='store_true', help='Detach scheduler and return terminal')
    p.add_argument('--no-taskset', action='store_true')
    p.add_argument('--no-compile', action='store_true')
    p.add_argument('--dry-run', action='store_true', help='Check all inputs; do not compile or launch')
    p.add_argument('--strict', action='store_true', help='Abort on any missing run/time/input/energy')
    p.add_argument('--skip-existing', action='store_true', help='Skip nonempty existing output ROOT+TXT pairs')
    p.add_argument('--worker-manifest', type=Path, help=argparse.SUPPRESS)

    return p

def collect(args):
    records, energy_col = read_offline_db(args.offline_db, args.energy_column if not args.energy_file else 1)
    timing_ids = read_timing_run_ids(args.times_file)
    energies_override = read_energy_file(args.energy_file) if args.energy_file else None
    choices = sorted(records) if not args.runs else list(dict.fromkeys(run_id(r) for r in args.runs))
    jobs, problems, skipped = [], [], []
    
    for rid in choices:
        if rid not in records:
            problems.append((rid, 'RunID not present in offline selection database'))
            continue

        if rid not in timing_ids:
            problems.append((rid, 'RunID absent from RunIV_Times.txt'))
            continue

        fields, line_number = records[rid]
        try:
            if energies_override is not None:
                energy = energies_override[rid]
            else:
                energy = float(fields[energy_col])
            
            if not math.isfinite(energy) or energy <= 0:
                raise ValueError('Energy is not a positive finite number')
        
        except (IndexError, KeyError, ValueError) as exc:
            problems.append((rid, f'Missing/invalid Ebeam (offline DB line {line_number}): {exc}'))
            continue

        name = args.input_template.format(run=rid)
        inp = args.reco_dir / name
        root_name = f'{DEFAULT_OUTPUT_PREFIX}{rid:07d}.root'
        root = args.output_dir / root_name
        txt = root.with_suffix('.txt')

        if not inp.is_file() or inp.stat().st_size == 0:
            problems.append((rid, f'Missing/empty reconstructed ROOT: {inp}'))
            continue

        if args.skip_existing and root.is_file() and txt.is_file() and root.stat().st_size > 0 and txt.stat().st_size > 0:
            skipped.append(rid)
            continue

        jobs.append({'run': rid, 'energy': energy, 'input': str(inp), 'root': str(root), 'txt': str(txt)})
    return jobs, problems, skipped

def queue_jobs(jobs, options, log_dir):
    pending = deque(jobs)
    running = {}
    failures = []
    successes = 0
    available = sorted(os.sched_getaffinity(0)) if hasattr(os, 'sched_getaffinity') else list(range(os.cpu_count() or 1))
    pin = not options.get('no_taskset', False) and bool(shutil_which('taskset'))
    slots = min(options['jobs'], len(available)) if pin else options['jobs']
    
    if pin:
        available = deque(available)

    print(f'[QUEUE] {len(jobs)} runs, {slots} parallel jobs, taskset={pin}', flush=True)
    started = time.monotonic()
    
    while pending or running:    
        while pending and len(running) < slots:
            job = pending.popleft()
            core = available.popleft() if pin else None
            cmd = [str(EXECUTABLE), str(job['run']), str(job['energy']), job['input'], options['calibration'], job['root'], job['txt']]
            
            if pin:
                cmd = ['taskset', '-c', str(core)] + cmd

            logfile = log_dir / f"run_{job['run']:07d}.log"
            handle = logfile.open('w')
            env = os.environ.copy()

            for key in ('OMP_NUM_THREADS', 'MKL_NUM_THREADS', 'OPENBLAS_NUM_THREADS', 'VECLIB_MAXIMUM_THREADS'):
                env[key] = '1'

            try:
                proc = subprocess.Popen(cmd, cwd=SCRIPT_DIR, env=env, stdout=handle, stderr=subprocess.STDOUT)
                running[proc.pid] = (proc, handle, job, core, logfile)
                print(f"[START] run {job['run']:07d}, E={job['energy']} MeV, PID={proc.pid}, CPU={core}", flush=True)
            except OSError as exc:
                handle.close()
                if pin: available.append(core)
                failures.append((job['run'], str(exc)))
                print(f"[FAIL] run {job['run']:07d}: {exc}", flush=True)

        for pid, item in list(running.items()):
            proc, handle, job, core, logfile = item
            status = proc.poll()
            if status is None:
                continue

            handle.close()
            
            running.pop(pid)
            if pin: available.append(core)

            if status == 0 and Path(job['root']).is_file() and Path(job['txt']).is_file():
                successes += 1
                print(f"[ OK ] run {job['run']:07d}: {job['root']}", flush=True)

            else:
                reason = f'exit={status}, see {logfile}'
                failures.append((job['run'], reason))
                print(f"[FAIL] run {job['run']:07d}: {reason}", flush=True)

        if pending or running:
            time.sleep(0.2)

    report = log_dir / 'analysis_report.csv'
    with report.open('w', newline='') as fh:
        writer = csv.writer(fh)
        writer.writerow(['run', 'result'])

        for job in jobs:
            failure = next((reason for rid, reason in failures if rid == job['run']), None)
            writer.writerow([f"{job['run']:07d}", failure or 'OK'])

    print(f'[DONE] Success={successes}, failed={len(failures)}, elapsed={time.monotonic()-started:.1f}s; report={report}', flush=True)
    return 1 if failures else 0

def shutil_which(name):
    from shutil import which
    return which(name)

def main():

    args = parser().parse_args()
    if args.worker_manifest:
        manifest = json.loads(args.worker_manifest.read_text())
        return queue_jobs(manifest['jobs'], manifest['options'], Path(manifest['log_dir']))

    if args.jobs < 1:
        raise ValueError('-j/--jobs must be >= 1')

    if '{run' not in args.input_template:
        raise ValueError('--input-template must contain {run} or {run:07d}')

    jobs, problems, skipped = collect(args)
    print(f'[SELECT] queued={len(jobs)}, missing/invalid={len(problems)}, already done={len(skipped)}', flush=True)

    for rid, problem in problems:
        print(f'[SKIP] {rid:07d}: {problem}', flush=True)

    if args.strict and problems:
        raise ValueError(f'{len(problems)} required inputs are missing/invalid; nothing launched')

    for job in jobs[:8]:
        print(f"[READY] {job['run']:07d} E={job['energy']} MeV <- {job['input']}", flush=True)

    if len(jobs) > 8:
        print(f'[READY] ... {len(jobs)-8} more', flush=True)

    if args.dry_run:
        return 1 if problems else 0

    if not jobs:
        print('No jobs to launch.', flush=True)
        return 1 if problems else 0

    if not args.calibration_file.is_file():
        raise FileNotFoundError(f'Missing calibration file: {args.calibration_file}')

    args.output_dir.mkdir(parents=True, exist_ok=True)
    log_dir = args.output_dir / 'logs'
    log_dir.mkdir(parents=True, exist_ok=True)

    if not args.no_compile:
        compile_analysis(SOURCE, EXECUTABLE)

    if not EXECUTABLE.is_file():
        raise FileNotFoundError(f'Missing executable: {EXECUTABLE}')

    opts = {'jobs': args.jobs, 'calibration': str(args.calibration_file), 'no_taskset': args.no_taskset}
    if args.background:
        stamp = datetime.now().strftime('%Y%m%d_%H%M%S')
        manifest_path = log_dir / f'analysis_manifest_{stamp}.json'
        launcher_log = log_dir / f'RunChi2_launcher_{stamp}.log'
        manifest_path.write_text(json.dumps({'jobs': jobs, 'options': opts, 'log_dir': str(log_dir)}, indent=2))

        with launcher_log.open('w') as fh:
            proc = subprocess.Popen([sys.executable, '-u', str(Path(__file__).resolve()), '--worker-manifest', str(manifest_path)],
                                    cwd=SCRIPT_DIR, stdout=fh, stderr=subprocess.STDOUT, stdin=subprocess.DEVNULL,
                                    start_new_session=True, close_fds=True)

        print(f'[BACKGROUND] worker PID={proc.pid}, log={launcher_log}', flush=True)
        return 0

    result = queue_jobs(jobs, opts, log_dir)
    return 1 if problems or result else 0

if __name__ == '__main__':
    try:
        raise SystemExit(main())
    except (OSError, ValueError, subprocess.CalledProcessError, RuntimeError) as exc:
        print(f'ERROR: {exc}', file=sys.stderr)
        raise SystemExit(1)
