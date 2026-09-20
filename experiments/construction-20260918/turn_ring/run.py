#!/usr/bin/env python3
"""Freeze and run a backward-Dial indexing microbenchmark on a bound GRID core."""
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
   'cgar-before.cpp':ROOT/'runs/cgar-native-short-build-v80-20260920/sources/cgar/cgar_planner/cgar.cpp',
   'cgar-after.cpp':ROOT/'runs/cgar-turn-ring-build-v81-20260920/sources/cgar/cgar_planner/cgar.cpp',
   'warehouse_lanes.hpp':ROOT/'cgar/tricks/warehouse_lanes.hpp','warehouse_native.hpp':ROOT/'cgar/tricks/warehouse_native.hpp'}
  for name,source in copies.items():shutil.copy2(source,raw/name)
  excerpts={}
  for which in ['before','after']:
   source=(raw/('cgar-'+which+'.cpp')).read_text();start=source.index('std::vector<int> TurnDistanceOracle::compute(');end=source.index('\nconst TurnTable* TurnDistanceOracle::table(',start)
   body=source[start:end];assert body.count('TurnDistanceOracle::compute')==1
   text=body.replace('TurnDistanceOracle::compute','BenchOracle::'+which)
   name='turn-'+which+'.inc';(raw/name).write_text(text);excerpts[name]=dict(source='cgar-'+which+'.cpp',source_sha256=sha(raw/('cgar-'+which+'.cpp')),original_body_sha256=hashlib.sha256(body.encode()).hexdigest())
  files=list(copies)+list(excerpts)
  write(raw/'request.json',dict(created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),root=str(ROOT),files={name:sha(raw/name) for name in files},excerpts=excerpts,before_source='fd2d23b5e2920959675e40e0f2f2c93aa1709c4a',after_source='pending V81 build binding'))
  command=['/usr/bin/python3',str(raw/'run.py'),'--execute','--raw',str(raw)];job=raw/'job.sh';job.write_text('#!/bin/bash\nset -eu\nexec '+' '.join(map(shlex.quote,command))+'\n')
  submit=['qsub','-h','-terse','-w','n','-cwd','-q','debian.q@research31,debian.q@research38,debian.q@research44,debian.q@research46,debian.q@research50,debian.q@research52,debian.q@research57','-pe','threaded','1','-binding','linear:1','-l','exclusive=false,h_rt=00:10:00,h_vmem=2G','-m','n','-N','turn_ring_bench','-j','y','-o',str(raw/'job.log'),'-S','/bin/bash',str(job)]
  r=subprocess.run(submit,capture_output=True,text=True);write(raw/'submission.json',dict(command=submit,returncode=r.returncode,stdout=r.stdout,stderr=r.stderr));print(r.stdout,end='');r.check_returncode();assert r.stdout.strip().isdigit();subprocess.run(['qrls',r.stdout.strip()],check=True);return
 request=json.loads((raw/'request.json').read_text());assert Path(request['root'])==ROOT
 for name,want in request['files'].items():assert sha(raw/name)==want,name
 sys.path.insert(0,str(ROOT/'tools'));from cpu_resources import cpu_resources
 resources=cpu_resources();assert resources['effective_cpu_quota'] is None;assert resources['cpu_model']=='AMD EPYC 9354 32-Core Processor';cpu=resources['representative_cpus'][0];os.sched_setaffinity(0,{cpu})
 write(raw/'allocation.json',dict(resources=resources,selected_cpu=cpu,job_id=os.environ['JOB_ID']))
 command=['/usr/bin/g++','-O3','-DNDEBUG','-std=c++17',str(raw/'benchmark.cpp'),'-o',str(raw/'benchmark')];subprocess.run(command,check=True)
 result=subprocess.run([str(raw/'benchmark')],text=True,capture_output=True);result.check_returncode();data=json.loads(result.stdout)
 data.update(completed_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),compiler_command=command,binary_sha256=sha(raw/'benchmark'),returncode=result.returncode)
 write(raw/'comparison.json',data);print(json.dumps(data,indent=2))
if __name__=='__main__':main()
