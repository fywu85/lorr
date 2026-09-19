#!/usr/bin/env python3
"""Reconstruct frozen flow fields and motion stalls from saved trajectories.

Read-only diagnosis, not a planner change or a causal throughput attribution.
All map geometry comes from the case input; no warehouse layout rules are used.
"""
import argparse
from array import array
import collections
import hashlib
import json
from pathlib import Path
import sys


def read_map(path):
    lines=path.read_text().splitlines()
    rows=int(lines[1].split()[1]);cols=int(lines[2].split()[1])
    assert lines[3]=='map' and len(lines[4:])==rows
    assert all(len(line)==cols for line in lines[4:])
    free=bytearray(c not in '@T' for line in lines[4:] for c in line)
    return rows,cols,free


def neighbors(rows,cols,free):
    result=array('i',[-1])*(len(free)*4)
    for u,ok in enumerate(free):
        if not ok:continue
        for d,v in enumerate([u+1,u+cols,u-1,u-cols]):
            if d==0 and u%cols==cols-1 or d==2 and u%cols==0:continue
            if 0<=v<len(free) and free[v]:result[4*u+d]=v
    return result


def fields(counts,adj,minimum,margin,strength,base):
    costs=bytearray([base])*len(adj)
    for edge,v in enumerate(adj):
        if v<0:continue
        u,d=divmod(edge,4)
        f=counts[edge];r=counts[4*v+(d+2)%4];total=f+r
        if total>=minimum and r>f and (r-f)*100>margin*total:
            costs[edge]+= (strength*(r-f)+total-1)//total
    return costs


def components(free,adj,cost,base):
    # Iterative Kosaraju, with reverse edges obtained from the map neighbors.
    visited=bytearray(len(free));order=[]
    for root,ok in enumerate(free):
        if not ok or visited[root]:continue
        stack=[(root,0)];visited[root]=1
        while stack:
            u,d=stack[-1]
            if d==4:
                order.append(u);stack.pop();continue
            stack[-1]=(u,d+1);v=adj[4*u+d]
            if v>=0 and cost[4*u+d]==base and not visited[v]:
                visited[v]=1;stack.append((v,0))
    labels=array('i',[-1])*len(free);sizes=[]
    for root in reversed(order):
        if labels[root]>=0:continue
        cid=len(sizes);stack=[root];labels[root]=cid;size=0
        while stack:
            u=stack.pop();size+=1
            for d in range(4):
                v=adj[4*u+d]
                if v>=0 and cost[4*v+(d+2)%4]==base and labels[v]<0:
                    labels[v]=cid;stack.append(v)
        sizes.append(size)
    outgoing=bytearray(len(sizes))
    for u,ok in enumerate(free):
        if not ok:continue
        for d in range(4):
            v=adj[4*u+d]
            if v>=0 and cost[4*u+d]==base and labels[u]!=labels[v]:outgoing[labels[u]]=1
    return labels,sizes,outgoing


def counters(log):
    flow=[];motion={}
    for line in log.read_text().splitlines():
        if line.startswith('[cgar-flow] '):
            flow.append({k:int(v) for k,v in (w.split('=',1) for w in line.split()[1:])})
        if line.startswith('[cgar-movement] '):
            row={k:int(v) for k,v in (w.split('=',1) for w in line.split()[1:])};motion[row['phase']]=row
    return flow,motion


