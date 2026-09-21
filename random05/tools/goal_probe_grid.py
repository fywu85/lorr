#!/usr/bin/env python3
"""Run the read-only trace cost diagnostic on a verified GRID allocation."""
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
ROOT=Path(__file__).resolve().parents[2]
if (Path(__file__).parent/'probe-spec.json').exists():
    ROOT=Path(json.loads((Path(__file__).parent/'probe-spec.json').read_text())['repo'])
sys.path.insert(0,str(ROOT/'tools'))
from cpu_resources import cpu_resources

def now():return datetime.datetime.now(datetime.timezone.utc).isoformat()
def sha(path):return hashlib.sha256(path.read_bytes()).hexdigest()
def write(path,value):path.write_text(json.dumps(value,indent=2)+'\n')
def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('action',choices=['submit','execute'])
    parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--case',type=Path)
    parser.add_argument('--trace',type=Path)
    parser.add_argument('--binary',type=Path)
    args=parser.parse_args();out=args.output.resolve()
    if args.action=='submit':
        if not all([args.case,args.trace,args.binary]):parser.error('submit needs case, trace and binary')
        case=json.loads(args.case.read_text());input_path=Path(case['input']).resolve()
        input_data=json.loads(input_path.read_text());hashes={str(input_path):sha(input_path)}
        for key in ('mapFile','agentFile','taskFile'):
            p=(input_path.parent/input_data[key]).resolve();hashes[str(p)]=sha(p)
        if case['env'].get('R05_WEIGHTS'):
            p=Path(case['env']['R05_WEIGHTS']).resolve();hashes[str(p)]=sha(p)
        out.mkdir(parents=True,exist_ok=False)
        for src,name in [(args.case,'case.json'),(args.trace,'trace.json'),(args.binary,'probe'),(Path(__file__),'runner.py')]:
            shutil.copy2(str(src),str(out/name))
        spec=dict(repo=str(ROOT),created_utc=now(),commit=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),
                  case=str(args.case.resolve()),trace=str(args.trace.resolve()),binary=str(args.binary.resolve()),
                  local_hashes={name:sha(out/name) for name in ['case.json','trace.json','probe','runner.py']},input_hashes=hashes,
                  purpose='Read-only classification of guided move costs in an already completed trace; not a benchmark score.')
        write(out/'probe-spec.json',spec)
        script=out/'job.sh';script.write_text('#!/bin/bash\nset -eu\nexec '+' '.join(shlex.quote(x) for x in ['/usr/bin/python3',str(out/'runner.py'),'execute','--output',str(out)])+'\n')
        hosts='research32|research33|research35|research36|research37|research39|research41|research46|research47|research48|research49|research50|research51|research52|research54|research55|research56'.split('|')
        command=['/opt/n1ge/bin/lx24-amd64/qsub','-terse','-w','n','-cwd','-q',','.join('debian.q@'+h+'*' for h in hosts),
                 '-pe','threaded','32','-binding','linear:16','-l','h_rt=00:30:00,h_vmem=1G,m_topology_inuse=*'+('CTT*'*16),
                 '-m','n','-N','r05_goal_probe','-j','y','-o',str(out/'scheduler.log'),'-S','/bin/bash',str(script)]
        result=subprocess.run(command,cwd=ROOT,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
        write(out/'submission.json',dict(command=command,returncode=result.returncode,response=result.stdout));print(result.stdout.strip());return result.returncode
    spec=json.loads((out/'probe-spec.json').read_text());resources=cpu_resources()
    write(out/'allocation.json',dict(started_utc=now(),resources=resources,job_id=os.environ.get('JOB_ID')))
    try:
        assert resources['cpu_model']=='AMD EPYC 9354 32-Core Processor','CPU model'
        assert resources['physical_cores_visible']==16,'physical binding'
        assert resources['effective_cpu_quota'] is None,'CPU quota'
        cpus=[cpu for group in resources['logical_cpus_by_physical_core'] for cpu in group[:2]];assert len(cpus)==32
        for name,digest in spec['local_hashes'].items():assert sha(out/name)==digest,name
        for name,digest in spec['input_hashes'].items():assert sha(Path(name))==digest,name
        env={k:v for k,v in os.environ.items() if not k.startswith(('R05_','CGAR_','WPPL_'))}
        env['OMP_NUM_THREADS']='32';env['OMP_DYNAMIC']='FALSE'
        command=['/usr/bin/prlimit','--as=32000000000','--','/usr/bin/taskset','-c',','.join(map(str,cpus)),str(out/'probe'),str(out/'case.json'),str(out/'trace.json'),str(out/'diagnostic.json')]
        with (out/'native.log').open('w') as log:result=subprocess.run(command,cwd=out,env=env,stdout=log,stderr=subprocess.STDOUT)
        if result.returncode:raise RuntimeError('diagnostic exit '+str(result.returncode))
        write(out/'completion.json',dict(finished_utc=now(),exit=0,valid=True,command=command,diagnostic_sha256=sha(out/'diagnostic.json')))
        return 0
    except Exception as error:
        write(out/'completion.json',dict(finished_utc=now(),exit=1,valid=False,error=str(error)));raise
if __name__=='__main__':raise SystemExit(main())
