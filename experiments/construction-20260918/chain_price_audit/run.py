#!/usr/bin/env python3
"""Freeze an offline replay, validate a prefix, then price sampled full-run tasks on GRID."""
import argparse,bisect,concurrent.futures,datetime,hashlib,json,os,re,shlex,shutil,subprocess,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[3]
HERE=Path(__file__).resolve().parent
sys.path.insert(0,str(ROOT/'tools'))
from cpu_resources import cpu_resources

def read(p):return json.loads(p.read_text())
def write(p,x):p.write_text(json.dumps(x,indent=2)+'\n')
def digest(p):
 h=hashlib.sha256()
 with p.open('rb') as f:
  for b in iter(lambda:f.read(1<<20),b''):h.update(b)
 return h.hexdigest()
def fields(line):return {k:int(v) for k,v in re.findall(r'(\w+)=(-?\d+)',line)}

def prepare_input(case,out,map_data,rows,cols,sample_count):
 raw=Path(case['raw_case']);data=read(raw/'WAREHOUSE.json')
 assert data['makespan']==case['steps']
 logs=(raw/'WAREHOUSE.log').read_text().splitlines()
 pubs=[fields(x) for x in logs if x.startswith('[cgar-flow] ')]
 assert [x['publications'] for x in pubs]==list(range(1,len(pubs)+1))
 tasks={int(x[0]):x for x in data['tasks']};assignments={}
 for robot,schedule in enumerate(data['actualSchedule']):
  for item in schedule.split(','):
   if not item:continue
   step,task=map(int,item.split(':'))
   if task>=0:
    assert task not in assignments,'Reference unexpectedly reassigned a task'
    assignments[task]=(step,robot)
 grouped={};counts={'warmup':0,'single_stop':0,'eligible':0}
 for task,(step,robot) in assignments.items():
  t=tasks[task];assert int(t[1])<=step
  if len(t[2])<=2:counts['single_stop']+=1;continue
  publication=bisect.bisect_left([x['step'] for x in pubs],step-1)
  if not publication:counts['warmup']+=1;continue
  counts['eligible']+=1
  key=hashlib.sha256(('%d:%d:%d'%(case['seed'],publication,task)).encode()).digest()
  grouped.setdefault(publication,[]).append((key,task,step,robot))
 # This selection occurs before reading the outcome event array.
 selected=[(pub,task,step,robot) for pub,items in sorted(grouped.items()) for _,task,step,robot in sorted(items)[:sample_count]]
 pickups={};completed={}
 for step,robot,task,stop in data['events']:
  if stop==1:pickups[task]=step
  if stop==len(tasks[task][2])//2:completed[task]=step
 assert len(completed)==data['numTaskFinished']
 queries=[]
 for pub,task,step,robot in selected:
  coords=tasks[task][2];pickup=pickups.get(task);finish=completed.get(task)
  if finish is not None:assert pickup is not None and step<=pickup<=finish
  queries.append({'seed':case['seed'],'task':task,'publication':pub,'assignment_step':step,'revealed_step':tasks[task][1],'robot':robot,'locations':[coords[k]*cols+coords[k+1] for k in range(0,len(coords),2)],'pickup_step':pickup,'completion_step':finish,'loaded_steps':finish-pickup if finish is not None else None,'censored_loaded_steps':data['makespan']-pickup if finish is None and pickup is not None else None})
 env=case['environment'];directions={'E':0,'S':1,'W':2,'N':3}
 payload={'rows':rows,'cols':cols,'map':map_data,'horizon':case['steps'],'starts':[[r*cols+c,directions[d]] for r,c,d in data['start']],'actions':[s.replace(',','') for s in data['actualPaths']], 'flow':{'warmup':int(env['CGAR_FLOW_WARMUP']),'strength':int(env['CGAR_FLOW_STRENGTH']),'minimum_samples':int(env['CGAR_FLOW_MIN_SAMPLES']),'margin_percent':int(env['CGAR_FLOW_MIN_MARGIN_PERCENT']),'refresh_interval':int(env['CGAR_FLOW_REFRESH_INTERVAL']),'cost_scale':int(env['CGAR_FLOW_COST_SCALE']),'turn_cost':int(env['CGAR_FLOW_COST_SCALE'])},'queries':queries,'expected_publications':pubs}
 write(out/'input.json',payload)
 write(out/'input-metadata.json',{'raw_case':str(raw),'raw_trajectory_file_sha256':digest(raw/'WAREHOUSE.json'),'expected_trajectory_fingerprint':case.get('trajectory_sha256'),'seed':case['seed'],'steps':case['steps'],'tasks_finished':data['numTaskFinished'],'selection_counts':counts,'sampled_tasks':len(queries),'sampled_completed':sum(q['completion_step'] is not None for q in queries),'selection':'64 lowest SHA256(seed:publication:task) per publication among assigned tasks with known multi-stop chains; outcomes examined afterward.','input_sha256':digest(out/'input.json')})

