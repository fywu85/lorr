"""Queue full KK diagnostic accounting; never interpret failed/short runs as scores."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import shlex
import shutil
import statistics
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[3]
BASE = ROOT / 'experiments/construction-20260918'
sys.path.insert(0, str(ROOT / 'tools'))
from cpu_resources import cpu_resources


def read(path):
    return json.loads(Path(path).read_text())


def write(path, value):
    Path(path).write_text(json.dumps(value, indent=2) + '\n')


def digest(path):
    h = hashlib.sha256()
    with Path(path).open('rb') as stream:
        for chunk in iter(lambda: stream.read(4 * 1024 * 1024), b''):
            h.update(chunk)
    return h.hexdigest()


def distribution(values):
    if not values:
        return {'n': 0}
    values = sorted(values)
    return {'n': len(values), 'mean': statistics.mean(values), 'median': statistics.median(values),
            'p99': values[min(len(values) - 1, (99 * len(values) + 99) // 100 - 1)],
            'min': values[0], 'max': values[-1]}


def accounting(data):
    n = data['completed_tasks']
    actions = data['completed_loaded_action_totals']
    total = data['team_size'] * data['steps']
    assert sum(sum(a.values()) for a in data['full_phase_actions'].values()) == total == 50000000
    loaded = sum(actions.values())
    assert loaded == data['completed_loaded_steps']['sum']
    amounts = {'completed_chain_lower_bound': data['completed_chain_shortest']['sum'],
        'completed_forward_detour': data['completed_loaded_forward_excess_over_shortest'],
        'completed_turns': actions['cr'] + actions['ccr'], 'completed_waits': actions['wait'],
        'completed_other': actions['other'],
        'empty_including_unfinished': data['empty_robot_steps_including_reassignments'],
        'loaded_unfinished': data['loaded_robot_steps_including_unfinished_tasks'] - loaded,
        'idle': data['unassigned_robot_steps']}
    assert amounts['completed_chain_lower_bound'] + amounts['completed_forward_detour'] == actions['fw']
    assert sum(amounts.values()) == total and all(v >= 0 for v in amounts.values())
    return {'robot_steps': total, 'components': amounts, 'per_completed_task': {k: v / n for k, v in amounts.items()},
            'robot_steps_per_completed_task': total / n}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--raw', type=Path, required=True)
    parser.add_argument('--execute', action='store_true')
    args = parser.parse_args(); raw = args.raw.resolve()
    spec = read(raw / 'spec.json')
    assert spec['steps'] == 5000
    request_path = raw / 'analysis-request.json'
    if not args.execute:
        assert not request_path.exists(), 'an analysis was already prepared'
        for name in ['decompose.py', 'decompose.cpp']:
            # This location preserves decompose.py's repository-root resolution.
            shutil.copy2(BASE / name, raw / name)
        shutil.copy2(Path(__file__), raw / 'support/analyze.py')
        files = [raw / 'decompose.py', raw / 'decompose.cpp', raw / 'support/analyze.py',
                 raw / 'spec.json', ROOT / 'tools/cpu_resources.py', ROOT / 'cgar/inc/nlohmann/json.hpp']
        request = {'files': {str(p): digest(p) for p in files},
            'output': str(BASE / 'results/kk-guidance-rematch-full-v2'),
            'scope': 'Competitor diagnosis only; clock-limited, shared weights affect scheduling and motion.'}
        write(request_path, request)
        script = raw / 'analysis.sh'
        command = ['/usr/bin/python3', str(raw / 'support/analyze.py'), '--raw', str(raw), '--execute']
        script.write_text('#!/bin/bash\nset -eu\nexec ' + ' '.join(shlex.quote(x) for x in command) + '\n')
        job = read(raw / 'submission.json')['response'].strip().split('.')[0]
        assert job.isdigit()
        submit = ['/opt/n1ge/bin/lx24-amd64/qsub', '-terse', '-w', 'e', '-cwd', '-q', 'debian.q',
            '-hold_jid', job, '-pe', 'threaded', '1', '-binding', 'linear:1',
            '-l', 'exclusive=true,h_rt=01:00:00,h_vmem=8G', '-m', 'n', '-N', 'kk_diag_analysis',
            '-j', 'y', '-o', str(raw / 'analysis.log'), '-S', '/bin/bash', str(script)]
        proc = subprocess.run(submit, cwd=str(ROOT), stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        write(raw / 'analysis-submission.json', {'command': submit, 'returncode': proc.returncode, 'response': proc.stdout})
        print(proc.stdout, end=''); return proc.returncode
    request = read(request_path)
    for path, expected in request['files'].items():
        assert digest(path) == expected, path
    allocation = cpu_resources()
    assert allocation['physical_cores_visible'] >= 1 and allocation['effective_cpu_quota'] is None
    os.sched_setaffinity(0, allocation['representative_cpus'][:1])
    assert digest(raw / 'lifelong') == spec['binary_sha256']
    assert digest(Path(spec['build']) / 'build.json') == spec['build_manifest_sha256']
    for path, expected in spec['input_hashes'].items():
        assert digest(path) == expected, path
    for path, expected in spec['source_hashes'].items():
        assert digest(Path(spec['source']) / path) == expected, path
    for guidance, expected in spec['config_hashes'].items():
        assert digest(raw / 'configs' / (guidance + '.json')) == expected
    cases = {}
    for case in spec['cases']:
        work = raw / case['name']
        summary = read(work / 'summary.json')
        if summary['result']['makespan'] == 5000 and (work / 'result.json').exists():
            cases[case['name']] = str(work / 'result.json')
    assert cases, 'No full trajectory to analyze; consult run summaries for failures.'
    write(raw / 'analysis-cases.json', cases)
    replay = raw / 'decomposition'
    subprocess.run(['/usr/bin/python3', str(raw / 'decompose.py'), '--output', str(replay),
                    '--cases', str(raw / 'analysis-cases.json')], check=True)
    provenance = read(replay / 'provenance.json')
    assert all(provenance[k] for k in ['fixture_passed', 'loaded_action_fixture_passed',
                                      'opposite_turn_fixture_passed', 'phase_boundary_fixture_passed'])
    assert provenance['source_sha256'] == digest(raw / 'decompose.cpp')
    assert provenance['script_sha256'] == digest(raw / 'decompose.py')
    out = Path(request['output']); out.mkdir(parents=True, exist_ok=False)
    rows = []
    for case in spec['cases']:
        name = case['name']; work = raw / name
        summary = read(work / 'summary.json'); meta = read(work / 'metadata.json')
        resources = read(work / 'allocation.json'); cpu = resources['resources']
        assert meta['case'] == case == summary['case']
        assert meta['binary_sha256'] == spec['binary_sha256']
        assert meta['source_hashes'] == spec['source_hashes']
        assert meta['config_sha256'] == spec['config_hashes'][case['guidance']]
        assert meta['environment']['LORR_KK_REMATCH_UNOPENED'] == str(case['rematch'])
        assert cpu['cpu_model'] == spec['expected_cpu_model'] and cpu['effective_cpu_quota'] is None
        assert cpu['physical_cores_visible'] == 16 and len(cpu['logical_cpu_affinity']) == 32
        assert all(len(c) == 2 for c in cpu['logical_cpus_by_physical_core'])
        row = dict(summary, hostname=cpu['hostname'], job_id=resources['job_id'], task_id=resources['task_id'])
        row['cpu_seconds_per_wall_second_whole_process'] = (summary['usage']['user_seconds'] + summary['usage']['system_seconds']) / summary['usage']['wall_seconds']
        if name in cases:
            data = read(replay / (name + '.json'))
            assert data['completed_tasks'] == summary['result']['numTaskFinished']
            assert data['assignments_after_pickup'] == 0
            if not case['rematch']:
                assert data['tasks_with_multiple_assignments'] == 0
            row.update(accounting(data))
            row['trajectory_file_sha256'] = provenance['inputs'][name]['sha256']
            row['assignment_observations'] = {k: data[k] for k in ['tasks_with_multiple_assignments',
                'first_assignment_pickup_shortest', 'last_assignment_pickup_shortest',
                'observed_first_assignment_pickup_delay', 'observed_last_assignment_pickup_delay']}
            shutil.copy2(replay / (name + '.json'), out / (name + '-decomposition.json'))
        diagnostics = read(work / 'search-diagnostics.json')
        if summary['valid_simulator_counters']:
            assert [r['step'] for r in diagnostics['lns']] == list(range(5000))
            assert [r['step'] for r in diagnostics['entry']] == list(range(5000))
        row['diagnostic_summaries'] = {}
        for key, fields in [('entry', ['scheduler_ms', 'elapsed_before_diag_ms']),
                            ('lns', ['limit_s', 'remaining_before_s', 'iteration_records', 'failures']),
                            ('rematching', ['agents', 'tasks'])]:
            row['diagnostic_summaries'][key] = {field: distribution([r[field] for r in diagnostics[key]]) for field in fields}
        rows.append(row)
        for filename in ['summary.json', 'allocation.json', 'metadata.json', 'search-diagnostics.json']:
            shutil.copy2(work / filename, out / (name + '-' + filename))
    # Each live arm was submitted to an exclusive, separate EPYC host.
    assert len({r['hostname'] for r in rows}) == len(rows)
    submit = read(raw / 'submission.json')['command']
    assert 'exclusive=true,h_rt=03:00:00,h_vmem=4G' in submit and 'linear:16' in submit
    controls = [r for r in rows if r['case']['guidance'] == 'supplied' and r['case']['rematch'] == 1]
    control_tasks = [r['result']['numTaskFinished'] for r in controls if r['valid_simulator_counters']]
    report = {'completed_utc': datetime.datetime.now(datetime.timezone.utc).isoformat(), 'rows': rows,
        'control_tasks': control_tasks,
        'control_observed_range': max(control_tasks) - min(control_tasks) if len(control_tasks) == 2 else None,
        'limits': ['Clock-limited competitor interventions alter both assignment and available planner work.',
                   'Uniform weights affect scheduler costs and motion guidance together.',
                   'Two controls provide an observed difference, not a confidence bound.',
                   'Entry diagnostic excludes its own print; complete one-second entry series is not certified.',
                   'Iteration records count merged updates, not all speculative attempts.',
                   'Memory and timeout results must be retained; these are not qualifying CGAR scores.'],
        'allocation': cpu_resources(), 'scope': request['scope'], 'throughput_goal_complete': False}
    for path, expected in request['files'].items():
        assert digest(path) == expected, path
    write(out / 'comparison.json', report)
    for source, target in [(replay / 'provenance.json', 'provenance.json'),
                            (request_path, 'analysis-request.json'),
                            (raw / 'analysis-submission.json', 'analysis-submission.json'),
                            (raw / 'spec.json', 'spec.json'), (raw / 'submission.json', 'submission.json')]:
        shutil.copy2(source, out / target)
    print(json.dumps([{k: r[k] for k in ['case', 'valid_simulator_counters', 'result', 'peak_rss_bytes']} for r in rows], indent=2))


if __name__ == '__main__':
    raise SystemExit(main())
