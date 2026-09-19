#!/usr/bin/env python3
"""Process-level failure checks; these one-step runs are not performance scores."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools'))
from cpu_resources import cpu_resources


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--binary', type=Path, required=True)
    p.add_argument('--output', type=Path, required=True)
    a = p.parse_args()
    a.output.mkdir(parents=True, exist_ok=False)
    cpu = min(os.sched_getaffinity(0))
    base = {k: v for k, v in os.environ.items() if not k.startswith('CGAR_')}
    base.update(CGAR_TEMPORAL='1', CGAR_ORIENTATION_GUIDANCE='1', CGAR_TEMPORAL_STEPS='32')
    cases = [
        ('missing_guidance', {'CGAR_ORIENTATION_GUIDANCE': '0'}, 30000, 125, 'CGAR_INITIALIZATION_ERROR'),
        ('insufficient_affinity', {'CGAR_TEMPORAL_WORKERS': '2', 'CGAR_TEMPORAL_THREADS': '2'}, 30000, 125, 'CGAR_INITIALIZATION_ERROR'),
        ('preprocess_timeout', {}, 1, 124, None),
        ('valid', {}, 30000, 0, 'Preprocessing success'),
    ]
    results = []
    for name, overrides, preprocess, expected, marker in cases:
        output = a.output / (name + '.json')
        command = ['taskset', '-c', str(cpu), str(a.binary.resolve()),
                   '-i', str(ROOT / 'mr24/warehouse.domain/WAREHOUSE.json'), '-o', str(output.resolve()),
                   '-s', '1', '-t', '1000', '-p', str(preprocess), '-d', '1']
        run = subprocess.run(command, cwd=ROOT / 'cgar', env=dict(base, **overrides),
                             capture_output=True, text=True, timeout=40)
        text = run.stdout + run.stderr
        (a.output / (name + '.log')).write_text(text)
        assert run.returncode == expected, (name, run.returncode, text[-2000:])
        if marker:
            assert marker in text, name
        if expected:
            assert not output.exists() and 'Preprocessing success' not in text, name
        else:
            data = json.loads(output.read_text())
            assert data['makespan'] == 1 and data['entryComputeSamples'] == 1
            assert 0 <= data['entryComputeMaxSeconds'] < 1
            assert all(data[k] == 0 for k in ['numPlannerErrors', 'numScheduleErrors', 'numEntryTimeouts'])
        results.append({'case': name, 'expected_exit': expected, 'exit': run.returncode,
                        'output_exists': output.exists(), 'passed': True})
    report = {'binary_sha256': hashlib.sha256(a.binary.read_bytes()).hexdigest(),
              'allocation': cpu_resources(), 'bound_cpu': cpu, 'results': results}
    (a.output / 'checks.json').write_text(json.dumps(report, indent=2) + '\n')
    print('INITIALIZATION_PROCESS_CHECKS passed cases=' + str(len(results)))


if __name__ == '__main__':
    main()
