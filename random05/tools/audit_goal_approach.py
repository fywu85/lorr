#!/usr/bin/env python3
"""Measure loaded motion near the next goal on already replayed full traces."""
import argparse
from collections import Counter, defaultdict, deque
import datetime
import hashlib
import json
from pathlib import Path
from result_horizon import executed_steps


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--manifest',type=Path,required=True)
    parser.add_argument('--output',type=Path,required=True)
    args=parser.parse_args()
    manifest=json.loads(args.manifest.read_text())
    data={row['name']:json.loads(Path(row['result']).read_text()) for row in manifest}
    lines=Path(manifest[0]['map']).read_text().splitlines()
    rows,cols=int(lines[1].split()[-1]),int(lines[2].split()[-1])
    free={y*cols+x for y,row in enumerate(lines[4:]) for x,c in enumerate(row) if c not in '@T'}
    adjacent={p:[(p//cols+dy)*cols+p%cols+dx if 0<=p//cols+dy<rows and 0<=p%cols+dx<cols and (p//cols+dy)*cols+p%cols+dx in free else -1 for dx,dy in [(1,0),(0,1),(-1,0),(0,-1)]] for p in free}
    goals={name:{t[0]:[t[2][k]*cols+t[2][k+1] for k in range(0,len(t[2]),2)] for t in d['tasks']} for name,d in data.items()}
    completed={name:{t for _,_,t,k in d['events'] if k==len(goals[name][t])} for name,d in data.items()}
    common=set.intersection(*completed.values())
    for task in common:
        assert all(goals[name][task]==goals[manifest[0]['name']][task] for name in data)
    distances={}
    def hop(goal,p):
        if goal not in distances:
            dist={goal:0};q=deque([goal])
            while q:
                u=q.popleft()
                for v in adjacent[u]:
                    if v>=0 and v not in dist:dist[v]=dist[u]+1;q.append(v)
            distances[goal]=dist
        return distances[goal][p]
    def bucket(d):
        return str(d) if d<=2 else '3-4' if d<=4 else '5-8' if d<=8 else '9+'
    output={}
    for row in manifest:
        name=row['name'];d=data[name];n=d['teamSize'];steps=executed_steps(d)
        assert len(completed[name])==d['numTaskFinished']
        paths=[r.split(',') for r in d['actualPaths']]
        positions=[r[0]*cols+r[1] for r in d['start']];directions=['ESWN'.index(r[2]) for r in d['start']]
        assignments=[-1]*n;stages=defaultdict(int);changes=defaultdict(list);events=defaultdict(list)
        for a,r in enumerate(d['actualSchedule']):
            for item in r.split(',') if r else []:
                time,task=map(int,item.split(':'));changes[time].append((a,task))
        for time,a,task,k in d['events']:events[time].append((a,task,k))
        all_counts=defaultdict(Counter);common_counts=defaultdict(Counter)
        for time in range(1,steps+1):
            for a,task in changes[time]:assignments[a]=task
            for a,path in enumerate(paths):
                action=path[time-1];task=assignments[a];p=positions[a]
                q=adjacent[p][directions[a]] if action=='F' else p
                assert q>=0
                if task>=0 and stages[task]>0:
                    goal=goals[name][task][stages[task]];distance=hop(goal,p)
                    bins=[all_counts[bucket(distance)]]
                    if task in common:bins.append(common_counts[bucket(distance)])
                    for counts in bins:
                        counts[action]+=1
                        if action=='F':counts['toward' if hop(goal,q)<distance else 'away' if hop(goal,q)>distance else 'lateral']+=1
                positions[a]=q
                if action=='R':directions[a]=(directions[a]+1)%4
                if action=='C':directions[a]=(directions[a]+3)%4
            for a,task,k in events[time]:
                assert assignments[a]==task and k==stages[task]+1
                assert positions[a]==goals[name][task][stages[task]]
                stages[task]=k
        output[name]=dict(result=row['result'],result_sha256=sha(Path(row['result'])),tasks=d['numTaskFinished'],steps=steps,loaded=all_counts,common_completed_loaded=common_counts)
    record=dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),tool_sha256=sha(Path(__file__)),manifest_sha256=sha(args.manifest),common_completed_tasks=len(common),common_task_ids_sha256=hashlib.sha256(json.dumps(sorted(common)).encode()).hexdigest(),runs=output,caveat='Observational action counts by geometric distance to the next waypoint. Same completed task IDs still have different assignments and traffic histories; differences are not a recoverable-throughput estimate.')
    args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps(record,indent=2)+'\n')
    for name,r in output.items():print(name,r['common_completed_loaded'])


if __name__=='__main__':main()
