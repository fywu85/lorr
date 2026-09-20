#!/usr/bin/env python3
"""Test-only ancestor-revisit coverage for the exact deferred-selection probe."""
import argparse,datetime,hashlib,json,os,shlex,shutil,subprocess,sys
from pathlib import Path
ROOT=next(p for p in Path(__file__).resolve().parents if (p/'tools/cpu_resources.py').is_file())
def write(p,v):p.write_text(json.dumps(v,indent=2)+'\n')
def read(p):return json.loads(p.read_text())
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def replace(s,old,new):
 assert s.count(old)==1,old
 return s.replace(old,new,1)
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--probe',type=Path);p.add_argument('--output',type=Path,required=True);p.add_argument('--archive',type=Path,required=True);p.add_argument('--execute',action='store_true');a=p.parse_args();raw=a.output.resolve();out=a.archive.resolve()
 if not a.execute:
  assert a.probe;probe=a.probe.resolve();raw.mkdir(parents=True,exist_ok=False);spec=read(probe/'spec.json');files=[]
  for label in ['baseline','candidate']:
   folder=raw/label;folder.mkdir()
   for name in ['temporal_pibt.hpp','temporal_geometry.hpp']:
    source=probe/label/name;assert sha(source)==spec['source_hashes'][label][name]
    s=source.read_text()
    if name=='temporal_pibt.hpp':
     s=replace(s,'    TemporalStats stats;','    long long debug_ancestor_revisits = 0;\n    TemporalStats stats;')
     s=replace(s,'    std::vector<int> selected_, candidate_order_;','    std::vector<int> selected_, candidate_order_;\n    std::vector<int> debug_active_ = std::vector<int>(choices_.size(), 0);')
     s=replace(s,'        ++stats.recursive_calls;','        struct StackGuard { int& value; StackGuard(int& v):value(v){++value;} ~StackGuard(){--value;} } debug_guard(debug_active_[r]);\n        ++stats.recursive_calls;')
     s=replace(s,'            remove(other);','            if (!repairing && debug_active_[other]) ++debug_ancestor_revisits;\n            remove(other);')
    dest=folder/name;dest.write_text(s);files.append(dest)
  driver=(probe/'driver.cpp').read_text();assert sha(probe/'driver.cpp')==spec['support_hashes']['driver.cpp']
  driver=replace(driver,'long long candidates=0,recursion=0,repairs=0;','long long candidates=0,recursion=0,repairs=0,ancestor_revisits=0;')
  driver=replace(driver,'  const auto&s=run->stats;','  const auto&s=run->stats;ancestor_revisits+=run->debug_ancestor_revisits;mix(run->debug_ancestor_revisits);')
  driver=replace(driver,'<<",\\\"recursion\\\":"<<recursion','<<",\\\"ancestor_revisits\\\":"<<ancestor_revisits<<",\\\"recursion\\\":"<<recursion')
  (raw/'driver.cpp').write_text(driver);files.append(raw/'driver.cpp')
  for source,name in [(Path(__file__),'ancestor_probe.py'),(ROOT/'tools/cpu_resources.py','cpu_resources.py')]:shutil.copy2(source,raw/name);files.append(raw/name)
  write(raw/'request.json',dict(root=str(ROOT),archive=str(out),source_probe=str(probe),source_spec=spec,files={str(f):sha(f) for f in files},scope='Instrumented ancestor-revisit coverage and equivalence only; timings are not performance measurements.'))
  command=['/usr/bin/python3',str(raw/'ancestor_probe.py'),'--execute','--output',str(raw),'--archive',str(out)];job=raw/'job.sh';job.write_text('#!/bin/bash\nset -eu\nexec '+' '.join(map(shlex.quote,command))+'\n')
  submit=['qsub','-terse','-w','e','-cwd','-q',','.join('debian.q@research%d.grid.gsb'%h for h in [44,46,50,52,57]),'-pe','threaded','1','-binding','linear:1','-l','exclusive=false,h_rt=00:10:00,h_vmem=4G','-m','n','-N','cgar_ancestor_probe','-j','y','-o',str(raw/'job.log'),'-S','/bin/bash',str(job)]
  r=subprocess.run(submit,cwd=ROOT,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT);write(raw/'submission.json',dict(command=submit,response=r.stdout,returncode=r.returncode));r.check_returncode();print(r.stdout,end='');return
 req=read(raw/'request.json');assert Path(req['root'])==ROOT and Path(req['archive'])==out
 for name,h in req['files'].items():assert sha(Path(name))==h,name
 sys.path.insert(0,str(raw));from cpu_resources import cpu_resources
 resources=cpu_resources();assert resources['physical_cores_visible']==1 and resources['effective_cpu_quota'] is None;cpu=resources['representative_cpus'][0];os.sched_setaffinity(0,{cpu});write(raw/'allocation.json',dict(resources=resources,selected_cpu=cpu,job_id=os.environ['JOB_ID']))
 for label in ['baseline','candidate']:
  folder=raw/label;subprocess.run([str(ROOT/'env/bin/x86_64-conda-linux-gnu-c++'),'-O3','-DNDEBUG','-std=c++17','-march=nocona','-mtune=haswell','-I'+str(folder),str(raw/'driver.cpp'),'-o',str(folder/'probe')],check=True)
 rows=[]
 for seed in req['source_spec']['fixture_seeds']:
  for mode in req['source_spec']['modes']:
   pair=[]
   for label in ['baseline','candidate']:
    r=subprocess.run([str(raw/label/'probe'),str(mode),str(seed)],text=True,stdout=subprocess.PIPE,check=True);d=json.loads(r.stdout);d['implementation']=label;pair.append(d);rows.append(d)
   for key in ['checksum','candidates','recursion','repairs','ancestor_revisits']:assert pair[0][key]==pair[1][key],(seed,mode,key)
   assert pair[0]['ancestor_revisits']>0,(seed,mode)
 out.mkdir(parents=True,exist_ok=False)
 for name in ['request.json','submission.json','allocation.json']:shutil.copy2(raw/name,out/name)
 write(out/'coverage.json',dict(completed_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),all_exact=True,all_eight_settings_revisit_construction_ancestors=True,rows=rows,binary_sha256={label:sha(raw/label/'probe') for label in ['baseline','candidate']},scope=req['scope']))
 print('ANCESTOR_COVERAGE_VERIFIED',[(r['fixture_seed'],r['mode'],r['ancestor_revisits']) for r in rows if r['implementation']=='baseline'],flush=True)
if __name__=='__main__':main()
