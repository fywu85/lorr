from pathlib import Path
import json,statistics,collections
root=Path(__file__).resolve().parents[2]; results=[]
def stats(values):
 values=sorted(values)
 return {'n':len(values),'mean':round(statistics.mean(values),2) if values else None,'median':statistics.median(values) if values else None,'p90':values[int(.9*(len(values)-1))] if values else None}
for name in ['WAREHOUSE','SORTATION']:
 for label,path in [('draft',root/'runs'/('cgar_'+name+'_s5000.json')),('fixed',root/'runs/cgar-fixed-20260917-v2'/(name+'.json'))]:
  d=json.loads(path.read_text());counts=collections.Counter()
  for p in d['actualPaths']:
   for c in 'FWRC':counts[c]+=p.count(c)
  assignments={}
  for agent,s in enumerate(d['actualSchedule']):
   for entry in s.split(','):
    if not entry:continue
    t,task=map(int,entry.split(':'))
    if task>=0:assignments[task]=(t,agent)
  tasks={t[0]:t for t in d['tasks']};pickup={};completed={};donebins=[0]*5
  for t,r,task,stop in d['events']:
   if stop==1:pickup[task]=t
   if stop==len(tasks[task][2])//2:
    completed[task]=t;donebins[min((t-1)//1000,4)]+=1
  startdist=[];startpickup=[];pickupdelay=[];deliverydelay=[];chain=[]
  waiting_initial=0
  for task,(at,agent) in assignments.items():
   locs=tasks[task][2]
   chain.append(sum(abs(locs[k]-locs[k-2])+abs(locs[k+1]-locs[k-1]) for k in range(2,len(locs),2)))
   if task in pickup:pickupdelay.append(pickup[task]-at+1)
   if task in completed:deliverydelay.append(completed[task]-pickup[task])
   if at==1:
    row,col,_=d['start'][agent];startdist.append(abs(row-locs[0])+abs(col-locs[1]))
    if task in pickup:startpickup.append(pickup[task])
    else:waiting_initial+=1
  horizon=d['makespan'];pickup_robot_steps=0;delivery_robot_steps=0
  for task,(at,agent) in assignments.items():
   first=pickup.get(task,horizon+1);last=completed.get(task,horizon)
   pickup_robot_steps+=max(0,min(first,horizon+1)-at)
   if task in pickup:delivery_robot_steps+=max(0,last-first)
  item={'instance':name,'version':label,'tasks':d['numTaskFinished'],'completed_per_1000':donebins,'actions':dict(counts),'action_percent':{k:round(100*v/(d['teamSize']*horizon),2) for k,v in counts.items()},'pickup_delay_completed_pickups':stats(pickupdelay),'delivery_delay_completed_tasks':stats(deliverydelay),'assigned_chain_manhattan':stats(chain),'initial_assignment_count':len(startdist),'initial_pickup_manhattan':stats(startdist),'initial_pickup_time_observed':stats(startpickup),'initial_still_unpicked':waiting_initial,'pickup_agent_steps':pickup_robot_steps,'delivery_agent_steps':delivery_robot_steps}
  results.append(item);print(json.dumps(item),flush=True)
  del d
out=root/'experiments/throughput-20260917/trajectory-summary.json';out.write_text(json.dumps(results,indent=2)+'\n')
