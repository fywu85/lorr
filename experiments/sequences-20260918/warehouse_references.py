#!/usr/bin/env python3
"""Run frozen KittyKnight/NMS on full WAREHOUSE with competition CPU allocation."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import shlex
import shutil
import subprocess
import sys
import time

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from benchmark_cgar import FIELDS, header
from benchmark_gridengine import write
from cpu_resources import cpu_resources


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--output', type=Path, required=True)
    p.add_argument('--reference', type=Path, help='Reuse a frozen reference package; otherwise freeze current kk/nms builds')
    p.add_argument('--execute', action='store_true')
    a = p.parse_args(); out = a.output.resolve()
    if not a.execute:
        out.mkdir(parents=True, exist_ok=False)
        if a.reference:
            old = json.loads((a.reference / 'spec.json').read_text())
            cases = [dict(c, steps=5000) for c in old['cases'] if c['instance'] == 'WAREHOUSE']
            assert len(cases) == 2
            for team in ['kk', 'nms']:
                binary = a.reference / (team + '-lifelong')
                assert hashlib.sha256(binary.read_bytes()).hexdigest() == old['binary_sha256'][team]
                shutil.copy2(binary, out / binary.name)
            spec = dict(old, cases=cases, reference_assets=str((a.reference / 'assets').resolve()),
                        frozen_reference_spec=str((a.reference / 'spec.json').resolve()))
        else:
            inputs = {p.stem: str(p.resolve()) for p in (ROOT / 'mr24').glob('*/*.json')}
            hashes, binaries = {}, {}
            for team in ['kk', 'nms']:
                project = (ROOT / team).resolve()
                if not (project / 'build/lifelong').is_file():
                    raise FileNotFoundError('Configure and build ' + team + '/build/lifelong first')
                target = out / 'assets' / team
                shutil.copytree(project, target, ignore=shutil.ignore_patterns('build', '.git', '__pycache__', '*.log', 'printer.txt'))
                binary = out / (team + '-lifelong'); shutil.copy2(project / 'build/lifelong', binary)
                binaries[team] = hashlib.sha256(binary.read_bytes()).hexdigest()
                hashes[team] = {str(p.relative_to(target)): hashlib.sha256(p.read_bytes()).hexdigest()
                                for p in target.rglob('*') if p.is_file()}
            cases = [dict(name=team+'-WAREHOUSE', team=team, instance='WAREHOUSE', input=inputs['WAREHOUSE'], steps=5000)
                     for team in ['kk', 'nms']]
            spec = dict(cases=cases, binary_sha256=binaries, source_hashes=hashes,
                        reference_assets=str((out / 'assets').resolve()), time_limit_ms=1000,
                        preprocess_limit_ms=1800000, logical_cpus_per_case=32, physical_cores_per_case=16,
                        memory_gib_per_case=128,
                        source_commit=subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=ROOT, text=True).strip(),
                        note='Archived leader policies retained; CGAR uses generic policies and a separate 32 GB target.')
        write(out / 'spec.json', spec)
        command = [sys.executable, str(Path(__file__).resolve()), '--execute', '--output', str(out)]
        script = out / 'job.sh'; script.write_text('#!/bin/bash\nset -eu\nexec ' + ' '.join(shlex.quote(s) for s in command) + '\n')
        submit = ['/opt/n1ge/bin/lx24-amd64/qsub', '-terse', '-w', 'e', '-cwd', '-q', 'debian.q', '-t', '1-2', '-tc', '2',
                  '-pe', 'threaded', '32', '-binding', 'linear:16', '-l', 'exclusive=true,h_rt=03:00:00,h_vmem=4G',
                  '-m', 'n', '-N', 'warehouse_references', '-j', 'y', '-o', str(out), '-S', '/bin/bash', str(script)]
        run = subprocess.run(submit, cwd=ROOT, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        write(out / 'submission.json', {'command': submit, 'response': run.stdout, 'exit': run.returncode})
        print(run.stdout, end=''); return run.returncode
    spec = json.loads((out / 'spec.json').read_text())
    case = spec['cases'][int(os.environ['SGE_TASK_ID']) - 1]
    work = out / case['name']; work.mkdir()
    source = Path(spec['reference_assets']) / case['team']
    for name, digest in spec['source_hashes'][case['team']].items():
        assert hashlib.sha256((source / name).read_bytes()).hexdigest() == digest, name
    shutil.copytree(source, work / 'cwd')
    resources = cpu_resources()
    assert len(resources['logical_cpu_affinity']) == 32 and resources['physical_cores_visible'] == 16
    assert resources['effective_cpu_quota'] is None or resources['effective_cpu_quota'] >= 32
    write(work / 'allocation.json', {'started_utc': datetime.datetime.now(datetime.timezone.utc).isoformat(),
                                    'resources': resources, 'job_id': os.environ['JOB_ID']})
    env = {k: v for k, v in os.environ.items() if not k.startswith('CGAR_') and k not in ['CONFIG_PATH', 'MAP_WEIGHT_PATH', 'LNS_NUM_THREADS']}
    env.update(OMP_NUM_THREADS='32', OMP_DYNAMIC='FALSE')
    command = [sys.executable, str(ROOT / 'tools/run_with_usage.py'), '--output', str(work / 'usage.json'), '--',
               str(out / (case['team'] + '-lifelong')), '-i', case['input'], '-o', str(work / 'result.json'),
               '-s', '5000', '-t', '1000', '-p', '1800000']
    start = time.monotonic()
    with (work / 'native.log').open('w') as log:
        run = subprocess.run(command, cwd=work / 'cwd', env=env, stdout=log, stderr=subprocess.STDOUT)
    data = header(work / 'result.json') if (work / 'result.json').exists() else dict.fromkeys(FIELDS)
    valid = run.returncode == 0 and data['makespan'] == 5000 and all(data[k] == 0 for k in FIELDS[3:])
    write(work / 'summary.json', dict(case, exit=run.returncode, valid=valid, result=data,
          wall_seconds=time.monotonic() - start, usage=json.loads((work / 'usage.json').read_text()),
          finished_utc=datetime.datetime.now(datetime.timezone.utc).isoformat()))
    return 0 if valid else 1


if __name__ == '__main__':
    raise SystemExit(main())
