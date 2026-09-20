#!/usr/bin/env python3
"""Freeze and verify native prewarming activation, resources and default compatibility."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import re
import shlex
import shutil
import subprocess
import sys

ROOT = next(p for p in Path(__file__).resolve().parents if (p/'tools/cpu_resources.py').is_file())
BASE = ROOT/'experiments/construction-20260918'

def read(p):
    return json.loads(p.read_text())

def write(p, data):
    p.write_text(json.dumps(data, indent=2)+'\n')

def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest()

def fields(line):
    return dict(word.split('=', 1) for word in line.split()[1:])

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--raw', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--commit', required=True)
    parser.add_argument('--hold-job')
    parser.add_argument('--execute', action='store_true')
    args = parser.parse_args()
    raw, out = args.raw.resolve(), args.output.resolve()
    support = raw/'prewarm-screen-support'
    if not args.execute:
        support.mkdir(exist_ok=False)
        sources = {'screen.py': Path(__file__), 'collect_cold.py': BASE/'collect_cold.py',
                   'profiles.json': BASE/'static_prewarm/variants.json',
                   'reference.json': BASE/'results/fresh-pickup-native-screen-v105/trajectory-fingerprints.json',
                   'cpu_resources.py': ROOT/'tools/cpu_resources.py'}
        for name, source in sources.items():
            shutil.copy2(source, support/name)
        write(raw/'prewarm-screen-request.json', dict(commit=args.commit, files={name:sha(support/name) for name in sources}))
        command = ['/usr/bin/python3', str(support/'screen.py'), '--raw', str(raw), '--output', str(out),
                   '--commit', args.commit, '--execute']
        job = raw/'prewarm-screen.sh'
        job.write_text('#!/bin/bash\nset -eu\nexec '+' '.join(map(shlex.quote, command))+'\n')
        submit = ['qsub', '-h', '-terse', '-w', 'e', '-cwd', '-q',
                  'debian.q@research44.grid.gsb,debian.q@research57.grid.gsb', '-pe', 'threaded', '1',
                  '-binding', 'linear:1', '-l', 'exclusive=false,h_rt=00:20:00,h_vmem=8G', '-m', 'n',
                  '-N', 'native_prewarm_screen', '-j', 'y', '-o', str(raw/'prewarm-screen.log'), '-S', '/bin/bash']
        if args.hold_job:
            submit += ['-hold_jid', args.hold_job]
        result = subprocess.run(submit+[str(job)], stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        write(raw/'prewarm-screen-submission.json', dict(command=submit+[str(job)], returncode=result.returncode, response=result.stdout))
        result.check_returncode()
        assert re.fullmatch(r'\d+\s*', result.stdout)
        print(result.stdout, end='', flush=True)
        subprocess.run(['qrls', result.stdout.strip()], check=True)
        return
    request = read(raw/'prewarm-screen-request.json')
    assert request['commit'] == args.commit
    for name, expected in request['files'].items():
        assert sha(support/name) == expected, name
    sys.path.insert(0, str(support))
    from cpu_resources import cpu_resources
    cpu = cpu_resources()
    assert cpu['effective_cpu_quota'] is None and cpu['physical_cores_visible'] == 1
    os.sched_setaffinity(0, cpu['representative_cpus'])
    spec, build, allocation = (read(raw/name) for name in ['spec.json', 'build.json', 'allocation.json'])
    assert spec['horizons'] == {'WAREHOUSE':800} and spec['time_limit_ms'] == 5000
    assert spec['cpus_per_instance'] == 8 and spec['trick'] == 'WAREHOUSE'
    for name, expected in dict(build['sources'], **build['test_sources']).items():
        data = subprocess.check_output(['git', 'show', args.commit+':'+name], cwd=str(ROOT))
        assert hashlib.sha256(data).hexdigest() == expected, name
    assert sha(raw/'lifelong') == build['binary_sha256']
    profiles = read(support/'profiles.json')
    seen, intervals, samples, assignments = set(), [], {}, {}
    for case in spec['cases']:
        name, env = case['name'], case['environment']
        assert case['variant'] not in seen and case['seed'] == case['repeat'] == 0
        assert env == profiles[case['variant']]
        seen.add(case['variant'])
        summary, meta = read(raw/name/'summary.json')[0], read(raw/name/'metadata.json')
        assert summary['valid'] and summary['makespan'] == summary['entry_compute_samples'] == 800
        assert summary['entry_compute_max_seconds'] <= 5 and summary['memory_valid']
        assert summary['peak_process_rss_bytes'] < 32000000000
        assert summary['trick_receipt_valid'] and summary['experiment_track_valid']
        assert meta['trick_argv'] == ['--trick', 'WAREHOUSE'] and meta['build_provenance'] == build
        assert meta['max_process_memory_bytes'] == 32000000000
        resources, binding = meta['cpu_resources'], summary['cpu']
        assert resources['effective_cpu_quota'] is None and resources['cpu_model'] == 'AMD EPYC 9354 32-Core Processor'
        assert binding == meta['cpu_binding'] and len(binding) == 8 and set(binding).issubset(allocation['selected_cpus'])
        core_of = {c:tuple(g) for g in resources['logical_cpus_by_physical_core'] for c in g}
        cores = {(resources['hostname'], core_of[c]) for c in binding}
        assert len(cores) == 8
        begin, end = (datetime.datetime.fromisoformat(meta[k]) for k in ['started_utc', 'finished_utc'])
        for x, y, occupied in intervals:
            assert not (begin < y and x < end and cores.intersection(occupied))
        intervals.append((begin, end, cores))
        lines = (raw/name/'WAREHOUSE.log').read_text().splitlines()
        receipts = [fields(s) for s in lines if s.startswith('[CGAR_TRICK_NATIVE_PREWARM] ')]
        threads = int(env['CGAR_TRICK_NATIVE_PREWARM_THREADS'])
        if threads:
            assert len(receipts) == 1
            receipt = dict(receipts[0])
            assert 0 < int(receipt.pop('wall_us')) < 30000000
            assert receipt == dict(complete='1', goals='38586', threads='8', stored_bytes='11911035168',
                                   input='map_only', initial_dispatch='unchanged')
        else:
            assert receipts == []
        quoted = [fields(s) for s in lines if s.startswith('[cgar-native-metric] ')]
        assert [int(m['t']) for m in quoted] == [0,200,400,600]
        assert all(m['turn'] == '1' and m['cost_limit'] == '201' and m['forward_base'] == '20' for m in quoted)
        assert any(s.startswith('[CGAR_TRICK_MATCH_HORIZON] ') for s in lines)
        data = read(raw/name/'WAREHOUSE.json')
        initial = []
        for schedule in data['actualSchedule']:
            pairs = [tuple(map(int, entry.split(':'))) for entry in schedule.split(',') if entry]
            assert pairs and pairs[0][0] == 1
            initial.append(pairs[0][1])
        assignments[name] = initial
        samples[name] = dict(prewarm=receipts, native=quoted)
    assert seen == set(profiles)
    first = assignments['trick_prewarm_base-s0-r0']
    assert len(first) == 10000 and all(values == first for values in assignments.values())
    subprocess.run(['/usr/bin/python3', str(support/'collect_cold.py'), '--input', str(raw), '--output', str(out)], check=True)
    fingerprints = read(out/'trajectory-fingerprints.json')
    assert fingerprints['trick_prewarm_base-s0-r0'] == read(support/'reference.json')['trick_fresh0-s0-r0']
    assert fingerprints['trick_prewarm_all-s0-r0'] != fingerprints['trick_prewarm_base-s0-r0']
    for name in ['prewarm-screen-request.json', 'prewarm-screen-submission.json']:
        shutil.copy2(raw/name, out/name)
    write(out/'prewarm-samples.json', samples)
    write(out/'comparison.json', dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(), all_valid=True,
          exact_default_trajectory=True, initial_assignments_identical=10000, source_commit=args.commit,
          source_files_verified=len(build['sources'])+len(build['test_sources']), binary_sha256=build['binary_sha256'],
          competition_budget_confirmed=False, scope='800-step capability/compatibility screen only; full throughput and fairness comparison required.'))
    print('NATIVE_PREWARM_SCREEN_VERIFIED', flush=True)

if __name__ == '__main__':
    main()
