#!/usr/bin/env python3
"""Extend paired matrix analysis with reassignment and actual empty-travel metrics."""
import collections
import json
from pathlib import Path
import sys
HERE=Path(__file__).resolve().parent
# The prior analyzer imports its own helper called analyze; avoid this filename
# shadowing it when launched as a script.
sys.path[0]=str(HERE.parent/'throughput-20260918-next')
sys.path.insert(0,str(HERE.parent/'throughput-20260918-strict'))
import analyze_matrix
from analyze import distribution
base_metrics=analyze_matrix.get_metrics


def get_metrics(item):
    result=base_metrics(item)
    data=json.loads(item[1].read_text());horizon=data['makespan']
    pickups={task:step for step,robot,task,stop in data['events'] if stop==1}
    assignments=collections.defaultdict(list)
    forward=waiting=turning=0
    for robot,schedule in enumerate(data['actualSchedule']):
        entries=[tuple(map(int,entry.split(':'))) for entry in schedule.split(',') if entry]
        path=data['actualPaths'][robot]
        assert len(path)==2*horizon-1
        for i,(start,task) in enumerate(entries):
            if task<0:continue
            assignments[task].append((start,robot))
            end=min(entries[i+1][0]-1 if i+1<len(entries) else horizon,pickups.get(task,horizon))
            if end<start:continue
            actions=path[2*(start-1):2*end:2]
            forward+=actions.count('F');waiting+=actions.count('W');turning+=actions.count('R')+actions.count('C')
    for entries in assignments.values():entries.sort()
    # The original helper's single-owner delay is replaced here: a task can have
    # multiple owners, so both first- and last-assignment times matter.
    result['observed_pickup_delay']=distribution([pickups[t]-entries[0][0]+1 for t,entries in assignments.items() if t in pickups])
    result['last_assignment_pickup_delay']=distribution([pickups[t]-entries[-1][0]+1 for t,entries in assignments.items() if t in pickups])
    result['unpicked_assignment_age']=distribution([horizon-entries[0][0]+1 for t,entries in assignments.items() if t not in pickups])
    result['reassigned_tasks']=sum(len(entries)>1 for entries in assignments.values())
    result['max_changes_per_task']=max((len(entries)-1 for entries in assignments.values()),default=0)
    result['assignments_after_pickup']=sum(start>pickups[t] for t,entries in assignments.items() if t in pickups for start,robot in entries)
    result['empty_forward_actions']=forward;result['empty_wait_actions']=waiting;result['empty_turn_actions']=turning
    lines=item[1].with_suffix('.log').read_text().splitlines()
    matches=[line for line in lines if line.startswith('[cgar-reassignment]')]
    result['last_reassignment_counters']={k:int(v) for k,v in (field.split('=',1) for field in matches[-1].split()[1:])} if matches else {}
    # Match the frozen experiment declaration, not the historical default of one.
    # A nondefault bound also needs the solver's independently emitted receipt.
    metadata_path=item[1].parent/'metadata.json'
    environment=json.loads(metadata_path.read_text()).get('environment',{}) if metadata_path.exists() else {}
    budget=int(environment.get('CGAR_REASSIGN_MATCH_TASK_BUDGET','1'))
    assert 1<=budget<=8,(item,budget)
    receipts=[dict(field.split('=',1) for field in line.split()[1:])
              for line in lines if line.startswith('[cgar-match-retarget-config] ')]
    if budget!=1:
        assert receipts==[dict(task_budget=str(budget),finite='1',cooldown='20',started='protected',
                               primary='protected',recovery='protected',fair='protected',fixed_work='1')],(item,receipts)
    else:
        assert not receipts,(item,receipts)
    result['declared_task_retarget_budget']=budget
    retarget_gaps=[b[0]-a[0] for entries in assignments.values() for a,b in zip(entries[1:],entries[2:])]
    result['min_repeat_retarget_gap']=min(retarget_gaps) if retarget_gaps else None
    if budget!=1:assert all(gap>=20 for gap in retarget_gaps),(item,retarget_gaps)
    assert result['assignments_after_pickup']==0,(item,result['assignments_after_pickup'])
    assert result['max_changes_per_task']<=budget,(item,result['max_changes_per_task'],budget)
    return result


if __name__=='__main__':
    analyze_matrix.get_metrics=get_metrics
    analyze_matrix.main()
