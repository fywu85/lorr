#!/usr/bin/env python3
"""Freeze, build and evaluate the independent Random05 campaign on GRID."""
import argparse, datetime, hashlib, json, os, resource, shlex, shutil, subprocess, sys, time
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
if (Path(__file__).parent/'spec.json').exists():
    ROOT=Path(json.loads((Path(__file__).parent/'spec.json').read_text())['repo'])
sys.path.insert(0,str(ROOT/'tools'))
from cpu_resources import cpu_resources
from concurrent.futures import ThreadPoolExecutor
def write(p,x):
    p.write_text(json.dumps(x,indent=2)+'\n')
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def now():return datetime.datetime.now(datetime.timezone.utc).isoformat()
def submit(a):
    out=a.output.resolve();out.mkdir(parents=True,exist_ok=False)
    spec={'kind':a.kind,'created_utc':now(),'repo':str(ROOT),
          'commit':subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip()}
    if a.kind=='nms4-build':
        shutil.copytree(ROOT/'nms',out/'source',ignore=shutil.ignore_patterns('build','.git','__pycache__','*.log','printer.txt'))
        p=out/'source/Solution/settings.hpp';body=p.read_text();assert body.count('THREADS = 32;')==1
        p.write_text(body.replace('THREADS = 32;','THREADS = 4;'))
        spec['change']='Only Solution/settings.hpp THREADS=32 changed to THREADS=4'
        cache=(ROOT/'nms/build/CMakeCache.txt').read_text().splitlines()
        spec['reference_flags']={k:next(l.split('=',1)[1] for l in cache if l.startswith(k+':STRING=')) for k in ['CMAKE_CXX_FLAGS','CMAKE_EXE_LINKER_FLAGS']}
        spec['source_hashes']={str(p.relative_to(out/'source')):sha(p) for p in (out/'source').rglob('*') if p.is_file()}
        physical=4;slots=8
    elif a.kind=='build':
        shutil.copytree(ROOT/'random05',out/'source',ignore=shutil.ignore_patterns('build','__pycache__'))
        spec['source_hashes']={str(p.relative_to(out/'source')):sha(p) for p in (out/'source').rglob('*') if p.is_file()}
        physical=4;slots=8
    else:
        spec['cases']=json.loads(a.cases.read_text())
        spec['cases_input']=str(a.cases.resolve())
        physical=sum(c.get('cores',1) for c in spec['cases'])
        # GRID counts logical slots. Reserve both SMT siblings even when a
        # case runs one worker per physical core, so bindings can coexist.
        slots=2*physical
        for c in spec['cases']:
            name=c['name'];work=out/name;work.mkdir()
            binary=Path(c['binary']).resolve();shutil.copy2(binary,work/'lifelong')
            c['binary_sha256']=sha(binary);c['original_binary']=str(binary)
            c['input']=str(Path(c['input']).resolve());c['input_hashes']={c['input']:sha(Path(c['input']))}
            weights=c.get('env',{}).get('R05_WEIGHTS')
            if weights:c['input_hashes'][str(Path(weights).resolve())]=sha(Path(weights))
            data=json.loads(Path(c['input']).read_text())
            for key in ['mapFile','agentFile','taskFile']:
                p=Path(c['input']).parent/data[key];c['input_hashes'][str(p.resolve())]=sha(p)
            if c.get('team')=='nms':
                shutil.copytree(ROOT/'nms',work/'cwd',ignore=shutil.ignore_patterns('build','.git','__pycache__','*.log','printer.txt'))
            else:(work/'cwd').mkdir()
    memory_per_slot=2 if a.kind in ('build','nms4-build') else max(1,(sum(c.get('memory_gib',4 if c.get('team')=='nms' else 2) for c in spec['cases'])+slots-1)//slots)
    spec.update(physical=physical,slots=slots,exclusive=False,memory_per_slot_gib=memory_per_slot)
    write(out/'spec.json',spec)
    # Freeze the runner too; ROOT for the frozen script is supplied by its spec.
    shutil.copy2(Path(__file__),out/'runner.py')
    command=['/usr/bin/python3',str(out/'runner.py'),'execute','--output',str(out)]
    script=out/'job.sh';script.write_text('#!/bin/bash\nset -eu\nexec '+' '.join(shlex.quote(x) for x in command)+'\n')
    args=['/opt/n1ge/bin/lx24-amd64/qsub','-terse','-w','n','-cwd','-q','debian.q','-pe','threaded',str(slots),
          '-binding','linear:'+str(physical),'-l','h_rt=03:00:00,h_vmem='+str(memory_per_slot)+'G,m_topology_inuse=*'+('CTT*'*physical),'-m','n','-N','r05_'+a.kind,
          '-j','y','-o',str(out/'scheduler.log'),'-S','/bin/bash',str(script)]
    r=subprocess.run(args,cwd=ROOT,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
    write(out/'submission.json',{'command':args,'returncode':r.returncode,'response':r.stdout})
    print(r.stdout,end='');return r.returncode
def execute(a):
    global ROOT
    out=a.output.resolve();spec=json.loads((out/'spec.json').read_text());ROOT=Path(spec['repo'])
    resources=cpu_resources();write(out/'allocation.json',dict(started_utc=now(),resources=resources,job_id=os.environ.get('JOB_ID')))
    assert resources['physical_cores_visible']==spec['physical'],('GRID did not honor requested core binding; refusing an unbound benchmark',resources)
    assert resources['effective_cpu_quota'] is None or resources['effective_cpu_quota']>=spec['slots'],resources
    if spec['kind'] in ('build','nms4-build'):
        for rel,digest in spec['source_hashes'].items():assert sha(out/'source'/rel)==digest,rel
        env=dict(os.environ);env['PATH']=str(ROOT/'env/bin')+':'+env['PATH']
        commands=[[str(ROOT/'env/bin/cmake'),'-S',str(out/'source'),'-B',str(out/'build'),
                   '-DCMAKE_BUILD_TYPE=Release','-DCMAKE_CXX_COMPILER='+str(ROOT/'env/bin/x86_64-conda-linux-gnu-c++'),'-DCMAKE_PREFIX_PATH='+str(ROOT/'env'),'-DBUILD_TESTING=ON'],
                  [str(ROOT/'env/bin/cmake'),'--build',str(out/'build'),'--parallel','4'],
                  [str(ROOT/'env/bin/ctest'),'--test-dir',str(out/'build'),'--output-on-failure']]
        if spec['kind']=='nms4-build':
            commands[0]+=['-D'+k+'='+v for k,v in spec['reference_flags'].items()]
            commands[1]+=['--target','lifelong'];commands=commands[:2]
        result=0
        with (out/'build.log').open('w') as f:
            for cmd in commands:
                result=subprocess.run(cmd,env=env,stdout=f,stderr=subprocess.STDOUT).returncode
                if result:break
        record={'exit':result,'finished_utc':now()}
        binary=out/('build/lifelong' if spec['kind']=='nms4-build' else 'build/lifelong_random05')
        if binary.exists():record['binary_sha256']=sha(binary)
        write(out/'completion.json',record);return result
    groups=resources['logical_cpus_by_physical_core'];start=0;cases=[]
    for c in spec['cases']:
        cpus=[cpu for g in groups[start:start+c.get('cores',1)] for cpu in g[:c.get('smt',1)]]
        start+=c.get('cores',1);cases.append((c,cpus))
    def run(item):
        c,cpus=item;work=out/c['name'];assert sha(work/'lifelong')==c['binary_sha256']
        for p,digest in c['input_hashes'].items():assert sha(Path(p))==digest,p
        env={k:v for k,v in os.environ.items() if not k.startswith(('R05_','CGAR_','WPPL_'))}
        env.update({k:str(v) for k,v in c.get('env',{}).items()})
        env['OMP_NUM_THREADS']=str(len(cpus));env['OMP_DYNAMIC']='FALSE'
        command=['/usr/bin/taskset','-c',','.join(map(str,cpus)),str(work/'lifelong'),'-i',c['input'],
                 '-o',str(work/'result.json'),'-s',str(c.get('steps',2000)),'-t',str(c.get('limit_ms',1000)),
                 '-p',str(c.get('preprocess_ms',30000)),'-d','2']
        if c.get('trick'):command+=['--trick','RANDOM-05']
        command=['/usr/bin/python3',str(ROOT/'tools/run_with_usage.py'),'--output',str(work/'usage.json'),'--']+command
        write(work/'case.json',dict(c,cpus=cpus,started_utc=now(),command=command))
        command=['/usr/bin/prlimit','--as=32000000000','--']+command
        with (work/'native.log').open('w') as f:
            r=subprocess.run(command,cwd=work/'cwd',env=env,stdout=f,stderr=subprocess.STDOUT)
        summary={'exit':r.returncode,'finished_utc':now(),'name':c['name'],'steps':c.get('steps',2000),
                 'binary_sha256':c['binary_sha256'],'environment':c.get('env',{}),'trick':c.get('trick',False)}
        if (work/'result.json').exists():
            data=json.loads((work/'result.json').read_text())
            summary['result']={k:data.get(k) for k in ['numTaskFinished','makespan','numPlannerErrors','numScheduleErrors','numEntryTimeouts','entryComputeSamples','entryComputeMaxSeconds']}
            for k in ['errors','scheduleErrors']:
                if k in data:summary[k]=len(data[k])
            times=data.get('entryComputeTimes',data.get('plannerTimes',[]))
            if times:summary['latency_seconds']={'mean':sum(times)/len(times),'max':max(times)}
            if data.get('actualPaths'):
                summary['actual_path_steps']=[min(len(x.split(',')) for x in data['actualPaths']),max(len(x.split(',')) for x in data['actualPaths'])]
                summary['trajectory_sha256']=hashlib.sha256(json.dumps(data['actualPaths'],separators=(',',':')).encode()).hexdigest()
            summary['valid']=r.returncode==0 and data.get('makespan')==c.get('steps',2000) and all(data.get(k,0)==0 for k in ['numPlannerErrors','numScheduleErrors','numEntryTimeouts']) and not data.get('errors') and not data.get('scheduleErrors')
        else:summary['valid']=False
        if (work/'usage.json').exists():summary['usage']=json.loads((work/'usage.json').read_text())
        write(work/'summary.json',summary);return summary
    with ThreadPoolExecutor(max_workers=len(cases)) as pool:results=list(pool.map(run,cases))
    write(out/'summary.json',results);write(out/'completion.json',{'finished_utc':now(),'valid':all(r['valid'] for r in results)})
    return 0 if all(r['valid'] for r in results) else 1
def main():
    p=argparse.ArgumentParser();p.add_argument('action',choices=['submit','execute'])
    p.add_argument('--kind',choices=['build','nms4-build','benchmark']);p.add_argument('--output',type=Path,required=True);p.add_argument('--cases',type=Path)
    a=p.parse_args();return execute(a) if a.action=='execute' else submit(a)
if __name__=='__main__':raise SystemExit(main())
