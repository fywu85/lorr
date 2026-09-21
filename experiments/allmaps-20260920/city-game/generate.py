#!/usr/bin/env python3
"""Verify CITY/GAME trick fields against the unchanged archived NMS constructor."""
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
ROOT=next(p for p in Path(__file__).resolve().parents if (p/'tools/cpu_resources.py').is_file())
HERE=ROOT/'experiments/allmaps-20260920/city-game'
MAPS={'city':'mr24/city.domain/maps/Paris_1_256.map','game':'mr24/game.domain/maps/brc202d.map'}
def sha(path):return hashlib.sha256(path.read_bytes()).hexdigest()
def read(path):return json.loads(path.read_text())
def write(path,value):path.write_text(json.dumps(value,indent=2)+'\n')
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,required=True);p.add_argument('--execute',action='store_true');a=p.parse_args();out=a.output.resolve();src=ROOT/'nms/Solution'
 native=[src/('Objects/'+n) for n in ['Environment/graph_guidance.cpp','Environment/map.cpp','Environment/info.cpp','Basic/position.cpp']]
 paths=native+list(src.rglob('*.hpp'))+list((ROOT/'nms/inc').rglob('*.h'))+[ROOT/name for name in MAPS.values()]
 sources={str(x.relative_to(ROOT)):sha(x) for x in paths};support=out/'support'
 if not a.execute:
  support.mkdir(parents=True,exist_ok=False)
  for name in ['generate.py','dump_native.cpp']:shutil.copy2(HERE/name,support/name)
  write(out/'request.json',dict(sources=sources,support={n:sha(support/n) for n in ['generate.py','dump_native.cpp']}))
  command=['/usr/bin/python3',str(support/'generate.py'),'--execute','--output',str(out)]
  job=out/'job.sh';job.write_text('#!/bin/bash\nset -eu\nexec '+' '.join(map(shlex.quote,command))+'\n')
  submit=['qsub','-h','-terse','-w','e','-cwd','-q','debian.q@research43.grid.gsb,debian.q@research44.grid.gsb,debian.q@research57.grid.gsb','-pe','threaded','1','-binding','linear:1','-l','exclusive=false,h_rt=00:10:00,h_vmem=4G','-m','n','-N','cgar_city_game_asset','-j','y','-o',str(out/'job.log'),'-S','/bin/bash',str(job)]
  r=subprocess.run(submit,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True);write(out/'submission.json',dict(command=submit,returncode=r.returncode,response=r.stdout));r.check_returncode();print(r.stdout,end='',flush=True);subprocess.run(['qrls',r.stdout.strip()],check=True);return
 request=read(out/'request.json');assert request['sources']==sources
 for name,expected in request['support'].items():assert sha(support/name)==expected
 sys.path.insert(0,str(ROOT/'tools'));from cpu_resources import cpu_resources
 resources=cpu_resources();write(out/'allocation.json',resources);assert resources['effective_cpu_quota'] is None and resources['physical_cores_visible']==1
 os.sched_setaffinity(0,resources['representative_cpus'])
 compiler=ROOT/'env/bin/x86_64-conda-linux-gnu-c++'
 command=[str(compiler),'-std=c++17','-O2','-ffunction-sections','-fdata-sections','-Wl,--gc-sections','-I'+str(src),'-I'+str(ROOT/'nms/inc'),'-isystem',str(ROOT/'env/include'),str(support/'dump_native.cpp')]+list(map(str,native))+['-o',str(out/'dump_native')]
 subprocess.run(command,check=True,cwd=ROOT);records=[]
 for name,relative in MAPS.items():
  map_path=ROOT/relative;dump=out/(name+'-native-forward.txt')
  with dump.open('w') as stream:subprocess.run([str(out/'dump_native'),str(map_path),name.upper()],check=True,stdout=stream,cwd=out)
  data=dump.read_text().splitlines();rows,cols=map(int,data[0].split());assert len(data)==rows*cols+1
  text_map=''.join(map_path.read_text().splitlines()[4:]);assert len(text_map)==rows*cols
  occupancy=[];masks=[];costs=[];adapted=[]
  for cell,line in enumerate(data[1:]):
   values=list(map(int,line.split()));assert len(values)==4
   wall=text_map[cell] in '@T';occupancy.append(int(wall));r,c=divmod(cell,cols)
   if wall:
    assert values==[0]*4;masks.append('x');costs.extend([20]*4);adapted.extend([4]*4)
   else:
    # Independent parity formula, not the compiled constructor implementation.
    expected=[20]*4;expected[0 if r%2 else 2]=200;expected[1 if c%2 else 3]=200
    assert values==expected,(name,cell,values,expected)
    masks.append(format(sum(1<<d for d,v in enumerate(values) if v==200),'x'));costs.extend(values);adapted.extend(16 if v==200 else 4 for v in values)
  def digest(values):return hashlib.sha256(bytes(values)).hexdigest()
  fnv=14695981039346656037
  for value in costs:fnv=((fnv^value)*1099511628211)&((1<<64)-1)
  hashes={name+'_map_sha256':sha(map_path),name+'_occupancy_sha256':digest(occupancy),name+'_field_sha256':digest(adapted),name+'_native_nobands_field_sha256':digest(costs)}
  for opposing in [4,8,12,24,32,48,64]:
   hashes[name+'_lane'+str(opposing)+'_field_sha256']=digest([opposing if x==16 else x for x in adapted])
  header=['#pragma once','#include <cstdint>','// Generated from unchanged archived NMS; explicit instance trick activation required.','// Generator: experiments/allmaps-20260920/city-game/generate.py.','namespace cgar { namespace tricks {','inline constexpr int '+name+'_rows = '+str(rows)+', '+name+'_cols = '+str(cols)+', '+name+'_free = '+str(occupancy.count(0))+';']
  header+=['inline constexpr char '+key+'[] = "'+value+'";' for key,value in hashes.items()]
  header+=['inline constexpr uint64_t '+name+'_native_nobands_fnv1a64 = '+str(fnv)+'ULL;','inline constexpr char '+name+'_masks[] =']
  masks=''.join(masks);header+=['    "'+masks[k:k+100]+'"' for k in range(0,len(masks),100)]+['    ;','static_assert(sizeof('+name+'_masks) == '+str(rows)+' * '+str(cols)+' + 1, "'+name+' asset size");','}}','']
  dest=out/(name+'_native.hpp');dest.write_text('\n'.join(header))
  records.append(dict(name=name,rows=rows,cols=cols,free_cells=occupancy.count(0),header_sha256=sha(dest),dump_sha256=sha(dump),field_hashes=hashes,native_fnv1a64=fnv,verified_forward_values=rows*cols*4,verified_nonforward_values=rows*cols*4*3,independent_formula_check=True,direction_offsets=[1,cols,-1,-cols]))
 assert request['sources']=={str(x.relative_to(ROOT)):sha(x) for x in paths}
 write(out/'generation.json',dict(completed_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),track='TRICK',status='Verified assets only; not installed, dispatched or benchmarked in CGAR',native_source_unchanged=True,compiler_command=command,native_binary_sha256=sha(out/'dump_native'),wall_normalization='unused20 in native field; NMS stores0 on walls',records=records))
 print('CITY_GAME_FIELDS_VERIFIED',json.dumps(records),flush=True)
if __name__=='__main__':main()
