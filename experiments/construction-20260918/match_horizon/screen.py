#!/usr/bin/env python3
"""Verify exact default compatibility and an active cutoff guard through horizon expiry."""
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
ROOT=next(p for p in Path(__file__).resolve().parents if (p/'tools/cpu_resources.py').is_file())
BASE=ROOT/'experiments/construction-20260918'
def read(p):return json.loads(p.read_text())
def write(p,x):p.write_text(json.dumps(x,indent=2)+'\n')
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def fields(line):return dict(word.split('=',1) for word in line.split()[1:])


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--raw',type=Path,required=True)
    p.add_argument('--output',type=Path,required=True);p.add_argument('--commit',required=True)
    p.add_argument('--hold-job');p.add_argument('--execute',action='store_true');a=p.parse_args()
    raw=a.raw.resolve();out=a.output.resolve();support=raw/'match-horizon-screen-support'
    if not a.execute:
        support.mkdir(exist_ok=False)
        copies={'screen.py':Path(__file__),'collect_cold.py':BASE/'collect_cold.py',
                'profiles.json':BASE/'match_horizon/screen-variants.json',
                'reference.json':BASE/'results/fresh-pickup-native-screen-v105/trajectory-fingerprints.json'}
        for name,source in copies.items():shutil.copy2(source,support/name)
        write(raw/'match-horizon-screen-request.json',dict(commit=a.commit,files={k:sha(support/k) for k in copies}))
        cmd=['/usr/bin/python3',str(support/'screen.py'),'--raw',str(raw),'--output',str(out),'--commit',a.commit,'--execute']
        job=raw/'match-horizon-screen.sh';job.write_text('#!/bin/bash\nset -eu\nexec '+' '.join(map(shlex.quote,cmd))+'\n')
        submit=['qsub','-h','-terse','-w','e','-cwd','-q','debian.q','-pe','threaded','1','-binding','linear:1',
                '-l','exclusive=false,h_rt=00:20:00,h_vmem=8G','-m','n','-N','match_horizon_screen','-j','y',
                '-o',str(raw/'match-horizon-screen.log'),'-S','/bin/bash']
        if a.hold_job:submit+=['-hold_jid',a.hold_job]
        result=subprocess.run(submit+[str(job)],text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
        write(raw/'match-horizon-screen-submission.json',dict(command=submit+[str(job)],returncode=result.returncode,response=result.stdout))
        result.check_returncode();assert re.fullmatch(r'\d+\s*',result.stdout)
        print(result.stdout,end='',flush=True);subprocess.run(['qrls',result.stdout.strip()],check=True);return
    request=read(raw/'match-horizon-screen-request.json');assert request['commit']==a.commit
    for name,h in request['files'].items():assert sha(support/name)==h
    sys.path.insert(0,str(ROOT/'tools'));from cpu_resources import cpu_resources
    cpu=cpu_resources();assert cpu['effective_cpu_quota'] is None and cpu['physical_cores_visible']==1
    os.sched_setaffinity(0,cpu['representative_cpus'])
    spec=read(raw/'spec.json');build=read(raw/'build.json');allocation=read(raw/'allocation.json')
    assert spec['horizons']=={'WAREHOUSE':800} and spec['time_limit_ms']==5000 and spec['cpus_per_instance']==4
    assert spec['trick']=='WAREHOUSE' and spec['experiment_track']=='TRICK' and len(spec['cases'])==3
    sources=dict(build['sources'],**build['test_sources'])
    for name,h in sources.items():assert hashlib.sha256(subprocess.check_output(['git','show',a.commit+':'+name],cwd=ROOT)).hexdigest()==h
    assert sha(raw/'lifelong')==build['binary_sha256'];profiles=read(support/'profiles.json')
    samples={};seen=set();intervals=[]
    for case in spec['cases']:
        name=case['name'];env=case['environment'];guard=int(env['CGAR_TRICK_MATCH_HORIZON'])
        assert guard in (0,1) and case['variant'] not in seen and case['seed']==0 and case['repeat']==0
        assert env==profiles[case['variant']];seen.add(case['variant'])
        summary=read(raw/name/'summary.json')[0];meta=read(raw/name/'metadata.json')
        assert summary['valid'] and summary['makespan']==summary['entry_compute_samples']==800 and summary['entry_compute_max_seconds']<=5
        assert summary['memory_valid'] and summary['peak_process_rss_bytes']<32000000000
        assert summary['trick_receipt_valid'] and summary['experiment_track_valid'] and meta['trick_argv']==['--trick','WAREHOUSE']
        assert meta['build_provenance']==build and meta['max_process_memory_bytes']==32000000000
        resources=meta['cpu_resources'];assert resources['effective_cpu_quota'] is None and resources['cpu_model']=='AMD EPYC 9354 32-Core Processor'
        binding=summary['cpu'];assert binding==meta['cpu_binding'] and len(binding)==4 and set(binding).issubset(allocation['selected_cpus'])
        coreof={c:tuple(g) for g in resources['logical_cpus_by_physical_core'] for c in g}
        cores={(resources['hostname'],coreof[c]) for c in binding};assert len(cores)==4
        begin,end=(datetime.datetime.fromisoformat(meta[k]) for k in ['started_utc','finished_utc'])
        for x,y,z in intervals:assert not (begin<y and x<end and cores.intersection(z))
        intervals.append((begin,end,cores))
        lines=(raw/name/'WAREHOUSE.log').read_text().splitlines()
        receipt=[fields(s) for s in lines if s.startswith('[CGAR_TRICK_MATCH_HORIZON] ')]
        expected=dict(enabled='1',policy='reject_worse_task_tier',cycle='whole',fields='resident_spatial',fallback='manhattan',fair='unchanged',budget='one')
        assert receipt==([expected] if guard else [])
        matches=[fields(s) for s in lines if s.startswith('[cgar-unopened-match] t=')]
        assert [int(m['t']) for m in matches]==[0,200,400,600]
        for m in matches:
            assert int(m['passes'])==int(m['t'])//10+1
            assert int(m['groups'])<=int(m['passes'])*64 and int(m['nodes'])<=int(m['groups'])*2048
        guarded=[fields(s) for s in lines if s.startswith('[cgar-match-horizon] ')]
        assert [int(m['t']) for m in guarded]==([0,200,400,600] if guard else [])
        if guard:
            assert int(guarded[1]['rejected'])>0 and int(guarded[1]['cycles'])>=int(guarded[1]['rejected'])
            assert all({k:v for k,v in m.items() if k!='t'}=={k:v for k,v in guarded[1].items() if k!='t'} for m in guarded[2:])
            assert all(int(m['rows'])>=2*int(m['cycles']) and int(m['worse_rows'])>=int(m['rejected']) for m in guarded)
            assert [fields(s) for s in lines if s.startswith('[cgar-horizon-passed] ')]==[dict(t='200',known_horizon='200',policy='ordinary',assumption='configured')]
        assert int(matches[-1]['moved'])>0
        samples[name]=dict(matching=matches,guard=guarded)
    assert seen==set(profiles)
    subprocess.run(['/usr/bin/python3',str(support/'collect_cold.py'),'--input',str(raw),'--output',str(out)],check=True)
    fingerprints=read(out/'trajectory-fingerprints.json')
    assert fingerprints['trick_guard_reference-s0-r0']==read(support/'reference.json')['trick_fresh0-s0-r0']
    assert fingerprints['trick_guard200_1-s0-r0']!=fingerprints['trick_guard200_0-s0-r0']
    for name in ['match-horizon-screen-request.json','match-horizon-screen-submission.json']:shutil.copy2(raw/name,out/name)
    write(out/'matching-samples.json',samples)
    write(out/'comparison.json',dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),all_valid=True,
          exact_default_trajectory=True,active_guard_and_expiry=True,source_commit=a.commit,source_files_verified=len(sources),
          binary_sha256=build['binary_sha256'],competition_budget_confirmed=False,
          scope='800-step capability/compatibility screen only. Full throughput and fairness comparison remains required.'))
    print('MATCH_HORIZON_SCREEN_VERIFIED',flush=True)

if __name__=='__main__':main()
