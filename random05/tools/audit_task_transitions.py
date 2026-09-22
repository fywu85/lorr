#!/usr/bin/env python3
"""Decompose audited traces into pickup transitions, loaded work and censored tails."""
import argparse
from collections import Counter
import datetime
import hashlib
import json
from pathlib import Path
import numpy as np
from result_horizon import executed_steps


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def totals(records):
    keys = ('transition_steps', 'approach_bound', 'approach_extra',
            'loaded_steps', 'loaded_bound', 'loaded_extra',
            'pickup_heading_excess', 'assignment_changes')
    return dict(count=len(records), **{key:sum(r[key] for r in records) for key in keys})


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--distances',type=Path,required=True)
    parser.add_argument('--manifest',type=Path,required=True)
    parser.add_argument('--output',type=Path,required=True)
    args=parser.parse_args()
    with args.distances.open('rb') as stream:
        magic,rows,cols,count,states=map(int,np.fromfile(stream,dtype='<u4',count=5))
        assert magic==0x4f524431 and states==4*count
        cells=np.fromfile(stream,dtype='<u4',count=count)
        distance=np.fromfile(stream,dtype='<u2').reshape(states,states)
    index={int(cell):i for i,cell in enumerate(cells)}
    approach=distance.reshape(states,count,4).min(axis=2)
    cases=json.loads(args.manifest.read_text())
    details={};definitions={};outputs=[]
    for case in cases:
        path=Path(case['result']);raw=json.loads(path.read_text())
        horizon=executed_steps(raw);n=raw['teamSize']
        assert all(raw[k]==0 for k in ('numPlannerErrors','numScheduleErrors','numEntryTimeouts'))
        tasks={t[0]:[index[t[2][i]*cols+t[2][i+1]] for i in range(0,len(t[2]),2)] for t in raw['tasks']}
        release={t[0]:t[1] for t in raw['tasks']}
        for task,goals in tasks.items():
            assert task not in definitions or definitions[task]==goals
            definitions[task]=goals
        actions=[p.split(',') for p in raw['actualPaths']]
        assert len(actions)==n and all(len(p)==horizon for p in actions)
        headings=[];changes=[]
        for a,(start,path_actions) in enumerate(zip(raw['start'],actions)):
            turns=np.fromiter((1 if x=='R' else -1 if x=='C' else 0 for x in path_actions),dtype=np.int32)
            headings.append((np.r_[0,np.cumsum(turns)]+'ESWN'.index(start[2]))%4)
            changes.append([tuple(map(int,item.split(':'))) for item in raw['actualSchedule'][a].split(',') if item])
        pickups={task:time for time,agent,task,stage in raw['events'] if stage==1}
        task_ids=list(tasks)
        task_goals=np.array([tasks[t][0] for t in task_ids])
        last_finish=[0]*n
        last_cell=[index[r[0]*cols+r[1]] for r in raw['start']]
        active=[None]*n;records={};completed={}
        approach_actions=Counter();loaded_actions=Counter();tail_actions=Counter()
        approach_steps=loaded_steps=tail_steps=0
        last_event=0
        for time,agent,task,stage in raw['events']:
            assert last_event<=time<=horizon;last_event=time
            if stage==1:
                assert active[agent] is None and task not in records
                ready=last_finish[agent];state=last_cell[agent]*4+int(headings[agent][ready])
                bound=max(1,int(approach[state,tasks[task][0]]),release[task]-ready)
                elapsed=time-ready;assert bound<=elapsed
                eligible=np.fromiter((release[t]<=ready and pickups.get(t,horizon+1)>ready for t in task_ids),dtype=bool)
                nearest=max(1,int(approach[state,task_goals[eligible]].min())) if eligible.any() else None
                tail=np.zeros(4,dtype=np.int64)
                for before,after in reversed(list(zip(tasks[task],tasks[task][1:]))):
                    matrix=distance[before*4:before*4+4,after*4:after*4+4].astype(np.int64)
                    assert np.all(matrix<65535)
                    tail=np.min(matrix+tail[None,:],axis=1)
                heading=int(headings[agent][time])
                records[task]=dict(task=task,agent=agent,ready=ready,pickup=time,
                    initial_transition=ready==0,transition_steps=elapsed,
                    approach_bound=bound,approach_extra=elapsed-bound,
                    nearest_visible_pickup_bound=nearest,
                    released_after_ready=release[task]>ready,
                    pickup_heading_excess=int(tail[heading]-tail.min()),
                    loaded_bound=int(tail[heading]),
                    assignment_changes=sum(ready<t<=time and assigned>=0 for t,assigned in changes[agent]),
                    transition_actions=dict(Counter(actions[agent][ready:time])))
                active[agent]=task;approach_steps+=elapsed
                approach_actions.update(actions[agent][ready:time])
            if stage==len(tasks[task]):
                assert active[agent]==task and task not in completed
                r=records[task];elapsed=time-r['pickup']
                assert elapsed>=r['loaded_bound']
                r.update(completed=time,loaded_steps=elapsed,loaded_extra=elapsed-r['loaded_bound'])
                completed[task]=r;loaded_steps+=elapsed
                loaded_actions.update(actions[agent][r['pickup']:time])
                active[agent]=None;last_finish[agent]=time;last_cell[agent]=tasks[task][-1]
        for agent in range(n):
            if active[agent] is not None:
                time=records[active[agent]]['pickup'];loaded_steps+=horizon-time
                loaded_actions.update(actions[agent][time:])
            else:
                time=last_finish[agent];tail_steps+=horizon-time
                tail_actions.update(actions[agent][time:])
        assert len(completed)==raw['numTaskFinished']
        assert approach_steps+loaded_steps+tail_steps==n*horizon
        assert sum(approach_actions.values())==approach_steps
        assert sum(loaded_actions.values())==loaded_steps
        assert sum(tail_actions.values())==tail_steps
        full=list(completed.values());handoffs=[r for r in full if not r['initial_transition']]
        initial=[r for r in full if r['initial_transition']]
        outputs.append(dict(case,result_sha256=sha(path),tasks=len(completed),steps=horizon,
            partition=dict(robot_steps=n*horizon,approach_steps=approach_steps,
                loaded_steps=loaded_steps,censored_unopened_tail_steps=tail_steps,
                approach_actions=dict(approach_actions),loaded_actions=dict(loaded_actions),
                censored_tail_actions=dict(tail_actions)),
            completed=totals(full),initial=totals(initial),handoffs=totals(handoffs),
            completed_transition_histogram=dict(Counter(r['transition_steps'] for r in full)),
            completion_phase_counts={str(t):sum(r['completed']<=t for r in full) for t in range(100,horizon+1,100)},
            released_after_ready=sum(r['released_after_ready'] for r in full),
            longest_completed_transitions=sorted(full,key=lambda r:r['transition_steps'],reverse=True)[:12]))
        details[case['name']]=completed
        print(case['name'],len(completed),'all completed',totals(full),'handoffs',totals(handoffs),flush=True)
    paired=[]
    for case in cases:
        if not case.get('compare_to'):continue
        left,right=case['name'],case['compare_to'];common=sorted(set(details[left])&set(details[right]))
        paired.append(dict(left=left,right=right,common_completed_tasks=len(common),
            common_task_ids_sha256=hashlib.sha256(json.dumps(common,separators=(',',':')).encode()).hexdigest(),
            counts={name:totals([details[name][t] for t in common]) for name in [left,right]}))
    result=dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
        tool_sha256=sha(Path(__file__)),manifest_sha256=sha(args.manifest),distance_sha256=sha(args.distances),
        rows=outputs,paired=paired,
        caveats=['Completed-task cohorts exclude censored work; the full robot-step partition includes it.',
            'Approach lower bounds ignore collisions and use the previous completion pose or initial pose. The chosen task may be released later.',
            'Nearest visible pickup bounds ignore competition and task length; they are not assignment optima or recoverable savings.',
            'Pickup-heading excess ignores the approach cost of obtaining another heading.',
            'Same task IDs still have different robots, preceding tasks, headings, release times and traffic histories. Observed differences do not prove causal throughput gains.'])
    args.output.parent.mkdir(parents=True,exist_ok=True)
    args.output.write_text(json.dumps(result,indent=2)+'\n')


if __name__=='__main__':
    main()
