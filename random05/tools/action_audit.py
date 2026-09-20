#!/usr/bin/env python3
"""Replay full trajectories and account for work before and after task pickup."""
import argparse
from collections import Counter, defaultdict, deque
import datetime
import hashlib
import json
from pathlib import Path

ACTIONS = 'FRCW'


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def audit(path, map_path):
    data = json.loads(path.read_text())
    lines = map_path.read_text().splitlines()
    rows, cols = int(lines[1].split()[-1]), int(lines[2].split()[-1])
    free = {y*cols+x for y, row in enumerate(lines[4:]) for x,c in enumerate(row) if c not in '@T'}
    adjacent = {}
    for p in free:
        y,x = divmod(p,cols)
        adjacent[p] = [((y+dy)*cols+x+dx if 0<=x+dx<cols and 0<=y+dy<rows
                         and (y+dy)*cols+x+dx in free else -1)
                        for dx,dy in [(1,0),(0,1),(-1,0),(0,-1)]]
    distances = {}
    def distance(a,b):
        if a not in distances:
            d={a:0};q=deque([a])
            while q:
                p=q.popleft()
                for v in adjacent[p]:
                    if v>=0 and v not in d:d[v]=d[p]+1;q.append(v)
            distances[a]=d
        return distances[a][b]
    n,steps = data['teamSize'],data['makespan']
    paths = [row.split(',') for row in data['actualPaths']]
    assert len(paths)==n and all(len(row)==steps for row in paths)
    goals={t[0]:[t[2][k]*cols+t[2][k+1] for k in range(0,len(t[2]),2)] for t in data['tasks']}
    changes=defaultdict(list);events=defaultdict(list)
    for a,row in enumerate(data['actualSchedule']):
        if row:
            for pair in row.split(','):
                time,task=map(int,pair.split(':'));changes[time].append((a,task))
    for time,a,task,stage in data['events']:events[time].append((a,task,stage))
    locations=[row[0]*cols+row[1] for row in data['start']]
    headings=['ESWN'.index(row[2]) for row in data['start']]
    assignment=[-1]*n;stage=defaultdict(int);work={};pickup={};finished={}
    phases={k:Counter() for k in ['unopened','loaded','idle']}
    switches=0;wait_streak=[0]*n;wait_lengths=[]
    for time in range(1,steps+1):
        for a,task in changes[time]:
            old=assignment[a]
            if old>=0 and old!=task:
                assert stage[old]==0 or stage[old]==len(goals[old]),('opened task reassigned',time,a,old,task)
                switches+=stage[old]==0
            assignment[a]=task
        chosen=[t for t in assignment if t>=0]
        assert len(chosen)==len(set(chosen)),('duplicate task',time)
        previous=locations.copy();owners={v:a for a,v in enumerate(previous)}
        for a,row in enumerate(paths):
            action=row[time-1];task=assignment[a]
            assert action in ACTIONS
            phase='idle' if task<0 else 'loaded' if stage[task]>0 else 'unopened'
            if task>=0:
                assert stage[task]<len(goals[task]),('completed task still assigned',time,a,task)
                if task not in work:work[task]={'unopened':Counter(),'loaded':Counter()}
                work[task][phase][action]+=1
            phases[phase][action]+=1
            if action=='W':wait_streak[a]+=1
            elif wait_streak[a]:wait_lengths.append(wait_streak[a]);wait_streak[a]=0
            if action=='F':locations[a]=adjacent[previous[a]][headings[a]];assert locations[a]>=0
            elif action=='R':headings[a]=(headings[a]+1)%4
            elif action=='C':headings[a]=(headings[a]+3)%4
        assert len(set(locations))==n,('vertex conflict',time)
        for a,(old,new) in enumerate(zip(previous,locations)):
            b=owners.get(new)
            assert old==new or b is None or locations[b]!=old,('edge swap',time,a,b)
        for a,task,next_stage in events[time]:
            assert assignment[a]==task,('event/assignment mismatch',time,a,task,assignment[a])
            assert next_stage==stage[task]+1 and locations[a]==goals[task][stage[task]],('event/position mismatch',time,a,task)
            stage[task]=next_stage
            if next_stage==1:pickup[task]=time
            if next_stage==len(goals[task]):finished[task]=time
    wait_lengths += [x for x in wait_streak if x]
    assert len(finished)==data['numTaskFinished']
    assert sum(sum(c.values()) for c in phases.values())==n*steps
    groups={k:{p:Counter() for p in ['unopened','loaded']} for k in ['completed','unfinished']}
    for task,counts in work.items():
        group=groups['completed' if task in finished else 'unfinished']
        for phase,count in counts.items():group[phase].update(count)
    lower_bound=sum(sum(distance(a,b) for a,b in zip(goals[t],goals[t][1:])) for t in finished)
    loaded=groups['completed']['loaded']
    assert loaded['F']>=lower_bound
    assert sum(loaded.values())==sum(finished[t]-pickup[t] for t in finished)
    ordered=sorted(wait_lengths)
    quantile=lambda fraction:ordered[min(len(ordered)-1,int((len(ordered)-1)*fraction))] if ordered else 0
    return dict(result=str(path),result_sha256=sha(path),map_sha256=sha(map_path),tasks=len(finished),
                steps=steps,agents=n,robot_steps=n*steps,phases=phases,work_by_final_status=groups,
                completed_loaded_lower_bound_cells=lower_bound,
                completed_loaded_detour_cells=loaded['F']-lower_bound,
                completed_loaded_mean_steps=sum(loaded.values())/len(finished),
                completed_chain_lengths=dict(Counter(len(goals[t]) for t in finished)),
                completed_loaded_work={str(t):dict(work[t]["loaded"]) for t in finished},
                unopened_reassignments=switches,task_starts=len(pickup),
                waiting_streaks=dict(count=len(ordered),p50=quantile(.5),p90=quantile(.9),p99=quantile(.99),maximum=max(ordered,default=0)),
                replay_valid=True,
                caveat='Work attribution is observational; different policies complete different task sets. Unfinished work is not a recoverable-throughput estimate.')


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--manifest',type=Path,required=True)
    parser.add_argument('--output',type=Path,required=True)
    args=parser.parse_args()
    manifest=json.loads(args.manifest.read_text());output={}
    for case in manifest:
        output[case['name']]=audit(Path(case['result']),Path(case['map']))
        print(case['name'],output[case['name']]['tasks'],'replay/accounting passed',flush=True)
    paired={}
    names=list(output)
    for left in names[:2]:
        for right in names[2:]:
            a=output[left]['completed_loaded_work'];b=output[right]['completed_loaded_work']
            shared=sorted(set(a)&set(b),key=int)
            counts={}
            for name,work in [(left,a),(right,b)]:
                total=Counter()
                for task in shared:total.update(work[task])
                counts[name]=dict(total)
            paired[left+' vs '+right]=dict(tasks=len(shared),counts=counts,
                task_ids_sha256=hashlib.sha256(json.dumps(shared,separators=(',',':')).encode()).hexdigest(),
                caveat='Same completed task IDs; assignments and congestion histories still differ.')
    for value in output.values():value.pop('completed_loaded_work')
    record=dict(created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                source_sha256=sha(Path(__file__)),manifest_sha256=sha(args.manifest),runs=output,paired=paired)
    args.output.parent.mkdir(parents=True,exist_ok=True)
    args.output.write_text(json.dumps(record,indent=2)+'\n')


if __name__=='__main__':
    main()
