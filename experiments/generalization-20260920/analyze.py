#!/usr/bin/env python3
"""Collect the nine-instance transfer check on a reserved GRID CPU."""
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
HERE = ROOT / 'experiments/generalization-20260920'


def read(p):
    return json.loads(p.read_text())


def write(p, data):
    p.write_text(json.dumps(data, indent=2) + '\n')


def digest(p):
    return hashlib.sha256(p.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--raw', type=Path, required=True)
    parser.add_argument('--hold-job')
    parser.add_argument('--execute', action='store_true')
    a = parser.parse_args()
    raw = a.raw.resolve()
    support, out = raw / 'analysis-support', HERE / 'results'
    if not a.execute:
        support.mkdir(exist_ok=False)
        own = support / 'analyze.py'
        shutil.copy2(Path(__file__), own)
        files = [own]
        for name in ['sequences-20260918/analyze.py', 'motion-20260918/analyze.py',
                     'assignment-20260918/analyze.py', 'throughput-20260918-next/analyze_matrix.py',
                     'throughput-20260918-strict/analyze.py']:
            dest = support / 'experiments' / name
            dest.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(ROOT / 'experiments' / name, dest)
            files.append(dest)
        for name in ['source-provenance.json', 'variants.json']:
            shutil.copy2(HERE / name, support / name)
            files.append(support / name)
        write(raw / 'analysis-request.json', dict(files={str(f): digest(f) for f in files}))
        command = ['/usr/bin/python3', str(own), '--execute', '--raw', str(raw)]
        job = raw / 'analysis.sh'
        job.write_text('#!/bin/bash\nset -eu\nexec ' + ' '.join(map(shlex.quote, command)) + '\n')
        submit = ['qsub', '-h', '-terse', '-w', 'n', '-cwd', '-q', 'debian.q', '-pe', 'threaded', '1',
                  '-binding', 'linear:1', '-l', 'exclusive=true,h_rt=00:45:00,h_vmem=8G', '-m', 'n',
                  '-N', 'generalization_analysis', '-j', 'y', '-o', str(raw / 'analysis.log'), '-S', '/bin/bash']
        if a.hold_job:
            submit += ['-hold_jid', a.hold_job]
        submit.append(str(job))
        r = subprocess.run(submit, cwd=ROOT, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        receipt = dict(command=submit, returncode=r.returncode, response=r.stdout)
        write(raw / 'analysis-submission.json', receipt)
        write(HERE / 'analysis-submission.json', receipt)
        print(r.stdout, end='')
        if r.returncode:
            return r.returncode
        assert re.fullmatch(r'\d+\s*', r.stdout), r.stdout
        subprocess.run(['qrls', r.stdout.strip()], check=True)
        return 0

    for path, sha in read(raw / 'analysis-request.json')['files'].items():
        assert digest(Path(path)) == sha, path
    sys.path.insert(0, str(ROOT / 'tools'))
    from cpu_resources import cpu_resources
    resources = cpu_resources()
    assert resources['effective_cpu_quota'] is None and resources['representative_cpus']
    os.sched_setaffinity(0, resources['representative_cpus'][:1])
    source = read(support / 'source-provenance.json')
    build, spec = read(raw / 'build.json'), read(raw / 'spec.json')
    assert build == source['build'] and digest(raw / 'lifelong') == source['binary_sha256']
    for name, sha in dict(build['sources'], **build['test_sources']).items():
        assert hashlib.sha256(subprocess.check_output(['git', 'show', source['exact_source_commit'] + ':' + name], cwd=ROOT)).hexdigest() == sha
    assert spec['trick'] is None and spec['experiment_track'] == 'GENERIC' and spec['horizons'] is None
    assert len(spec['cases']) == 1 and spec['cases'][0]['seed'] == 0
    assert spec['time_limit_ms'] == 1000 and spec['cpus_per_instance'] == 4
    case = spec['cases'][0]
    assert case['environment'] == next(iter(read(support / 'variants.json').values()))
    times = read(ROOT / 'mr24/simulation_time.json')
    assert set(spec['instances']) == set(times) - {'WAREHOUSE'}
    subprocess.run(['/usr/bin/python3', str(support / 'experiments/sequences-20260918/analyze.py'),
                    '--input', str(raw), '--output', str(out), '--workers', '1'], check=True)
    meta = read(out / 'run-metadata.json')[case['name']]
    cpu = meta['cpu_resources']
    assert cpu['effective_cpu_quota'] is None and cpu['cpu_model'] == 'AMD EPYC 9354 32-Core Processor'
    assert meta['trick'] is None and meta['trick_argv'] == [] and meta['experiment_track'] == 'GENERIC'
    assert meta['build_provenance'] == build and meta['max_process_memory_bytes'] == 32000000000
    assert meta['environment'] == dict(case['environment'], CGAR_SEED='0')
    allocation = read(out / 'allocation.json')
    assert len(allocation['selected_cpus']) == len(meta['cpu_binding']) == 36
    core_of = {c: tuple(group) for group in cpu['logical_cpus_by_physical_core'] for c in group}
    metrics = {row['instance']: row for row in read(out / 'metrics.json')}
    rows, occupied = [], set()
    for summary in read(out / 'run-summaries.json')[case['name']]:
        name, binding = summary['instance'], summary['cpu']
        assert len(binding) == 4 and set(binding).issubset(meta['cpu_binding'])
        cores = {core_of[c] for c in binding}
        assert len(cores) == 4 and not occupied.intersection(cores)
        occupied.update(cores)
        assert meta['instances'][name]['steps'] == times[name]
        log = (raw / case['name'] / (name + '.log')).read_text()
        assert '[CGAR_TRICK]' not in log and '[cgar-temporal-score]' not in log
        assert summary['trick_receipt_valid'] and summary['experiment_track_valid']
        usage = summary['process_resources']
        row = dict(instance=name, steps=times[name], seed=0, valid=summary['valid'], outcome=summary['outcome'],
                   tasks=None, max_entry_seconds=summary['entry_compute_max_seconds'], peak_rss_bytes=summary['peak_process_rss_bytes'],
                   wall_seconds=usage['wall_seconds'], average_cpu_cores=(usage['user_seconds'] + usage['system_seconds']) / usage['wall_seconds'],
                   reserved_physical_cores=4, complete_entry_samples=summary['entry_compute_samples'],
                   planner_errors=summary['planner_errors'], schedule_errors=summary['schedule_errors'],
                   simulator_timeouts=summary['timeouts'], internal_timeouts=summary['internal_timeouts'])
        if summary['valid']:
            m = metrics[name]
            assert summary['makespan'] == summary['entry_compute_samples'] == m['steps'] == times[name]
            assert summary['entry_compute_max_seconds'] <= 1 and summary['peak_process_rss_bytes'] <= 32000000000
            assert all(summary[k] == 0 for k in ['planner_errors', 'schedule_errors', 'timeouts', 'internal_timeouts', 'exit'])
            assert m['movement_diagnostics']['complete'] and m['tasks'] == summary['after']
            header = read(raw / case['name'] / (name + '.json'))
            assert header['experimentTrack'] == 'GENERIC' and header['trick'] == ''
            assert sum(m['movement_phases'][str(k)][action] for k in range(3) for action in ['fw', 'cr', 'ccr', 'wait']) == times[name] * header['teamSize']
            row.update(tasks=m['tasks'], robots=header['teamSize'], mean_entry_ms=m['total_decision_seconds'] * 1000 / times[name],
                       trajectory_sha256=m['trajectory_sha256'], completed_per_1000=m['completed_per_1000'],
                       last_planner_counters=m.get('last_planner_counters', {}))
            del header
        rows.append(row)
    assert len(rows) == len({r['instance'] for r in rows}) == 9
    completion = read(raw / 'completion.json')
    assert completion['returncode'] == int(any(not r['valid'] for r in rows))
    report = dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(), exact_source_commit=source['exact_source_commit'],
                  binary_sha256=source['binary_sha256'], track='GENERIC', trick_argv=[], unchanged_warehouse_configuration=True,
                  seed=0, rows=rows, valid_instances=sum(r['valid'] for r in rows), invalid_instances=sum(not r['valid'] for r in rows),
                  all_physical_core_groups_disjoint=True, no_cpu_quota=True,
                  scope='One full planner seed on each of nine fixed competition instances. No map-specific settings, independent workload draws, six-seed confirmation or new SoTA claim.')
    for name in ['submission.json', 'analysis-request.json', 'analysis-submission.json', 'completion.json']:
        shutil.copy2(raw / name, out / name)
    write(out / 'verification.json', report)
    lines = ['# Generic transfer results', '', 'One planner seed0; unchanged warehouse-selected configuration; full competition horizons.', '',
             '| Instance | Outcome | Tasks | Mean step ms | Max step ms | Peak GB | Wall min |', '|---|---|---:|---:|---:|---:|---:|']
    for r in rows:
        lines.append('| {} | {} | {} | {} | {} | {:.3f} | {:.2f} |'.format(r['instance'], r['outcome'], r['tasks'] if r['valid'] else 'n/a',
                     round(r['mean_entry_ms'], 2) if r['valid'] else 'n/a', round(r['max_entry_seconds'] * 1000, 2) if r['max_entry_seconds'] is not None else 'n/a',
                     r['peak_rss_bytes'] / 1e9, r['wall_seconds'] / 60))
    (out / 'summary.md').write_text('\n'.join(lines) + '\n')
    status = read(HERE / 'status.json')
    status.update(state='complete', completed_utc=completion['finished_utc'], evidence='results/verification.json', valid_instances=report['valid_instances'])
    write(HERE / 'status.json', status)
    print('GENERALIZATION_VERIFIED', report['valid_instances'], 'valid,', report['invalid_instances'], 'invalid')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
