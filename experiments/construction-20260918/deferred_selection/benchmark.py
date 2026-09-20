#!/usr/bin/env python3
"""Isolated exact-work probe: defer rejected TemporalPibt selection stores."""
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

HERE = Path(__file__).resolve().parent
DEFAULT_ROOT = HERE.parents[2]

def write(path, value):
    path.write_text(json.dumps(value, indent=2) + '\n')

def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--execute', action='store_true')
    args = parser.parse_args()
    out = args.output.resolve()
    if not args.execute:
        root = DEFAULT_ROOT
        out.mkdir(parents=True, exist_ok=False)
        commit = subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=root, text=True).strip()
        files = {'driver.cpp': HERE / 'benchmark.cpp', 'candidate.patch': HERE / 'candidate.patch',
                 'benchmark.py': HERE / 'benchmark.py', 'cpu_resources.py': root / 'tools/cpu_resources.py'}
        for name, source in files.items():
            shutil.copy2(source, out / name)
        source_hashes = {}
        for label in ['baseline', 'candidate']:
            folder = out / label
            folder.mkdir()
            for name in ['temporal_pibt.hpp', 'temporal_geometry.hpp']:
                (folder / name).write_bytes(subprocess.check_output(
                    ['git', 'show', commit + ':cgar/cgar_planner/' + name], cwd=root))
            if label == 'candidate':
                subprocess.run(['git', 'apply', '--unsafe-paths', str(out / 'candidate.patch')], cwd=folder, check=True)
            source_hashes[label] = {p.name: sha(p) for p in sorted(folder.glob('*.hpp'))}
        write(out / 'spec.json', {'root': str(root), 'source_commit': commit,
              'source_hashes': source_hashes, 'support_hashes': {name: sha(out/name) for name in files},
              'cpu_model': 'AMD EPYC 9354 32-Core Processor', 'repetitions': 8,
              'fixture_seeds': [81473, 2749], 'modes': [0, 1, 2, 3],
              'scope': 'Shared-host paired single-core component timing with exact selected-plan/score/work checks. Not warehouse quality or whole-entry speedup evidence.'})
        command = [sys.executable, str(out / 'benchmark.py'), '--execute', '--output', str(out)]
        (out / 'job.sh').write_text('#!/bin/bash\nset -eu\nexec ' + ' '.join(shlex.quote(x) for x in command) + '\n')
        submit = ['qsub', '-terse', '-w', 'e', '-cwd', '-q', ','.join(
            'debian.q@research%d.grid.gsb' % h for h in [44,46,50,52,57]),
            '-pe', 'threaded', '1', '-binding', 'linear:1', '-l',
            'exclusive=false,h_rt=00:15:00,h_vmem=4G', '-m', 'n', '-N', 'cgar_store_probe',
            '-j', 'y', '-o', str(out / 'job.log'), '-S', '/bin/bash', str(out / 'job.sh')]
        result = subprocess.run(submit, cwd=root, text=True, capture_output=True)
        write(out / 'submission.json', {'command': submit, 'returncode': result.returncode,
              'stdout': result.stdout, 'stderr': result.stderr})
        print(result.stdout, end='')
        return result.returncode
    spec = json.loads((out / 'spec.json').read_text())
    root = Path(spec['root'])
    for name, digest in spec['support_hashes'].items():
        assert sha(out / name) == digest, name
    sys.path.insert(0, str(out))
    from cpu_resources import cpu_resources
    resources = cpu_resources()
    assert resources['cpu_model'] == spec['cpu_model'], resources
    assert resources['physical_cores_visible'] == 1, resources
    assert resources['effective_cpu_quota'] is None or resources['effective_cpu_quota'] >= 1, resources
    cpu = resources['representative_cpus'][0]
    os.sched_setaffinity(0, {cpu})
    write(out / 'allocation.json', {'resources': resources, 'selected_cpu': cpu})
    compiler = root / 'env/bin/x86_64-conda-linux-gnu-c++'
    for label in ['baseline', 'candidate']:
        folder = out / label
        for name, digest in spec['source_hashes'][label].items():
            assert sha(folder / name) == digest, name
        command = [str(compiler), '-O3', '-DNDEBUG', '-std=c++17', '-march=nocona', '-mtune=haswell',
                   '-I' + str(folder), str(out / 'driver.cpp'), '-o', str(folder / 'benchmark')]
        subprocess.run(command, cwd=root, check=True)
        write(out / (label + '-binary.json'), {'command': command, 'sha256': sha(folder / 'benchmark')})
    observations = []
    for repeat in range(spec['repetitions']):
        for seed in spec['fixture_seeds']:
            for mode in spec['modes']:
                for label in (['baseline', 'candidate'] if repeat % 2 == 0 else ['candidate', 'baseline']):
                    result = subprocess.run([str(out / label / 'benchmark'), str(mode), str(seed)],
                                            cwd=root, text=True, capture_output=True, check=True)
                    row = json.loads(result.stdout)
                    assert row['mode'] == mode and row['fixture_seed'] == seed
                    row.update(repeat=repeat, implementation=label)
                    observations.append(row)
                    write(out / 'observations.json', observations)
    summary = {}
    for seed in spec['fixture_seeds']:
        for mode in spec['modes']:
            rows = [r for r in observations if r['mode'] == mode and r['fixture_seed'] == seed]
            for key in ['checksum','candidates','recursion','repairs']:
                assert len({r[key] for r in rows}) == 1, (seed,mode,key)
            measures = {}
            for metric in ['kernel_seconds','kernel_cpu_seconds']:
                times = {label: [r[metric] for r in rows if r['implementation'] == label]
                         for label in ['baseline','candidate']}
                ratios = [next(r[metric] for r in rows if r['repeat']==i and r['implementation']=='candidate') /
                          next(r[metric] for r in rows if r['repeat']==i and r['implementation']=='baseline')
                          for i in range(spec['repetitions'])]
                measures[metric] = {'times':times, 'paired_ratios':ratios,
                    'median_paired_candidate_over_baseline':statistics.median(ratios)}
            summary['%d/mode%d'%(seed,mode)] = {'checksum':rows[0]['checksum'],
                'candidates':rows[0]['candidates'],'recursion':rows[0]['recursion'],
                'repairs':rows[0]['repairs'],'measurements':measures}
    write(out / 'summary.json', summary)
    write(out / 'completion.json', {'exit':0,'observations':len(observations),
          'completed_utc':datetime.datetime.now(datetime.timezone.utc).isoformat()})
    print(json.dumps(summary,indent=2))
    return 0

if __name__ == '__main__':
    raise SystemExit(main())
