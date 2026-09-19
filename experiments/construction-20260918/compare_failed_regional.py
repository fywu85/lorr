#!/usr/bin/env python3
"""Preserve explicit full-run failures and verify the remaining completed cases."""
import argparse,json,statistics
from pathlib import Path
from verify_full import verify,read
from compare_regional import fields
HERE=Path(__file__).resolve().parent

def main():
 p=argparse.ArgumentParser(description=__doc__)
 p.add_argument('--input',type=Path,required=True);p.add_argument('--output',type=Path,required=True)
 p.add_argument('--commit',required=True);p.add_argument('--variable',required=True,choices=['CGAR_TEMPORAL_DISTANCE_SCALE','CGAR_TEMPORAL_CANDIDATE_LIMIT']);p.add_argument('--control',type=int,required=True)
 a=p.parse_args(); report=verify(a.input,a.output,a.commit,allow_failed=True)
 assert report['failed_cases']>0,'Use the strict comparison for an entirely valid matrix'
 reference={r['seed']:r for r in read(HERE/'results/pickup-full-regions-six-seed-v44.json')['rows'] if r['environment']['CGAR_TEMPORAL_REGIONS']=='4'}
 controls={r['seed']:r for r in report['rows'] if int(r['environment'][a.variable])==a.control}
 assert sorted(controls)==[0,2]
 for seed,r in controls.items():assert r['tasks']==reference[seed]['tasks'] and r['trajectory_sha256']==reference[seed]['trajectory_sha256']
 changed={a.variable}
 if a.variable=='CGAR_TEMPORAL_CANDIDATE_LIMIT':changed.add('CGAR_TEMPORAL_REGION_ROUNDS')
 diagnostics={}
 for r in report['rows']+report['failures']:
  env=r['environment'];base=controls[r['seed']]['environment']
  assert {k:v for k,v in env.items() if k not in changed}=={k:v for k,v in base.items() if k not in changed}
  if 'tasks' not in r:continue
  lines=(Path(r['raw_case'])/'WAREHOUSE.log').read_text().splitlines()
  global_rows=[fields(x) for x in lines if x.startswith('[cgar-temporal]')]
  timing=[fields(x) for x in lines if x.startswith('[cgar-temporal-timing]')]
  regions=[fields(x) for x in lines if x.startswith('[cgar-temporal-regions]')]
  for rows in [global_rows,timing,regions]:assert [x['step'] for x in rows]==list(range(200,5001,200))
  for x in global_rows:assert x['candidate_limit']==int(env['CGAR_TEMPORAL_CANDIDATE_LIMIT']) and x['workers']==int(env['CGAR_TEMPORAL_WORKERS'])
  for x in timing:assert x['distance_scale']==int(env.get('CGAR_TEMPORAL_DISTANCE_SCALE','50'))
  for x in regions:
   assert x['regions']==int(env['CGAR_TEMPORAL_REGIONS']) and x['threads']==int(env['CGAR_TEMPORAL_REGION_THREADS']) and x['rounds']==int(env['CGAR_TEMPORAL_REGION_ROUNDS'])
   assert x['repairs']==(x['kept']+x['reverted'])*int(env['CGAR_TEMPORAL_REGION_STEPS']) and x['score_after']+1e-6>=x['score_before']
  diagnostics[r['case']]={'global_samples':global_rows,'timing_samples':timing,'regional_samples':regions}
 groups=[]
 values=sorted({int(r['environment'][a.variable]) for r in report['rows']+report['failures']}-{a.control})
 for value in values:
  candidates=[r for r in report['rows'] if int(r['environment'][a.variable])==value]
  failed=[r for r in report['failures'] if int(r['environment'][a.variable])==value]
  assert sorted([r['seed'] for r in candidates+failed])==[0,2]
  pairs=[]
  for r in candidates:
   c=controls[r['seed']];pair={'seed':r['seed'],'tasks':r['tasks'],'control_tasks':c['tasks'],'task_difference':r['tasks']-c['tasks'],'final1000_difference':r['final1000']-c['final1000'],'age_p90_difference':r['outstanding_age_p90']-c['outstanding_age_p90']}
   for k in ['empty_robot_steps','loaded_turns','loaded_waits']:pair[k+'_percent']=(r[k]/c[k]-1)*100
   pairs.append(pair)
  mean=statistics.mean(r['tasks'] for r in candidates) if not failed else None
  baseline=statistics.mean(r['tasks'] for r in controls.values())
  groups.append({'value':value,'valid_seeds':[r['seed'] for r in candidates],'failed_seeds':[r['seed'] for r in failed],'mean_tasks':mean,'mean_effect_percent':(mean/baseline-1)*100 if mean is not None else None,'pairs':pairs,'status':'invalid_timeout_not_promoted' if failed else 'first_pair_complete_not_promoted'})
 report.update(scope='Full matrix with explicit failures; failed runs have no quality result or partial-plan acceptance. Completed controls and cases are independently verified.',variable=a.variable,control_value=a.control,exact_control_seeds=sorted(controls),groups=groups,diagnostics=diagnostics,promoted_as_benchmark_reference=False,throughput_goal_complete=False)
 (a.output/'comparison.json').write_text(json.dumps(report,indent=2)+'\n')
 print(json.dumps({'groups':groups,'failures':report['failures'],'completed_entry_samples':report['complete_entry_samples']},indent=2))
if __name__=='__main__':main()
