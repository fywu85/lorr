#!/usr/bin/env python3
"""Freeze and verify the neutral-service startup comparison; no quality promotion."""
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

ROOT = next(p for p in Path(__file__).resolve().parents if (p / 'tools/cpu_resources.py').is_file())
BASE = ROOT / 'experiments/construction-20260918'
def read(p): return json.loads(p.read_text())
def write(p, x): p.write_text(json.dumps(x, indent=2) + '\n')
def sha(p): return hashlib.sha256(p.read_bytes()).hexdigest()
def fields(line): return dict(x.split('=', 1) for x in line.split()[1:])

def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--raw', required=True, type=Path)
    p.add_argument('--output', required=True, type=Path)
    p.add_argument('--commit', required=True)
    p.add_argument('--hold-job')
    p.add_argument('--execute', action='store_true')
    a = p.parse_args(); raw = a.raw.resolve(); out = a.output.resolve(); support = raw / 'neutral-screen-support'
    if not a.execute:
        support.mkdir(exist_ok=False)
        copies = {'screen.py':Path(__file__), 'collect_cold.py':BASE/'collect_cold.py',
                  'reference.json':BASE/'results/horizon-margin-screen-v92/trajectory-fingerprints.json',
                  'profiles.json':BASE/'tricks/neutral_tail/variants.json'}
        files = {}
        for name, source in copies.items():
            shutil.copy2(source, support/name); files[name] = sha(support/name)
        write(raw/'neutral-screen-request.json', dict(commit=a.commit, files=files))
        command = ['/usr/bin/python3', str(support/'screen.py'), '--raw', str(raw), '--output', str(out), '--commit', a.commit, '--execute']
        job = raw/'neutral-screen.sh'; job.write_text('#!/bin/bash\nset -eu\nexec '+' '.join(map(shlex.quote,command))+'\n')
        submit = ['qsub','-h','-terse','-w','e','-cwd','-q','debian.q','-pe','threaded','1','-binding','linear:1',
                  '-l','exclusive=false,h_rt=00:20:00,h_vmem=8G','-m','n','-N','neutral_screen','-j','y',
                  '-o',str(raw/'neutral-screen.log'),'-S','/bin/bash']
        if a.hold_job: submit += ['-hold_jid', a.hold_job]
        result = subprocess.run(submit+[str(job)], text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        write(raw/'neutral-screen-submission.json',dict(command=submit+[str(job)],returncode=result.returncode,response=result.stdout))
        result.check_returncode(); assert re.fullmatch(r'\d+\s*',result.stdout)
        print(result.stdout,end='',flush=True); subprocess.run(['qrls',result.stdout.strip()],check=True); return
    request = read(raw/'neutral-screen-request.json'); assert request['commit'] == a.commit
    for name, value in request['files'].items(): assert sha(support/name) == value
    sys.path.insert(0,str(ROOT/'tools')); from cpu_resources import cpu_resources
    cpu = cpu_resources(); assert cpu['effective_cpu_quota'] is None and cpu['physical_cores_visible'] == 1
    os.sched_setaffinity(0,cpu['representative_cpus'])
    spec = read(raw/'spec.json'); build = read(raw/'build.json'); allocation = read(raw/'allocation.json')
    assert spec['horizons'] == {'WAREHOUSE':200} and spec['time_limit_ms'] == 5000 and spec['cpus_per_instance'] == 4
    assert spec['trick'] == 'WAREHOUSE' and spec['experiment_track'] == 'TRICK' and len(spec['cases']) == 2
    sources = dict(build['sources'],**build['test_sources'])
    for name, value in sources.items(): assert hashlib.sha256(subprocess.check_output(['git','show',a.commit+':'+name],cwd=ROOT)).hexdigest() == value
    assert sha(raw/'lifelong') == build['binary_sha256']
    profiles = read(support/'profiles.json'); seen = set(); intervals = []; receipts = {}
    for case in spec['cases']:
        name = case['name']; env = case['environment']; enabled = int(env['CGAR_TRICK_NATIVE_NEUTRAL_TAIL'])
        assert case['seed'] == 0 and env == profiles[case['variant']] and enabled not in seen; seen.add(enabled)
        s = read(raw/name/'summary.json')[0]; meta = read(raw/name/'metadata.json')
        assert s['valid'] and s['makespan'] == s['entry_compute_samples'] == 200 and s['entry_compute_max_seconds'] <= 5
        assert s['memory_valid'] and s['peak_process_rss_bytes'] < 32000000000
        assert s['trick_receipt_valid'] and s['experiment_track_valid'] and meta['trick_argv'] == ['--trick','WAREHOUSE']
        assert meta['build_provenance'] == build and meta['max_process_memory_bytes'] == 32000000000
        resources = meta['cpu_resources']; assert resources['effective_cpu_quota'] is None and resources['cpu_model'] == 'AMD EPYC 9354 32-Core Processor'
        binding = s['cpu']; assert binding == meta['cpu_binding'] and len(binding) == 4 and set(binding).issubset(allocation['selected_cpus'])
        coreof = {c:tuple(g) for g in resources['logical_cpus_by_physical_core'] for c in g}
        cores = {(resources['hostname'],coreof[c]) for c in binding}; assert len(cores) == 4
        begin,end = (datetime.datetime.fromisoformat(meta[k]) for k in ['started_utc','finished_utc'])
        for x,y,z in intervals: assert not (begin<y and x<end and cores.intersection(z))
        intervals.append((begin,end,cores))
        lines = (raw/name/'WAREHOUSE.log').read_text().splitlines()
        receipt = [fields(x) for x in lines if x.startswith('[CGAR_TRICK_NATIVE_SERVICE] ')]
        assert receipt == ([dict(policy='neutral_tail',credit='after_action_hit',reservations='complete',protected='unchanged')] if enabled else [])
        counts = [fields(x) for x in lines if x.startswith('[cgar-native-service] ')]
        assert len(counts) == enabled
        if enabled: assert int(counts[0]['step']) == 200 and int(counts[0]['served_choices']) >= int(counts[0]['changed_choices']) > 0
        assert not any(x.startswith('[cgar-horizon-first-rank-change] ') for x in lines)
        receipts[name] = counts
    assert seen == {0,1}
    subprocess.run(['/usr/bin/python3',str(support/'collect_cold.py'),'--input',str(raw),'--output',str(out)],check=True)
    fingerprints = read(out/'trajectory-fingerprints.json')
    assert fingerprints['trick_p90_neutral0-s0-r0'] == read(support/'reference.json')['trick_native_horizon5000_margin1-s0-r0']
    assert fingerprints['trick_p90_neutral1-s0-r0'] != fingerprints['trick_p90_neutral0-s0-r0']
    for name in ['neutral-screen-request.json','neutral-screen-submission.json']: shutil.copy2(raw/name,out/name)
    write(out/'comparison.json',dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),all_valid=True,exact_control=True,
          source_commit=a.commit,source_files_verified=len(sources),binary_sha256=build['binary_sha256'],receipts=receipts,steps=200,
          competition_budget_confirmed=False,scope='Startup feasibility and active-policy check only. Full throughput/fairness validation required.'))
    print('NEUTRAL_SCREEN_VERIFIED',flush=True)

if __name__ == '__main__': main()
