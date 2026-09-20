#!/usr/bin/env python3
"""Verify configured-horizon isolation, full controls and complete-entry resources."""
import argparse,datetime,hashlib,importlib,json,os,re,shlex,shutil,subprocess,sys
from pathlib import Path
ROOT=next(p for p in Path(__file__).resolve().parents if (p/'tools/cpu_resources.py').is_file())
BASE=ROOT/'experiments/construction-20260918'
def read(p):return json.loads(p.read_text())
def write(p,x):p.write_text(json.dumps(x,indent=2)+'\n')
def digest(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def tokens(line):return dict(x.split('=',1) for x in line.split()[1:])
def prefix(d,t):
 h=hashlib.sha256();h.update(json.dumps(d['start'],separators=(',',':')).encode())
 for path in d['actualPaths']:h.update(path[:max(0,2*t-1)].encode());h.update(b'\n')
 for line in d['actualSchedule']:
  h.update(','.join(e for e in line.split(',') if e and int(e.split(':')[0])<=t).encode());h.update(b'\n')
 h.update(json.dumps([e for e in d['events'] if e[0]<=t],separators=(',',':')).encode());return h.hexdigest()
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--raw',type=Path,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--commit',required=True);p.add_argument('--hold-job');p.add_argument('--execute',action='store_true');a=p.parse_args();raw=a.raw.resolve();out=a.output.resolve();support=raw/'known-horizon-support'
 if not a.execute:
  support.mkdir(exist_ok=False);files=[];copies={'analyze.py':Path(__file__),'profile.json':BASE/'tricks/native_metric/short-variants.json','reference.json':BASE/'results/native-short-full-v80/comparison.json','reference-screen.json':BASE/'results/turn-ring-native-screen-v81/trajectory-fingerprints.json'}
  for name in ['sequences-20260918/analyze.py','motion-20260918/analyze.py','assignment-20260918/analyze.py','throughput-20260918-next/analyze_matrix.py','throughput-20260918-strict/analyze.py','construction-20260918/verify_full.py','construction-20260918/collect_cold.py']:
   copies['experiments/'+name]=ROOT/'experiments'/name
  for name,source in copies.items():
   dest=support/name;dest.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(source,dest);files.append(dest)
  write(raw/'known-horizon-request.json',dict(root=str(ROOT),commit=a.commit,files={str(f):digest(f) for f in files}))
  command=['/usr/bin/python3',str(support/'analyze.py'),'--raw',str(raw),'--output',str(out),'--commit',a.commit,'--execute'];job=raw/'known-horizon-analysis.sh';job.write_text('#!/bin/bash\nset -eu\nexec '+' '.join(map(shlex.quote,command))+'\n')
  submit=['qsub','-h','-terse','-w','n','-cwd','-q','debian.q','-pe','threaded','1','-binding','linear:1','-l','exclusive=false,h_rt=00:30:00,h_vmem=8G','-m','n','-N','known_horizon_analysis','-j','y','-o',str(raw/'known-horizon-analysis.log'),'-S','/bin/bash']
  if a.hold_job:submit+=['-hold_jid',a.hold_job]
  submit.append(str(job));r=subprocess.run(submit,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True);receipt=dict(command=submit,returncode=r.returncode,response=r.stdout)
  write(raw/'known-horizon-submission.json',receipt);write(raw/'motion-analysis-submission.json',receipt);r.check_returncode();assert r.stdout.strip().isdigit();print(r.stdout,end='');subprocess.run(['qrls',r.stdout.strip()],check=True);return
 request=read(raw/'known-horizon-request.json');assert request['commit']==a.commit and Path(request['root'])==ROOT
 for name,sha in request['files'].items():assert digest(Path(name))==sha,name
 sys.path.insert(0,str(ROOT/'tools'));from cpu_resources import cpu_resources
 cpu=cpu_resources();assert cpu['effective_cpu_quota'] is None;os.sched_setaffinity(0,cpu['representative_cpus'][:1])
 spec=read(raw/'spec.json');build=read(raw/'build.json');H=(spec['horizons'] or {'WAREHOUSE':5000})['WAREHOUSE'];assert H in (200,5000)
 assert spec['trick']=='WAREHOUSE' and spec['experiment_track']=='TRICK' and spec['instances']==['WAREHOUSE'] and spec['time_limit_ms']==5000 and spec['cpus_per_instance']==4
 sources=dict(build['sources'],**build['test_sources'])
 for name,sha in sources.items():assert hashlib.sha256(subprocess.check_output(['git','show',a.commit+':'+name],cwd=ROOT)).hexdigest()==sha,name
 assert digest(raw/'lifelong')==build['binary_sha256'];profile=read(support/'profile.json')['trick_native_short0_weight5'];reference=read(support/'reference.json')
 assert reference['source_commit']=='85faca5570ddb0f78944e741f11eb30e2362b5d2' and reference['complete_all_pairs'] and reference['all_valid_within_deadline_and_memory'] and not reference['failures']
 refrows={r['case']:r for r in reference['rows']};cases=spec['cases'];assert len(cases)==(3 if H==200 else 4);byseed={};receipts={};intervals=[];allocation=read(raw/'allocation.json')
 for case in cases:
  name=case['name'];env=dict(case['environment']);configured=int(env.pop('CGAR_TRICK_KNOWN_HORIZON'));assert env==profile and configured in ((0,200,5000) if H==200 else (0,5000));assert name=='trick_native_horizon%d-s%d-r0'%(configured,case['seed'])
  assert configured not in byseed.setdefault(case['seed'],{});byseed[case['seed']][configured]=name
  meta=read(raw/name/'metadata.json');s=read(raw/name/'summary.json')[0]
  assert meta['build_provenance']==build and meta['max_process_memory_bytes']==32000000000 and meta['trick_argv']==['--trick','WAREHOUSE']
  assert s['trick_receipt_valid'] and s['experiment_track_valid'] and s['memory_valid'];assert s['peak_process_rss_bytes']<32000000000
  resources=meta['cpu_resources'];assert resources['effective_cpu_quota'] is None and resources['cpu_model']=='AMD EPYC 9354 32-Core Processor';binding=s['cpu'];assert binding==meta['cpu_binding'] and len(binding)==4 and set(binding).issubset(allocation['selected_cpus'])
  coreof={c:tuple(g) for g in resources['logical_cpus_by_physical_core'] for c in g};cores={(resources['hostname'],coreof[c]) for c in binding};assert len(cores)==4
  begin,end=(datetime.datetime.fromisoformat(meta[k]) for k in ['started_utc','finished_utc'])
  for x,y,z in intervals:assert not(begin<y and x<end and cores.intersection(z))
  intervals.append((begin,end,cores));lines=(raw/name/'WAREHOUSE.log').read_text().splitlines()
  assert sum('installed_fnv1a64=18323636419238823753' in l for l in lines if l.startswith('[CGAR_TRICK] '))==1
  components=[tokens(l) for l in lines if l.startswith('[CGAR_TRICK_COMPONENTS] ')]
  assert components==[dict(instance='WAREHOUSE',lanes='1',short_tasks='0',matching='1',remaining_flow='1',native_metric='1',native_bands='1',hrrn='1',oldest_admission='1',started_tasks='protected')]
  extra=[tokens(l) for l in lines if l.startswith('[CGAR_TRICK_HORIZON] ')]
  assert extra==([dict(known_horizon=str(configured),assumption='configured',lower_bound='spatial_plus_service',core='full',assignments='new_only',fair='unchanged',held='unchanged',all_impossible='assign',after_horizon='ordinary')] if configured else [])
  first=[int(tokens(l)['t']) for l in lines if l.startswith('[cgar-horizon-first-rank-change] ')];assert len(first)<=1
  assert not any(l.startswith(('[cgar-flow]','[cgar-chain-pricing]','[cgar-match-budget-shadow]','[cgar-horizon-passed]')) for l in lines)
  if not configured:assert not first and not any(l.startswith('[cgar-horizon] ') for l in lines)
  if H==200 and configured==200:assert first,'active startup tier was vacuous'
  if H==200 and configured==5000:assert not first,'long-horizon prefix unexpectedly active'
  receipts[name]=dict(configured_horizon=configured,assumption='configured' if configured else None,first_rank_change=first[0] if first else None)
  if s['valid']:assert s['makespan']==s['entry_compute_samples']==H and s['entry_compute_max_seconds']<=5
  else:assert H==5000,'startup failed'
 assert set(byseed)==({0} if H==200 else {0,2})
 helpers=support/'experiments/construction-20260918';pairs=[]
 if H==200:
  subprocess.run(['/usr/bin/python3',str(helpers/'collect_cold.py'),'--input',str(raw),'--output',str(out)],check=True)
  fingerprints=read(out/'trajectory-fingerprints.json');expected=read(support/'reference-screen.json')['trick_native_short0_weight5-s0-r0']
  for h in [0,5000]:assert fingerprints[byseed[0][h]]==expected
  result=dict(full_run=False,all_valid=True,exact_controls=True)
 else:
  subprocess.run(['/usr/bin/python3',str(support/'experiments/sequences-20260918/analyze.py'),'--input',str(raw),'--output',str(out),'--workers','1'],check=True)
  sys.path.insert(0,str(helpers));verifier=importlib.import_module('verify_full');verifier.ROOT=ROOT;result=verifier.verify(raw,out,a.commit,allow_failed=True,decision_limit_ms=5000);write(out/'verification.json',result)
  rows={r['case']:r for r in result['rows']}
  for seed,names in byseed.items():
   if any(name not in rows for name in names.values()):continue
   control,candidate=(rows[names[h]] for h in [0,5000]);ref=refrows['trick_native_short0_weight5-s%d-r0'%seed]
   assert (control['tasks'],control['trajectory_sha256'])==(ref['tasks'],ref['trajectory_sha256'])
   t=receipts[names[5000]]['first_rank_change'];assert t is not None and 0<=t<5000
   baseline=read(raw/names[0]/'WAREHOUSE.json');trial=read(raw/names[5000]/'WAREHOUSE.json');sha=prefix(baseline,t);assert sha==prefix(trial,t),'changed before first comparator reversal'
   assert max(e[0] for e in baseline['events'])==5000 and max(e[0] for e in trial['events'])==5000
   pairs.append(dict(seed=seed,control_tasks=control['tasks'],tasks=candidate['tasks'],difference=candidate['tasks']-control['tasks'],final1000_difference=candidate['final1000']-control['final1000'],age_p90_difference=candidate['outstanding_age_p90']-control['outstanding_age_p90'],first_rank_change=t,exact_prefix_steps=t,prefix_sha256=sha,empty_robot_step_difference=candidate['empty_robot_steps']-control['empty_robot_steps']))
  result.update(full_run=True,complete_all_pairs=len(pairs)==len(byseed),exact_controls=len(pairs)==len(byseed),pairs=pairs)
 result.update(source_commit=a.commit,source_files_verified=len(sources),binary_sha256=build['binary_sha256'],receipts=receipts,checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),competition_budget_confirmed=False,promoted=False,
  scope='TRICK --trick WAREHOUSE with configured known horizon. Original fairness, held/started protection and matching retained. New-admission lower-bound tier only. Full no-horizon controls exact against V80; action/schedule/event prefixes exact before earliest logged comparator reversal. Shared5s,32decimalGB; no strict1s or competition-horizon-knowledge claim.')
 for name in ['known-horizon-request.json','known-horizon-submission.json']:shutil.copy2(raw/name,out/name)
 write(out/'comparison.json',result);print('KNOWN_HORIZON_VERIFIED',len(cases),'cases',flush=True)
if __name__=='__main__':main()
