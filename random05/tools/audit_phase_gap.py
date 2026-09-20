#!/usr/bin/env python3
"""Audit phase counts and initial task choices on three frozen Random05 traces."""
from pathlib import Path
import json,hashlib,datetime
from collections import Counter,deque,defaultdict
root=Path(__file__).resolve().parents[2];out=root/'random05/results/4k-gap-audit-v69';out.mkdir(parents=True,exist_ok=True)
cases={'previous3857':'record-triage-split-full-v65/32-record-triage1.25-seed4/32-record-triage1.25-seed4/result.json','current3872':'directed-triage-split-full-v69/32-directed-triage-mix0.5-scale1.25-seed4/32-directed-triage-mix0.5-scale1.25-seed4/result.json','nms32':'nms-original-full-v1/nms-original-s0/result.json'}
mp=root/'Benchmark-Archive/2024 Competition/Main Round Evaluation Instances/random.domain/maps/random-32-32-20.map'
lines=mp.read_text().splitlines();rows=int(lines[1].split()[-1]);cols=int(lines[2].split()[-1]);free={y*cols+x for y,row in enumerate(lines[4:]) for x,c in enumerate(row) if c not in '@T'}
adj={v:[(y+dy)*cols+x+dx for dx,dy in [(1,0),(0,1),(-1,0),(0,-1)] if 0<=x+dx<cols and 0<=y+dy<rows and (y+dy)*cols+x+dx in free] for v in free for y,x in [divmod(v,cols)]};cache={}
def dist(a,b):
 if a not in cache:
  d={a:0};q=deque([a])
  while q:
   v=q.popleft()
   for u in adj[v]:
    if u not in d:d[u]=d[v]+1;q.append(u)
  cache[a]=d
 return cache[a][b]
records={};report={}
for name,rel in cases.items():
 p=root/'runs/random05'/rel;raw=p.read_bytes();d=json.loads(raw);records[name]=d
 assert d['makespan']==2000 and d['teamSize']==800 and all(d[k]==0 for k in ['numPlannerErrors','numScheduleErrors','numEntryTimeouts'])
 tasks={t[0]:[t[2][i]*cols+t[2][i+1] for i in range(0,len(t[2]),2)] for t in d['tasks']};release={t[0]:t[1] for t in d['tasks']};hops={t:sum(dist(a,b) for a,b in zip(goals,goals[1:])) for t,goals in tasks.items()}
 first_assignment={a:int(s.split(',')[0].split(':')[1]) for a,s in enumerate(d['actualSchedule']) if s and s.split(',')[0].split(':')[0]=='1'}
 blocks=[];arrivals=[[] for _ in range(8)];completed=[[] for _ in range(8)]
 for time,a,t,stage in d['events']:
  arrivals[(time-1)//250].append((time,a,t,stage))
  if stage==len(tasks[t]):completed[(time-1)//250].append(t)
 paths=[s.split(',') for s in d['actualPaths']]
 for b in range(8):
  counts=Counter(action for path in paths for action in path[b*250:(b+1)*250]);finished=completed[b]
  blocks.append(dict(start_step=b*250+1,end_step=(b+1)*250,completions=len(finished),waypoints=len(arrivals[b]),actions=dict(counts),completed_mean_stops=sum(len(tasks[t]) for t in finished)/len(finished),completed_mean_internal_hops=sum(hops[t] for t in finished)/len(finished),completed_initial_orders=sum(release[t]==0 for t in finished)))
 report[name]=dict(path=str(p.relative_to(root)),sha256=hashlib.sha256(raw).hexdigest(),tasks=d['numTaskFinished'],blocks=blocks,first_assignment=dict(count=len(first_assignment),chain_stops=dict(Counter(len(tasks[t]) for t in first_assignment.values())),internal_hops=sum(hops[t] for t in first_assignment.values()),approach_hops=sum(dist(d['start'][a][0]*cols+d['start'][a][1],tasks[t][0]) for a,t in first_assignment.items())))
a=records['previous3857'];b=records['current3872'];first=2001
for x,y in zip(a['actualPaths'],b['actualPaths']):first=min(first,next((i+1 for i,(u,v) in enumerate(zip(x.split(','),y.split(','))) if u!=v),2001))
result=dict(created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),first_action_divergence_3857_3872=first,runs=report,caveat='Observational phase comparisons on identical task/start input. Policies reveal later tasks at different times and choose different chains. Early NMS completion lead is not an estimate of recoverable gain.')
(out/'audit.json').write_text(json.dumps(result,indent=2)+'\n')
for name,r in report.items():print(name,'initial assignments',r['first_assignment'],'first block',r['blocks'][0])
md=['# Where the current Random05 gap occurs','','The directional-cutoff record3872 and preceding3857 have identical actions through step1543. Their15-task difference is entirely later; this does not establish a generic movement improvement.','','| Steps | Ours tasks | NMS tasks | Ours waypoints | NMS waypoints | Ours forwards | NMS forwards |','|---|---:|---:|---:|---:|---:|---:|']
for x,y in zip(report['current3872']['blocks'],report['nms32']['blocks']):md.append('| {}–{} | {} | {} | {} | {} | {} | {} |'.format(x['start_step'],x['end_step'],x['completions'],y['completions'],x['waypoints'],y['waypoints'],x['actions'].get('F',0),y['actions'].get('F',0)))
md+=['','NMS leads by108completions in the first250steps; ours leads in every later block. This identifies a phase worth investigating, not a causal savings estimate. Initial assignment and completed-task chain lengths are in [the raw audit](audit.json), along with motion counts and input trace hashes.','']
(out/'REPORT.md').write_text('\n'.join(md))
