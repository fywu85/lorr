#!/usr/bin/env python3
"""Verify unchanged generic matching trajectory after native-metric capability changes."""
import argparse,datetime,hashlib,json,os,re,shlex,shutil,subprocess,sys
from pathlib import Path
ROOT=next(p for p in Path(__file__).resolve().parents if (p/'tools/cpu_resources.py').is_file())
BASE=ROOT/'experiments/construction-20260918'
def read(p):return json.loads(p.read_text())
def write(p,d):p.write_text(json.dumps(d,indent=2)+'\n')
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--raw',type=Path,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--commit',required=True);p.add_argument('--hold-job');p.add_argument('--execute',action='store_true');a=p.parse_args();raw=a.raw.resolve();out=a.output.resolve();support=raw/'generic-control-support'
 if not a.execute:
  support.mkdir(exist_ok=False);files=[]
  for src,name in [(Path(__file__),'analyze_generic.py'),(BASE/'collect_cold.py','collect_cold.py'),(BASE/'results/match-quota-screen-v64/trajectory-fingerprints.json','reference.json'),(BASE/'warehouse-reference-variants.json','profile.json')]:
   dest=support/name;shutil.copy2(src,dest);files.append(dest)
  write(raw/'generic-control-request.json',dict(commit=a.commit,files={str(f):sha(f) for f in files}))
  cmd=['/usr/bin/python3',str(support/'analyze_generic.py'),'--raw',str(raw),'--output',str(out),'--commit',a.commit,'--execute'];job=raw/'generic-control.sh';job.write_text('#!/bin/bash\nset -eu\nexec '+' '.join(map(shlex.quote,cmd))+'\n')
  submit=['qsub','-h','-terse','-w','n','-cwd','-q','debian.q','-pe','threaded','1','-binding','linear:1','-l','exclusive=false,h_rt=00:20:00,h_vmem=8G','-m','n','-N','native_generic_check','-j','y','-o',str(raw/'generic-control.log'),'-S','/bin/bash']
  if a.hold_job:submit+=['-hold_jid',a.hold_job]
  submit.append(str(job));r=subprocess.run(submit,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True);write(raw/'generic-control-submission.json',dict(command=submit,returncode=r.returncode,response=r.stdout));print(r.stdout,end='')
  if r.returncode:return r.returncode
  assert re.fullmatch(r'\d+\s*',r.stdout);subprocess.run(['qrls',r.stdout.strip()],check=True);return 0
 request=read(raw/'generic-control-request.json');assert request['commit']==a.commit
 for name,digest in request['files'].items():assert sha(Path(name))==digest
 sys.path.insert(0,str(ROOT/'tools'));from cpu_resources import cpu_resources
 cpu=cpu_resources();assert cpu['effective_cpu_quota'] is None;os.sched_setaffinity(0,cpu['representative_cpus'][:1])
 spec=read(raw/'spec.json');build=read(raw/'build.json');assert spec['trick'] is None and spec['experiment_track']=='GENERIC'
 assert spec['horizons']=={'WAREHOUSE':800} and spec['instances']==['WAREHOUSE'] and spec['time_limit_ms']==5000 and spec['cpus_per_instance']==4
 assert len(spec['cases'])==1;case=spec['cases'][0];assert case['seed']==0 and case['environment']==next(iter(read(support/'profile.json').values()))
 sources=dict(build['sources'],**build['test_sources'])
 for name,digest in sources.items():assert hashlib.sha256(subprocess.check_output(['git','show',a.commit+':'+name],cwd=ROOT)).hexdigest()==digest
 assert sha(raw/'lifelong')==build['binary_sha256']
 name=case['name'];summary=read(raw/name/'summary.json')[0];meta=read(raw/name/'metadata.json');allocation=read(raw/'allocation.json')
 assert summary['valid'] and summary['makespan']==summary['entry_compute_samples']==800 and summary['entry_compute_max_seconds']<=5 and summary['peak_process_rss_bytes']<32000000000
 assert summary['trick_receipt_valid'] and summary['experiment_track_valid'] and meta['trick'] is None and not meta['trick_argv']
 assert meta['build_provenance']==build and meta['max_process_memory_bytes']==32000000000
 resources=meta['cpu_resources'];assert resources['effective_cpu_quota'] is None and resources['cpu_model']=='AMD EPYC 9354 32-Core Processor'
 binding=summary['cpu'];assert binding==meta['cpu_binding'] and len(binding)==4 and set(binding).issubset(allocation['selected_cpus'])
 core_of={c:tuple(group) for group in resources['logical_cpus_by_physical_core'] for c in group};assert len({core_of[c] for c in binding})==4
 log=(raw/name/'WAREHOUSE.log').read_text();assert not any(line.startswith(('[CGAR_TRICK','[cgar-native-metric]')) for line in log.splitlines())
 subprocess.run(['/usr/bin/python3',str(support/'collect_cold.py'),'--input',str(raw),'--output',str(out)],check=True)
 fingerprint=read(out/'trajectory-fingerprints.json')[name];assert fingerprint==read(support/'reference.json')['match_g64-s0-r0']
 result=dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),source_commit=a.commit,source_files_verified=len(sources),binary_sha256=build['binary_sha256'],all_valid=True,exact_generic_prefix=True,steps=800,seed=0,trajectory_sha256=fingerprint,decision_limit_ms=5000,competition_budget_confirmed=False,scope='Default generic matching behavior is exactly unchanged across800steps; startup validation, not a new full-run quality score.')
 for name in ['generic-control-request.json','generic-control-submission.json']:shutil.copy2(raw/name,out/name)
 write(out/'comparison.json',result);print('NATIVE_GENERIC_CONTROL_VERIFIED',json.dumps(result));return 0
if __name__=='__main__':raise SystemExit(main())
