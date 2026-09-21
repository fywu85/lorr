#!/usr/bin/env python3
"""Summarize task completions and robot actions in fixed simulation intervals."""
import argparse
from collections import Counter
import datetime
import hashlib
import json
from pathlib import Path
from result_horizon import executed_steps


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--manifest',type=Path,required=True)
    parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--interval',type=int,default=500)
    args=parser.parse_args()
    assert args.interval>0
    records=[]
    for case in json.loads(args.manifest.read_text()):
        path=Path(case['result']);raw=path.read_bytes();data=json.loads(raw)
        steps=executed_steps(data);assert steps%args.interval==0
        assert all(data[k]==0 for k in ('numPlannerErrors','numScheduleErrors','numEntryTimeouts'))
        lengths={t[0]:len(t[2])//2 for t in data['tasks']}
        phases=[dict(start=i+1,end=i+args.interval,tasks=0,waypoints=0,
                     actions=Counter(),completed_chain_lengths=Counter())
                for i in range(0,steps,args.interval)]
        for time,agent,task,stage in data['events']:
            assert 1<=time<=steps
            phase=phases[(time-1)//args.interval];phase['waypoints']+=1
            if stage==lengths[task]:
                phase['tasks']+=1;phase['completed_chain_lengths'][lengths[task]]+=1
        assert len(data['actualPaths'])==data['teamSize']
        for path_text in data['actualPaths']:
            path_actions=path_text.split(',');assert len(path_actions)==steps
            for phase in phases:phase['actions'].update(path_actions[phase['start']-1:phase['end']])
        cumulative=0
        for phase in phases:
            assert sum(phase['actions'].values())==args.interval*data['teamSize']
            cumulative+=phase['tasks'];phase['cumulative_tasks']=cumulative
        assert cumulative==data['numTaskFinished']
        records.append(dict(case,sha256=hashlib.sha256(raw).hexdigest(),tasks=cumulative,phases=phases))
        print(case['name'],[p['cumulative_tasks'] for p in phases],flush=True)
    out=dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
             manifest_sha256=hashlib.sha256(args.manifest.read_bytes()).hexdigest(),
             interval=args.interval,runs=records,
             caveat='Observational differences after trajectories diverge; task sets and reveal times differ. These counts are not causal estimates of recoverable work.')
    args.output.parent.mkdir(parents=True,exist_ok=True)
    args.output.write_text(json.dumps(out,indent=2)+'\n')


if __name__=='__main__':
    main()
