#!/usr/bin/env python3
"""Read-only audit of CGAR's dense-map task eligibility against actual run events."""
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
SOURCE='8352226'
BASELINE='5d3c5bf'

def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def write(p,x):p.write_text(json.dumps(x,indent=2)+'\n')
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--raw',type=Path,required=True);p.add_argument('--execute',action='store_true');a=p.parse_args();out=a.raw.resolve()
 if not a.execute:
  out.mkdir(exist_ok=False);shutil.copy2(Path(__file__),out/'audit.py')
  current=subprocess.check_output(['git','show',SOURCE+':cgar/cgar_planner/cgar.cpp'],cwd=ROOT).decode().split('void DistanceOracle::init')[0]
  before=subprocess.check_output(['git','show',BASELINE+':cgar/cgar_planner/cgar.cpp'],cwd=ROOT).decode().split('void DistanceOracle::init')[0]
  assert current==before, 'certificate code changed across compared solver sources'
  main=r'''
} // namespace cgar
#include <iostream>
#include <fstream>
int main(int argc,char** argv) {
 if(argc!=2)return 2;
 std::ifstream f(argv[1]);std::string line;for(int i=0;i<4;++i)std::getline(f,line);
 std::vector<int> map;while(std::getline(f,line))for(char ch:line)if(ch!='\r')map.push_back(ch=='@'||ch=='T');
 if(map.size()!=1024)return 3;
 for(int n:{700,800}) {
  const auto c=cgar::build_certificate_feasible(map,32,32,n);
  std::cout<<n<<' '<<c.core_size<<' '<<c.valid<<' '<<c.capacity<<' ';
  for(char x:c.core)std::cout<<int(x);std::cout<<'\n';
 }
}
'''
  (out/'certificate.cpp').write_text(current+main)
  build=json.loads((ROOT/'runs/cgar-random-native-build-v1-r2-20260920/requested.json').read_text())
  inputs={name:str(ROOT/('runs/cgar-priority-portfolio-strict-seeds-v2-20260920/generic_noise50_cold-s0-r0/'+name+'.json')) for name in ['RANDOM-04','RANDOM-05']}
  write(out/'request.json',dict(source=SOURCE,baseline=BASELINE,certificate_prefix_identical=True,sources=build['sources'],result_files=inputs,result_sha256={n:sha(Path(x)) for n,x in inputs.items()},helper_sha256=sha(out/'audit.py'),certificate_translation_unit_sha256=sha(out/'certificate.cpp')))
  cmd=['/usr/bin/python3',str(out/'audit.py'),'--raw',str(out),'--execute'];script=out/'job.sh';script.write_text('#!/bin/bash\nset -eu\nexec '+' '.join(map(shlex.quote,cmd))+'\n')
  submit=['qsub','-h','-terse','-w','e','-cwd','-q','debian.q@research43.grid.gsb,debian.q@research44.grid.gsb,debian.q@research57.grid.gsb','-pe','threaded','1','-binding','linear:1','-l','exclusive=false,h_rt=00:10:00,h_vmem=4G','-m','n','-N','cgar_capacity_audit','-j','y','-o',str(out/'job.log'),'-S','/bin/bash',str(script)]
  r=subprocess.run(submit,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True);write(out/'submission.json',dict(command=submit,returncode=r.returncode,response=r.stdout));r.check_returncode();print(r.stdout,end='',flush=True);subprocess.run(['qrls',r.stdout.strip()],check=True);return
 request=json.loads((out/'request.json').read_text());assert sha(out/'audit.py')==request['helper_sha256'];assert sha(out/'certificate.cpp')==request['certificate_translation_unit_sha256']
 def check():
  for name,h in request['sources'].items():assert sha(ROOT/name)==h,name
  for name,path in request['result_files'].items():assert sha(Path(path))==request['result_sha256'][name]
 check();sys.path.insert(0,str(ROOT/'tools'));from cpu_resources import cpu_resources
 resources=cpu_resources();write(out/'allocation.json',resources);assert resources['physical_cores_visible']==1 and resources['effective_cpu_quota'] is None
 os.sched_setaffinity(0,resources['representative_cpus'])
 cmd=[str(ROOT/'env/bin/x86_64-conda-linux-gnu-c++'),'-std=c++17','-O2','-pthread','-I'+str(ROOT/'cgar/inc'),'-I'+str(ROOT/'cgar/cgar_planner'),'-isystem',str(ROOT/'env/include'),str(out/'certificate.cpp'),'-o',str(out/'certificate')]
 subprocess.run(cmd,check=True)
 mapfile=ROOT/'mr24/random.domain/maps/random-32-32-20.map';text=subprocess.check_output([str(out/'certificate'),str(mapfile)],text=True);(out/'certificate.txt').write_text(text)
 certs={}
 for line in text.splitlines():
  n,size,valid,capacity,mask=line.split();assert len(mask)==1024;certs[int(n)]=dict(core_size=int(size),valid=bool(int(valid)),capacity=int(capacity),core=[c=='1' for c in mask])
 rows=[]
 for name,path in request['result_files'].items():
  d=json.loads(Path(path).read_text());cert=certs[d['teamSize']];core=cert['core'];tasks={x[0]:x for x in d['tasks']}
  rejected={tid for tid,t in tasks.items() if any(not core[r*32+c] for r,c in zip(t[2][0::2],t[2][1::2]))}
  completed={tid for tick,robot,tid,stage in d['events'] if stage==len(tasks[tid][2])//2}
  assigned={int(e.split(':')[1]) for line in d['actualSchedule'] for e in line.split(',') if e and int(e.split(':')[1])>=0}
  outstanding=set(tasks)-completed;never=set(tasks)-assigned
  start_outside=sum(not core[int(x[0])*32+int(x[1])] for x in d['start'])
  if name=='RANDOM-05':assert not (assigned&rejected) and not (completed&rejected) and start_outside==31
  assert len(completed)==d['numTaskFinished']
  rows.append(dict(instance=name,team_size=d['teamSize'],tasks_completed=len(completed),revealed_tasks=len(tasks),certified_core_cells=cert['core_size'],free_cells=819,certificate_valid=cert['valid'],capacity=cert['capacity'],initial_robots_outside_core=start_outside,capacity_filter_active=name=='RANDOM-05',tasks_with_any_stop_outside_core=len(rejected),initial_tasks_with_stop_outside_core=sum(tasks[t][1]==0 for t in rejected),outside_core_tasks_assigned=len(rejected&assigned),outside_core_tasks_completed=len(rejected&completed),outstanding_tasks=len(outstanding),outstanding_outside_core_tasks=len(outstanding&rejected),never_assigned_tasks=len(never),never_assigned_outside_core_tasks=len(never&rejected),eligible_outstanding_tasks=len(outstanding-rejected)))
 check();write(out/'audit.json',dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),source=SOURCE,baseline=BASELINE,certificate_prefix_identical=True,rows=rows,scope='Structural admission exclusion measured from the actual certificate and full seed0 run events. Not a counterfactual throughput gain; no solver or standalone source changes.'))
 print(json.dumps(rows,indent=2),flush=True)
if __name__=='__main__':main()
