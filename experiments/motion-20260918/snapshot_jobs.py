#!/usr/bin/env python3
"""Build and run paired movement snapshots with explicit GRID allocations."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import shlex
import shutil
import subprocess
import sys
ROOT=Path(__file__).resolve().parents[2];HERE=Path(__file__).resolve().parent
sys.path.insert(0,str(ROOT/'tools'))
from cpu_resources import cpu_resources
from benchmark_gridengine import write

def sha(path):return hashlib.sha256(path.read_bytes()).hexdigest()
def submit(out,command,slots,cores,name,array=None):
    script=out/(name+'.sh');script.write_text('#!/bin/bash\nset -eu\nexec '+' '.join(shlex.quote(x) for x in command)+'\n')
    cmd=['/opt/n1ge/bin/lx24-amd64/qsub','-terse','-w','e','-cwd','-q','debian.q','-pe','threaded',str(slots),'-binding','linear:'+str(cores),'-l','exclusive=true,h_rt=01:00:00,h_vmem=4G','-m','n','-N',name,'-j','y','-o',str(out),'-S','/bin/bash']
    if array:cmd+=['-t','1-'+str(array),'-tc','10']
    run=subprocess.run(cmd+[str(script)],cwd=str(ROOT),text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
    write(out/(name+'-submission.json'),{'command':cmd+[str(script)],'returncode':run.returncode,'response':run.stdout});print(run.stdout,end='');return run.returncode

def build(out):
    write(out/'build-allocation.json',cpu_resources());manifest={}
    env=dict(os.environ);env['PATH']=str(ROOT/'env/bin')+':'+env['PATH']
    # Avoid linking stale objects against a newer Cgar class layout.
    for team in ['cgar','nms']:
        subprocess.run(['cmake','--build',str(ROOT/team/'build'),'--target','lifelong','-j4'],env=env,check=True)
    for team in ['cgar','nms']:
        project=(ROOT/team).resolve();b=project/'build';flags={}
        for line in (b/'CMakeFiles/lifelong.dir/flags.make').read_text().splitlines():
            if ' = ' in line:
                key,value=line.split(' = ',1);flags[key]=shlex.split(value)
        link=shlex.split((b/'CMakeFiles/lifelong.dir/link.txt').read_text())
        obj=out/(team+'-snapshot.o')
        command=[link[0]]+flags['CXX_DEFINES']+flags['CXX_INCLUDES']+flags['CXX_FLAGS']
        if team=='nms':command+=['-DSNAPSHOT_NMS=1']
        compile_command=command+['-c',str(HERE/'snapshot_driver.cpp'),'-o',str(obj)]
        subprocess.run(compile_command,cwd=str(b),check=True)
        # Same reference objects as its archived executable, replacing only main.
        command=[];objects={}
        for arg in link:
            if arg.endswith('/src/driver.cpp.o'):command.append(str(obj))
            elif arg.endswith('.o'):
                absolute=b/arg;command.append(str(absolute));objects[str(absolute.relative_to(ROOT))]=sha(absolute)
            elif arg=='lifelong':command.append(str(out/(team+'-snapshot')))
            else:command.append(arg)
        subprocess.run(command,cwd=str(b),check=True)
        sources={str(p.relative_to(ROOT)):sha(p) for folder in ['src','inc','cgar_planner','default_planner','Solution'] for p in (project/folder).rglob('*') if p.is_file() and p.suffix in ['.cpp','.hpp','.h']}
        manifest[team]={'binary_sha256':sha(out/(team+'-snapshot')),'linked_objects':objects,'sources':sources,'compile':compile_command,'link':command}
    manifest['driver_sha256']=sha(HERE/'snapshot_driver.cpp');write(out/'build.json',manifest)
    inputs=out/'snapshots';inputs.mkdir();cases=[]
    for item in json.loads((HERE/'results/snapshot-inputs.json').read_text()):
        source=HERE/item['snapshot'];assert sha(source)==item['sha256']
        dest=inputs/source.name;shutil.copy2(source,dest)
        cases.append(dict(item,snapshot=str(dest)))
    write(inputs/'manifest.json',cases)
    write(out/'build-completion.json',{'valid':True})

def execute(out):
    spec=json.loads((out/'spec.json').read_text());case=spec['cases'][int(os.environ['SGE_TASK_ID'])-1]
    work=out/case['name'];work.mkdir();resources=cpu_resources()
    assert resources['physical_cores_visible']==16 and len(resources['logical_cpu_affinity'])==32,resources
    assert resources['effective_cpu_quota'] is None or resources['effective_cpu_quota']>=32,resources
    write(work/'allocation.json',resources)
    assert sha(Path(case['snapshot']))==case['sha256']
    env={k:v for k,v in os.environ.items() if not k.startswith('CGAR_')};env.update({'CGAR_SEED':'0','OMP_NUM_THREADS':'32','OMP_DYNAMIC':'FALSE'})
    results=[]
    for team in ['cgar','nms']:
        binary=Path(spec['build_dir'])/(team+'-snapshot');assert sha(binary)==spec['build'][team]['binary_sha256']
        command=[str(binary),case['snapshot'],str(work/(team+'.json')),'100','1000']
        if team=='cgar':command=['taskset','-c',str(resources['representative_cpus'][0])]+command
        command=[sys.executable,str(ROOT/'tools/run_with_usage.py'),'--output',str(work/(team+'-usage.json')),'--']+command
        with (work/(team+'.log')).open('w') as log:r=subprocess.run(command,cwd=str(work),env=env,stdout=log,stderr=subprocess.STDOUT)
        results.append({'team':team,'exit':r.returncode})
    write(work/'completion.json',results)
    return int(any(r['exit'] for r in results))

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('mode',choices=['build','run','execute-build','execute-run']);p.add_argument('--output',required=True,type=Path);p.add_argument('--build-dir',type=Path);args=p.parse_args();out=args.output.resolve()
    if args.mode=='execute-build':build(out);return 0
    if args.mode=='execute-run':return execute(out)
    if args.mode=='run' and not args.build_dir:p.error('--build-dir is required for run')
    out.mkdir(parents=True,exist_ok=False)
    if args.mode=='build':return submit(out,[sys.executable,str(Path(__file__).resolve()),'execute-build','--output',str(out)],4,4,'motion_build')
    build_dir=args.build_dir.resolve();assert json.loads((build_dir/'build-completion.json').read_text())['valid']
    spec={'build_dir':str(build_dir),'build':json.loads((build_dir/'build.json').read_text()),'cases':json.loads((build_dir/'snapshots/manifest.json').read_text())}
    write(out/'spec.json',spec)
    return submit(out,[sys.executable,str(Path(__file__).resolve()),'execute-run','--output',str(out)],32,16,'motion_replay',len(spec['cases']))
if __name__=='__main__':raise SystemExit(main())
