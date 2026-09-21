#!/usr/bin/env python3
"""Verify a frozen matched validation with optional prior-solver comparisons."""
import argparse
import datetime
import hashlib
import json
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[2]
PROTOCOL_COMMIT = '22e7cd1'


def read(path):
    return json.loads(path.read_text())


def verify_allocation(case, resources, declared, expected_steps=2000):
    """Use the frozen protocol allocation; old protocols retain four-core rules."""
    cores = declared.get('physical_cores', 4)
    smt = declared.get('smt', 1)
    workers = declared.get('workers', cores * smt)
    assert cores > 0 and smt in (1, 2) and workers == cores * smt
    assert resources['cpu_model'] == declared.get('cpu_model', 'AMD EPYC 9354 32-Core Processor')
    assert resources['physical_cores_visible'] == case['cores'] == cores
    assert case['smt'] == smt and case['steps'] == expected_steps and case['limit_ms'] == 1000
    assert case.get('preprocess_ms', 30000) == 30000
    quota = resources['effective_cpu_quota']
    assert quota is None or quota >= workers
    if case.get('team') != 'nms':
        assert int(case['env']['R05_THREADS']) == workers
    return dict(physical_cores=cores, smt=smt, workers=workers,
                cpu_model=resources['cpu_model'])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--batch', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--protocol-commit', default=PROTOCOL_COMMIT)
    parser.add_argument('--protocol-json', help='Repository-relative JSON protocol frozen at protocol-commit')
    args = parser.parse_args()
    def frozen_json(path):
        return json.loads(subprocess.check_output(
            ['git', 'show', args.protocol_commit + ':' + path], cwd=ROOT))
    protocol = {}
    if args.protocol_json:
        protocol = frozen_json(args.protocol_json)
        candidate_source = protocol['candidate_source_commit']
        candidate = protocol['candidate_binary_sha256']
        reference = protocol['reference_binary_sha256']
        frozen = frozen_json(protocol['manifest'])
        generation = read(ROOT / protocol['generation'])
        assert generation['created_utc'] > protocol['declared_utc'], 'inputs generated before declaration'
        assert [r['seed'] for r in generation['records']] == protocol['seeds'], 'changed validation seeds'
        committed = subprocess.check_output(
            ['git', 'show', '-s', '--format=%cI', args.protocol_commit], cwd=ROOT, text=True).strip()
        assert datetime.datetime.fromisoformat(generation['created_utc']) > datetime.datetime.fromisoformat(committed), 'inputs generated before protocol commit'
    else:
        frozen = frozen_json('random05/experiments/fresh-validation-full.json')
        generation = read(ROOT / 'random05/results/fresh-validation-v1/generation.json')
        reference = read(ROOT / 'random05/results/nms4-repeats-full-v3/summary.json')[0]['binary_sha256']
        candidate = read(ROOT / 'random05/results/guidance-local-validation-split-full-v31/flips1-seed5-four/summary.json')[0]['binary_sha256']
        candidate_source = 'b824f5d'
    instance = protocol.get('instance', 'RANDOM-05')
    expected_steps = {'RANDOM-01':600, 'RANDOM-02':600, 'RANDOM-03':800,
                      'RANDOM-04':1000, 'RANDOM-05':2000}[instance]
    assert generation.get('instance', 'RANDOM-05') == instance
    assert all(Path(r['input']).stem == instance for r in generation['records'])
    records = {str(Path(r['input']).parent): r for r in generation['records']}
    baseline = protocol.get('baseline_binary_sha256')
    assert bool(baseline) == bool(protocol.get('baseline_source_commit')), 'incomplete baseline provenance'
    suffixes = ('ours', 'nms-repeat1', 'nms-repeat2') + (('baseline',) if baseline else ())
    expected_names = {'seed{}-{}'.format(r['seed'], suffix) for r in generation['records']
                      for suffix in suffixes}
    assert len(frozen) == len(expected_names) and {c['name'] for c in frozen} == expected_names
    source_checks = {}
    solvers = {'ours': (candidate, candidate_source)}
    if baseline:
        solvers['baseline'] = (baseline, protocol['baseline_source_commit'])
    for role, (binary, commit) in solvers.items():
        builds = []
        for path in (ROOT / 'runs/random05').glob('build-*/completion.json'):
            completion = read(path)
            if completion.get('exit') == 0 and completion.get('binary_sha256') == binary:
                builds.append(read(path.parent / 'spec.json'))
        assert builds, (role, 'build provenance not found')
        checked = []
        for source, digest in builds[0]['source_hashes'].items():
            if source == 'CMakeLists.txt' or (source.startswith(('src/', 'simulator/')) and Path(source).suffix in ('.cpp', '.hpp', '.h')):
                content = subprocess.check_output(['git', 'show', commit + ':random05/' + source], cwd=ROOT)
                assert hashlib.sha256(content).hexdigest() == digest, (role, 'source mismatch', source)
                checked.append(source)
        source_checks[role] = dict(commit=commit, binary_sha256=binary, checked_files=checked)
    runs = {}
    for expected in frozen:
        name = expected['name']
        directory = args.batch / name
        spec = read(directory / 'spec.json')
        assert len(spec['cases']) == 1, name
        case = spec['cases'][0]
        for key, value in expected.items():
            assert case[key] == value, (name, 'changed frozen configuration', key)
        allocation = read(directory / 'allocation.json')['resources']
        declared_allocation = verify_allocation(case, allocation, protocol.get('allocation', {}), expected_steps)
        generated = records[str(Path(case['input']).parent)]
        for filename, digest in generated['hashes'].items():
            path = Path(case['input']).parent / filename
            assert case['input_hashes'][str(path)] == digest, (name, filename)
            assert hashlib.sha256(path.read_bytes()).hexdigest() == digest, (name, filename)
        summary = read(directory / 'summary.json')[0]
        expected_binary = candidate if name.endswith('-ours') else baseline if name.endswith('-baseline') else reference
        assert case['binary_sha256'] == summary['binary_sha256'] == expected_binary, name
        assert summary['valid'] and summary['exit'] == 0, name
        assert summary['result']['makespan'] == expected_steps, name
        # The unchanged NMS simulator reports wall time for the full combined
        # entry in plannerTimes; PILOT also exposes entryComputeTimes. Validate
        # the actual per-step series for both, rather than requiring a PILOT-
        # specific summary field from the reference executable.
        raw = read(directory / name / 'result.json')
        timing_field = 'plannerTimes' if case.get('team') == 'nms' else 'entryComputeTimes'
        samples = raw[timing_field]
        assert len(samples) == expected_steps, (name, 'incomplete timing series')
        assert all(0 <= value < 1 for value in samples), (name, 'entry deadline')
        assert max(samples) == summary['latency_seconds']['max'], name
        if case.get('team') != 'nms':
            assert summary['result']['entryComputeSamples'] == expected_steps, name
        for key in ('numPlannerErrors', 'numScheduleErrors', 'numEntryTimeouts'):
            assert summary['result'][key] == 0, (name, key)
        assert summary['usage']['peak_rss_kib'] * 1024 < 32000000000, name
        assert summary['latency_seconds']['max'] < 1, name
        runs[name] = dict(tasks=summary['result']['numTaskFinished'],
                          finished_utc=summary['finished_utc'], binary_sha256=expected_binary,
                          latency_seconds=summary['latency_seconds'], timing_source=timing_field,
                          timing_samples=len(samples), usage=summary['usage'],
                          evidence=str(directory / 'summary.json'))
    comparisons = []
    for generated in generation['records']:
        prefix = 'seed' + str(generated['seed'])
        ours = runs[prefix + '-ours']['tasks']
        nms = [runs[prefix + '-nms-repeat' + str(k)]['tasks'] for k in (1, 2)]
        comparison = dict(seed=generated['seed'], ours=ours, nms_repeats=nms,
                          stronger_nms=max(nms), gain_percent=(ours / max(nms) - 1) * 100)
        if baseline:
            old = runs[prefix + '-baseline']['tasks']
            comparison.update(baseline=old, gain_over_baseline_percent=(ours / old - 1) * 100)
        comparisons.append(comparison)
    ratio = sum(c['ours'] for c in comparisons) / sum(c['stronger_nms'] for c in comparisons)
    result = dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                  protocol_commit=args.protocol_commit, candidate_source_commit=candidate_source,
                  all_valid=True, instance=instance, steps=expected_steps, allocation=declared_allocation, source_checks=source_checks, comparisons=comparisons,
                  aggregate_gain_percent=(ratio-1)*100, runs=runs,
                  caveat='Frozen held-out task/start inputs on the same map and the stated allocation. These are not the colleague\'s private inputs.')
    if baseline:
        result['baseline_source_commit'] = protocol['baseline_source_commit']
        result['aggregate_gain_over_baseline_percent'] = (sum(c['ours'] for c in comparisons) / sum(c['baseline'] for c in comparisons) - 1) * 100
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2) + '\n')
    for c in comparisons:
        print('seed{}: {} vs {}, {:+.2f}%'.format(c['seed'], c['ours'], c['nms_repeats'], c['gain_percent']))
    print('Aggregate gain versus stronger NMS repetitions: {:+.2f}%'.format(result['aggregate_gain_percent']))


if __name__ == '__main__':
    main()
