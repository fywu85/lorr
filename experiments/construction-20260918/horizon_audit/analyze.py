#!/usr/bin/env python3
"""Read-only end-censoring audit of already verified Warehouse trajectories."""
import argparse,datetime,hashlib,json,os,shlex,shutil,subprocess,sys
from pathlib import Path
ROOT=next(p for p in Path(__file__).resolve().parents if (p/'tools/cpu_resources.py').is_file())
def read(p):return json.loads(p.read_text())
def write(p,x):p.write_text(json.dumps(x,indent=2)+'\n')
def digest(p):
 h=hashlib.sha256()
 with p.open('rb') as f:
  for chunk in iter(lambda:f.read(4*1024*1024),b''):h.update(chunk)
 return h.hexdigest()
def dist(v):
 v=sorted(v);return dict(n=len(v),sum=sum(v),mean=sum(v)/len(v) if v else None,p90=v[(len(v)-1)*9//10] if v else None)
def audit(d,thresholds):
 H=d['makespan'];N=d['teamSize'];assert len(d['actualSchedule'])==N
 tasks={r[0]:dict(stops=len(r[2])//2,pickup=-1,finish=-1,first=None,assignments=0) for r in d['tasks']};last=[0]*N
 for t,robot,tid,stop in d['events']:
  assert 0<t<=H and 0<=robot<N and 0<stop<=tasks[tid]['stops'];x=tasks[tid]
  if stop==1:x['pickup']=t
  if stop==x['stops']:
   assert x['finish']<0;x['finish']=t;last[robot]=max(last[robot],t)
 assert sum(x['finish']>=0 for x in tasks.values())==d['numTaskFinished']
 phase=dict(idle=0,empty=0,loaded=0);tail=dict(phase);unfinished=dict(phase);tail_completed_task_steps=0;assignment_entries=[]
 for robot,line in enumerate(d['actualSchedule']):
  entries=[tuple(map(int,e.split(':'))) for e in line.split(',') if e]
  assert all(0<=t<=H and (tid==-1 or tid in tasks) for t,tid in entries)
  assert all(a[0]<b[0] for a,b in zip(entries,entries[1:]))
  if not entries or entries[0][0]>1:entries.insert(0,(0,-1))
  covered=0
  for k,(t,tid) in enumerate(entries):
   start=max(1,t);end=entries[k+1][0]-1 if k+1<len(entries) else H
   if end<start:continue
   covered+=end-start+1
   if tid<0:
    segments=[('idle',start,end)];x=None
   else:
    x=tasks[tid];x['first']=t if x['first'] is None else min(t,x['first']);x['assignments']+=1;assignment_entries.append((t,tid))
    assert x['finish']<0 or end<=x['finish'],'schedule retained a completed task'
    if x['pickup']<0:segments=[('empty',start,end)]
    else:segments=[('empty',start,min(end,x['pickup'])),('loaded',max(start,x['pickup']+1),end)]
   for name,a,b in segments:
    count=max(0,b-a+1);post=max(0,b-max(a,last[robot]+1)+1)
    phase[name]+=count;tail[name]+=post
    if x is not None:
     if x['finish']<0:unfinished[name]+=count
     else:tail_completed_task_steps+=post
  assert covered==H,(robot,covered,H)
 assert sum(phase.values())==N*H
 assert sum(tail.values())==sum(H-t for t in last)
 cohorts={}
 for threshold in thresholds:
  admitted=[x for x in tasks.values() if x['first'] is not None and x['first']>=threshold]
  finished=[x for x in admitted if x['finish']>=0]
  cohorts[str(threshold)]=dict(first_admitted=len(admitted),completed=len(finished),completion_fraction=len(finished)/len(admitted) if admitted else None,
   picked_unfinished=sum(x['pickup']>=0 and x['finish']<0 for x in admitted),unpicked=sum(x['pickup']<0 for x in admitted),
   completed_elapsed=dist([x['finish']-max(1,x['first'])+1 for x in finished]),
   assignment_events_including_retargets=sum(t>=threshold for t,tid in assignment_entries))
 completed_durations=[x['finish']-max(1,x['first'])+1 for x in tasks.values() if x['finish']>=0 and x['first'] is not None]
 return dict(steps=H,robots=N,completed=d['numTaskFinished'],all_robot_steps=N*H,phase_steps=phase,
  post_last_completion_steps=tail,post_last_completion_total=sum(tail.values()),post_last_completion_fraction=sum(tail.values())/(N*H),
  post_last_completion_work_on_tasks_completed_by_another_robot=tail_completed_task_steps,
  robots_with_no_completion=sum(t==0 for t in last),unfinished_task_steps=unfinished,
  unfinished_task_total=sum(unfinished.values()),unfinished_task_fraction=sum(unfinished.values())/(N*H),
  completed_task_duration=dist(completed_durations),late_first_admission_cohorts=cohorts)
def fixtures():
 d=dict(makespan=10,teamSize=2,numTaskFinished=1,tasks=[[0,0,[0,0,0,1]],[1,0,[0,1,0,2]],[2,0,[0,3,0,4]]],
  events=[[3,0,0,1],[5,0,0,2],[8,0,1,1]],actualSchedule=['0:-1,2:0,6:1','1:0,2:2'])
 r=audit(d,[2,6]);assert r['phase_steps']==dict(idle=1,empty=15,loaded=4)
 assert r['post_last_completion_steps']==dict(idle=0,empty=13,loaded=2)
 assert r['unfinished_task_steps']==dict(idle=0,empty=12,loaded=2)
 assert r['post_last_completion_work_on_tasks_completed_by_another_robot']==1
 assert r['late_first_admission_cohorts']['6']['first_admitted']==1 and r['late_first_admission_cohorts']['6']['picked_unfinished']==1
 assert r['late_first_admission_cohorts']['2']['first_admitted']==2 and r['completed_task_duration']['sum']==5
 return dict(handoff_boundary_and_unfinished_fixture=r)
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--verification',type=Path,required=True);p.add_argument('--motion-accounting',type=Path,required=True)
 p.add_argument('--output',type=Path,required=True);p.add_argument('--archive',type=Path,required=True);p.add_argument('--execute',action='store_true');a=p.parse_args();raw=a.output.resolve();out=a.archive.resolve()
 if not a.execute:
  v=read(a.verification);assert v['all_valid_within_deadline_and_memory'] and not v['failures'];raw.mkdir(parents=True,exist_ok=False)
  rows=[r for r in v['rows'] if r['environment']['CGAR_TRICK_NATIVE_BANDS']=='1' and r['environment']['CGAR_TRICK_UNOPENED_MATCH']=='1' and r['environment']['CGAR_PICKUP_WEIGHT']=='5'];assert len(rows)==2
  files=[]
  for src,name in [(Path(__file__),'analyze.py'),(a.verification,'verification.json'),(a.motion_accounting,'motion-accounting.json')]:
   dest=raw/name;shutil.copy2(src,dest);files.append(dest)
  write(raw/'request.json',dict(created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),root=str(ROOT),archive=str(out),cases=[r['case'] for r in rows],files={str(f):digest(f) for f in files}))
  command=['/usr/bin/python3',str(raw/'analyze.py'),'--verification',str(raw/'verification.json'),'--motion-accounting',str(raw/'motion-accounting.json'),'--output',str(raw),'--archive',str(out),'--execute']
  job=raw/'job.sh';job.write_text('#!/bin/bash\nset -eu\nexec '+' '.join(map(shlex.quote,command))+'\n')
  submit=['qsub','-h','-terse','-w','n','-cwd','-q','debian.q','-pe','threaded','1','-binding','linear:1','-l','exclusive=false,h_rt=00:30:00,h_vmem=8G','-m','n','-N','horizon_work_audit','-j','y','-o',str(raw/'job.log'),'-S','/bin/bash',str(job)]
  r=subprocess.run(submit,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True);write(raw/'submission.json',dict(command=submit,returncode=r.returncode,response=r.stdout));r.check_returncode();assert r.stdout.strip().isdigit();print(r.stdout,end='');subprocess.run(['qrls',r.stdout.strip()],check=True);return
 req=read(raw/'request.json');assert Path(req['root'])==ROOT and Path(req['archive'])==out
 for name,sha in req['files'].items():assert digest(Path(name))==sha
 sys.path.insert(0,str(ROOT/'tools'));from cpu_resources import cpu_resources
 cpu=cpu_resources();assert cpu['effective_cpu_quota'] is None;os.sched_setaffinity(0,{cpu['representative_cpus'][0]})
 out.mkdir(parents=True,exist_ok=False);write(out/'fixtures.json',fixtures());write(out/'allocation.json',dict(cpu_resources=cpu,selected_cpu=cpu['representative_cpus'][0],job_id=os.environ['JOB_ID']))
 motion=read(raw/'motion-accounting.json');rows={r['case']:r for r in read(raw/'verification.json')['rows']};reports={}
 for name in req['cases']:
  row=rows[name];path=Path(row['raw_case'])/'WAREHOUSE.json';sha=digest(path);assert sha==motion['inputs'][name]['sha256'];d=read(path)
  assert d['makespan']==5000 and d['teamSize']==10000 and d['numTaskFinished']==row['tasks']
  assert not any(d[k] for k in ['numPlannerErrors','numScheduleErrors','numEntryTimeouts'])
  r=audit(d,[4000,4250,4500,4600,4750,4900]);assert r['phase_steps']['empty']==row['empty_robot_steps'];assert digest(path)==sha
  r.update(input_path=str(path),input_sha256=sha,trajectory_sha256=row['trajectory_sha256']);reports[name]=r;print('HORIZON_AUDIT',name,r['post_last_completion_total'],r['unfinished_task_total'],flush=True)
 for name in ['request.json','submission.json','verification.json','motion-accounting.json']:shutil.copy2(raw/name,out/name)
 write(out/'accounting.json',dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),all_valid=True,reports=reports,
  limits='Saved-trajectory accounting only. End-censored work includes time that cannot all be recovered. Retargeted tasks can complete on another robot, so post-last-completion work and unfinished-task work are different. Late cohorts use first admission, not every retarget. No horizon policy implemented or benchmark score.'))
if __name__=='__main__':main()
