#!/usr/bin/env python3
"""Offline cell waiting/turning concentration and full decision-latency quantiles.

Pure diagnosis from completed trajectories; no information is fed to the planner.
Wait and turn actions remain separate: required turns are not measured queue delay.
"""
import argparse
from array import array
import collections
import hashlib
import json
import math
from pathlib import Path
from flow_field_audit import read_map, neighbors


def replay(start, path, cols, adj, begin, visits, exits, waits, turns):
    u=start[0]*cols+start[1];d={'E':0,'S':1,'W':2,'N':3}[start[2]]
    for t,action in enumerate(path[::2]):
        included=t>=begin
        if included:visits[u]+=1
        if action=='F':
            if included:exits[u]+=1
            u=adj[4*u+d]
            assert u>=0
        elif action in 'RC':
            if included:turns[u]+=1
            d=(d+(1 if action=='R' else 3))%4
        else:
            assert action=='W'
            if included:waits[u]+=1
    return u,d


def concentration(values, cells):
    ordered=sorted((values[u] for u in cells),reverse=True);total=sum(ordered)
    return {'total':total,'cells':len(cells),'cells_with_positive_count':sum(x>0 for x in ordered),
            'top_cell_shares':{str(percent):sum(ordered[:max(1,math.ceil(len(cells)*percent/100))])/total if total else None
                               for percent in [1,5,10,25]}}


def latency(values):
    ordered=sorted(values);assert ordered
    return dict(samples=len(values),mean_ms=1000*sum(values)/len(values),
                **{key+'_ms':1000*ordered[math.ceil(len(values)*q)-1]
                   for key,q in [('p50',.5),('p90',.9),('p95',.95),('p99',.99),('max',1)]})


def analyze(case,window):
    metadata=json.loads((case/'metadata.json').read_text());summary=json.loads((case/'summary.json').read_text())[0]
    assert summary['valid'] and summary['makespan']==5000
    instance=summary['instance'];inp=Path(metadata['instances'][instance]['input']);spec=json.loads(inp.read_text());map_path=inp.parent/spec['mapFile']
    rows,cols,free=read_map(map_path);adj=neighbors(rows,cols,free);n=len(free)
    path=case/(instance+'.json');data=json.loads(path.read_text());horizon=data['makespan'];fleet=data['teamSize']
    assert horizon>=window and len(data['actualPaths'])==len(data['start'])==fleet
    visits=array('Q',[0])*n;exits=array('Q',[0])*n;waits=array('Q',[0])*n;turns=array('Q',[0])*n;totals=collections.Counter()
    for start,actions in zip(data['start'],data['actualPaths']):
        assert len(actions)==2*horizon-1
        replay(start,actions,cols,adj,horizon-window,visits,exits,waits,turns)
        totals.update(actions[::2])
    stationary=array('Q',(w+t for w,t in zip(waits,turns)))
    assert sum(visits)==fleet*window==sum(exits)+sum(stationary)
    # Compare independent replay with production cumulative phase diagnostics.
    points={};flow=[]
    for line in path.with_suffix('.log').read_text().splitlines():
        if line.startswith('[cgar-movement] '):
            row={k:int(v) for k,v in (x.split('=',1) for x in line.split()[1:])}
            if row['steps'] in [horizon-window,horizon]:points[row['steps'],row['phase']]=row
        elif line.startswith('[cgar-flow] '):
            flow.append({k:int(v) for k,v in (x.split('=',1) for x in line.split()[1:])})
    assert all((t,phase) in points for t in [horizon-window,horizon] for phase in [0,1,2])
    production={key:sum(points[horizon,phase][key]-points[horizon-window,phase][key] for phase in [0,1,2])
                for key in ['fw','cr','ccr','wait','closer','farther','equal','unknown']}
    assert production['fw']==sum(exits) and production['wait']==sum(waits) and production['cr']+production['ccr']==sum(turns)
    for action,key in [('F','fw'),('R','cr'),('C','ccr'),('W','wait')]:assert totals[action]==sum(points[horizon,phase][key] for phase in [0,1,2])
    free_cells=[u for u,ok in enumerate(free) if ok];active=[u for u in free_cells if visits[u]]
    timing=latency(data['entryComputeTimes']);assert timing['samples']==5000
    assert abs(timing['max_ms']/1000-summary['entry_compute_max_seconds'])<1e-10
    zero_exit=[u for u in active if not exits[u]]
    result={'case':case.name,'raw_case':str(case),'tasks':data['numTaskFinished'],'steps':horizon,'robots':fleet,'window_steps':window,
            'map_sha256':hashlib.sha256(map_path.read_bytes()).hexdigest(),'binary_sha256':metadata['binary_sha256'],
            'complete_replay_matches_production':True,'late_window':production,'latency_nearest_rank':timing,
            'free_cells':len(free_cells),'visited_cells':len(active),
            'concentration_over_free_cells':{k:concentration(v,free_cells) for k,v in [('wait',waits),('turn',turns),('stationary',stationary)]},
            'concentration_over_visited_cells':{k:concentration(v,active) for k,v in [('wait',waits),('turn',turns),('stationary',stationary)]},
            'occupied_cells_with_no_exit':len(zero_exit),'stationary_steps_on_no_exit_cells':sum(stationary[u] for u in zero_exit),
            'flow_publications':flow,
            'limitation':'Concentration is observational and traffic-volume dependent, not a delay model or causal benefit. Turns include unavoidable orientation changes. The window censors cell visits at both ends. No weighted-distance or route-choice counterfactual is computed.'}
    print(case.name,'late',production,'wait_top5',result['concentration_over_free_cells']['wait']['top_cell_shares']['5'],flush=True)
    return result


def self_test():
    adj=neighbors(1,3,bytearray([1,1,1]));v=array('Q',[0])*3;e=array('Q',[0])*3;w=array('Q',[0])*3;t=array('Q',[0])*3
    assert replay([0,0,'E'],'F,R,W,C,F',3,adj,0,v,e,w,t)==(2,0)
    assert list(v)==[1,4,0] and list(e)==[1,1,0] and list(w)==[0,1,0] and list(t)==[0,2,0]
    assert concentration(w,[0,1,2])['top_cell_shares']['5']==1
    assert latency([x/1000 for x in range(1,101)])['p95_ms']==95
    print('DWELL_AUDIT hand-counted waits, turns, exits, visits and nearest-rank quantile passed')


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--cases',type=Path,nargs='+');p.add_argument('--output',type=Path);p.add_argument('--window',type=int,default=1000);p.add_argument('--self-test',action='store_true');a=p.parse_args()
    if a.self_test:self_test();return
    if not a.cases or not a.output or a.window<=0:p.error('positive window, cases and output are required')
    assert not a.output.exists();self_test()
    out={'script_sha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),'cases':[analyze(q.resolve(),a.window) for q in a.cases]}
    a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(out,indent=2)+'\n')


if __name__=='__main__':main()
