#!/usr/bin/env python3
"""Inspect late unopened assignments using visible hop-based feasibility only."""
import argparse
from collections import defaultdict, deque
from pathlib import Path
from result_horizon import executed_steps
import datetime, hashlib, json


def audit(result, map_path):
    raw=result.read_bytes();data=json.loads(raw)
    lines=map_path.read_text().splitlines();cols=int(lines[2].split()[-1])
    free={r*cols+c for r,row in enumerate(lines[4:]) for c,x in enumerate(row) if x not in '@T'}
    adjacent={p:[q for q in (p+1,p+cols,p-1,p-cols) if q in free and abs(q//cols-p//cols)+abs(q%cols-p%cols)==1] for p in free}
    hops={}
    def dist(a,b):
        if a not in hops:
            d={a:0};queue=deque([a])
            while queue:
                p=queue.popleft()
                for q in adjacent[p]:
                    if q not in d:d[q]=d[p]+1;queue.append(q)
            hops[a]=d
        return hops[a][b]
    tasks={i:[xy[j]*cols+xy[j+1] for j in range(0,len(xy),2)] for i,_,xy in data['tasks']}
    releases={i:t for i,t,_ in data['tasks']}
    internal={i:sum(dist(a,b) for a,b in zip(g,g[1:])) for i,g in tasks.items()}
    changes=defaultdict(list);events=defaultdict(list)
    for a,row in enumerate(data['actualSchedule']):
        for pair in row.split(',') if row else []:
            t,i=map(int,pair.split(':'));changes[t].append((a,i))
    for t,a,i,s in data['events']:events[t].append((a,i,s))
    paths=[row.split(',') for row in data['actualPaths']]
    pos=[r*cols+c for r,c,d in data['start']];heading=['ESWN'.index(d) for r,c,d in data['start']]
    assignment=[-1]*data['teamSize'];stage=defaultdict(int);forward=0;frames=[]
    horizon=executed_steps(data)
    for t in range(1,horizon+1):
        for a,i in changes[t]:assignment[a]=i
        for a,path in enumerate(paths):
            action=path[t-1]
            if action=='F':pos[a]+=(1,cols,-1,-cols)[heading[a]];forward+=1
            elif action=='R':heading[a]=(heading[a]+1)%4
            elif action=='C':heading[a]=(heading[a]+3)%4
        for a,i,s in events[t]:stage[i]=s
        if t not in [1250,1500,1750,1850,1900,1950]:continue
        next_assignment=assignment.copy()
        for a,i in changes[t+1]:next_assignment[a]=i
        agents=[a for a,i in enumerate(assignment) if i<0 or stage[i] in (0,len(tasks[i]))]
        pool=sorted(i for i in tasks if releases[i]<=t and stage[i]==0)
        remaining=horizon-t
        rate=t*data['teamSize']/forward*1.25
        feasible={a:[i for i in pool if (dist(pos[a],tasks[i][0])+internal[i])*rate<=remaining] for a in agents}
        selected=sum(next_assignment[a] in feasible[a] for a in agents)
        # A maximum-cardinality feasible matching avoids counting one short task
        # repeatedly as an opportunity for many robots.
        owner={}
        def match(a,seen):
            for i in feasible[a]:
                if i in seen:continue
                seen.add(i)
                if i not in owner or match(owner[i],seen):owner[i]=a;return True
            return False
        maximum=sum(match(a,set()) for a in agents)
        assert selected<=maximum, "feasibility matching lost a valid existing assignment"
        frames.append(dict(step=t,unlocked_robots=len(agents),visible_unopened_tasks=len(pool),
                           estimated_steps_per_hop=rate,selected_feasible=selected,
                           maximum_estimated_feasible=maximum,
                           extra_estimated_feasible=maximum-selected,
                           infeasible_chosen_with_an_alternative=sum(next_assignment[a] not in feasible[a] and bool(feasible[a]) for a in agents)))
    return dict(created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),result=str(result),
                result_sha256=hashlib.sha256(raw).hexdigest(),tasks_finished=data['numTaskFinished'],
                frames=frames,caveat='Observational diagnostic, not a throughput bound or a causal gain. Feasibility uses the current hop distance and observed global movement rate, not the planner\'s directional blend. Each matching uses only tasks revealed by that frame. Completed-only future outcomes are not used.')

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--result',type=Path,required=True);p.add_argument('--map',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args()
    record=audit(a.result,a.map);a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(record,indent=2)+'\n')
    for f in record['frames']:print(f)
