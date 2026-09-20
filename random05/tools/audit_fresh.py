#!/usr/bin/env python3
"""Verify the predeclared six-run validation and summarize matched comparisons."""
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


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--batch', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    frozen = json.loads(subprocess.check_output(
        ['git', 'show', PROTOCOL_COMMIT + ':random05/experiments/fresh-validation-full.json'], cwd=ROOT))
    generation = read(ROOT / 'random05/results/fresh-validation-v1/generation.json')
    records = {str(Path(r['input']).parent): r for r in generation['records']}
    reference = read(ROOT / 'random05/results/nms4-repeats-full-v3/summary.json')[0]['binary_sha256']
    candidate = read(ROOT / 'random05/results/guidance-local-validation-split-full-v31/flips1-seed5-four/summary.json')[0]['binary_sha256']
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
        assert allocation['cpu_model'] == 'AMD EPYC 9354 32-Core Processor', name
        assert allocation['physical_cores_visible'] == case['cores'] == 4, name
        assert case['smt'] == 1 and case['steps'] == 2000 and case['limit_ms'] == 1000, name
        quota = allocation['effective_cpu_quota']
        assert quota is None or quota >= 4, name
        generated = records[str(Path(case['input']).parent)]
        for filename, digest in generated['hashes'].items():
            path = Path(case['input']).parent / filename
            assert case['input_hashes'][str(path)] == digest, (name, filename)
            assert hashlib.sha256(path.read_bytes()).hexdigest() == digest, (name, filename)
        summary = read(directory / 'summary.json')[0]
        expected_binary = candidate if name.endswith('-ours') else reference
        assert case['binary_sha256'] == summary['binary_sha256'] == expected_binary, name
        assert summary['valid'] and summary['exit'] == 0, name
        assert summary['result']['makespan'] == 2000, name
        for key in ('numPlannerErrors', 'numScheduleErrors', 'numEntryTimeouts'):
            assert summary['result'][key] == 0, (name, key)
        assert summary['usage']['peak_rss_kib'] * 1024 < 32000000000, name
        assert summary['latency_seconds']['max'] < 1, name
        runs[name] = dict(tasks=summary['result']['numTaskFinished'],
                          finished_utc=summary['finished_utc'], binary_sha256=expected_binary,
                          latency_seconds=summary['latency_seconds'], usage=summary['usage'],
                          evidence=str(directory / 'summary.json'))
    comparisons = []
    for generated in generation['records']:
        prefix = 'seed' + str(generated['seed'])
        ours = runs[prefix + '-ours']['tasks']
        nms = [runs[prefix + '-nms-repeat' + str(k)]['tasks'] for k in (1, 2)]
        comparisons.append(dict(seed=generated['seed'], ours=ours, nms_repeats=nms,
                                stronger_nms=max(nms), gain_percent=(ours / max(nms) - 1) * 100))
    ratio = sum(c['ours'] for c in comparisons) / sum(c['stronger_nms'] for c in comparisons)
    result = dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                  protocol_commit=PROTOCOL_COMMIT, candidate_source_commit='b824f5d',
                  all_valid=True, comparisons=comparisons,
                  aggregate_gain_percent=(ratio-1)*100, runs=runs,
                  caveat='Two held-out task/start instances on the same map. These are not the colleague\'s private inputs; no 32-worker validation is implied.')
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2) + '\n')
    for c in comparisons:
        print('seed{}: {} vs {}, {:+.2f}%'.format(c['seed'], c['ours'], c['nms_repeats'], c['gain_percent']))
    print('Aggregate gain versus stronger NMS repetitions: {:+.2f}%'.format(result['aggregate_gain_percent']))


if __name__ == '__main__':
    main()
