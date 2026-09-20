#!/usr/bin/env python3
"""Report already-verified entry-completion times in consecutive full-run windows."""
import datetime,hashlib,json,os,sys
from pathlib import Path
ROOT=next(p for p in Path(__file__).resolve().parents if (p/'tools/cpu_resources.py').is_file())
def read(p): return json.loads(p.read_text())
def digest(p):
 h=hashlib.sha256()
 with p.open('rb') as f:
  for b in iter(lambda:f.read(4*1024*1024),b''):h.update(b)
 return h.hexdigest()
raw=Path(sys.argv[1]).resolve();request=read(raw/'request.json')
for name,sha in request['files'].items():assert digest(Path(name))==sha,name
sys.path.insert(0,str(ROOT/'tools'));from cpu_resources import cpu_resources
cpu=cpu_resources();assert cpu['effective_cpu_quota'] is None;os.sched_setaffinity(0,cpu['representative_cpus'][:1])
v=read(raw/'verification.json');assert v['all_valid_within_deadline_and_memory'] and not v['failures']
motion=read(raw/'motion-accounting.json');reports={}
for row in v['rows']:
 name=row['case'];p=Path(row['raw_case'])/'WAREHOUSE.json';assert digest(p)==motion['inputs'][name]['sha256'];d=read(p);times=d['entryComputeTimes']
 assert len(times)==d['entryComputeSamples']==d['makespan']==5000 and all(0<=x<=5 for x in times)
 assert max(times)==d['entryComputeMaxSeconds']==row['max_entry_seconds'] and abs(sum(times)/5-row['mean_entry_ms'])<1e-7
 windows=[]
 for start in range(0,5000,1000):
  part=times[start:start+1000];ordered=sorted(part)
  windows.append(dict(start_tick=start,end_tick_exclusive=start+1000,samples=len(part),mean_ms=sum(part),p90_ms=1000*ordered[899],max_ms=1000*max(part)))
 reports[name]=dict(windows=windows,input_sha256=motion['inputs'][name]['sha256'],source_commit=v['exact_production_source_commit'],tasks=row['tasks'])
 assert digest(p)==motion['inputs'][name]['sha256'];print('LATENCY_WINDOWS',name,flush=True)
out=Path(request['archive']);out.mkdir(parents=True,exist_ok=False)
(out/'timing.json').write_text(json.dumps(dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),all_valid=True,reports=reports,cpu_resources=cpu,selected_cpu=cpu['representative_cpus'][0],job_id=os.environ['JOB_ID'],limits='Saved entry-completion wall times from shared-host5s development. Complete fixed work; not exclusive-host/strict1s certification or controlled speedup.'),indent=2)+'\n')
import shutil
for name in ['request.json','submission.json','latency.py']:shutil.copy2(raw/name,out/name)
