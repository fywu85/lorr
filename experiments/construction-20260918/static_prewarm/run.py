#!/usr/bin/env python3
"""Freeze an isolated all-goal native prewarm feasibility probe; no solver edits."""
import argparse,datetime,hashlib,json,os,re,shlex,shutil,subprocess,sys
from pathlib import Path
ROOT=next(p for p in Path(__file__).resolve().parents if (p/'tools/cpu_resources.py').is_file())
BASE=ROOT/'experiments/construction-20260918'
def read(p):return json.loads(p.read_text())
def write(p,x):p.write_text(json.dumps(x,indent=2)+'\n')
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--raw',type=Path,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--execute',action='store_true');p.add_argument('--threads',type=int,choices=[4,8,16],default=4);a=p.parse_args();raw=a.raw.resolve();out=a.output.resolve()
 if not a.execute:
  raw.mkdir(exist_ok=False,parents=True);build=ROOT/'runs/cgar-native-turn-build-v113-20260920';b=read(build/'build.json')
  copies={'run.py':Path(__file__),'probe.cpp':Path(__file__).with_name('probe.cpp'),'build.json':build/'build.json','warehouse.map':ROOT/'mr24/warehouse.domain/maps/warehouse_large.map','cpu_resources.py':ROOT/'tools/cpu_resources.py'}
  for name,h in dict(b['sources'],**b['test_sources']).items():
   source=build/'sources'/name;assert sha(source)==h;copies[name]=source
  for source in (ROOT/'cgar/inc').rglob('*'):
   if source.is_file():copies.setdefault(str(source.relative_to(ROOT)),source)
  for name,source in copies.items():target=raw/name;target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(source,target)
  write(raw/'request.json',dict(created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),root=str(ROOT),output=str(out),files={n:sha(raw/n) for n in copies},source_commit=b['source_commit'],threads=a.threads,mode='isolated_feasibility_only'))
  cmd=['/usr/bin/python3',str(raw/'run.py'),'--raw',str(raw),'--output',str(out),'--threads',str(a.threads),'--execute'];job=raw/'job.sh';job.write_text('#!/bin/bash\nset -eu\nexec '+' '.join(map(shlex.quote,cmd))+'\n')
  submit=['qsub','-h','-terse','-w','e','-cwd','-q','debian.q@research44.grid.gsb,debian.q@research57.grid.gsb','-pe','threaded',str(a.threads),'-binding','linear:'+str(a.threads),'-l','exclusive=false,h_rt=00:15:00,h_vmem='+str(32//a.threads)+'G','-m','n','-N','native_prewarm_probe','-j','y','-o',str(raw/'job.log'),'-S','/bin/bash',str(job)]
  r=subprocess.run(submit,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True);write(raw/'submission.json',dict(command=submit,returncode=r.returncode,response=r.stdout));r.check_returncode();assert re.fullmatch(r'\d+\s*',r.stdout);print(r.stdout,end='');subprocess.run(['qrls',r.stdout.strip()],check=True);return
 request=read(raw/'request.json');assert request['output']==str(out) and request['threads']==a.threads
 for name,h in request['files'].items():assert sha(raw/name)==h,name
 sys.path.insert(0,str(raw));from cpu_resources import cpu_resources
 cpu=cpu_resources();assert cpu['effective_cpu_quota'] is None and cpu['physical_cores_visible']==a.threads;assert cpu['cpu_model']=='AMD EPYC 9354 32-Core Processor';os.sched_setaffinity(0,cpu['representative_cpus']);write(raw/'allocation.json',cpu)
 b=read(raw/'build.json')
 for name,h in dict(b['sources'],**b['test_sources']).items():assert sha(raw/name)==h
 compiler=ROOT/'env/bin/x86_64-conda-linux-gnu-c++';command=[str(compiler),'-O3','-DNDEBUG','-std=c++17','-pthread','-I'+str(raw/'cgar/cgar_planner'),'-I'+str(raw/'cgar/inc'),'-isystem',str(ROOT/'env/include'),'-Wl,-rpath,'+str(ROOT/'env/lib'),str(raw/'probe.cpp'),str(raw/'cgar/cgar_planner/cgar.cpp'),'-o',str(raw/'probe')]
 subprocess.run(command,check=True);subprocess.run([str(raw/'probe'),str(raw/'warehouse.map'),str(raw/'report.json'),str(a.threads)],check=True)
 report=read(raw/'report.json');assert report['all_valid'] and report['offline_only'] and report['goals']==38586 and report['threads']==a.threads and report['independent_heap_states']==4*4*38586
 for name,h in request['files'].items():assert sha(raw/name)==h,name
 out.mkdir(exist_ok=False,parents=True)
 for name in ['request.json','submission.json','allocation.json','report.json']:shutil.copy2(raw/name,out/name)
 write(out/'verification.json',dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),all_valid=True,offline_only=True,source_commit=b['source_commit'],source_files_verified=len(b['sources'])+len(b['test_sources']),binary_sha256=sha(raw/'probe'),compile_command=command,job_id=os.environ['JOB_ID'],limits=report['limits']))
if __name__=='__main__':main()
