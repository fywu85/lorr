#!/usr/bin/env python3
"""Freeze and run a complete-pickup indexing microbenchmark on a bound GRID core."""
import argparse,datetime,hashlib,json,os,shlex,shutil,subprocess,sys
from pathlib import Path
ROOT=next(p for p in Path(__file__).resolve().parents if (p/'tools/cpu_resources.py').is_file())
def write(p,x):p.write_text(json.dumps(x,indent=2)+'\n')
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--raw',type=Path,required=True);p.add_argument('--execute',action='store_true');a=p.parse_args();raw=a.raw.resolve()
 if not a.execute:
  raw.mkdir(parents=True,exist_ok=False)
  copies={'run.py':Path(__file__),'benchmark.cpp':Path(__file__).with_name('benchmark.cpp'),
   'pickup-before.hpp':ROOT/'runs/cgar-native-match-build-v75-20260920/sources/cgar/cgar_planner/pickup_full.hpp',
   'pickup-original.hpp':ROOT/'runs/cgar-trick-score-match-build-v72-20260920/sources/cgar/cgar_planner/pickup_full.hpp',
   'pickup-after.hpp':ROOT/'cgar/cgar_planner/pickup_full.hpp',
   'warehouse_lanes.hpp':ROOT/'cgar/tricks/warehouse_lanes.hpp','warehouse_native.hpp':ROOT/'cgar/tricks/warehouse_native.hpp'}
  for name,source in copies.items():shutil.copy2(source,raw/name)
  write(raw/'request.json',dict(created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),root=str(ROOT),files={name:sha(raw/name) for name in copies},before_source='1c936a018d61debabaae4bcd5bf38db2c8143e6e',original_source='c696d5f9d5b9b5bc6557830e6e43680913b3e7b7',after_source='pending V77 build binding'))
  command=['/usr/bin/python3',str(raw/'run.py'),'--execute','--raw',str(raw)];job=raw/'job.sh';job.write_text('#!/bin/bash\nset -eu\nexec '+' '.join(map(shlex.quote,command))+'\n')
  submit=['qsub','-h','-terse','-w','n','-cwd','-q','debian.q','-pe','threaded','1','-binding','linear:1','-l','exclusive=false,h_rt=00:10:00,h_vmem=2G','-m','n','-N','pickup_ring_bench','-j','y','-o',str(raw/'job.log'),'-S','/bin/bash',str(job)]
  r=subprocess.run(submit,capture_output=True,text=True);write(raw/'submission.json',dict(command=submit,returncode=r.returncode,stdout=r.stdout,stderr=r.stderr));print(r.stdout,end='');r.check_returncode();assert r.stdout.strip().isdigit();subprocess.run(['qrls',r.stdout.strip()],check=True);return
 request=json.loads((raw/'request.json').read_text());assert Path(request['root'])==ROOT
 for name,want in request['files'].items():assert sha(raw/name)==want,name
 sys.path.insert(0,str(ROOT/'tools'));from cpu_resources import cpu_resources
 resources=cpu_resources();assert resources['effective_cpu_quota'] is None;cpu=resources['representative_cpus'][0];os.sched_setaffinity(0,{cpu})
 write(raw/'allocation.json',dict(resources=resources,selected_cpu=cpu,job_id=os.environ['JOB_ID']))
 command=['/usr/bin/g++','-O3','-DNDEBUG','-std=c++17',str(raw/'benchmark.cpp'),'-o',str(raw/'benchmark')];subprocess.run(command,check=True)
 result=subprocess.run([str(raw/'benchmark')],text=True,capture_output=True);result.check_returncode();data=json.loads(result.stdout)
 data.update(completed_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),compiler_command=command,binary_sha256=sha(raw/'benchmark'),returncode=result.returncode)
 write(raw/'comparison.json',data);print(json.dumps(data,indent=2))
if __name__=='__main__':main()
