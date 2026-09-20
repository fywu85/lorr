#!/usr/bin/env python3
"""Build and validate on reserved cores, recording frozen source and binary provenance."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import shlex
import shutil
import subprocess
import sys
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'tools'))
from benchmark_gridengine import SOURCES,write
from cpu_resources import cpu_resources


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--output',required=True,type=Path)
    p.add_argument('--memory-gib-per-slot',type=int,default=4)
    p.add_argument('--shared-host',action='store_true',help='Allow a shared GRID host for compilation and functional tests')
    p.add_argument('--execute',action='store_true')
    args=p.parse_args();out=args.output.resolve()
    if args.memory_gib_per_slot < 1:p.error('memory must be positive')
    if not args.execute:
        out.mkdir(parents=True,exist_ok=False)
        sources={s:hashlib.sha256((ROOT/s).read_bytes()).hexdigest() for s in SOURCES+['cgar/tests/cgar_regression.cpp']}
        for name in sources:
            dest=out/'sources'/name;dest.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(ROOT/name,dest)
        write(out/'requested.json',{'sources':sources,'commit':subprocess.check_output(['git','rev-parse','HEAD'],cwd=str(ROOT),text=True).strip(), 'source_dirty':bool(subprocess.check_output(['git','status','--porcelain'],cwd=str(ROOT),text=True)), 'shared_build_host':args.shared_host})
        command=['/usr/bin/python3',str(Path(__file__).resolve()),'--execute','--output',str(out)]
        (out/'job.sh').write_text('#!/bin/bash\nset -eu\nexec '+' '.join(shlex.quote(s) for s in command)+'\n')
        submit=['/opt/n1ge/bin/lx24-amd64/qsub','-terse','-w','e','-cwd','-q','debian.q','-pe','threaded','4','-binding','linear:4','-l','exclusive='+('false' if args.shared_host else 'true')+',h_rt=00:20:00,h_vmem='+str(args.memory_gib_per_slot)+'G','-m','n','-N','lorr_build','-j','y','-o',str(out/'build.log'),'-S','/bin/bash',str(out/'job.sh')]
        r=subprocess.run(submit,cwd=str(ROOT),text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
        write(out/'submission.json',{'command':submit,'returncode':r.returncode,'response':r.stdout});print(r.stdout,end='');return r.returncode
    spec=json.loads((out/'requested.json').read_text())
    def check_sources():
        for name,sha in spec['sources'].items(): assert hashlib.sha256((ROOT/name).read_bytes()).hexdigest()==sha,name
    check_sources();write(out/'allocation.json',cpu_resources())
    env={k:v for k,v in os.environ.items() if not k.startswith('CGAR_')};env['PATH']=str(ROOT/'env/bin')+':'+env['PATH']
    subprocess.run(['cmake','--build','cgar/build','--target','lifelong','cgar_regression','-j4'],cwd=str(ROOT),env=env,check=True)
    subprocess.run([str(ROOT/'cgar/build/cgar_regression')],cwd=str(ROOT),env=env,check=True)
    check_sources();shutil.copy2(ROOT/'cgar/build/lifelong',out/'lifelong')
    write(out/'build.json',{'source_commit':spec['commit'],'source_dirty':spec['source_dirty'],'sources':{name:spec['sources'][name] for name in SOURCES},'test_sources':{'cgar/tests/cgar_regression.cpp':spec['sources']['cgar/tests/cgar_regression.cpp']},'binary_sha256':hashlib.sha256((out/'lifelong').read_bytes()).hexdigest()})
    return 0


if __name__=='__main__':raise SystemExit(main())
