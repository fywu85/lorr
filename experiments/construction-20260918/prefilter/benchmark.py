#!/usr/bin/env python3
"""Time an isolated candidate-list prefilter, including setup and destruction."""
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

ROOT = Path(__file__).resolve().parents[3]
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(ROOT / 'tools'))
from cpu_resources import cpu_resources


def write(path, value):
    path.write_text(json.dumps(value, indent=2) + '\n')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--execute', action='store_true')
    parser.add_argument('--patch', type=Path, default=HERE / 'candidate.patch')
    args = parser.parse_args()
    out = args.output.resolve()
    if not args.execute:
        out.mkdir(parents=True, exist_ok=False)
        commit = subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=ROOT, text=True).strip()
        shutil.copy2(HERE / 'benchmark.cpp', out / 'driver.cpp')
        shutil.copy2(args.patch.resolve(), out / 'candidate.patch')
        source_hashes = {}
        for label in ['baseline', 'candidate']:
            folder = out / label
            folder.mkdir()
            for name in ['temporal_pibt.hpp', 'temporal_geometry.hpp']:
                contents = subprocess.check_output(['git', 'show', commit + ':cgar/cgar_planner/' + name], cwd=ROOT)
                (folder / name).write_bytes(contents)
            if label == 'candidate':
                subprocess.run(['git', 'apply', '--unsafe-paths', str(out / 'candidate.patch')], cwd=folder, check=True)
            source_hashes[label] = {p.name: hashlib.sha256(p.read_bytes()).hexdigest() for p in folder.glob('*.hpp')}
        write(out / 'spec.json', {'source_commit': commit, 'source_hashes': source_hashes,
            'driver_sha256': hashlib.sha256((out / 'driver.cpp').read_bytes()).hexdigest(),
            'patch_sha256': hashlib.sha256((out / 'candidate.patch').read_bytes()).hexdigest(),
            'cpu_model': 'AMD EPYC 9354 32-Core Processor', 'repetitions': 6,
            'modes': {'0': 'cold equal-weight global search', '1': 'region-filtered search',
                      '2': 'valid warm initial plan and unequal weights'},
            'purpose': 'Synthetic fixed-work kernel equivalence and runtime, including indexing setup/free; not lifelong throughput.'})
        command = [sys.executable, str(Path(__file__).resolve()), '--execute', '--output', str(out)]
        (out / 'job.sh').write_text('#!/bin/bash\nset -eu\nexec ' + ' '.join(shlex.quote(x) for x in command) + '\n')
        hosts = [31, 38, 44, 46, 50, 52, 57]
        submit = ['qsub', '-terse', '-w', 'e', '-cwd', '-q',
            ','.join('debian.q@research%d.grid.gsb' % h for h in hosts),
            '-pe', 'threaded', '1', '-binding', 'linear:1', '-l',
            'exclusive=true,h_rt=00:15:00,h_vmem=4G', '-m', 'n', '-N', 'prefilter_probe',
            '-j', 'y', '-o', str(out / 'job.log'), '-S', '/bin/bash', str(out / 'job.sh')]
        result = subprocess.run(submit, cwd=ROOT, text=True, capture_output=True)
        write(out / 'submission.json', {'command': submit, 'returncode': result.returncode,
                                       'stdout': result.stdout, 'stderr': result.stderr})
        print(result.stdout, end='')
        return result.returncode
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
        folder = out / label
        for name, digest in spec['source_hashes'][label].items():
            assert hashlib.sha256((folder / name).read_bytes()).hexdigest() == digest, name
        command = [str(compiler), '-O3', '-DNDEBUG', '-std=c++17', '-march=nocona', '-mtune=haswell',
                   '-I' + str(folder), str(out / 'driver.cpp'), '-o', str(folder / 'benchmark')]
        subprocess.run(command, cwd=ROOT, check=True)
        write(out / (label + '-binary.json'), {'command': command,
              'sha256': hashlib.sha256((folder / 'benchmark').read_bytes()).hexdigest()})
    observations = []
    for repeat in range(spec['repetitions']):
        for mode in range(3):
            for label in (['baseline', 'candidate'] if repeat % 2 == 0 else ['candidate', 'baseline']):
                result = subprocess.run([str(out / label / 'benchmark'), str(mode)], cwd=ROOT,
                                        text=True, capture_output=True, check=True)
                row = json.loads(result.stdout)
                row.update(repeat=repeat, implementation=label)
                observations.append(row)
                write(out / 'observations.json', observations)
    summary = {}
    for mode in range(3):
        rows = [r for r in observations if r['mode'] == mode]
        assert len({r['checksum'] for r in rows}) == 1, 'Selected plans, score bits or search counters changed'
        for key in ['candidates', 'recursion', 'repairs']:
            assert len({r[key] for r in rows}) == 1, key
        times = {label: [r['kernel_seconds'] for r in rows if r['implementation'] == label]
                 for label in ['baseline', 'candidate']}
        summary[mode] = {'checksum': rows[0]['checksum'], 'times': times,
                         'candidates': rows[0]['candidates'], 'recursion': rows[0]['recursion'],
                         'repairs': rows[0]['repairs'],
                         'median_ratio_candidate_over_baseline':
                         statistics.median(times['candidate']) / statistics.median(times['baseline'])}
    write(out / 'summary.json', summary)
    write(out / 'completion.json', {'exit': 0, 'observations': len(observations)})
    print(json.dumps(summary, indent=2))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
