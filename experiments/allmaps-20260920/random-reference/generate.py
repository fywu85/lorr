#!/usr/bin/env python3
"""Freeze and verify NMS/KK RANDOM guidance assets; does not install them."""
import argparse,datetime,hashlib,json,os,shlex,shutil,subprocess,sys
from pathlib import Path
ROOT=next(p for p in Path(__file__).resolve().parents if (p/'tools/cpu_resources.py').is_file())
HERE=ROOT/'experiments/allmaps-20260920/random-reference'
MAP=ROOT/'mr24/random.domain/maps/random-32-32-20.map'
WEIGHTS={'kk02':'random_weight_101.w','kk03':'random_weight_040.w','kk04':'random_700_w002.w'}
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def read(p):return json.loads(p.read_text())
def write(p,x):p.write_text(json.dumps(x,indent=2)+'\n')
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,required=True);p.add_argument('--execute',action='store_true');a=p.parse_args();out=a.output.resolve();support=out/'support';src=ROOT/'nms/Solution'
 native=[src/('Objects/'+n) for n in ['Environment/graph_guidance.cpp','Environment/guidance_map.cpp','Environment/map.cpp','Environment/info.cpp','Basic/position.cpp']]
 arrows=src/'Data/guidance_map_random.txt'
 paths=native+list(src.rglob('*.hpp'))+list((ROOT/'nms/inc').rglob('*.h'))+[MAP,arrows]+[ROOT/'kk/data/map_weights'/x for x in WEIGHTS.values()]
 sources={str(x.relative_to(ROOT)):sha(x) for x in paths}
 if not a.execute:
  support.mkdir(parents=True,exist_ok=False)
  for n in ['generate.py','dump_native.cpp']:shutil.copy2(HERE/n,support/n)
  write(out/'request.json',dict(sources=sources,support={n:sha(support/n) for n in ['generate.py','dump_native.cpp']}))
  cmd=['/usr/bin/python3',str(support/'generate.py'),'--output',str(out),'--execute'];job=out/'job.sh';job.write_text('#!/bin/bash\nset -eu\nexec '+' '.join(map(shlex.quote,cmd))+'\n')
  cmd=['qsub','-h','-terse','-w','e','-cwd','-q','debian.q@research43.grid.gsb,debian.q@research44.grid.gsb,debian.q@research57.grid.gsb','-pe','threaded','1','-binding','linear:1','-l','exclusive=false,h_rt=00:10:00,h_vmem=4G','-m','n','-N','cgar_random_refs','-j','y','-o',str(out/'job.log'),'-S','/bin/bash',str(job)]
  r=subprocess.run(cmd,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,universal_newlines=True);write(out/'submission.json',dict(command=cmd,returncode=r.returncode,response=r.stdout));r.check_returncode();print(r.stdout,end='',flush=True);subprocess.run(['qrls',r.stdout.strip()],check=True);return
 request=read(out/'request.json');assert request['sources']==sources
 for n,h in request['support'].items():assert sha(support/n)==h
 sys.path.insert(0,str(ROOT/'tools'));from cpu_resources import cpu_resources
 allocation=cpu_resources();write(out/'allocation.json',allocation);assert allocation['physical_cores_visible']==1 and allocation['effective_cpu_quota'] is None
 os.sched_setaffinity(0,allocation['representative_cpus'])
 compiler=ROOT/'env/bin/x86_64-conda-linux-gnu-c++';cmd=[str(compiler),'-std=c++17','-O2','-ffunction-sections','-fdata-sections','-Wl,--gc-sections','-I'+str(src),'-I'+str(ROOT/'nms/inc'),'-isystem',str(ROOT/'env/include'),str(support/'dump_native.cpp')]+list(map(str,native))+['-o',str(out/'dump_native')]
 subprocess.run(cmd,check=True);dump=out/'nms-forward.txt'
 with dump.open('w') as f:subprocess.run([str(out/'dump_native'),str(MAP),str(arrows)],stdout=f,check=True)
 occupancy=[int(c in '@T') for c in ''.join(MAP.read_text().splitlines()[4:])];assert len(occupancy)==1024 and occupancy.count(0)==819
 lines=arrows.read_text().splitlines();assert lines[0].split()==['32','32'];layout=''.join(lines[1:33]);assert len(layout)==1024
 raw=[list(map(int,l.split())) for l in dump.read_text().splitlines()];assert len(raw)==1024
 nms=[]
 for cell,row in enumerate(raw):
  assert len(row)==4
  if occupancy[cell]:assert row==[0]*4;nms.extend([20]*4);continue
  arrow=layout[cell];assert arrow in '.>v<^'
  expected=[2 if arrow=='.' or '>v<^'[d]==arrow else 6 for d in range(4)]
  assert row==expected,(cell,row,expected);nms.extend(x*10 for x in row)
 records=[];header=['#pragma once','#include <cstdint>','// Verified NMS arrows and KK forward-field adaptations; explicit --trick required.','// Generator: experiments/allmaps-20260920/random-reference/generate.py.','namespace cgar { namespace tricks {']
 def add(name,costs,extra):
  assert len(costs)==4096 and min(costs)>0 and max(costs)<256
  fnv=14695981039346656037
  for x in costs:fnv=((fnv^x)*1099511628211)&((1<<64)-1)
  digest=hashlib.sha256(bytes(costs)).hexdigest();prefix='random_'+name
  header.extend(['inline constexpr char '+prefix+'_native_nobands_field_sha256[] = "'+digest+'";','inline constexpr uint64_t '+prefix+'_native_nobands_fnv1a64 = '+str(fnv)+'ULL;','inline constexpr uint8_t '+prefix+'_forward[] = {'])
  header.extend('    '+','.join(map(str,costs[k:k+32]))+',' for k in range(0,len(costs),32));header.extend(['};','static_assert(sizeof('+prefix+'_forward)==4096,"random reference field size");'])
  records.append(dict(name=name,field_sha256=digest,fnv1a64=fnv,min_cost=min(costs),max_cost=max(costs),forward_values=4096,**extra))
 add('nms',nms,dict(kind='exact uniformly scaled NMS guidance',cost_scale=10,turn_cost=20,original_turn_cost=2,nonforward_values_checked=1024*4*3,independent_arrow_formula=True))
 for name,filename in WEIGHTS.items():
  values=read(ROOT/'kk/data/map_weights'/filename);assert len(values)==5120
  costs=[];error=0;turn=[]
  for cell in range(1024):
   if occupancy[cell]:costs.extend([20]*4);continue
   for d in range(4):
    scaled=20*values[cell*5+d];quant=int(round(scaled));error=max(error,abs(scaled-quant));costs.append(quant)
   turn.append(values[cell*5+4])
  assert error<0.00001
  if name=='kk02':assert set(turn)=={1.0}
  add(name,costs,dict(kind='KK forward costs; uniform turn20 adaptation',source='kk/data/map_weights/'+filename,cost_scale=20,forward_quantization_max_abs_error=error,original_stay_min=min(turn),original_stay_max=max(turn),uniform_turn_cost=20,original_nonuniform_stay_not_transferred=len(set(turn))>1))
 header.extend(['}}','']);dest=out/'random_reference.hpp';dest.write_text('\n'.join(header))
 assert request['sources']=={str(x.relative_to(ROOT)):sha(x) for x in paths}
 write(out/'generation.json',dict(completed_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),status='Verified assets only; not yet installed or benchmarked in CGAR',track='TRICK',compiler_command=cmd,reference_binary_sha256=sha(out/'dump_native'),header_sha256=sha(dest),occupancy_sha256=hashlib.sha256(bytes(occupancy)).hexdigest(),rows=32,cols=32,free_cells=819,wall_entries_normalized=20,records=records))
 print('RANDOM_REFERENCE_FIELDS_VERIFIED',flush=True)
if __name__=='__main__':main()
