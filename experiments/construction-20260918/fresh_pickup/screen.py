#!/usr/bin/env python3
"""Freeze and verify exact OFF/ON fresh-pickup audit replays on reserved CPUs."""
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


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--raw', required=True, type=Path)
    parser.add_argument('--output', required=True, type=Path)
    parser.add_argument('--commit', required=True)
    parser.add_argument('--mode', required=True, choices=['native', 'generic'])
    parser.add_argument('--hold-job')
    parser.add_argument('--execute', action='store_true')
    a = parser.parse_args(); raw = a.raw.resolve(); out = a.output.resolve(); support = raw / 'fresh-screen-support'
    if not a.execute:
        support.mkdir(exist_ok=False)
        copies = {'screen.py':Path(__file__), 'audit_tools.py':Path(__file__).with_name('audit_tools.py'),
                  'collect_cold.py':BASE/'collect_cold.py', 'profile.json':BASE/'fresh_pickup'/(a.mode+'-variants.json'),
                  'reference.json':BASE/('results/p90-pickup-combined-v99/best-record.json' if a.mode=='native' else
                                        'results/match-quota-screen-v64/trajectory-fingerprints.json')}
        for name, source in copies.items(): shutil.copy2(source, support/name)
        write(raw/'fresh-screen-request.json', dict(commit=a.commit, mode=a.mode, files={k:sha(support/k) for k in copies}))
        command = ['/usr/bin/python3', str(support/'screen.py'), '--raw',str(raw), '--output',str(out),
                   '--commit',a.commit, '--mode',a.mode, '--execute']
        job = raw/'fresh-screen.sh'; job.write_text('#!/bin/bash\nset -eu\nexec '+' '.join(map(shlex.quote,command))+'\n')
        submit = ['qsub','-h','-terse','-w','e','-cwd','-q','debian.q','-pe','threaded','1','-binding','linear:1',
                  '-l','exclusive=false,h_rt=00:20:00,h_vmem=8G','-m','n','-N','fresh_screen','-j','y',
                  '-o',str(raw/'fresh-screen.log'),'-S','/bin/bash']
        if a.hold_job: submit += ['-hold_jid',a.hold_job]
        result = subprocess.run(submit+[str(job)], text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        write(raw/'fresh-screen-submission.json', dict(command=submit+[str(job)],returncode=result.returncode,response=result.stdout))
        result.check_returncode(); assert re.fullmatch(r'\d+\s*',result.stdout)
        print(result.stdout,end='',flush=True); subprocess.run(['qrls',result.stdout.strip()],check=True); return
    request = read(raw/'fresh-screen-request.json'); assert request['commit']==a.commit and request['mode']==a.mode
    for name, value in request['files'].items(): assert sha(support/name)==value
    sys.path.insert(0,str(support)); from audit_tools import canonical, prefix, audit_samples, real_diagnostics
    sys.path.insert(0,str(ROOT/'tools')); from cpu_resources import cpu_resources
    cpu=cpu_resources(); assert cpu['effective_cpu_quota'] is None and cpu['physical_cores_visible']==1
    os.sched_setaffinity(0,cpu['representative_cpus'])
    spec=read(raw/'spec.json'); build=read(raw/'build.json'); allocation=read(raw/'allocation.json'); native=a.mode=='native'
    assert spec['horizons']=={'WAREHOUSE':800} and spec['time_limit_ms']==5000 and spec['cpus_per_instance']==4
    assert spec['trick']==('WAREHOUSE' if native else None) and spec['experiment_track']==('TRICK' if native else 'GENERIC')
    assert spec['instances']==['WAREHOUSE'] and len(spec['cases'])==2
    sources=dict(build['sources'],**build['test_sources'])
    for name,value in sources.items():
        assert hashlib.sha256(subprocess.check_output(['git','show',a.commit+':'+name],cwd=ROOT)).hexdigest()==value
    assert sha(raw/'lifelong')==build['binary_sha256']
    profiles=read(support/'profile.json'); samples={}; hashes={}; prefixes={}; counters={}; intervals=[]; seen=set(); bindings={}
    reference=read(support/'reference.json')
    if native:
        reference_data=read(Path(reference['raw_case'])/'WAREHOUSE.json')
        assert canonical(reference_data)==reference['trajectory_sha256']
        reference_prefix=prefix(reference_data,800)
        reference_tasks={t[0]:t for t in reference_data['tasks']}
        del reference_data
    for case in spec['cases']:
        name=case['name']; env=case['environment']; enabled=int(env['CGAR_FRESH_PICKUP_AUDIT'])
        assert enabled in (0,1) and enabled not in seen and case['seed']==0 and case['repeat']==0
        assert env==profiles[case['variant']]; seen.add(enabled)
        summary=read(raw/name/'summary.json')[0]; meta=read(raw/name/'metadata.json')
        assert summary['valid'] and summary['makespan']==summary['entry_compute_samples']==800
        assert summary['entry_compute_max_seconds']<=5 and summary['memory_valid'] and summary['peak_process_rss_bytes']<32000000000
        assert all(summary[k]==0 for k in ['planner_errors','schedule_errors','timeouts','internal_timeouts','exit'])
        assert summary['trick_receipt_valid'] and summary['experiment_track_valid']
        assert meta['trick_argv']==(['--trick','WAREHOUSE'] if native else [])
        assert meta['build_provenance']==build and meta['max_process_memory_bytes']==32000000000
        resources=meta['cpu_resources']; assert resources['effective_cpu_quota'] is None and resources['cpu_model']=='AMD EPYC 9354 32-Core Processor'
        binding=summary['cpu']; assert binding==meta['cpu_binding'] and len(binding)==4 and set(binding).issubset(allocation['selected_cpus'])
        coreof={c:tuple(g) for g in resources['logical_cpus_by_physical_core'] for c in g}
        cores={(resources['hostname'],coreof[c]) for c in binding}; assert len(cores)==4
        begin,end=(datetime.datetime.fromisoformat(meta[k]) for k in ['started_utc','finished_utc'])
        for x,y,z in intervals: assert not (begin<y and x<end and cores.intersection(z))
        intervals.append((begin,end,cores)); bindings[name]=dict(hostname=resources['hostname'],cpus=binding)
        lines=(raw/name/'WAREHOUSE.log').read_text().splitlines()
        samples[name]=audit_samples(lines,enabled,800,int(env['CGAR_FLOW_COST_SCALE']))
        counters[enabled]=real_diagnostics(lines); assert counters[enabled]
        data=read(raw/name/'WAREHOUSE.json'); hashes[enabled]=canonical(data); prefixes[enabled]=prefix(data,800)
        if native:
            assert prefixes[enabled]==reference_prefix
            assert all(reference_tasks[t[0]]==t for t in data['tasks'])
        else:
            assert hashes[enabled]==reference['match_g64-s0-r0']
        del data
    assert seen=={0,1} and hashes[0]==hashes[1] and prefixes[0]==prefixes[1]
    assert counters[0]==counters[1], 'audit changed real diagnostics'
    subprocess.run(['/usr/bin/python3',str(support/'collect_cold.py'),'--input',str(raw),'--output',str(out)],check=True)
    assert set(read(out/'trajectory-fingerprints.json').values())=={hashes[0]}
    for name in ['fresh-screen-request.json','fresh-screen-submission.json']: shutil.copy2(raw/name,out/name)
    write(out/'audit-samples.json',samples)
    result=dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),all_valid=True,mode=a.mode,
                exact_reference=True,exact_audit_trajectory=True,exact_real_diagnostic_lines=len(counters[0]),steps=800,
                source_commit=a.commit,source_files_verified=len(sources),binary_sha256=build['binary_sha256'],
                trajectory_sha256=hashes[0],prefix_sha256=prefixes[0],bindings=bindings,
                competition_budget_confirmed=False,scope='Read-only audit validation. Guidance-cost witnesses are not physical steps or predicted throughput.')
    write(out/'comparison.json',result); print('FRESH_SCREEN_VERIFIED',json.dumps(result),flush=True)

if __name__=='__main__': main()
