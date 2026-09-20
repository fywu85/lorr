#!/usr/bin/env python3
"""Verify execution-only Warehouse speed trials against an exact known trajectory."""
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
SOURCE = '3e319f175859396b91f97364b320c6794a85cbb7'

def read(path):
    return json.loads(path.read_text())

def write(path, data):
    path.write_text(json.dumps(data, indent=2)+'\n')

def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--raw', type=Path, required=True)
    p.add_argument('--output', type=Path, required=True)
    p.add_argument('--hold-job')
    p.add_argument('--execute', action='store_true')
    p.add_argument('--attempt', choices=['1','2'], default='1')
    a=p.parse_args();raw=a.raw.resolve();out=a.output.resolve();suffix='' if a.attempt=='1' else '-r2';support=raw/('runtime-screen-support'+suffix)
    if not a.execute:
        support.mkdir(exist_ok=False)
        copies={'screen.py':Path(__file__), 'collect_cold.py':BASE/'collect_cold.py',
                'profiles.json':BASE/'strict_runtime/screen-variants.json',
                'reference.json':BASE/'results/native-prewarm-screen-v117/trajectory-fingerprints.json',
                'cpu_resources.py':ROOT/'tools/cpu_resources.py'}
        for name,src in copies.items(): shutil.copy2(src,support/name)
        write(raw/('runtime-screen-request'+suffix+'.json'),dict(created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),source=SOURCE,files={n:sha(support/n) for n in copies}))
        command=['/usr/bin/python3',str(support/'screen.py'),'--raw',str(raw),'--output',str(out),'--attempt',a.attempt,'--execute']
        job=raw/('runtime-screen'+suffix+'.sh');job.write_text('#!/bin/bash\nset -eu\nexec '+' '.join(map(shlex.quote,command))+'\n')
        submit=['qsub','-h','-terse','-w','e','-cwd','-q','debian.q@research44.grid.gsb,debian.q@research57.grid.gsb','-pe','threaded','1','-binding','linear:1','-l','exclusive=false,h_rt=00:15:00,h_vmem=8G','-m','n','-N','runtime_screen','-j','y','-o',str(raw/('runtime-screen'+suffix+'.log')),'-S','/bin/bash']
        if a.hold_job:submit+=['-hold_jid',a.hold_job]
        submit.append(str(job));r=subprocess.run(submit,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True)
        write(raw/('runtime-screen-submission'+suffix+'.json'),dict(command=submit,returncode=r.returncode,response=r.stdout));r.check_returncode();assert re.fullmatch(r'\d+\s*',r.stdout)
        print(r.stdout,end='',flush=True);subprocess.run(['qrls',r.stdout.strip()],check=True);return
    request=read(raw/('runtime-screen-request'+suffix+'.json'));assert request['source']==SOURCE
    for name,expected in request['files'].items():assert sha(support/name)==expected,name
    sys.path.insert(0,str(support));from cpu_resources import cpu_resources
    cpu=cpu_resources();assert cpu['effective_cpu_quota'] is None and cpu['physical_cores_visible']==1;os.sched_setaffinity(0,cpu['representative_cpus'])
    spec,build,allocation=(read(raw/n) for n in ['spec.json','build.json','allocation.json'])
    assert spec['horizons']=={'WAREHOUSE':800} and spec['time_limit_ms']==5000 and spec['cpus_per_instance']==8
    assert spec['trick']=='WAREHOUSE' and spec['experiment_track']=='TRICK'
    assert sha(raw/'lifelong')==build['binary_sha256'] and build['source_commit']==SOURCE
    sources=dict(build['sources'],**build['test_sources'])
    for name,expected in sources.items():assert hashlib.sha256(subprocess.check_output(['git','show',SOURCE+':'+name],cwd=ROOT)).hexdigest()==expected,name
    subprocess.run(['/usr/bin/python3',str(support/'collect_cold.py'),'--input',str(raw),'--output',str(out)],check=True)
    fingerprints=read(out/'trajectory-fingerprints.json');profiles=read(support/'profiles.json');expected=read(support/'reference.json')['trick_prewarm_base-s0-r0']
    assert len(spec['cases'])==len(profiles)==4 and set(fingerprints.values())=={expected}
    intervals=[];work={};rows=[]
    for case in spec['cases']:
        name=case['name'];assert case['environment']==profiles[case['variant']] and case['seed']==case['repeat']==0
        summary=read(raw/name/'summary.json')[0];meta=read(raw/name/'metadata.json')
        assert summary['valid'] and summary['memory_valid'] and summary['peak_process_rss_bytes']<32000000000
        assert summary['trick_receipt_valid'] and summary['experiment_track_valid'] and meta['trick_argv']==['--trick','WAREHOUSE']
        assert meta['build_provenance']==build and meta['max_process_memory_bytes']==32000000000
        resources=meta['cpu_resources'];binding=summary['cpu'];assert resources['effective_cpu_quota'] is None
        assert resources['cpu_model']=='AMD EPYC 9354 32-Core Processor' and binding==meta['cpu_binding'] and len(binding)==8
        assert set(binding).issubset(allocation['selected_cpus'])
        core_of={c:tuple(g) for g in resources['logical_cpus_by_physical_core'] for c in g};cores={(resources['hostname'],core_of[c]) for c in binding};assert len(cores)==8
        begin,end=(datetime.datetime.fromisoformat(meta[k]) for k in ['started_utc','finished_utc'])
        for x,y,z in intervals:assert not(begin<y and x<end and cores.intersection(z))
        intervals.append((begin,end,cores))
        d=read(raw/name/'WAREHOUSE.json');times=d['entryComputeTimes'];assert len(times)==800 and max(times)==summary['entry_compute_max_seconds']
        logs=(raw/name/'WAREHOUSE.log').read_text().splitlines()
        work[name]=[s for s in logs if s.startswith(('[cgar-temporal] ','[cgar-temporal-regions] ','[cgar-orientation] '))]
        assert len(work[name])==12
        uses=[s for s in logs if s.startswith('[cgar-turn-prefetch] ')]
        assert len(uses)==(4 if case['environment'].get('CGAR_TURN_PREFETCH_THREADS') else 0)
        resource=summary['process_resources']
        rows.append(dict(case=name,environment=case['environment'],trajectory_sha256=expected,first_entry_seconds=times[0],post_first_max_seconds=max(times[1:]),mean_entry_ms=1000*sum(times)/len(times),max_entry_seconds=max(times),p99_seconds=sorted(times)[791],peak_rss_bytes=summary['peak_process_rss_bytes'],wall_seconds=summary['wall_seconds'],cpu_seconds=resource['user_seconds']+resource['system_seconds'],prefetch_samples=uses,raw_case=str(raw/name)))
    def comparable(lines):
        result=[]
        for line in lines:
            if line.startswith('[cgar-orientation] '):
                # A larger cache legitimately avoids rebuilds. All guidance,
                # fallback and policy/search counters must still agree exactly.
                line=' '.join(x for x in line.split() if not x.startswith('builds='))
            result.append(line)
        return result
    assert all(comparable(x)==comparable(work['trick_runtime_base-s0-r0']) for x in work.values()),'execution configuration changed sampled decisions/search work'
    for name,lines in work.items():
        if name!='trick_runtime_combined8-s0-r0':assert lines==work['trick_runtime_base-s0-r0']
    builds=lambda lines:[int(next(x.split('=')[1] for x in l.split() if x.startswith('builds='))) for l in lines if l.startswith('[cgar-orientation] ')]
    assert all(x<=y for x,y in zip(builds(work['trick_runtime_combined8-s0-r0']),builds(work['trick_runtime_base-s0-r0'])))
    result=dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),all_valid=True,all_trajectories_exact=True,all_sampled_search_work_exact=True,source_commit=SOURCE,source_and_test_files=len(sources),binary_sha256=build['binary_sha256'],rows=rows,scope='800-step execution-equivalence and resource screen, eight bound physical cores per case on shared GRID. No prefix quality ranking, isolated speedup or strict1s full-run certification.')
    write(out/'comparison.json',result);write(out/'search-work.json',work)
    for name in [('runtime-screen-request'+suffix+'.json'),('runtime-screen-submission'+suffix+'.json')]:shutil.copy2(raw/name,out/name)
    print('RUNTIME_SCREEN_VERIFIED',[(r['case'],r['first_entry_seconds'],r['max_entry_seconds']) for r in rows],flush=True)

if __name__=='__main__':main()
