#!/usr/bin/env python3
"""Verify explicit trick activation, initial dispatch, cold ramp and full-run validity on GRID."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import re
import shlex
import shutil
import subprocess
import sys

ROOT = next(p for p in Path(__file__).resolve().parents if (p / 'tools/cpu_resources.py').is_file())
BASE = ROOT / 'experiments/construction-20260918'
sys.path.insert(0, str(ROOT / 'tools'))
from cpu_resources import cpu_resources


def read(path):
    return json.loads(path.read_text())


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def write(path, data):
    path.write_text(json.dumps(data, indent=2) + '\n')


def initial_schedule(data):
    result = []
    for schedule in data['actualSchedule']:
        entries = [list(map(int, entry.split(':'))) for entry in schedule.split(',') if entry]
        first = [task for step, task in entries if step == 1]
        assert len(first) == 1 and first[0] >= 0
        result.append(first[0])
    assert len(result) == 10000 and len(set(result)) == 10000
    return result


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--raw', type=Path, required=True)
    p.add_argument('--reference-raw', type=Path, required=True)
    p.add_argument('--output', type=Path, required=True)
    p.add_argument('--commit', required=True)
    p.add_argument('--hold-job')
    p.add_argument('--execute', action='store_true')
    a = p.parse_args()
    raw, reference, out = a.raw.resolve(), a.reference_raw.resolve(), a.output.resolve()
    if not a.execute:
        support = raw / 'trick-analysis-support'
        support.mkdir(exist_ok=False)
        files = []
        own = support / 'analyze.py'
        shutil.copy2(Path(__file__), own); files.append(own)
        for name in ['sequences-20260918/analyze.py', 'motion-20260918/analyze.py',
                     'assignment-20260918/analyze.py', 'throughput-20260918-next/analyze_matrix.py',
                     'throughput-20260918-strict/analyze.py', 'construction-20260918/verify_full.py']:
            dst = support / 'experiments' / name; dst.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(ROOT / 'experiments' / name, dst); files.append(dst)
        shutil.copy2(BASE / 'results/pickup-full-regions-six-seed-v44.json', support / 'reference.json')
        files.append(support / 'reference.json')
        write(raw / 'trick-analysis-request.json', {'raw': str(raw), 'reference_raw': str(reference),
              'output': str(out), 'commit': a.commit, 'files': {str(path): digest(path) for path in files},
              'root': str(ROOT)})
        command = ['/usr/bin/python3', str(own), '--execute', '--raw', str(raw),
                   '--reference-raw', str(reference), '--output', str(out), '--commit', a.commit]
        (raw / 'trick-analysis.sh').write_text('#!/bin/bash\nset -eu\nexec ' + ' '.join(map(shlex.quote, command)) + '\n')
        submit = ['qsub', '-terse', '-w', 'e', '-cwd', '-q', 'debian.q', '-pe', 'threaded', '1',
                  '-binding', 'linear:1', '-l', 'exclusive=true,h_rt=00:30:00,h_vmem=8G', '-m', 'n',
                  '-N', 'trick_analysis', '-j', 'y', '-o', str(raw / 'trick-analysis.log'), '-S', '/bin/bash']
        if a.hold_job: submit += ['-hold_jid', a.hold_job]
        submit.append(str(raw / 'trick-analysis.sh'))
        result = subprocess.run(submit, cwd=str(ROOT), stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        write(raw / 'trick-analysis-submission.json', dict(command=submit, returncode=result.returncode, response=result.stdout))
        print(result.stdout, end=''); return result.returncode
    request = read(raw / 'trick-analysis-request.json')
    for path, sha in request['files'].items(): assert digest(Path(path)) == sha, path
    # Frozen script lives under raw/support, so use the recorded repository root.
    root = Path(request['root'])
    support = raw / 'trick-analysis-support'
    cpu = cpu_resources(); assert cpu['effective_cpu_quota'] is None and cpu['representative_cpus']
    os.sched_setaffinity(0, cpu['representative_cpus'][:1])
    spec, build = read(raw / 'spec.json'), read(raw / 'build.json')
    sources = dict(build['sources']); sources.update(build['test_sources'])
    for path, sha in sources.items():
        assert hashlib.sha256(subprocess.check_output(['git', 'show', a.commit + ':' + path], cwd=str(root))).hexdigest() == sha
    assert digest(raw / 'lifelong') == build['binary_sha256']
    subprocess.run(['/usr/bin/python3', str(support / 'experiments/sequences-20260918/analyze.py'),
                    '--input', str(raw), '--output', str(out), '--workers', '1'], check=True)
    is_full = not spec['horizons'] or spec['horizons']['WAREHOUSE'] == 5000
    verified = None
    if is_full:
        sys.path.insert(0, str(support / 'experiments/construction-20260918'))
        import verify_full
        verify_full.ROOT = root
        verified = verify_full.verify(raw, out, a.commit, allow_failed=True)
        write(out / 'verification.json', verified)
    reference_cases = {case['seed']: case['name'] for case in read(reference / 'spec.json')['cases']}
    records, intervals = [], []
    allocation = read(raw / 'allocation.json')
    canonical = {row['seed']: row for row in read(support / 'reference.json')['rows']
                 if row['environment']['CGAR_TEMPORAL_REGIONS'] == '4'}
    metrics = {row['case']: row for row in read(out / 'metrics.json')}
    for case in spec['cases']:
        name = case['name']; folder = raw / name
        summary, meta = read(folder / 'summary.json')[0], read(folder / 'metadata.json')
        trick = spec.get('trick'); expected_track = 'TRICK' if trick else 'GENERIC'
        assert meta['trick'] == trick and meta['experiment_track'] == expected_track
        assert meta['trick_argv'] == (['--trick', trick] if trick else [])
        assert meta['build_provenance'] == build
        resources = meta['cpu_resources']
        assert resources['effective_cpu_quota'] is None and resources['cpu_model'] == 'AMD EPYC 9354 32-Core Processor'
        binding = summary['cpu']; assert binding == meta['cpu_binding'] and len(binding) == spec['cpus_per_instance']
        core_of = {cpu: tuple(group) for group in resources['logical_cpus_by_physical_core'] for cpu in group}
        cores = {(resources['hostname'], core_of[cpu]) for cpu in binding}
        assert len(cores) == len(binding) and set(binding).issubset(allocation['selected_cpus'])
        begin, end = (datetime.datetime.fromisoformat(meta[k]) for k in ['started_utc', 'finished_utc'])
        for other_begin, other_end, other_cores in intervals:
            assert not (begin < other_end and other_begin < end and cores.intersection(other_cores))
        intervals.append((begin, end, cores))
        log = (folder / 'WAREHOUSE.log').read_text()
        receipts = [line for line in log.splitlines() if line.startswith('[CGAR_TRICK] ')]
        if trick:
            assert len(receipts) == 1
            fields = dict(token.split('=', 1) for token in receipts[0].split()[1:])
            assert fields['instance'] == trick and fields['field_sha256'] == meta['expected_trick_field_sha256']
            assert '[cgar-flow] ' not in log
        else: assert not receipts
        assert summary['trick_receipt_valid'] and summary['experiment_track_valid']
        assert summary['memory_valid'] and summary['peak_process_rss_bytes'] < 32000000000
        if not summary['valid']:
            assert summary['outcome'] == 'timeout' and summary['exit'] == 124 and summary['after'] is None
            records.append({'case': name, 'seed': case['seed'], 'valid': False, 'outcome': 'explicit_timeout',
                            'timeouts': re.findall(r'CGAR_TIMEOUT[^\n]+', log)})
            continue
        data = read(folder / 'WAREHOUSE.json')
        assert data['experimentTrack'] == expected_track and data['trick'] == (trick or '')
        assert data['makespan'] == meta['instances']['WAREHOUSE']['steps']
        assert all(data[key] == 0 for key in ['numPlannerErrors', 'numScheduleErrors', 'numEntryTimeouts'])
        times = data['entryComputeTimes']; assert len(times) == data['makespan'] and max(times) <= 1
        baseline = read(reference / reference_cases[case['seed']] / 'WAREHOUSE.json')
        assert initial_schedule(data) == initial_schedule(baseline)
        row = {'case': name, 'seed': case['seed'], 'valid': True, 'tasks': data['numTaskFinished'],
               'steps': data['makespan'], 'track': expected_track, 'initial_dispatch_exact_to_generic': True,
               'max_first81_entry_ms': max(times[:81]) * 1000, 'max_first101_entry_ms': max(times[:101]) * 1000,
               'max_all_entry_ms': max(times) * 1000, 'peak_rss_bytes': summary['peak_process_rss_bytes'],
               'planner_receipt_and_result_track_verified': True,
               'trajectory_sha256': metrics[name]['trajectory_sha256']}
        if is_full and not trick:
            assert row['tasks'] == canonical[case['seed']]['tasks']
            assert row['trajectory_sha256'] == canonical[case['seed']]['trajectory_sha256']
            row['full_generic_trajectory_exact_to_reference'] = True
        records.append(row)
        del data, baseline
    write(out / 'trick-verification.json', {'source_commit': a.commit, 'verified_source_and_test_files': len(sources),
          'checked_utc': datetime.datetime.now(datetime.timezone.utc).isoformat(), 'track': 'TRICK' if spec.get('trick') else 'GENERIC',
          'full_run': is_full, 'all_valid': all(row['valid'] for row in records), 'rows': records,
          'scope': 'Startup screens establish no full-run record; TRICK results never replace the generic frontier.'})
    for name in ['trick-analysis-request.json', 'trick-analysis-submission.json', 'completion.json', 'submission.json']:
        shutil.copy2(raw / name, out / name)
    for path, sha in request['files'].items(): assert digest(Path(path)) == sha, path
    print('TRICK_ANALYSIS_COMPLETE ' + json.dumps(records))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
