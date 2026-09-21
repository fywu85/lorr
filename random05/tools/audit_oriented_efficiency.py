#!/usr/bin/env python3
"""Independent collision-free loaded-route lower bounds on audited trajectories."""
import argparse,datetime,hashlib,json
from pathlib import Path
import numpy as np
from result_horizon import executed_steps

def digest(path):return hashlib.sha256(path.read_bytes()).hexdigest()
def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--distances',type=Path,required=True)
    parser.add_argument('--manifest',type=Path,required=True)
    parser.add_argument('--output',type=Path,required=True)
    args=parser.parse_args()
    with args.distances.open('rb') as stream:
        magic,rows,cols,count,states=map(int,np.fromfile(stream,dtype='<u4',count=5))
        assert magic==0x4f524431 and states==count*4
        cells=np.fromfile(stream,dtype='<u4',count=count)
        distances=np.fromfile(stream,dtype='<u2').reshape(states,states)
    index={int(cell):i for i,cell in enumerate(cells)}
    assert np.all(np.diag(distances)==0)
    for cell in range(count):
        for direction in range(4):
            assert distances[cell*4+direction,cell*4+(direction+1)%4]==1
            assert distances[cell*4+direction,cell*4+(direction+2)%4]==2
    records=[];cohorts={}
    for case in json.loads(args.manifest.read_text()):
        path=Path(case['result']);raw=json.loads(path.read_text());horizon=executed_steps(raw)
        assert all(raw[k]==0 for k in ('numPlannerErrors','numScheduleErrors','numEntryTimeouts'))
        goals={t[0]:[index[t[2][k]*cols+t[2][k+1]] for k in range(0,len(t[2]),2)] for t in raw['tasks']}
        orientation=[]
        for start,actions in zip(raw['start'],raw['actualPaths']):
            turns=np.fromiter((1 if a=='R' else -1 if a=='C' else 0 for a in actions.split(',')),dtype=np.int32)
            assert len(turns)==horizon
            orientation.append((np.r_[0,np.cumsum(turns)]+ 'ESWN'.index(start[2]))%4)
        pickups={};completed={}
        for time,agent,task,stage in raw['events']:
            if stage==1:pickups[task]=(time,int(orientation[agent][time]))
            if stage==len(goals[task]):completed[task]=time
        assert len(completed)==raw['numTaskFinished']
        work={};total_bound=0;total_actual=0
        for task,end in completed.items():
            start,heading=pickups[task];chain=goals[task]
            tail=np.zeros(4,dtype=np.int64)
            for before,after in reversed(list(zip(chain,chain[1:]))):
                matrix=distances[before*4:before*4+4,after*4:after*4+4].astype(np.int64)
                assert np.all(matrix<65535)
                tail=np.min(matrix+tail[None,:],axis=1)
            bound=int(tail[heading]);actual=end-start
            assert 0<=bound<=actual,(case['name'],task,bound,actual)
            total_bound+=bound;total_actual+=actual;work[task]=(actual,bound)
        robot_steps=raw['teamSize']*horizon
        records.append(dict(case,result_sha256=digest(path),tasks=len(work),steps=horizon,
            loaded_actual_steps=total_actual,loaded_relaxed_minimum_steps=total_bound,
            loaded_extra_steps=total_actual-total_bound,
            loaded_extra_fraction=(total_actual-total_bound)/total_actual,
            loaded_extra_fraction_of_all_robot_steps=(total_actual-total_bound)/robot_steps))
        cohorts[case['name']]=work
        print(case['name'],len(work),'loaded',total_actual,'bound',total_bound,'extra',total_actual-total_bound,flush=True)
    paired=[]
    for case in json.loads(args.manifest.read_text()):
        if not case.get('compare_to'):continue
        left,right=case['name'],case['compare_to'];shared=sorted(set(cohorts[left])&set(cohorts[right]))
        counts={name:dict(loaded_actual_steps=sum(cohorts[name][task][0] for task in shared),
                         loaded_relaxed_minimum_steps=sum(cohorts[name][task][1] for task in shared)) for name in [left,right]}
        paired.append(dict(left=left,right=right,common_tasks=len(shared),counts=counts,
            common_task_ids_sha256=hashlib.sha256(json.dumps(shared,separators=(',',':')).encode()).hexdigest()))
    result=dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
        manifest_sha256=digest(args.manifest),distance_sha256=digest(args.distances),
        source_sha256=digest(Path(__file__)),distance_source_sha256=digest(Path(__file__).with_name('oriented_distances.cpp')),
        rows=records,paired=paired,caveat='Independent directed unit-action shortest paths include turns and actual pickup headings but ignore collisions and allow zero-time repeated waypoints. These are relaxed lower bounds on already completed loaded work, not recoverable throughput, scheduling bounds, or unseen-input validation. Common task IDs still have different assignments, headings and traffic histories.')
    args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps(result,indent=2)+'\n')
if __name__=='__main__':main()
