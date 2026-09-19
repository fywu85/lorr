#!/usr/bin/env python3
"""Compare frozen oracle implementations on one reserved physical CPU."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import shlex
import shutil
import statistics
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools'))
from cpu_resources import cpu_resources


def write(path, value):
    path.write_text(json.dumps(value, indent=2) + '\n')


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--baseline', type=Path)
    p.add_argument('--candidate', type=Path)
    p.add_argument('--output', type=Path, required=True)
    p.add_argument('--execute', action='store_true')
    a = p.parse_args()
    out = a.output.resolve()
    if not a.execute:
        if not a.baseline or not a.candidate:
            p.error('baseline and candidate build directories are required')
        out.mkdir(parents=True, exist_ok=False)
        shutil.copy2(Path(__file__).with_suffix('.cpp'), out / 'driver.cpp')
        sources = {}
        for label, build in [('baseline', a.baseline.resolve()), ('candidate', a.candidate.resolve())]:
            spec = json.loads((build / 'requested.json').read_text())
            assert (build / 'build.json').is_file(), 'regression build must complete first'
            for name, digest in spec['sources'].items():
                source = build / 'sources' / name
                assert hashlib.sha256(source.read_bytes()).hexdigest() == digest, name
                dest = out / label / name
                dest.parent.mkdir(parents=True, exist_ok=True)
                shutil.copy2(source, dest)
            sources[label] = spec
        write(out / 'spec.json', {
            'builds': sources, 'driver_sha256': hashlib.sha256((out / 'driver.cpp').read_bytes()).hexdigest(),
            'map': 'mr24/warehouse.domain/maps/warehouse_large.map',
            'purpose': 'Complete distance-table runtime only; not planner throughput.',
            'cpu_model': 'AMD EPYC 9354 32-Core Processor', 'repetitions': 6,
        })
        cmd = [sys.executable, str(Path(__file__).resolve()), '--execute', '--output', str(out)]
        (out / 'job.sh').write_text('#!/bin/bash\nset -eu\nexec ' + ' '.join(shlex.quote(x) for x in cmd) + '\n')
        hosts = [31, 38, 44, 46, 50, 52, 57]
        submit = ['/opt/n1ge/bin/lx24-amd64/qsub', '-terse', '-w', 'e', '-cwd', '-q',
                  ','.join('debian.q@research%d.grid.gsb' % h for h in hosts),
                  '-pe', 'threaded', '1', '-binding', 'linear:1', '-l',
                  'exclusive=true,h_rt=00:15:00,h_vmem=4G', '-m', 'n', '-N', 'oracle_timing',
                  '-j', 'y', '-o', str(out / 'job.log'), '-S', '/bin/bash', str(out / 'job.sh')]
        r = subprocess.run(submit, cwd=ROOT, text=True, capture_output=True)
        write(out / 'submission.json', {'command': submit, 'returncode': r.returncode,
                                       'stdout': r.stdout, 'stderr': r.stderr})
        print(r.stdout, end='')
        return r.returncode
    spec = json.loads((out / 'spec.json').read_text())
    resources = cpu_resources()
    assert resources['cpu_model'] == spec['cpu_model'], resources
    assert resources['effective_cpu_quota'] is None or resources['effective_cpu_quota'] >= 1
    cpu = resources['representative_cpus'][0]
    os.sched_setaffinity(0, {cpu})
    write(out / 'allocation.json', {'resources': resources, 'selected_cpu': cpu})
    assert hashlib.sha256((out / 'driver.cpp').read_bytes()).hexdigest() == spec['driver_sha256']
    compiler = ROOT / 'env/bin/x86_64-conda-linux-gnu-c++'
    for label in ['baseline', 'candidate']:
        source = out / label / 'cgar/cgar_planner'
        for name, digest in spec['builds'][label]['sources'].items():
            assert hashlib.sha256((out / label / name).read_bytes()).hexdigest() == digest, name
        command = [str(compiler), '-O3', '-DNDEBUG', '-std=c++17', '-pthread',
                   '-march=nocona', '-mtune=haswell', '-I' + str(source),
                   '-I' + str(ROOT / 'cgar/inc'), '-isystem', str(ROOT / 'env/include'),
                   str(out / 'driver.cpp'), str(source / 'cgar.cpp'), '-o', str(out / label / 'benchmark')]
        subprocess.run(command, cwd=ROOT, check=True)
        write(out / (label + '-binary.json'), {'command': command,
              'sha256': hashlib.sha256((out / label / 'benchmark').read_bytes()).hexdigest()})
    observations = []
    for repeat in range(spec['repetitions']):
        for mode in range(3):
            for label in (['baseline', 'candidate'] if repeat % 2 == 0 else ['candidate', 'baseline']):
                r = subprocess.run([str(out / label / 'benchmark'), str(ROOT / spec['map']), str(mode)],
                                   cwd=ROOT, check=True, text=True, capture_output=True)
                row = json.loads(r.stdout)
                row.update(repeat=repeat, implementation=label)
                observations.append(row)
                write(out / 'observations.json', observations)
    summaries = {}
    for mode in range(3):
        rows = [r for r in observations if r['mode'] == mode]
        assert len({r['checksum'] for r in rows}) == 1, 'distance table contents changed'
        times = {label: [r['table_seconds'] for r in rows if r['implementation'] == label]
                 for label in ['baseline', 'candidate']}
        summaries[mode] = {'checksum': rows[0]['checksum'], 'times': times,
                          'median_ratio_candidate_over_baseline':
                          statistics.median(times['candidate']) / statistics.median(times['baseline'])}
    write(out / 'summary.json', summaries)
    write(out / 'completion.json', {'exit': 0, 'observations': len(observations)})
    print(json.dumps(summaries, indent=2))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
