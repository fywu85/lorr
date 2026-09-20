#!/usr/bin/env python3
"""Run frozen, offline saved-state counterfactuals on a bound GRID allocation."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import shlex

ROOT=Path(__file__).resolve().parents[2]
if (Path(__file__).parent/'probe-spec.json').exists():
    ROOT=Path(json.loads((Path(__file__).parent/'probe-spec.json').read_text())['repo'])
sys.path.insert(0,str(ROOT/'tools'))
from cpu_resources import cpu_resources

def now(): return datetime.datetime.now(datetime.timezone.utc).isoformat()
def sha(path): return hashlib.sha256(path.read_bytes()).hexdigest()
def write(path,data): path.write_text(json.dumps(data,indent=2)+'\n')
def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('action',choices=['submit','execute'])
    parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--snapshot',type=Path)
    parser.add_argument('--case',type=Path,help='The full benchmark case.json whose state was captured')
    parser.add_argument('--binary',type=Path)
    parser.add_argument('--future-k',type=int,default=1280)
    parser.add_argument('--steps',type=int,default=128)
    parser.add_argument('--future-seeds',default='0,1,2')
    args=parser.parse_args();out=args.output.resolve()
    if args.action=='submit':
        out.mkdir(parents=True,exist_ok=False)
        case=json.loads(args.case.read_text());snapshot=json.loads(args.snapshot.read_text())
        expected=json.loads(Path(case['input']).read_text());input_path=Path(case['input']).resolve()
        hashes={str(input_path):sha(input_path)}
        for key in ('mapFile','agentFile','taskFile'):
            p=input_path.parent/expected[key];hashes[str(p.resolve())]=sha(p)
        weights=case.get('env',{}).get('R05_WEIGHTS')
        if weights:hashes[str(Path(weights).resolve())]=sha(Path(weights))
        shutil.copy2(args.binary,out/'probe');shutil.copy2(args.snapshot,out/'snapshot.json')
        spec=dict(repo=str(ROOT),created_utc=now(),commit=subprocess.check_output(
            ['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),
            case=case,snapshot=str(args.snapshot.resolve()),snapshot_sha256=sha(args.snapshot),
            binary=str(args.binary.resolve()),binary_sha256=sha(args.binary),input_hashes=hashes,
            future_k=args.future_k,steps=args.steps,future_seeds=[int(x) for x in args.future_seeds.split(',')],
            physical=16,slots=32,cpu_model='AMD EPYC 9354 32-Core Processor',
            purpose='offline counterfactual diagnostic, not a benchmark throughput score')
        write(out/'probe-spec.json',spec);shutil.copy2(Path(__file__),out/'runner.py')
        script=out/'job.sh';script.write_text('#!/bin/bash\nset -eu\nexec '+ ' '.join(shlex.quote(x) for x in
            ['/usr/bin/python3',str(out/'runner.py'),'execute','--output',str(out)])+'\n')
        command=['/opt/n1ge/bin/lx24-amd64/qsub','-terse','-w','n','-cwd','-q',
            'debian.q@research33*,debian.q@research39*','-pe','threaded','32','-binding','linear:16',
            '-l','h_rt=02:00:00,h_vmem=1G,m_topology_inuse=*'+('CTT*'*16),'-m','n',
            '-N','r05_probe','-j','y','-o',str(out/'scheduler.log'),'-S','/bin/bash',str(script)]
        result=subprocess.run(command,cwd=ROOT,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
        write(out/'submission.json',dict(command=command,returncode=result.returncode,response=result.stdout))
        print(result.stdout.strip());return result.returncode
    spec=json.loads((out/'probe-spec.json').read_text());resources=cpu_resources()
    write(out/'allocation.json',dict(started_utc=now(),resources=resources,job_id=os.environ.get('JOB_ID')))
    try:
        assert resources['cpu_model']==spec['cpu_model'],'CPU model'
        assert resources['physical_cores_visible']==spec['physical'],'physical core binding'
        assert resources['effective_cpu_quota'] is None or resources['effective_cpu_quota']>=spec['slots'],'quota'
        assert sha(out/'probe')==spec['binary_sha256'],'probe binary'
        assert sha(out/'snapshot.json')==spec['snapshot_sha256'],'snapshot'
        for filename,digest in spec['input_hashes'].items():assert sha(Path(filename))==digest,filename
        cpus=[cpu for group in resources['logical_cpus_by_physical_core'] for cpu in group[:2]]
        env={key:value for key,value in os.environ.items() if not key.startswith(('R05_','CGAR_','WPPL_'))}
        env.update(spec['case']['env']);env['R05_SNAPSHOT_EVERY']='0';env['R05_THREADS']='32'
        env['OMP_NUM_THREADS']='32';env['OMP_DYNAMIC']='FALSE'
        results=[]
        for seed in spec['future_seeds']:
            command=['/usr/bin/prlimit','--as=32000000000','--','/usr/bin/taskset','-c',','.join(map(str,cpus)),
                str(out/'probe'),str(out/'snapshot.json'),spec['case']['input'],str(out/('seed'+str(seed)+'.json')),
                str(spec['future_k']),str(spec['steps']),str(seed)]
            with (out/('seed'+str(seed)+'.log')).open('w') as log:
                result=subprocess.run(command,cwd=out,env=env,stdout=log,stderr=subprocess.STDOUT)
            results.append(dict(seed=seed,exit=result.returncode,command=command))
            write(out/'summary.json',results)
            if result.returncode:raise RuntimeError('probe seed {} failed'.format(seed))
        write(out/'completion.json',dict(finished_utc=now(),valid=True,exit=0))
        return 0
    except Exception as error:
        write(out/'completion.json',dict(finished_utc=now(),valid=False,error=str(error),exit=1))
        raise
if __name__=='__main__':raise SystemExit(main())
