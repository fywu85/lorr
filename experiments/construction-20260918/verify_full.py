"""Validate a completed full warehouse matrix before comparing its policies."""
import datetime
import hashlib
import json
import re
from pathlib import Path
import shutil
import subprocess

ROOT = Path(__file__).resolve().parents[2]


def read(path):
    return json.loads(path.read_text())


def verify(raw, archive, commit, allow_failed=False):
    raw, archive = Path(raw).resolve(), Path(archive).resolve()
    build = read(raw / 'build.json')
    sources = dict(build['sources'])
    sources.update(build['test_sources'])
    for path, expected in sources.items():
        actual = subprocess.check_output(['git', 'show', commit + ':' + path], cwd=ROOT)
        assert hashlib.sha256(actual).hexdigest() == expected, ('source', path)
    binary = hashlib.sha256((raw / 'lifelong').read_bytes()).hexdigest()
    assert binary == build['binary_sha256'], 'binary differs from frozen build'
    spec = read(archive / 'spec.json')
    assert spec['instances'] == ['WAREHOUSE'] and spec['time_limit_ms'] == 1000
    count = len(spec['cases'])
    completion = read(raw / 'completion.json')
    assert len(completion['case_returncodes']) == count
    assert completion['returncode'] == int(any(completion['case_returncodes']))
    if not allow_failed:
        assert completion['returncode'] == 0 and completion['case_returncodes'] == [0] * count
    metrics = {m['case']: m for m in read(archive / 'metrics.json')}
    summaries, metadata = read(archive / 'run-summaries.json'), read(archive / 'run-metadata.json')
    assert len(summaries) == len(metadata) == count
    assert set(metrics) == {name for name, records in summaries.items() if records[0]['valid']}
    allocation = read(archive / 'allocation.json')
    rows, intervals, failures = [], [], []
    for case in spec['cases']:
        name = case['name']
        meta = metadata[name]
        assert len(summaries[name]) == 1
        s = summaries[name][0]
        assert completion['case_returncodes'][spec['cases'].index(case)] == int(not s['valid']), name
        assert meta['build_provenance'] == build and meta['binary_sha256'] == binary, name
        assert meta['plan_time_limit_ms'] == 1000 and meta['max_process_memory_bytes'] == 32000000000
        resources = meta['cpu_resources']
        assert resources['effective_cpu_quota'] is None
        assert resources['cpu_model'] == 'AMD EPYC 9354 32-Core Processor'
        binding = s['cpu']
        assert binding == meta['cpu_binding'] and len(binding) == spec['cpus_per_instance']
        assert set(binding).issubset(allocation['selected_cpus'])
        core_of = {cpu: tuple(group) for group in resources['logical_cpus_by_physical_core'] for cpu in group}
        cores = {(resources['hostname'], core_of[cpu]) for cpu in binding}
        assert len(cores) == len(binding), ('shared hardware threads', name)
        begin, end = (datetime.datetime.fromisoformat(meta[k]) for k in ['started_utc', 'finished_utc'])
        assert end > begin
        for other, a, b, occupied in intervals:
            assert not (begin < b and a < end and cores.intersection(occupied)), ('overlapping physical allocation', name, other)
        intervals.append((name, begin, end, cores))
        assert meta['instances']['WAREHOUSE']['steps'] == 5000
        assert s['memory_valid'] and s['peak_process_rss_bytes'] < 32000000000
        if not s['valid']:
            assert allow_failed, ('invalid full run', name)
            assert s['outcome'] == 'timeout' and s['exit'] == 124 and s['internal_timeouts'] > 0, name
            assert s['after'] is None and s['makespan'] is None, ('partial result', name)
            log = (raw / name / 'WAREHOUSE.log').read_text()
            matches = re.findall(r'CGAR_TIMEOUT timestep=(\d+) elapsed_ms=([\d.]+) stage=(\S+)', log)
            assert len(matches) == 1, ('missing explicit timeout', name)
            step, elapsed, stage = matches[0]
            assert float(elapsed) >= 1000
            failures.append({'case': name, 'seed': case['seed'], 'variant': case['variant'],
                             'environment': case['environment'], 'outcome': s['outcome'],
                             'failed_timestep': int(step), 'elapsed_ms': float(elapsed), 'stage': stage,
                             'peak_rss_bytes': s['peak_process_rss_bytes'], 'finished_utc': meta['finished_utc'],
                             'reserved_physical_cores': len(cores), 'raw_case': str(raw / name)})
            continue
        m = metrics[name]
        assert s['valid'] and s['outcome'] == 'success', name
        assert s['makespan'] == s['entry_compute_samples'] == m['steps'] == 5000, name
        assert meta['instances']['WAREHOUSE']['steps'] == 5000
        assert all(s[k] == 0 for k in ['planner_errors', 'schedule_errors', 'timeouts', 'internal_timeouts', 'exit']), name
        assert s['entry_timing_valid'] and s['entry_compute_max_seconds'] <= 1, name
        assert s['memory_valid'] and s['peak_process_rss_bytes'] < 32000000000, name
        assert s['after'] == m['tasks'] and s['entry_compute_max_seconds'] == m['max_decision_seconds'], name
        assert m['movement_diagnostics']['complete'], name
        assert sum(m['movement_phases'][str(k)][a] for k in range(3) for a in ['fw', 'cr', 'ccr', 'wait']) == 50000000, name
        process = s['process_resources']
        empty, loaded = m['movement_phases']['1'], m['movement_phases']['2']
        rows.append({'case': name, 'seed': case['seed'], 'variant': case['variant'],
                     'environment': case['environment'], 'tasks': m['tasks'],
                     'final1000': m['completed_per_1000'][-1],
                     'outstanding_age_p90': m['outstanding_task_age']['p90'],
                     'empty_robot_steps': sum(empty[a] for a in ['fw', 'cr', 'ccr', 'wait']),
                     'loaded_turns': loaded['cr'] + loaded['ccr'], 'loaded_waits': loaded['wait'],
                     'trajectory_sha256': m['trajectory_sha256'],
                     'mean_entry_ms': m['total_decision_seconds'] / 5,
                     'max_entry_seconds': s['entry_compute_max_seconds'],
                     'peak_rss_bytes': s['peak_process_rss_bytes'],
                     'average_cpu_cores': (process['user_seconds'] + process['system_seconds']) / process['wall_seconds'],
                     'wall_seconds': s['wall_seconds'], 'finished_utc': meta['finished_utc'],
                     'reserved_physical_cores': len(cores), 'raw_case': str(raw / name),
                     'evidence': str(archive.relative_to(ROOT))})
    for name in ['submission.json', 'motion-analysis-submission.json', 'completion.json']:
        shutil.copy2(raw / name, archive / name)
    return {'checked_utc': datetime.datetime.now(datetime.timezone.utc).isoformat(),
            'exact_production_source_commit': commit, 'verified_source_and_test_files': len(sources),
            'binary_sha256': binary, 'full_cases': count, 'complete_entry_samples': len(rows) * 5000,
            'failed_cases': len(failures), 'failures': failures,
            'all_valid_within_deadline_and_memory': not failures,
            'all_completed_runs_valid_within_deadline_and_memory': True,
            'disjoint_physical_bindings_during_concurrent_execution': True,
            'no_cpu_quota': True, 'rows': rows}
