#!/usr/bin/env python3
"""Describe occupied forward cells during assigned robots' waits (not inferred intent)."""
import argparse
from collections import Counter, defaultdict
import datetime
import hashlib
import json
from pathlib import Path
from result_horizon import executed_steps


def analyze(path, cols):
    data=json.loads(path.read_text());steps=executed_steps(data)
    changes=defaultdict(list)
    for a,row in enumerate(data['actualSchedule']):
        if row:
            for pair in row.split(','):
                time,task=map(int,pair.split(':'));changes[time].append((a,task))
    paths=[row.split(',') for row in data['actualPaths']]
    loc=[row[0]*cols+row[1] for row in data['start']]
    direction=['ESWN'.index(row[2]) for row in data['start']]
    assignment=[-1]*data['teamSize'];counts=Counter();epochs=defaultdict(Counter)
    offsets=(1,cols,-1,-cols)
    for tick in range(1,steps+1):
        for a,task in changes[tick]:assignment[a]=task
        owner={v:a for a,v in enumerate(loc)};phase=epochs[str((tick-1)//200*200)]
        def record(key):counts[key]+=1;phase[key]+=1
        for a,actions in enumerate(paths):
            active=assignment[a]>=0
            record('assigned_robot_steps' if active else 'unassigned_robot_steps')
            if not active or actions[tick-1]!='W':continue
            record('assigned_waits')
            dest=loc[a]+offsets[direction[a]]
            # Horizontal row wrap cannot be a legal forward neighbor.
            if direction[a]%2==0 and loc[a]//cols!=dest//cols:continue
            b=owner.get(dest)
            if b is None:continue
            record('wait_facing_robot')
            if paths[b][tick-1]=='F':record('front_robot_leaves')
            else:
                record('front_robot_stays')
                record('stationary_front_assigned' if assignment[b]>=0 else 'stationary_front_unassigned')
        for a,actions in enumerate(paths):
            action=actions[tick-1]
            if action=='F':loc[a]+=offsets[direction[a]]
            elif action=='R':direction[a]=(direction[a]+1)%4
            elif action=='C':direction[a]=(direction[a]+3)%4
    return dict(result=str(path),result_sha256=hashlib.sha256(path.read_bytes()).hexdigest(),
                tasks=data['numTaskFinished'],steps=steps,counts=counts,epochs=epochs)


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--manifest',type=Path,required=True)
    parser.add_argument('--output',type=Path,required=True)
    args=parser.parse_args();declarations=json.loads(args.manifest.read_text())
    result=dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),runs={},
        caveat='Observational adjacency only: facing a robot does not establish an intended forward move or a recoverable throughput loss. Assigned tasks can be internally triaged; the trace exposes schedule assignment, not policy goal activity.')
    for case in declarations:
        row=analyze(Path(case['result']),int(case['columns']));result['runs'][case['name']]=row
        print(case['name'],row['counts'],flush=True)
    args.output.parent.mkdir(parents=True,exist_ok=True)
    args.output.write_text(json.dumps(result,indent=2)+'\n')


if __name__=='__main__':main()
