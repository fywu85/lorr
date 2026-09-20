#!/usr/bin/env python3
"""Freeze and run an offline time-zero pickup diagnostic on four reserved cores."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import re
import resource
import shlex
import shutil
import subprocess
import sys

ROOT=next(p for p in Path(__file__).resolve().parents if (p/'tools/cpu_resources.py').is_file())
BASE=ROOT/'experiments/construction-20260918'
def read(p): return json.loads(p.read_text())
def write(p,x): p.write_text(json.dumps(x,indent=2)+'\n')
def sha(p):
    h=hashlib.sha256()
    with p.open('rb') as f:
        for block in iter(lambda:f.read(4*1024*1024),b''):h.update(block)
    return h.hexdigest()


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--raw',type=Path,required=True)
    p.add_argument('--output',type=Path,required=True);p.add_argument('--execute',action='store_true');a=p.parse_args()
    raw=a.raw.resolve();out=a.output.resolve()
    if not a.execute:
        raw.mkdir(parents=True,exist_ok=False)
        copies={'run.py':Path(__file__),'probe.cpp':Path(__file__).with_name('probe.cpp'),
                'audit_tools.py':BASE/'fresh_pickup/audit_tools.py',
                'reference.json':BASE/'results/p90-pickup-combined-v99/verification.json',
                'source-build.json':BASE/'build-provenance/v105/build.json'}
        for name in ['cgar/cgar_planner/pickup_full.hpp','cgar/cgar_planner/assignment_permutation.hpp',
                     'cgar/cgar_planner/tricks.hpp','cgar/tricks/warehouse_lanes.hpp',
                     'cgar/tricks/warehouse_native.hpp','cgar/inc/nlohmann/json.hpp']:
            copies[name]=ROOT/name
        for name,source in copies.items():
            target=raw/name;target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(source,target)
        request=dict(prepared_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                     files={name:sha(raw/name) for name in copies},output=str(out),offline_only=True,
                     source_commit=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip())
        write(raw/'request.json',request)
        command=['/usr/bin/python3',str(raw/'run.py'),'--raw',str(raw),'--output',str(out),'--execute']
        script=raw/'job.sh';script.write_text('#!/bin/bash\nset -eu\nexec '+' '.join(map(shlex.quote,command))+'\n')
        hosts=['research38','research44','research46','research50','research52','research57']
        submit=['qsub','-h','-terse','-w','e','-cwd','-q',','.join('debian.q@'+h for h in hosts),
                '-pe','threaded','4','-binding','linear:4','-l','exclusive=false,h_rt=00:20:00,h_vmem=4G',
                '-m','n','-N','bootstrap_probe','-j','y','-o',str(raw/'job.log'),'-S','/bin/bash',str(script)]
        result=subprocess.run(submit,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True)
        write(raw/'submission.json',dict(command=submit,returncode=result.returncode,response=result.stdout))
        result.check_returncode();assert re.fullmatch(r'\d+\s*',result.stdout)
        print(result.stdout,end='',flush=True);subprocess.run(['qrls',result.stdout.strip()],check=True);return
    request=read(raw/'request.json');assert request['output']==str(out)
    for name,h in request['files'].items():assert sha(raw/name)==h,name
    sys.path.insert(0,str(raw));from audit_tools import canonical
    sys.path.insert(0,str(ROOT/'tools'));from cpu_resources import cpu_resources
    cpu=cpu_resources();assert cpu['effective_cpu_quota'] is None and cpu['physical_cores_visible']==4
    assert cpu['cpu_model']=='AMD EPYC 9354 32-Core Processor'
    os.sched_setaffinity(0,cpu['representative_cpus']);write(raw/'allocation.json',cpu)
    build=read(raw/'source-build.json')
    for name,h in request['files'].items():
        if name in build['sources']:
            assert h==build['sources'][name]
            assert hashlib.sha256(subprocess.check_output(['git','show',build['source_commit']+':'+name],cwd=ROOT)).hexdigest()==h
    compile_command=['g++','-std=c++17','-O3','-DNDEBUG','-pthread','-I'+str(raw/'cgar/cgar_planner'),
                     '-I'+str(raw/'cgar/inc'),str(raw/'probe.cpp'),'-o',str(raw/'probe')]
    subprocess.run(compile_command,check=True)
    reference=read(raw/'reference.json');assert reference['all_valid_within_deadline_and_memory'] and not reference['failures']
    selected=[r for r in reference['rows'] if r['variant']=='trick_p90_pickup8'];assert {r['seed'] for r in selected}=={0,2}
    out.mkdir(parents=True,exist_ok=False);sources=[];fixtures={};initial_stats=[]
    for row in selected:
        result_path=Path(row['raw_case'])/'WAREHOUSE.json';original=sha(result_path);data=read(result_path)
        assert data['makespan']==5000 and data['teamSize']==10000 and data['numTaskFinished']==row['tasks']
        assert canonical(data)==row['trajectory_sha256']
        meta=read(Path(row['raw_case'])/'metadata.json');input_path=Path(meta['instances']['WAREHOUSE']['input'])
        problem=read(input_path);map_path=input_path.parent/problem['mapFile'];lines=map_path.read_text().splitlines()
        rows=int(lines[1].split()[1]);cols=int(lines[2].split()[1]);assert lines[3]=='map'
        grid=lines[4:];assert len(grid)==rows and all(len(line)==cols for line in grid)
        obstacles=[int(ch not in '.GSE') for line in grid for ch in line]
        tasks={t[0]:t for t in data['tasks']};agents=[];ids=[]
        for robot,(start,schedule) in enumerate(zip(data['start'],data['actualSchedule'])):
            entries=[tuple(map(int,e.split(':'))) for e in schedule.split(',') if e]
            first=next((tick,task) for tick,task in entries if task>=0)
            assert first[0] in (0,1) and tasks[first[1]][1]==0
            r,c,orientation=start;heading='ESWN'.index(orientation) if isinstance(orientation,str) else orientation
            coordinates=tasks[first[1]][2];goal=coordinates[0]*cols+coordinates[1]
            agents.append([robot,r*cols+c,heading,first[1],goal]);ids.append(first[1])
        assert len(agents)==10000 and len(set(ids))==10000
        fixture=dict(schema=1,rows=rows,cols=cols,map=obstacles,agents=agents)
        fingerprint=hashlib.sha256(json.dumps(fixture,separators=(',',':')).encode()).hexdigest()
        if fingerprint not in fixtures:
            name='initial-'+str(len(fixtures));write(raw/(name+'.json'),fixture);fixtures[fingerprint]=name
        sources.append(dict(case=row['case'],seed=row['seed'],tasks=row['tasks'],raw_sha256=original,
                            trajectory_sha256=row['trajectory_sha256'],fixture_sha256=fingerprint,
                            fixture=fixtures[fingerprint],map_sha256=sha(map_path),input_sha256=sha(input_path)))
        pickup={task:tick for tick,robot,task,stop in data['events'] if stop==1}
        # Initial task completion/pickup times describe this cohort only. Do not
        # sum them as original-holder travel when an unopened retarget occurred.
        times=sorted(pickup[t] for t in ids)
        initial_stats.append(dict(seed=row['seed'],initial_tasks=len(ids),all_picked=len(times)==len(ids),
                                  pickup_times=dict(mean=sum(times)/len(times),p90=times[(len(times)-1)*9//10],max=times[-1]),
                                  interpretation='Elapsed pickup times for the initial task cohort; not original-holder travel.'))
        assert sha(result_path)==original;del data
    results={}
    for fingerprint,name in fixtures.items():
        command=[str(raw/'probe'),'--trick','WAREHOUSE',str(raw/(name+'.json')),str(raw/(name+'-result.json'))]
        subprocess.run(command,check=True);report=read(raw/(name+'-result.json'))
        assert report['offline_only'] and report['independent_heap_fields']==4
        assert report['native_before']-report['native_after']==report['native_saving']
        assert sum(g['native_before'] for g in report['groups'])==report['native_before']
        assert sum(g['accepted_saving'] for g in report['groups'])==report['accepted_native_saving']
        shutil.copy2(raw/(name+'-result.json'),out/(name+'-result.json'));results[name]=dict(fixture_sha256=fingerprint,result_sha256=sha(out/(name+'-result.json')))
    for name in ['request.json','submission.json','allocation.json']:shutil.copy2(raw/name,out/name)
    write(out/'verification.json',dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),all_valid=True,
          offline_only=True,sources=sources,initial_task_cohorts=initial_stats,unique_fixtures=results,
          compile_command=compile_command,binary_sha256=sha(raw/'probe'),frozen_headers_match_source=build['source_commit'],
          child_peak_rss_bytes=resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss*1024,
          scope='Offline conditional cost witnesses and computational cost only. No live policy change, competition-time feasibility, physical saving or throughput improvement claimed.'))
    print('BOOTSTRAP_AUDIT_VERIFIED',len(results),'unique initial assignments',flush=True)

if __name__=='__main__':main()
