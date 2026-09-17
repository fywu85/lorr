from pathlib import Path
import json,statistics
root=Path(__file__).resolve().parents[2];out=[]
def stat(x):
 x=sorted(x);return {'mean':round(statistics.mean(x),2),'median':statistics.median(x),'p90':x[int(.9*(len(x)-1))]}
for name in ['WAREHOUSE','SORTATION']:
 for label,p in [('draft',root/'runs'/('cgar_'+name+'_s5000.json')),('fixed',root/'runs/cgar-fixed-20260917-v2'/(name+'.json'))]:
  d=json.loads(p.read_text());tasks={x[0]:x for x in d['tasks']};pickups={task:t for t,r,task,stop in d['events'] if stop==1};distances=[];delays=[];times=[]
  for r,sched in enumerate(d['actualSchedule']):
   at,task=next((t,k) for t,k in (map(int,x.split(':')) for x in sched.split(',')) if k>=0)
   row,col,o=d['start'][r];ori={'E':0,'S':1,'W':2,'N':3}[o]
   for a in d['actualPaths'][r][::2][:at-1]:
    if a=='F':row+=(0,1,0,-1)[ori];col+=(1,0,-1,0)[ori]
    elif a=='R':ori=(ori+1)%4
    elif a=='C':ori=(ori+3)%4
   target=tasks[task][2];distances.append(abs(row-target[0])+abs(col-target[1]));times.append(at)
   if task in pickups:delays.append(pickups[task]-at+1)
  item={'instance':name,'version':label,'cohort':'first task per robot','n':len(distances),'assignment_step':stat(times),'pickup_manhattan':stat(distances),'observed_pickup_delay':stat(delays),'observed_pickups':len(delays)};out.append(item);print(json.dumps(item),flush=True)
(root/'experiments/throughput-20260917/initial-cohorts.json').write_text(json.dumps(out,indent=2)+'\n')
