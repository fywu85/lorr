#!/usr/bin/env python3
"""Extract common motion workloads immediately before decisions 101 and 501."""
import argparse
import concurrent.futures
import hashlib
import json
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]

def extract(item):
    name,source,out=item
    config=next((ROOT/'mr24').glob('*/'+name+'.json'))
    inp=json.loads(config.read_text());mp=config.parent/inp['mapFile'];lines=mp.read_text().splitlines()
    rows=int(lines[1].split()[1]);cols=int(lines[2].split()[1]);grid=[int(c in '@T') for line in lines[4:] for c in line]
    assert len(grid)==rows*cols
    data=json.loads(source.read_text());assert not any(data[k] for k in ['numPlannerErrors','numScheduleErrors','numEntryTimeouts'])
    tasks={i:(revealed,[cells[k]*cols+cells[k+1] for k in range(0,len(cells),2)]) for i,revealed,cells in data['tasks']}
    manifest=[]
    for t in [100,500]:
        states=[]
        for i,(r,c,o) in enumerate(data['start']):
            p=r*cols+c;d='ESWN'.index(o)
            for action in data['actualPaths'][i][:2*t:2]:
                if action=='F':p+=[1,cols,-1,-cols][d]
                elif action=='R':d=(d+1)%4
                elif action=='C':d=(d+3)%4
            states.append([p,d])
        assert len({s[0] for s in states})==len(states)
        assert all(not grid[s[0]] for s in states)
        stops={}
        for at,robot,task,stop in data['events']:
            if at<=t:stops[task]=stop
        goals=[]
        for entries in data['actualSchedule']:
            task=-1
            for entry in entries.split(','):
                if not entry:continue
                at,tid=map(int,entry.split(':'))
                if at>t+1:break
                task=tid
            if task<0 or stops.get(task,0)>=len(tasks[task][1]):goals.append(-1)
            else:goals.append(tasks[task][1][stops.get(task,0)])
        value={'instance':name,'source_timestep':t,'source':str(source.relative_to(ROOT)),'source_sha256':hashlib.sha256(source.read_bytes()).hexdigest(),
               'map_name':mp.name,'rows':rows,'cols':cols,'map':grid,'states':states,'goals':goals,
               'contract':'Only current goals retained; no later stops or reassignment. Reached goals retired, agents remain movable.'}
        dest=out/(name+'-t'+str(t)+'.json');dest.write_text(json.dumps(value,separators=(',',':'))+'\n')
        manifest.append({'name':dest.stem,'snapshot':str(dest),'sha256':hashlib.sha256(dest.read_bytes()).hexdigest(),'source_sha256':value['source_sha256']})
    return manifest

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',required=True,type=Path);args=p.parse_args()
    out=args.output.resolve();out.mkdir(parents=True,exist_ok=False)
    names=list(json.loads((ROOT/'mr24/simulation_time.json').read_text()))
    cases=[(name,ROOT/'runs/cgar-assignment-swap-full-20260918/baseline-s0-r0'/(name+'.json'),out) for name in names]
    with concurrent.futures.ProcessPoolExecutor(max_workers=4) as pool:items=list(pool.map(extract,cases))
    (out/'manifest.json').write_text(json.dumps([x for xs in items for x in xs],indent=2)+'\n')
if __name__=='__main__':main()
