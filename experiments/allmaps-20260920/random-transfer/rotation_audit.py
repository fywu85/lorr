#!/usr/bin/env python3
"""Count actual rotation reversals, excluding nearby task/goal changes."""
import argparse
from collections import Counter
import datetime
import hashlib
import json
import os
from pathlib import Path
import shlex
import shutil
import subprocess
import sys
ROOT=next(p for p in Path(__file__).resolve().parents if (p/'tools/cpu_resources.py').is_file())

def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def write(p,x):p.write_text(json.dumps(x,indent=2)+'\n')
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--raw',type=Path,required=True);p.add_argument('--execute',action='store_true');a=p.parse_args();raw=a.raw.resolve()
 if not a.execute:
  raw.mkdir(exist_ok=False);shutil.copy2(Path(__file__),raw/'audit.py')
  names={'random04_generic1503':'cgar-dense-scheduler-generic-full-v1-20260920/generic_match64_direct_pickup4-s0-r0/RANDOM-04.json','random05_generic2036':'cgar-priority-portfolio-strict-seeds-v2-20260920/generic_noise50_cold-s0-r0/RANDOM-05.json','random05_field2574':'cgar-random05-scheduler-field-full-v1-20260920/trick_match64_direct_pickup4-s0-r0/RANDOM-05.json'}
  paths={n:str(ROOT/'runs'/r) for n,r in names.items()};write(raw/'request.json',dict(inputs=paths,sha256={n:sha(Path(p)) for n,p in paths.items()},helper_sha256=sha(raw/'audit.py')))
  cmd=['/usr/bin/python3',str(raw/'audit.py'),'--raw',str(raw),'--execute'];job=raw/'job.sh';job.write_text('#!/bin/bash\nset -eu\nexec '+' '.join(map(shlex.quote,cmd))+'\n')
  submit=['qsub','-h','-terse','-w','e','-cwd','-q','debian.q@research44.grid.gsb','-pe','threaded','1','-binding','linear:1','-l','exclusive=false,h_rt=00:10:00,h_vmem=4G','-m','n','-N','cgar_rotation_audit','-j','y','-o',str(raw/'job.log'),'-S','/bin/bash',str(job)]
  r=subprocess.run(submit,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True);write(raw/'submission.json',dict(command=submit,returncode=r.returncode,response=r.stdout));r.check_returncode();print(r.stdout,end='',flush=True);subprocess.run(['qrls',r.stdout.strip()],check=True);return
 req=json.loads((raw/'request.json').read_text());assert sha(raw/'audit.py')==req['helper_sha256']
 sys.path.insert(0,str(ROOT/'tools'));from cpu_resources import cpu_resources
 resources=cpu_resources();write(raw/'allocation.json',resources);assert resources['physical_cores_visible']==1 and resources['effective_cpu_quota'] is None
 os.sched_setaffinity(0,resources['representative_cpus']);rows=[]
 for name,path in req['inputs'].items():
  path=Path(path);assert sha(path)==req['sha256'][name];d=json.loads(path.read_text());n=d['teamSize'];H=d['makespan']
  changes=[set() for _ in range(n)]
  for tick,r,task,stage in d['events']:changes[r].add(tick)
  for r,line in enumerate(d['actualSchedule']):
   for e in line.split(','):
    if e:changes[r].add(int(e.split(':')[0]))
  counts=Counter();pairs=Counter();eligible=0;inverse=0;turn_then_forward=0;turn_then_wait=0;triple_turns=0
  for r,line in enumerate(d['actualPaths']):
   acts=line.split(',');assert len(acts)==H and set(acts)<=set('FRCW');counts.update(acts);pairs.update(a+b for a,b in zip(acts,acts[1:]))
   for t in range(1,H):
    # Conservatively discard both neighboring event/assignment time labels.
    if t in changes[r] or t+1 in changes[r]:continue
    if acts[t-1] in 'RC':
     eligible+=1;inverse+=(acts[t-1]+acts[t]) in ('RC','CR');turn_then_forward+=acts[t]=='F';turn_then_wait+=acts[t]=='W'
    if t>=2 and t-1 not in changes[r] and all(x in 'RC' for x in acts[t-2:t+1]):triple_turns+=1
  assert sum(counts.values())==n*H
  rows.append(dict(case=name,tasks=d['numTaskFinished'],robots=n,steps=H,action_counts=dict(counts),action_transitions=dict(pairs),turns_per_forward=(counts['R']+counts['C'])/counts['F'],stable_goal_turn_predecessors=eligible,stable_goal_immediate_inverse_pairs=inverse,stable_goal_inverse_fraction=inverse/eligible,stable_goal_turn_then_forward=turn_then_forward,stable_goal_turn_then_forward_fraction=turn_then_forward/eligible,stable_goal_turn_then_wait=turn_then_wait,stable_goal_three_consecutive_turn_windows=triple_turns))
  assert sha(path)==req['sha256'][name]
 write(raw/'audit.json',dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),rows=rows,scope='Observed actual actions only. Adjacent task/waypoint/assignment events are excluded conservatively for stable-goal counts. A reversal has zero net pose change over two ticks, but the audit does not prove that forcing a forward move is feasible or improves throughput. No policy/source mutation.'))
 print(json.dumps(rows,indent=2),flush=True)
if __name__=='__main__':main()
