#!/usr/bin/env python3
"""Export a tuned reference field without touching the independent solver tree."""
import argparse
import datetime
import hashlib
import json
import math
import os
from pathlib import Path
import shlex
import shutil
import subprocess
import sys

ROOT=next(p for p in Path(__file__).resolve().parents if (p/'tools/cpu_resources.py').is_file())
HERE=ROOT/'experiments/allmaps-20260920/random-transfer/guidance'
REFERENCE='233f5bfc26cffef8dc039632b96391b50a55fee6'


def sha(path):return hashlib.sha256(path.read_bytes()).hexdigest()
def read(path):return json.loads(path.read_text())
def write(path,value):path.write_text(json.dumps(value,indent=2)+'\n')


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--execute',action='store_true')
    a=parser.parse_args();out=a.output.resolve();support=out/'support';source=out/'reference'
    map_path=(ROOT/'mr24/random.domain/maps/random-32-32-20.map').resolve()
    if not a.execute:
        support.mkdir(parents=True,exist_ok=False)
        for name in ['generate.py','dump.cpp']:shutil.copy2(HERE/name,support/name)
        names=subprocess.check_output(['git','ls-tree','-r','--name-only',REFERENCE,'--','random05/simulator/inc'],cwd=ROOT,text=True).splitlines()
        names+=['random05/src/engine.cpp','random05/src/engine.hpp'];hashes={}
        for name in names:
            data=subprocess.check_output(['git','show',REFERENCE+':'+name],cwd=ROOT)
            target=source/name;target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(data);hashes[name]=sha(target)
        write(out/'request.json',dict(reference_commit=REFERENCE,reference_sources=hashes,map_path=str(map_path),map_sha256=sha(map_path),support={n:sha(support/n) for n in ['generate.py','dump.cpp']},standalone_worktree_untouched=True))
        command=['/usr/bin/python3',str(support/'generate.py'),'--output',str(out),'--execute']
        job=out/'job.sh';job.write_text('#!/bin/bash\nset -eu\nexec '+' '.join(map(shlex.quote,command))+'\n')
        submit=['qsub','-h','-terse','-w','e','-cwd','-q','debian.q@research43.grid.gsb,debian.q@research44.grid.gsb,debian.q@research57.grid.gsb','-pe','threaded','4','-binding','linear:4','-l','exclusive=false,h_rt=00:15:00,h_vmem=2G','-m','n','-N','cgar_r05_field','-j','y','-o',str(out/'job.log'),'-S','/bin/bash',str(job)]
        result=subprocess.run(submit,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True)
        write(out/'submission.json',dict(command=submit,returncode=result.returncode,response=result.stdout));result.check_returncode()
        print(result.stdout,end='',flush=True);subprocess.run(['qrls',result.stdout.strip()],check=True);return
    request=read(out/'request.json');assert request['reference_commit']==REFERENCE
    def check_sources():
        assert sha(map_path)==request['map_sha256']
        for name,value in request['reference_sources'].items():assert sha(source/name)==value,name
        for name,value in request['support'].items():assert sha(support/name)==value,name
    check_sources()
    sys.path.insert(0,str(ROOT/'tools'));from cpu_resources import cpu_resources
    resources=cpu_resources();write(out/'allocation.json',resources)
    if resources['effective_cpu_quota'] is not None or resources['physical_cores_visible']!=4:
        raise RuntimeError('field export allocation rejected: expected4physicalcores withoutquota, observed '+str(resources))
    os.sched_setaffinity(0,resources['representative_cpus'])
    command=[str(ROOT/'env/bin/x86_64-conda-linux-gnu-c++'),'-std=c++17','-O2','-fopenmp','-ffunction-sections','-fdata-sections','-Wl,--gc-sections','-I'+str(source/'random05/src'),'-I'+str(source/'random05/simulator/inc'),'-isystem',str(ROOT/'env/include'),str(support/'dump.cpp'),str(source/'random05/src/engine.cpp'),'-o',str(out/'dump_field')]
    subprocess.run(command,cwd=out,check=True)
    for threads in [1,4]:
        with (out/('field-threads'+str(threads)+'.txt')).open('w') as stream:
            subprocess.run([str(out/'dump_field'),str(map_path),str(threads)],stdout=stream,cwd=out,check=True)
    assert (out/'field-threads1.txt').read_bytes()==(out/'field-threads4.txt').read_bytes(),'thread count changed the reference field'
    lines=(out/'field-threads1.txt').read_text().splitlines();assert lines[0]=='32 32 819' and len(lines)==1025
    grid=''.join(map_path.read_text().splitlines()[4:]);assert len(grid)==1024
    floats=[];integers=[];occupancy=[];errors=[];valid_edges=[]
    for cell,line in enumerate(lines[1:]):
        values=list(map(float,line.split()));assert len(values)==5
        wall=grid[cell] in '@T';occupancy.append(int(wall));floats.extend(values)
        assert all(math.isfinite(v) and (v==0 if wall else v>0) for v in values)
        if not wall:assert abs(values[4]-.6)<1e-6
        for direction,value in enumerate(values[:4]):
            integer=20 if wall else int(math.floor(10*value+.5))
            assert 1<=integer<=255
            integers.append(integer)
            if not wall:errors.append(abs(integer/10.0-value))
            row,col=divmod(cell,32);dr,dc=[(0,1),(1,0),(0,-1),(-1,0)][direction];r,c=row+dr,col+dc
            if not wall and 0<=r<32 and 0<=c<32 and grid[r*32+c] not in '@T':valid_edges.append(value)
    assert occupancy.count(0)==819 and max(errors)<=.0500001
    assert abs(sum(valid_edges)/len(valid_edges)-2)<1e-5
    write(out/'field-float.json',dict(rows=32,cols=32,free_cells=819,cell_weights=floats,directions=['east','south','west','north','turn']))
    write(out/'field-integer.json',dict(rows=32,cols=32,free_cells=819,forward=integers,turn=6,scale=10,wall_forward=20,occupancy=occupancy))
    check_sources()
    write(out/'generation.json',dict(completed_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),reference_commit=REFERENCE,standalone_worktree_untouched=True,compiler_command=command,binary_sha256=sha(out/'dump_field'),native_text_sha256=sha(out/'field-threads1.txt'),native_float_json_sha256=sha(out/'field-float.json'),adapted_json_sha256=sha(out/'field-integer.json'),adapted_forward_bytes_sha256=hashlib.sha256(bytes(integers)).hexdigest(),occupancy_bytes_sha256=hashlib.sha256(bytes(occupancy)).hexdigest(),worker_counts_identical=[1,4],forward_values=4096,free_cells=819,scale=10,turn=6,maximum_rounding_error=max(errors),mean_valid_forward_cost=sum(valid_edges)/len(valid_edges),adapted_min=min(integers),adapted_max=max(integers),cgar_integration=False,quality_claim=False))
    print('REFERENCE_RANDOM_FIELD_EXPORTED_AND_ADAPTED',max(errors),min(integers),max(integers),flush=True)


if __name__=='__main__':main()