def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',required=True,type=Path);p.add_argument('--execute',action='store_true');a=p.parse_args();out=a.output.resolve()
 if not a.execute:
  out.mkdir(parents=True,exist_ok=False)
  shutil.copy2(HERE/'replay.cpp',out/'replay.cpp')
  for sub in ['inc','cgar_planner']:shutil.copytree(ROOT/'cgar'/sub,out/'include'/sub,ignore=shutil.ignore_patterns('*.cpp','*.o'))
  source=(ROOT/'cgar/cgar_planner/cgar.cpp').read_text();prefix=source[:source.index('\nint ChainCostCache::estimate')]
  (out/'oracle.cpp').write_text(prefix+'\n} // namespace cgar\n')
  reference=read(HERE.parent/'results/pickup-full-regions-six-seed-v44.json')
  cases=[dict(r,steps=5000) for r in reference['rows'] if r['environment']['CGAR_TEMPORAL_REGIONS']=='4'];cases.sort(key=lambda x:x['seed']);assert [x['seed'] for x in cases]==list(range(6))
  prefix_case=dict(cases[0],raw_case=str(ROOT/'runs/cgar-deadline-duplicate-timing-v50-20260919/optimized'),steps=200,trajectory_sha256='a4bfe67f610e2451bb40e68da0e9fad0b75985d3f7701528334fee73f93cea8b')
  spec={'created_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'source_commit':subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),'production_cgar_cpp_sha256':digest(ROOT/'cgar/cgar_planner/cgar.cpp'),'source_hashes':{str(f.relative_to(out)):digest(f) for f in out.rglob('*') if f.is_file()},'cases':cases,'prefix_case':prefix_case,'sample_per_publication':64,'purpose':'Offline information audit only; no planner or throughput change. Fields available at assignment, deterministic outcome-blind sample, explicit censoring.'}
  write(out/'spec.json',spec)
  command=[sys.executable,str(Path(__file__).resolve()),'--execute','--output',str(out)];script=out/'job.sh';script.write_text('#!/bin/bash\nset -eu\nexec '+' '.join(shlex.quote(x) for x in command)+'\n')
  submit=['qsub','-terse','-w','e','-cwd','-q',','.join('debian.q@research%d.grid.gsb'%h for h in [31,38,44,46,50,52,57]),'-pe','threaded','4','-binding','linear:4','-l','exclusive=true,h_rt=00:25:00,h_vmem=8G','-m','n','-N','chain_audit','-j','y','-o',str(out/'job.log'),'-S','/bin/bash',str(script)]
  r=subprocess.run(submit,cwd=ROOT,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT);write(out/'submission.json',{'command':submit,'response':r.stdout,'returncode':r.returncode});print(r.stdout,end='');return r.returncode
 spec=read(out/'spec.json')
 for name,want in spec['source_hashes'].items():assert digest(out/name)==want,name
 resources=cpu_resources();cpus=resources['representative_cpus'][:4];assert len(cpus)==4 and resources['effective_cpu_quota'] is None and resources['cpu_model']=='AMD EPYC 9354 32-Core Processor'
 write(out/'allocation.json',{'resources':resources,'selected_cpus':cpus})
 command=[str(ROOT/'env/bin/x86_64-conda-linux-gnu-c++'),'-O3','-DNDEBUG','-std=c++17','-march=nocona','-mtune=haswell','-I'+str(out/'include/cgar_planner'),'-I'+str(out/'include/inc'),'-I'+str(ROOT/'env/include'),str(out/'replay.cpp'),str(out/'oracle.cpp'),'-pthread','-o',str(out/'replay')]
 subprocess.run(command,cwd=out,check=True);write(out/'binary.json',{'command':command,'sha256':digest(out/'replay')})
 map_path=ROOT/'mr24/warehouse.domain/maps/warehouse_large.map';lines=map_path.read_text().splitlines();rows=int(lines[1].split()[1]);cols=int(lines[2].split()[1]);grid=lines[4:];assert len(grid)==rows and all(len(x)==cols for x in grid);map_data=[int(c in '@TO') for row in grid for c in row]
 write(out/'map.json',{'path':str(map_path),'sha256':digest(map_path),'rows':rows,'cols':cols,'free_cells':map_data.count(0)})
 def run_case(label,case,cpu):
  folder=out/label;folder.mkdir()
  prepare_input(case,folder,map_data,rows,cols,spec['sample_per_publication'])
  cmd=[sys.executable,str(ROOT/'tools/run_with_usage.py'),'--output',str(folder/'resources.json'),'--','taskset','-c',str(cpu),str(out/'replay'),str(folder/'input.json'),str(folder/'prices.json')]
  with (folder/'replay.log').open('w') as log:r=subprocess.run(cmd,cwd=out,stdout=log,stderr=subprocess.STDOUT)
  if r.returncode:raise RuntimeError(label+' replay failed; see log')
  result=read(folder/'prices.json');assert result['all_publication_counters_exact']
  usage=read(folder/'resources.json');assert usage['peak_rss_kib']*1024<32000000000
  print(label,'queries',len(result['queries']),'publications',len(result['publications']),flush=True)
  return {'case':label,'queries':len(result['queries']),'publications':len(result['publications']),'cpu':cpu,'prices_sha256':digest(folder/'prices.json')}
 checked=[run_case('prefix',spec['prefix_case'],cpus[0])]
 # One replay per physical core. Preparing large JSON happens in these same
 # worker processes, with the parent pinned to that core before preparation.
 def batch(cases,cpu):
  os.sched_setaffinity(0,{cpu});return [run_case('seed'+str(c['seed']),c,cpu) for c in cases]
 # Forked workers inherit this frozen function and immutable small map.
 import multiprocessing
 queues=[];workers=[]
 def worker(cases,cpu,q):
  try:q.put({'rows':batch(cases,cpu)})
  except BaseException as e:q.put({'error':repr(e)});raise
 for k,cpu in enumerate(cpus):
  group=spec['cases'][k::len(cpus)]
  if not group:continue
  q=multiprocessing.Queue();proc=multiprocessing.Process(target=worker,args=(group,cpu,q));proc.start();queues.append(q);workers.append(proc)
 for proc,q in zip(workers,queues):
  row=q.get();proc.join();assert proc.exitcode==0 and 'error' not in row,row;checked+=row['rows']
 write(out/'completion.json',{'completed':True,'finished_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'cases':checked,'all_publication_counters_exact':True})
 return 0
if __name__=='__main__':raise SystemExit(main())
