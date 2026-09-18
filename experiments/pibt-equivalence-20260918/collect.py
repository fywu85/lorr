#!/usr/bin/env python3
"""Retain compact PIBT study evidence; full trajectories and binaries stay in runs/."""
import argparse
import hashlib
import json
from pathlib import Path
import shutil

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]


def read(path):
    return json.loads(path.read_text())


def write(path, value):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(value, indent=2) + '\n')


def copy_files(source, target, names):
    target.mkdir(parents=True, exist_ok=True)
    for name in names:
        shutil.copy2(source / name, target / name)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    defaults = {'build': 'initial-build', 'small': 'equivalence', 'snapshots': 'snapshot-equivalence',
                'screen': 'screen', 'full': 'full', 'controls': 'controls'}
    for name, suffix in defaults.items():
        parser.add_argument('--' + name, type=Path, default=ROOT / ('runs/cgar-pibt-' + suffix + '-20260918'))
    args = parser.parse_args()
    out = HERE / 'results'
    build = read(args.build / 'build.json')
    for name, expected in dict(build['sources'], **build['test_sources']).items():
        assert hashlib.sha256((ROOT / name).read_bytes()).hexdigest() == expected, name
    copy_files(args.build, out / 'build-provenance', ['build.json', 'requested.json', 'allocation.json', 'submission.json'])
    log = (args.build / 'build.log').read_text()
    assert 'All CGAR regression checks passed' in log
    write(out / 'build-provenance/regression.json', {'passed': True, 'checks': [line for line in log.splitlines()
          if 'passed' in line or 'explicit_timeout=1' in line]})
    for name in ['small', 'snapshots']:
        source = getattr(args, name)
        rows = read(source / 'results.json')
        assert all(r['identical'] and all(r[t]['exit'] == 0 and r[t]['valid']
                   for t in ['pibt_reference', 'cgar_reference']) for r in rows)
        copy_files(source, out / name, ['results.json', 'completion.json', 'build.json',
                                       'requested.json', 'allocation.json', 'submission.json'])
    for name in ['full', 'controls']:
        source = getattr(args, name)
        assert read(source / 'completion.json')['returncode'] == 0
        # Large-trajectory analysis must have completed on GRID first.
        assert (out / name / 'metrics.json').is_file()
        assert read(source / 'spec.json') == read(out / name / 'spec.json')
        copy_files(source, out / name, ['build.json', 'completion.json', 'submission.json',
                                       'motion-analysis-submission.json'])
    source = args.screen
    copy_files(source, out / 'screen', ['build.json', 'completion.json', 'submission.json', 'spec.json', 'allocation.json'])
    summaries, metadata, failures = {}, {}, []
    for case in read(source / 'spec.json')['cases']:
        name = case['name']
        summaries[name] = read(source / name / 'summary.json')
        metadata[name] = read(source / name / 'metadata.json')
        for row in summaries[name]:
            if not row['valid']:
                lines = (source / name / (row['instance'] + '.log')).read_text().splitlines()
                failures.append({'case': name, 'instance': row['instance'], 'exit': row['exit'],
                                 'outcome': row['outcome'], 'timeout_lines': [x for x in lines if 'CGAR_TIMEOUT' in x]})
    write(out / 'screen/run-summaries.json', summaries)
    write(out / 'screen/run-metadata.json', metadata)
    write(out / 'screen/failures.json', failures)
    previous_path = ROOT / 'experiments/motion-20260918/results/admission/metrics.json'
    previous = {(r['case'], r['instance']): r for r in read(previous_path)}
    comparisons = []
    for row in read(out / 'full/metrics.json'):
        if not row['case'].startswith('baseline-'):
            continue
        old = previous[row['case'], row['instance']]
        comparisons.append({'case': row['case'], 'instance': row['instance'],
                            'previous': old['trajectory_sha256'], 'current': row['trajectory_sha256'],
                            'identical': old['trajectory_sha256'] == row['trajectory_sha256'],
                            'tasks': row['tasks'], 'steps': row['steps']})
    assert len(comparisons) == 30 and all(r['identical'] for r in comparisons)
    write(out / 'baseline-equivalence.json', {'previous_metrics': str(previous_path.relative_to(ROOT)),
          'previous_metrics_sha256': hashlib.sha256(previous_path.read_bytes()).hexdigest(), 'comparisons': comparisons})
    upstream = read(HERE / 'upstream.json')
    for name, expected in upstream['files'].items():
        assert hashlib.sha256((HERE / 'vendor/pibt2' / name).read_bytes()).hexdigest() == expected, name
    print('Packaged conformance, strict timeouts, 300 full runs, and unchanged baseline evidence.')


if __name__ == '__main__':
    main()
