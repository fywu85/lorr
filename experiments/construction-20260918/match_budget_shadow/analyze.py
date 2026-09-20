#!/usr/bin/env python3
"""Verify exact unshadowed trajectories and bound read-only matching witnesses."""
import argparse,datetime,hashlib,importlib,json,os,re,shlex,shutil,subprocess,sys
from pathlib import Path
ROOT=next(p for p in Path(__file__).resolve().parents if (p/'tools/cpu_resources.py').is_file())
BASE=ROOT/'experiments/construction-20260918'
REF_SOURCE='85faca5570ddb0f78944e741f11eb30e2362b5d2'
def read(p):return json.loads(p.read_text())
def write(p,x):p.write_text(json.dumps(x,indent=2)+'\n')
def digest(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def fields(line):return {k:float(v) if '.' in v else int(v) for k,v in re.findall(r'(\w+)=([-\d.]+)',line)}
def real_work(lines):
 prefixes=('[cgar] t=','[cgar-scheduler] t=','[cgar-unopened-match] t=','[cgar-temporal]','[cgar-temporal-starts]',
  '[cgar-temporal-regions]','[cgar-pickup-full]','[cgar-temporal-table-batch]','[cgar-native-metric]')
 return [(line.split(']')[0],{k:v for k,v in fields(line).items() if k not in ('seconds','elapsed','elapsed_ms','milliseconds')})
  for line in lines if line.startswith(prefixes)]
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--raw',type=Path,required=True);p.add_argument('--output',type=Path,required=True)
 p.add_argument('--commit',required=True);p.add_argument('--hold-job');p.add_argument('--execute',action='store_true');a=p.parse_args()
 raw=a.raw.resolve();out=a.output.resolve();support=raw/'match-budget-shadow-support';spec=read(raw/'spec.json');horizon=(spec['horizons'] or {'WAREHOUSE':5000})['WAREHOUSE'];assert horizon in (200,5000)
 reference=BASE/('results/turn-ring-native-screen-v81/trajectory-fingerprints.json' if horizon==200 else 'results/native-short-full-v80/comparison.json')
 if not a.execute:
  assert reference.exists(),'Await completed unshadowed reference before freezing analysis'
  support.mkdir(exist_ok=False);files=[]
  copies={'analyze.py':Path(__file__),'reference.json':reference,'profiles.json':BASE/'tricks/native_metric/short-variants.json'}
  for name in ['sequences-20260918/analyze.py','motion-20260918/analyze.py','assignment-20260918/analyze.py',
   'throughput-20260918-next/analyze_matrix.py','throughput-20260918-strict/analyze.py','construction-20260918/verify_full.py','construction-20260918/collect_cold.py']:
   copies['experiments/'+name]=ROOT/'experiments'/name
  for name,source in copies.items():
   dest=support/name;dest.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(source,dest);files.append(dest)
  write(raw/'match-budget-shadow-request.json',dict(root=str(ROOT),commit=a.commit,reference_source=REF_SOURCE,raw=str(raw),output=str(out),files={str(f):digest(f) for f in files}))
  cmd=['/usr/bin/python3',str(support/'analyze.py'),'--execute','--raw',str(raw),'--output',str(out),'--commit',a.commit]
  job=raw/'match-budget-shadow-analysis.sh';job.write_text('#!/bin/bash\nset -eu\nexec '+' '.join(map(shlex.quote,cmd))+'\n')
  submit=['qsub','-h','-terse','-w','n','-cwd','-q','debian.q','-pe','threaded','1','-binding','linear:1','-l','exclusive=false,h_rt=00:30:00,h_vmem=8G','-m','n','-N','match_budget_audit','-j','y','-o',str(raw/'match-budget-shadow-analysis.log'),'-S','/bin/bash']
  if a.hold_job:submit+=['-hold_jid',a.hold_job]
  submit.append(str(job));r=subprocess.run(submit,cwd=ROOT,capture_output=True,text=True);receipt=dict(command=submit,returncode=r.returncode,stdout=r.stdout,stderr=r.stderr)
  write(raw/'match-budget-shadow-submission.json',receipt);write(raw/'motion-analysis-submission.json',receipt);print(r.stdout,end='');r.check_returncode();assert r.stdout.strip().isdigit();subprocess.run(['qrls',r.stdout.strip()],check=True);return
 request=read(raw/'match-budget-shadow-request.json');assert request['commit']==a.commit and request['reference_source']==REF_SOURCE and Path(request['root'])==ROOT
 for filename,sha in request['files'].items():assert digest(Path(filename))==sha,filename
 sys.path.insert(0,str(ROOT/'tools'));from cpu_resources import cpu_resources
 cpu=cpu_resources();assert cpu['effective_cpu_quota'] is None;os.sched_setaffinity(0,cpu['representative_cpus'][:1])
 assert spec['trick']=='WAREHOUSE' and spec['experiment_track']=='TRICK' and spec['instances']==['WAREHOUSE']
 assert spec['time_limit_ms']==5000 and spec['cpus_per_instance']==4
 build=read(raw/'build.json');sources=dict(build['sources'],**build['test_sources'])
 for name,sha in sources.items():assert hashlib.sha256(subprocess.check_output(['git','show',a.commit+':'+name],cwd=ROOT)).hexdigest()==sha,name
 assert digest(raw/'lifelong')==build['binary_sha256'];profiles=read(support/'profiles.json');reference=read(support/'reference.json');cases=spec['cases']
 if horizon==200:
  assert len(cases)==4 and {c['seed'] for c in cases}=={0}
  ref_rows=None
 else:
  assert len(cases)==4 and {c['seed'] for c in cases}=={0,2}
  assert reference['source_commit']==REF_SOURCE and reference['complete_all_pairs'] and reference['all_valid_within_deadline_and_memory'] and not reference['failures']
  ref_rows={r['case']:r for r in reference['rows']}
 allocation=read(raw/'allocation.json');seen=set();audits={};intervals=[]
 for case in cases:
  name=case['name'];env=dict(case['environment']);stride=int(env.pop('CGAR_MATCH_BUDGET_AUDIT_STRIDE'));assert stride in (10,100);short=int(env['CGAR_TRICK_SHORT_TASKS']);weight=int(env['CGAR_PICKUP_WEIGHT'])
  assert short in (0,1) and weight in ((1,5) if horizon==200 else (5,));variant='trick_native_short%d_weight%d'%(short,weight)
  assert env==profiles[variant] and name==variant+'-s%d-r0'%case['seed'];key=(short,weight,case['seed']);assert key not in seen;seen.add(key)
  summary=read(raw/name/'summary.json')[0];meta=read(raw/name/'metadata.json');assert summary['valid'] and summary['makespan']==summary['entry_compute_samples']==horizon
  assert summary['entry_compute_max_seconds']<=5 and summary['peak_process_rss_bytes']<32000000000 and summary['trick_receipt_valid'] and summary['experiment_track_valid']
  assert meta['build_provenance']==build and meta['max_process_memory_bytes']==32000000000 and meta['trick_argv']==['--trick','WAREHOUSE']
  resources=meta['cpu_resources'];assert resources['effective_cpu_quota'] is None and resources['cpu_model']=='AMD EPYC 9354 32-Core Processor'
  binding=summary['cpu'];assert binding==meta['cpu_binding'] and len(binding)==4 and set(binding).issubset(allocation['selected_cpus'])
  core_of={c:tuple(group) for group in resources['logical_cpus_by_physical_core'] for c in group};cores={(resources['hostname'],core_of[c]) for c in binding};assert len(cores)==4
  begin,end=(datetime.datetime.fromisoformat(meta[k]) for k in ['started_utc','finished_utc'])
  for oldbegin,oldend,oldcores in intervals:assert not(begin<oldend and oldbegin<end and cores.intersection(oldcores))
  intervals.append((begin,end,cores));lines=(raw/name/'WAREHOUSE.log').read_text().splitlines()
  assert sum(line.startswith('[cgar-match-budget-shadow-config] stride=%d read_only=1 after_real_match=1 resident_only=1 include_budget=1 cooldown=20 task_disjoint_witnesses=1'%stride) for line in lines)==1
  assert any('installed_fnv1a64=18323636419238823753' in line for line in lines if line.startswith('[CGAR_TRICK] '))
  assert not any(line.startswith(('[cgar-flow]','[cgar-chain-pricing]')) for line in lines)
  samples=[fields(line) for line in lines if line.startswith('[cgar-match-budget-shadow] ')]
  assert [r['t'] for r in samples]==list(range(0,horizon,stride))
  for r in samples:
   assert r['unit']==20 and r['read_only']==1 and r['passes']==r['t']//stride+1
   assert r['groups']<=64*r['passes'] and 2*r['groups']<=r['selected']<=32*r['groups'] and r['nodes']<=2048*64*r['passes']
   assert r['matrix_entries']<=32*r['selected'] and r['witness_rows']==r['unique_tasks'] and r['witness_budget_rows']<=r['witness_rows']
   assert r['accepted_cycles']==r['budget_cycles']+r['unprotected_cycles'] and r['budget_cycles']==r['witness_cycles']+r['duplicate_cycles']
   assert r['witness_saving']>=80*r['witness_cycles']
   if 'fully_protected_cycles' in r:
    assert 0<=r['fully_protected_cycles']<=r['witness_cycles']
    assert 2*r['fully_protected_cycles']<=r['fully_protected_rows']<=r['witness_budget_rows']
    assert 80*r['fully_protected_cycles']<=r['fully_protected_saving']<=r['witness_saving']
   else:assert stride==100,'Full-cadence study requires protected-subset accounting'
  for old,new in zip(samples,samples[1:]):
   for k in old:
    if k not in ('unit','read_only'):assert new[k]>=old[k],(name,k)
  refraw=(ROOT/'runs/cgar-turn-ring-native-screen-v81-20260920'/name) if horizon==200 else Path(ref_rows[name]['raw_case'])
  assert real_work(lines)==real_work((refraw/'WAREHOUSE.log').read_text().splitlines()),'real work changed: '+name
  last=samples[-1];denom=max(1,last['assignments']);audits[name]=dict(samples=samples,sample_tick=last['t'],unique_excluded_saving_forward_units_per_assignment=last['witness_saving']/20/denom,
   actual_first_move_saving_forward_units_per_assignment=last['real_saving']/20/denom,sample_stride=stride,descriptive_witness_to_committed_guidance_ratio=last['witness_saving']/last['real_saving'] if stride==10 and last['real_saving'] else None,
   fully_protected_saving_forward_units_per_assignment=last.get('fully_protected_saving',0)/20/denom,
   mixed_saving_forward_units_per_assignment=(last['witness_saving']-last.get('fully_protected_saving',0))/20/denom,
   witness_rows=last['witness_rows'],witness_cycles=last['witness_cycles'],real_work_exact=True)
 helpers=support/'experiments/construction-20260918'
 if horizon==200:
  subprocess.run(['/usr/bin/python3',str(helpers/'collect_cold.py'),'--input',str(raw),'--output',str(out)],check=True)
  fingerprints=read(out/'trajectory-fingerprints.json');assert all(fingerprints[c['name']]==reference[c['name']] for c in cases)
  result=dict(full_run=False,all_valid=True,exact_reference=True,source_commit=a.commit)
 else:
  subprocess.run(['/usr/bin/python3',str(support/'experiments/sequences-20260918/analyze.py'),'--input',str(raw),'--output',str(out),'--workers','1'],check=True)
  sys.path.insert(0,str(helpers));verifier=importlib.import_module('verify_full');verifier.ROOT=ROOT
  result=verifier.verify(raw,out,a.commit,allow_failed=False,decision_limit_ms=5000)
  for row in result['rows']:assert row['tasks']==ref_rows[row['case']]['tasks'] and row['trajectory_sha256']==ref_rows[row['case']]['trajectory_sha256']
  write(out/'verification.json',result);result.update(full_run=True,exact_reference=True,source_commit=a.commit)
 result.update(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),source_files_verified=len(sources),binary_sha256=build['binary_sha256'],reference_source=REF_SOURCE,audits=audits,
  decision_limit_ms=5000,competition_budget_confirmed=False,promoted=False,
  scope='Read-only matching-budget shadow. Every trajectory and sampled real work counter must equal the frozen unshadowed reference. Unique task-disjoint cycle savings are guidance costs, not realized physical steps or causal throughput. Stride100 observations are sparse snapshot sums, without a ratio. Even stride10 is bounded, chronological sampling on the unchanged trajectory; its ratio is descriptive guidance accounting, not a complete measure or upper bound. No new policy or throughput frontier.')
 for name in ['match-budget-shadow-request.json','match-budget-shadow-submission.json']:shutil.copy2(raw/name,out/name)
 write(out/'comparison.json',result);print('MATCH_BUDGET_SHADOW_VERIFIED',len(cases),'cases; exact trajectories and real work')
if __name__=='__main__':main()