def analyze(case):
    metadata=json.loads((case/'metadata.json').read_text())
    summaries=json.loads((case/'summary.json').read_text())
    assert len(summaries)==1 and summaries[0]['valid']
    instance=summaries[0]['instance'];input_path=Path(metadata['instances'][instance]['input'])
    input_data=json.loads(input_path.read_text());map_path=input_path.parent/input_data['mapFile']
    rows,cols,free=read_map(map_path);adj=neighbors(rows,cols,free)
    path=case/(instance+'.json');data=json.loads(path.read_text())
    horizon=data['makespan'];fleet=data['teamSize'];assert len(data['actualPaths'])==len(data['start'])==fleet
    env=metadata['environment'];warmup=int(env.get('CGAR_FLOW_WARMUP','128'))
    strength=int(env.get('CGAR_FLOW_STRENGTH','0'));minimum=int(env.get('CGAR_FLOW_MIN_SAMPLES','8'))
    margin=int(env.get('CGAR_FLOW_MIN_MARGIN_PERCENT','0'));base=int(env.get('CGAR_FLOW_COST_SCALE','1'))
    assert int(env.get('CGAR_FLOW_REFRESH_INTERVAL','0'))==0,'This audit is for frozen fields only'
    counts=array('Q',[0])*len(adj);directions={'E':0,'S':1,'W':2,'N':3}
    for start,text in zip(data['start'],data['actualPaths']):
        u=start[0]*cols+start[1];d=directions[start[2]]
        assert free[u] and len(text)==2*horizon-1
        for action in text[:2*min(warmup,horizon):2]:
            if action=='F':
                counts[4*u+d]+=1;u=adj[4*u+d];assert u>=0
            elif action=='R':d=(d+1)%4
            elif action=='C':d=(d+3)%4
            else:assert action=='W'
    cost=fields(counts,adj,minimum,margin,strength,base)
    penalized=sum(c>base for c in cost);observed=sum(counts)
    flow,motion=counters(path.with_suffix('.log'))
    if strength:
        assert len(flow)==1 and flow[0]['samples']==warmup
        assert observed==flow[0]['moves'] and penalized==flow[0]['penalized_edges'],(case,observed,penalized,flow)
    labels,sizes,outgoing=components(free,adj,cost,base)
    largest=max(sizes)
    ends=list(range(1000,horizon+1,1000))
    if not ends or ends[-1]!=horizon:ends.append(horizon)
    windows=[collections.Counter() for _ in ends];snapshots=[collections.Counter() for _ in ends]
    for start,text in zip(data['start'],data['actualPaths']):
        u=start[0]*cols+start[1];d=directions[start[2]];last_forward=0;recent=[u]*256;last_turn=None;wi=0
        for t,action in enumerate(text[::2],1):
            windows[wi][action]+=1
            if action=='F':
                windows[wi]['penalized_F' if cost[4*u+d]>base else 'neutral_F']+=1
                u=adj[4*u+d];assert u>=0;last_forward=t;last_turn=None
            elif action in 'RC':
                if last_turn is not None and action!=last_turn:windows[wi]['opposite_turn_without_intervening_forward']+=1
                last_turn=action;d=(d+(1 if action=='R' else 3))%4
            else:assert action=='W'
            recent[(t-1)%256]=u
            if t==ends[wi]:
                snap=snapshots[wi];stall=t-last_forward
                for threshold in [100,500,1000]:snap['robots_no_forward_for_'+str(threshold)]+=stall>=threshold
                unique=len(set(recent[:min(t,256)]))
                for threshold in [1,4,16]:snap['robots_at_most_'+str(threshold)+'_cells_last_256']+=unique<=threshold
                cid=labels[u]
                snap['robots_in_largest_neutral_scc']+=sizes[cid]==largest
                snap['robots_in_neutral_sink_scc_lt_100']+=not outgoing[cid] and sizes[cid]<100
                wi+=1
        assert wi==len(ends)
    totals=sum(windows,collections.Counter())
    assert sum(totals[a] for a in 'FRCW')==fleet*horizon
    diagnostic_complete=bool(motion) and all(m['steps']==horizon for m in motion.values())
    if diagnostic_complete:
        for action,key in [('F','fw'),('R','cr'),('C','ccr'),('W','wait')]:
            assert totals[action]==sum(m[key] for m in motion.values()),(case,action)
    result={'case':case.name,'raw_case':str(case),'instance':instance,'steps':horizon,'robots':fleet,
            'tasks':data['numTaskFinished'],'binary_sha256':metadata['binary_sha256'],
            'map_sha256':hashlib.sha256(map_path.read_bytes()).hexdigest(),
            'flow_parameters':{'warmup':warmup,'strength':strength,'minimum':minimum,'margin':margin,'base':base},
            'reconstructed_moves':observed,'reconstructed_penalized_edges':penalized,
            'reconstruction_matches_production_flow_log':bool(strength),'actions_match_full_production_movement_logs':diagnostic_complete,
            'neutral_graph':{'free_cells':sum(free),'components':len(sizes),'largest_component':max(sizes),
                             'largest_component_fraction':max(sizes)/sum(free),'sink_components':sum(not v for v in outgoing),
                             'cells_in_sink_components_lt_100':sum(s for s,o in zip(sizes,outgoing) if not o and s<100)},
            'windows':[{'end':end,'actions':dict(w),'snapshot':dict(s)} for end,w,s in zip(ends,windows,snapshots)],
            'limitation':'Neutral-only SCCs are an observational abstraction; penalized edges remain usable. Stall counts include all robots. Opposite-turn count permits intervening waits. No causal effect is inferred.'}
    print(case.name,'tasks',result['tasks'],'penalized',penalized,'neutral_sccs',len(sizes),flush=True)
    return result,cost


def self_test():
    free=bytearray([1,1,1]);adj=neighbors(1,3,free);cost=bytearray([1])*12
    labels,sizes,out=components(free,adj,cost,1);assert sizes==[3]
    cost[4]=2
    labels,sizes,out=components(free,adj,cost,1)
    assert sorted(sizes)==[1,2] and labels[0]==labels[1]!=labels[2] and not out[labels[0]] and out[labels[2]]
    count=array('Q',[0])*12;count[0]=6;count[6]=2
    c=fields(count,adj,8,50,1,1);assert c[6]==1,'strict 50% boundary'
    count[0]=7;count[6]=1;c=fields(count,adj,8,50,1,1);assert c[6]==2 and sum(x>1 for x in c)==1
    print('FLOW_AUDIT self-checks passed exact boundary, directed SCC, neutral connectivity',flush=True)


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--cases',nargs='+',type=Path);p.add_argument('--output',type=Path);p.add_argument('--self-test',action='store_true');a=p.parse_args()
    if a.self_test:self_test();return
    if not a.cases or not a.output:p.error('--cases and --output are required')
    assert not a.output.exists()
    results=[];costs=[]
    for case in a.cases:
        result,cost=analyze(case.resolve());results.append(result);costs.append(cost)
    comparisons=[]
    for i in range(len(results)):
        for j in range(i+1,len(results)):
            assert results[i]['map_sha256']==results[j]['map_sha256']
            sa=results[i]['flow_parameters']['base'];sb=results[j]['flow_parameters']['base']
            comparisons.append({'a':results[i]['case'],'b':results[j]['case'],
              'both_penalized':sum(a>sa and b>sb for a,b in zip(costs[i],costs[j])),
              'one_only_penalized':sum((a>sa)!=(b>sb) for a,b in zip(costs[i],costs[j]))})
    output={'source_sha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),'cases':results,'field_comparisons':comparisons}
    a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(output,indent=2)+'\n')


if __name__=='__main__':main()
