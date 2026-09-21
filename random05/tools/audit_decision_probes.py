#!/usr/bin/env python3
"""Audit saved-state counterfactuals against the original complete trajectory."""
import argparse
from collections import Counter, defaultdict
import datetime
import hashlib
import json
from pathlib import Path
from result_horizon import executed_steps
import statistics


def read(path):return json.loads(path.read_text())
def sha(path):return hashlib.sha256(path.read_bytes()).hexdigest()
def ranks(values):
    return [sum(v<x for v in values)+(sum(v==x for v in values)-1)/2 for x in values]
def correlation(left,right):
    a,b=ranks(left),ranks(right);ma,mb=statistics.mean(a),statistics.mean(b)
    da=sum((x-ma)**2 for x in a);db=sum((y-mb)**2 for y in b)
    return sum((x-ma)*(y-mb) for x,y in zip(a,b))/(da*db)**.5 if da*db else None


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--original',type=Path,required=True)
    parser.add_argument('--capture',type=Path,required=True,help='Snapshot benchmark case directory')
    parser.add_argument('--probe-root',type=Path,default=Path('runs/random05'))
    parser.add_argument('--output',type=Path,required=True)
    args=parser.parse_args();args.output.mkdir(parents=True,exist_ok=True)
    original=read(args.original);captured=read(args.capture/'result.json')
    fields=('actualPaths','plannerPaths','actualSchedule','plannerSchedule','events','tasks')
    for field in fields:assert original[field]==captured[field],field
    assert original['numTaskFinished']==captured['numTaskFinished']==3852
    snapshots={int(p.stem.split('-')[1]):(p,read(p)) for p in (args.capture/'cwd/snapshots').glob('step-*.json')}
    assert sorted(snapshots)==list(range(250,2000,250))
    paths=[row.split(',') for row in original['actualPaths']];events=defaultdict(list);changes=defaultdict(list)
    for t,a,task,stage in original['events']:events[t].append((a,task,stage))
    for a,row in enumerate(original['actualSchedule']):
        for pair in row.split(',') if row else []:
            t,task=map(int,pair.split(':'));changes[t].append((a,task))
    goals={task[0]:[task[2][i]*32+task[2][i+1] for i in range(0,len(task[2]),2)] for task in original['tasks']}
    reveals={task[0]:task[1] for task in original['tasks']}
    positions=[s[0]*32+s[1] for s in original['start']];headings=['ESWN'.index(s[2]) for s in original['start']]
    schedule=[-1]*len(paths);stages=defaultdict(int);finished=set();cumulative={}
    totals=dict(finished=0,waypoints=0,forwards=0,rotations=0,waits=0);cumulative[0]=dict(totals)
    for time in range(1,executed_steps(original)+1):
        for a,task in changes[time]:schedule[a]=task
        for a,row in enumerate(paths):
            action=row[time-1]
            if action=='F':positions[a]+=(1,32,-1,-32)[headings[a]];totals['forwards']+=1
            elif action in 'RC':headings[a]=(headings[a]+(1 if action=='R' else 3))%4;totals['rotations']+=1
            else:assert action=='W';totals['waits']+=1
        for a,task,stage in events[time]:
            assert schedule[a]==task and positions[a]==goals[task][stage-1]
            stages[task]=stage;totals['waypoints']+=1
            if stage==len(goals[task]):finished.add(task);schedule[a]=-1;totals['finished']+=1
        cumulative[time]=dict(totals)
        if time in snapshots:
            _,snapshot=snapshots[time]
            assert [s[0] for s in snapshot['states']]==positions
            assert [s[2] for s in snapshot['states']]==headings
            assert snapshot['schedule']==schedule
            pool={task for task in goals if reveals[task]<=time and task not in finished}
            assert pool=={task['id'] for task in snapshot['tasks']}
            for task in snapshot['tasks']:
                assert task['stage']==stages[task['id']] and task['locations']==goals[task['id']]
            selected=next(c for c in snapshot['candidates'] if c['candidate']==snapshot['selected_candidate'])
            assert ''.join('FRCW'[a] for a in selected['actions'])==''.join(row[time] for row in paths)
    control_dir=args.probe_root/'decision-probe-original-control-v67'
    assert read(control_dir/'completion.json')['valid']
    control=read(control_dir/'seed-1.json');assert control['future_k']==16320 and control['future_seed']==-1
    row=control['counterfactuals'][0];assert row['selected'] and row['reproduced_first_decision']
    start=control['timestep']
    for outcome in row['outcomes']:
        for field in totals:assert outcome[field]==cumulative[start+outcome['step']][field]-cumulative[start][field]
    states=[]
    for time,(snapshot_path,snapshot) in sorted(snapshots.items()):
        directory=args.probe_root/('decision-probe-v67-t'+str(time))
        assert read(directory/'completion.json')['valid']
        spec=read(directory/'probe-spec.json');assert spec['snapshot_sha256']==sha(snapshot_path)
        allocation=read(directory/'allocation.json')['resources']
        assert allocation['cpu_model']==spec['cpu_model'] and allocation['physical_cores_visible']==spec['physical']
        for file,digest in spec['input_hashes'].items():assert sha(Path(file))==digest
        runs=[read(directory/('seed'+str(seed)+'.json')) for seed in spec['future_seeds']]
        assert all(run['steps']==128 and run['future_k']==1280 for run in runs)
        candidates=[]
        for k,candidate in enumerate(snapshot['candidates']):
            values=[]
            for run in runs:
                trial=run['counterfactuals'][k]
                assert trial['candidate']==candidate['candidate'] and trial['score']==candidate['score']
                assert trial['reproduced_first_decision'] and len(trial['outcomes'])==128
                values.append(trial['finished'])
            candidates.append(dict(rank=candidate['rank'],score=candidate['score'],values=values,
                mean=statistics.mean(values),selected=candidate['candidate']==snapshot['selected_candidate']))
        chosen=next(c for c in candidates if c['selected']);winner=max(candidates,key=lambda c:c['mean'])
        correlations={}
        for horizon in (8,16,32,64,128):
            means=[statistics.mean(run['counterfactuals'][k]['outcomes'][horizon-1]['finished'] for run in runs) for k in range(len(candidates))]
            correlations[str(horizon)]=correlation([c['score'] for c in candidates],means)
        loo=[]
        for held in range(len(runs)):
            winner_index=max(range(len(candidates)),key=lambda k:sum(v for i,v in enumerate(candidates[k]['values']) if i!=held))
            loo.append(candidates[winner_index]['values'][held]-chosen['values'][held])
        states.append(dict(timestep=time,candidates=candidates,selected_mean=chosen['mean'],best_observed_mean=winner['mean'],
            best_observed_rank=winner['rank'],spearman_by_horizon=correlations,leave_one_seed_out_gains=loo,
            snapshot_sha256=sha(snapshot_path),probe_binary_sha256=spec['binary_sha256']))
    report=dict(created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),original_sha256=sha(args.original),
        capture_sha256=sha(args.capture/'result.json'),full_trajectory_fields_equal=list(fields),
        snapshots_match_original_state=True,control_cumulative_fields_equal=list(totals),control_steps=128,
        states=states,caveats=[
            'All states come from one development trajectory, not independent task/start inputs.',
            'Downstream work1280 differs from real search16320; common seeds reduce but do not remove noise.',
            'Best observed means select on the same samples and overstate recoverable gain.',
            'Leave-one-seed-out root choice uses offline counterfactual outcomes, not a deployable policy.',
            'These partial continuations are diagnostics, never full-run throughput frontiers.'])
    (args.output/'audit.json').write_text(json.dumps(report,indent=2)+'\n')
    lines=['# Saved-state decision audit','',
        'The diagnostic capture reproduces all six full trajectory fields of the3,852-task original. All seven snapshots match its positions, headings, task pools, stages and assignments. A full-budget continuation control matches all five cumulative motion/task metrics at every one of128steps.','',
        'Each state compares eight distinct fully evaluated decisions under128steps of closed-loop replanning atK1280, with common downstream seeds0/1/2. The live solver usedK16320.','',
        '| Real step | Selected mean completions | Best observed mean | Best rank | Score/128-step completions Spearman | Leave-one-seed-out mean gain |',
        '|---:|---:|---:|---:|---:|---:|']
    for state in states:
        corr=state['spearman_by_horizon']['128']
        lines.append('| {} | {:.2f} | {:.2f} | {} | {} | {:+.2f} |'.format(state['timestep'],state['selected_mean'],state['best_observed_mean'],state['best_observed_rank'],'n/a' if corr is None else '{:.3f}'.format(corr),statistics.mean(state['leave_one_seed_out_gains'])))
    lines+=['','These small, correlated samples do not establish systematic scoring failure or a causal throughput gain. The best observed mean has selection bias. Leave-one-seed-out selection is an offline diagnostic using information unavailable to the live solver. Full benchmarks determine improvements.','','[Machine-readable checks and all candidate outcomes](audit.json).','']
    (args.output/'REPORT.md').write_text('\n'.join(lines));print('\n'.join(lines))
if __name__=='__main__':main()
