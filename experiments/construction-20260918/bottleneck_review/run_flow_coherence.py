#!/usr/bin/env python3
"""Freeze and replay guidance/traffic diagnostics on three reserved GRID cores."""
import argparse
import concurrent.futures
import datetime
import hashlib
import json
import os
from pathlib import Path
import queue
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
    result = hashlib.sha256()
    with path.open('rb') as stream:
        for chunk in iter(lambda: stream.read(4 * 1024 * 1024), b''):
            result.update(chunk)
    return result.hexdigest()


def write(path, value):
    path.write_text(json.dumps(value, indent=2) + '\n')


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--output', type=Path, required=True)
    p.add_argument('--execute', action='store_true')
    a = p.parse_args(); raw = a.output.resolve()
    if not a.execute:
        raw.mkdir(parents=True, exist_ok=False)
        support = raw / 'support'; support.mkdir()
        reports = {'seed%d' % seed: BASE / ('results/current-bottleneck-audit-v50/seed%d/seed%d.json' % (seed, seed)) for seed in range(6)}
        reports.update(kittyknight=BASE / 'results/loaded-motion-audit-v1/kittyknight.json',
                       nms_diagnostic=BASE / 'results/nms-task-pool-audit/nms_diagnostic.json')
        cases = []
        for name, path in reports.items():
            data = read(path); provenance = read(path.parent / 'provenance.json')
            trajectory = (ROOT / data['input']).resolve()
            matches = [x for x in provenance['inputs'].values() if (ROOT / x['path']).resolve() == trajectory]
            assert len(matches) == 1
            actions = {k: sum(row[k] for row in data['full_phase_actions'].values()) for k in ['fw', 'cr', 'ccr', 'wait', 'other']}
            cases.append({'name': name, 'input': str(trajectory), 'input_sha256': matches[0]['sha256'],
                          'report': str(path), 'report_sha256': digest(path), 'learned': name.startswith('seed'),
                          'tasks': data['completed_tasks'], 'actions': actions, 'input_bytes': trajectory.stat().st_size})
        files = []
        for src, target in [(Path(__file__), support / 'run_flow_coherence.py'),
                (Path(__file__).with_name('flow_coherence.cpp'), support / 'flow_coherence.cpp'),
                (ROOT / 'cgar/cgar_planner/flow_guidance.hpp', support / 'flow_guidance.hpp'),
                (ROOT / 'cgar/inc/nlohmann/json.hpp', support / 'nlohmann/json.hpp'),
                (ROOT / 'mr24/warehouse.domain/maps/warehouse_large.map', support / 'warehouse.map')]:
            target.parent.mkdir(parents=True, exist_ok=True); shutil.copy2(src, target); files.append(target)
        write(raw / 'spec.json', {'cases': cases, 'files': {str(path): digest(path) for path in files},
              'source_commit': subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=str(ROOT), text=True).strip(),
              'result_directory': str(BASE / 'results/flow-coherence-v55')})
        cmd = ['/usr/bin/python3', str(support / 'run_flow_coherence.py'), '--output', str(raw), '--execute']
        (raw / 'job.sh').write_text('#!/bin/bash\nset -eu\nexec ' + ' '.join(map(shlex.quote, cmd)) + '\n')
        submit = ['qsub', '-terse', '-w', 'e', '-cwd', '-q', 'debian.q', '-pe', 'threaded', '3',
                  '-binding', 'linear:3', '-l', 'exclusive=true,h_rt=00:30:00,h_vmem=8G', '-m', 'n',
                  '-N', 'flow_coherence', '-j', 'y', '-o', str(raw / 'job.log'), '-S', '/bin/bash', str(raw / 'job.sh')]
        result = subprocess.run(submit, cwd=str(ROOT), stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        write(raw / 'submission.json', dict(command=submit, returncode=result.returncode, response=result.stdout))
        print(result.stdout, end=''); return result.returncode
    spec = read(raw / 'spec.json'); support = raw / 'support'
    for path, sha in spec['files'].items(): assert digest(Path(path)) == sha, path
    cpu = cpu_resources(); cpus = cpu['representative_cpus'][:3]
    assert len(cpus) == 3 and int(os.environ['NSLOTS']) >= 3 and cpu['effective_cpu_quota'] is None
    os.sched_setaffinity(0, cpus)
    write(raw / 'allocation.json', {'resources': cpu, 'selected_cpus': cpus, 'job_id': os.environ['JOB_ID']})
    binary = raw / 'coherence'
    command = ['taskset', '-c', str(cpus[0]), str(ROOT / 'env/bin/x86_64-conda-linux-gnu-c++'),
               '-std=c++17', '-O2', '-I' + str(support), str(support / 'flow_coherence.cpp'),
               '-Wl,-rpath,' + str(ROOT / 'env/lib'), '-o', str(binary)]
    subprocess.run(command, check=True)
    subprocess.run(['taskset', '-c', str(cpus[0]), str(binary), '--self-test'], check=True)
    slots = queue.Queue()
    for cpu_id in cpus: slots.put(cpu_id)
    def run(case):
        selected = slots.get()
        try:
            os.sched_setaffinity(0, {selected})
            path = Path(case['input']); assert digest(path) == case['input_sha256']
            assert digest(Path(case['report'])) == case['report_sha256']
            output = raw / (case['name'] + '.json')
            subprocess.run([str(binary), str(support / 'warehouse.map'), str(path), str(output), str(int(case['learned']))], check=True)
            data = read(output)
            assert data['team_size'] == 10000 and data['steps'] == 5000 and data['tasks'] == case['tasks']
            assert data['actions'] == case['actions'] and sum(data['actions'].values()) == 50000000
            assert data['topology']['straight_degree2_cells'] == 24360 and data['topology']['incident_edges'] == 34120
            assert data['fixtures_passed'] == 7 and not data['planner_errors'] and not data['schedule_errors']
            assert data['entry_timeouts'] == (1 if case['name'] == 'nms_diagnostic' else 0)
            if case['learned']:
                logged = []
                with path.with_suffix('.log').open() as stream:
                    for line in stream:
                        if line.startswith('[cgar-flow] '):
                            logged.append({k: int(v) for k, v in re.findall(r'(\w+)=(-?\d+)', line)})
                assert len(logged) == len(data['publications']) == 10
                for actual, expected in zip(data['publications'], logged):
                    for key in ['step', 'moves', 'samples', 'penalized_edges', 'publications']:
                        assert actual[key] == expected[key], (case['name'], key, actual, expected)
                data['all_production_publication_counters_exact'] = True
            data['input_sha256'] = case['input_sha256']; data['bound_cpu'] = selected
            write(output, data); print('COHERENCE_CASE_COMPLETE ' + case['name'], flush=True)
            return case['name'], data
        finally: slots.put(selected)
    with concurrent.futures.ThreadPoolExecutor(max_workers=3) as pool:
        cases = dict(pool.map(run, spec['cases']))
    out = Path(spec['result_directory']); out.mkdir(exist_ok=False)
    for name in cases: shutil.copy2(raw / (name + '.json'), out / (name + '.json'))
    for name in ['spec.json', 'allocation.json', 'submission.json']:
        shutil.copy2(raw / name, out / name)
    write(out / 'verification.json', {'checked_utc': datetime.datetime.now(datetime.timezone.utc).isoformat(),
          'cases': list(cases), 'robot_steps': 400000000, 'fixtures_passed': 7,
          'all_saved_input_hashes_exact': True, 'all_action_totals_exact_to_independent_accounting': True,
          'all_60_generic_publication_frames_exact': True, 'all_vertex_and_edge_actions_replayed_valid': True,
          'binary_sha256': digest(binary), 'compile_command': command,
          'limits': 'Read-only comparison; no causal or achieved throughput improvement is established.'})
    for path, sha in spec['files'].items(): assert digest(Path(path)) == sha, path
    print('FLOW_COHERENCE_COMPLETE', flush=True)
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
