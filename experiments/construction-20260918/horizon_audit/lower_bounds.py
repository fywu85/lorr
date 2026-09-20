#!/usr/bin/env python3
"""Freeze and run exact spatial-bound and free-pool witness audits."""
import argparse,datetime,hashlib,json,os,shlex,shutil,subprocess,sys
from pathlib import Path
ROOT=next(p for p in Path(__file__).resolve().parents if (p/'tools/cpu_resources.py').is_file());BASE=ROOT/'experiments/construction-20260918'
def read(p):return json.loads(p.read_text())
def write(p,x):p.write_text(json.dumps(x,indent=2)+'\n')
def digest(p):
 h=hashlib.sha256()
 with p.open('rb') as f:
  for b in iter(lambda:f.read(4*1024*1024),b''):h.update(b)
 return h.hexdigest()
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,required=True);p.add_argument('--archive',type=Path,required=True);p.add_argument('--reference',type=Path,default=BASE/'results/horizon-work-audit-v84/accounting.json');p.add_argument('--execute',action='store_true');a=p.parse_args();raw=a.output.resolve();out=a.archive.resolve()
 if not a.execute:
  raw.mkdir(parents=True,exist_ok=False);files=[]
  for src,name in [(Path(__file__),'lower_bounds.py'),(Path(__file__).with_suffix('.cpp.inc'),'lower_bounds.cpp.inc'),(BASE/'decompose.py','decompose.py'),(a.reference,'reference.json')]:
   dest=raw/name;shutil.copy2(src,dest);files.append(dest)
  text=(BASE/'decompose.cpp').read_text();needle='    std::ofstream(argv[3]) << report.dump(2)';assert text.count(needle)==1;text=text.replace(needle,(raw/'lower_bounds.cpp.inc').read_text()+'\n'+needle)
  cpp=raw/'decompose.cpp';cpp.write_text(text);files.append(cpp)
  write(raw/'request.json',dict(created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),root=str(ROOT),archive=str(out),files={str(f):digest(f) for f in files},map_sha256=digest(ROOT/'mr24/warehouse.domain/maps/warehouse_large.map'),json_header_sha256=digest(ROOT/'cgar/inc/nlohmann/json.hpp')))
  command=['/usr/bin/python3',str(raw/'lower_bounds.py'),'--output',str(raw),'--archive',str(out),'--execute'];job=raw/'job.sh';job.write_text('#!/bin/bash\nset -eu\nexec '+' '.join(map(shlex.quote,command))+'\n')
  submit=['qsub','-h','-terse','-w','n','-cwd','-q','debian.q','-pe','threaded','1','-binding','linear:1','-l','exclusive=false,h_rt=00:30:00,h_vmem=8G','-m','n','-N','horizon_bound_audit','-j','y','-o',str(raw/'job.log'),'-S','/bin/bash',str(job)]
  r=subprocess.run(submit,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True);write(raw/'submission.json',dict(command=submit,returncode=r.returncode,response=r.stdout));r.check_returncode();assert r.stdout.strip().isdigit();print(r.stdout,end='');subprocess.run(['qrls',r.stdout.strip()],check=True);return
 request=read(raw/'request.json');assert Path(request['root'])==ROOT and Path(request['archive'])==out
 for name,sha in request['files'].items():assert digest(Path(name))==sha
 assert digest(ROOT/'mr24/warehouse.domain/maps/warehouse_large.map')==request['map_sha256'] and digest(ROOT/'cgar/inc/nlohmann/json.hpp')==request['json_header_sha256']
 sys.path.insert(0,str(ROOT/'tools'));from cpu_resources import cpu_resources
 cpu=cpu_resources();assert cpu['effective_cpu_quota'] is None;os.sched_setaffinity(0,cpu['representative_cpus'][:1]);out.mkdir(parents=True,exist_ok=False)
 replay=raw/'replay';write(raw/'fixture-cases.json',{'extra_loaded_fixture':str(replay/'loaded-fixture.json')})
 subprocess.run(['/usr/bin/python3',str(raw/'decompose.py'),'--output',str(replay),'--cases',str(raw/'fixture-cases.json'),'--map',str(replay/'fixture.map')],check=True);binary=replay/'decompose'
 residual=read(replay/'loaded-fixture-result.json')['known_horizon_bounds']['completed_single_holder_residuals_by_bound']['1']['0-49'];assert residual['excess']['n']==1 and residual['excess']['sum']==2 and residual['duration_sum']==5 and residual['bound_sum']==3
 retargeted=read(replay/'fixture-result.json')['known_horizon_bounds']['completed_single_holder_residuals_by_bound']['1'];assert all(x['excess']['n']==0 for x in retargeted.values())
 (raw/'bound-fixture.map').write_text('type octile\nheight 2\nwidth 10\nmap\n..........\n..........\n')
 d=dict(teamSize=2,makespan=5,numTaskFinished=0,numPlannerErrors=0,numScheduleErrors=0,numEntryTimeouts=0,start=[[0,0,'E'],[0,4,'W']],actualPaths=['W,W,W,W,W']*2,actualSchedule=['0:-1,5:0','0:-1'],tasks=[[0,0,[0,0,0,9]],[1,0,[0,0]]],events=[])
 fixtures={}
 for label in ['available','not_revealed','already_assigned']:
  x=json.loads(json.dumps(d))
  if label=='not_revealed':x['tasks'][1][1]=5
  if label=='already_assigned':x['actualSchedule'][1]='1:1'
  path=raw/(label+'.json');write(path,x);dest=raw/(label+'-result.json');subprocess.run([str(binary),str(raw/'bound-fixture.map'),str(path),str(dest)],check=True)
  r=read(dest)['known_horizon_bounds'];assert r['chronological_witnesses']==int(label=='available')
  if label=='available':assert r['examples'][0]['actual_bound']==10 and r['examples'][0]['remaining']==1 and r['examples'][0]['alternative_bound']==1
  fixtures[label]=r
 # Independent prospective-margin fixture: calibration task needs a turn and
 # one forward step (duration2, bound1). At admission9, actual repeated pickup
 # has bound2 with R2 and is demoted; same-cell alternative has bound1+margin1=2.
 margin_fixtures={}
 for label in ['observed','same_tick_completion_unobserved','retargeted_excluded']:
  d=dict(teamSize=3,makespan=10,numTaskFinished=1,numPlannerErrors=0,numScheduleErrors=0,numEntryTimeouts=0,
   start=[[0,3,'E'],[0,0,'S'],[0,2,'W']],actualPaths=['W,W,W,W,W,W,W,W,R,C','C,F,W,W,W,W,W,W,W,W','W,W,W,W,W,W,W,W,W,W'],
   actualSchedule=['0:-1,9:1','1:0,3:-1','0:-1'],tasks=[[0,0,[0,1]],[1,0,[0,4,0,4]],[2,0,[0,3]]],events=[[2,1,0,1]])
  if label=='same_tick_completion_unobserved':
   d['actualPaths'][1]='C,W,W,W,W,W,W,W,F,W';d['actualSchedule'][1]='1:0,10:-1';d['events']=[[9,1,0,1]]
  if label=='retargeted_excluded':
   d['actualPaths'][1]='C,W,W,W,W,W,W,W,W,W';d['actualPaths'][2]='W,F,W,W,W,W,W,W,W,W'
   d['actualSchedule'][1]='1:0,2:-1';d['actualSchedule'][2]='0:-1,2:0,3:-1';d['events']=[[2,2,0,1]]
  path=raw/('margin-'+label+'.json');write(path,d);dest=raw/('margin-'+label+'-result.json');subprocess.run([str(binary),str(raw/'bound-fixture.map'),str(path),str(dest)],check=True)
  r=read(dest)['known_horizon_bounds']['prospective_margin_audit'];assert r['feasible_unfinished_exposed']==1 and r['mean_margin_unsafe']==int(label=='observed') and r['chronological_witnesses']==int(label=='observed')
  if label=='observed':
   x=r['examples'][0];assert x['bound']==2 and x['remaining']==2 and x['bucket_count']==1 and x['bucket_excess_sum']==1 and x['alternative_bound']==1
  margin_fixtures[label]=r
 fixtures['prospective_margin']=margin_fixtures
 fixtures['completed_single_holder_residual']=residual;fixtures['retargeted_completion_excluded']=True;write(out/'fixtures.json',fixtures);reports={}
 for name,ref in read(raw/'reference.json')['reports'].items():
  path=Path(ref['input_path']);assert digest(path)==ref['input_sha256'];dest=raw/(name+'.json')
  subprocess.run([str(binary),str(ROOT/'mr24/warehouse.domain/maps/warehouse_large.map'),str(path),str(dest)],check=True)
  d=read(dest);assert d['completed_tasks']==ref['completed'] and d['empty_robot_steps_including_reassignments']==ref['phase_steps']['empty'];assert digest(path)==ref['input_sha256']
  r=d['known_horizon_bounds'];reports[name]=r;shutil.copy2(dest,out/dest.name);print('HORIZON_BOUND_COMPLETE',name,json.dumps(r['cohorts']),r['chronological_witnesses'],flush=True)
 for name in ['request.json','submission.json','reference.json']:shutil.copy2(raw/name,out/name)
 write(out/'accounting.json',dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),all_valid=True,fixtures_passed=3,reports=reports,binary_sha256=digest(binary),cpu_resources=cpu,selected_cpu=cpu['representative_cpus'][0]))
if __name__=='__main__':main()
