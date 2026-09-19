#!/usr/bin/env python3
"""Fixed-work component timing and gprof sampling on one reserved physical core."""
import argparse, datetime, hashlib, json, os, shlex, shutil, subprocess, sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[3]
HERE=Path(__file__).resolve().parent
sys.path.insert(0,str(ROOT/'tools'))
from cpu_resources import cpu_resources

def write(path,value): path.write_text(json.dumps(value,indent=2)+'\n')
def sha(path): return hashlib.sha256(path.read_bytes()).hexdigest()
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,required=True);p.add_argument('--execute',action='store_true');a=p.parse_args();out=a.output.resolve()
 if not a.execute:
  out.mkdir(parents=True,exist_ok=False)
  commit=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip()
  for name in ['temporal_pibt.hpp','temporal_geometry.hpp']:
   (out/name).write_bytes(subprocess.check_output(['git','show',commit+':cgar/cgar_planner/'+name],cwd=ROOT))
  shutil.copy2(HERE/'driver.cpp',out/'driver.cpp')
  write(out/'spec.json',{'source_commit':commit,'sources':{x.name:sha(x) for x in out.iterdir() if x.suffix in ['.hpp','.cpp']},'purpose':'Synthetic component profile only; not warehouse quality or end-to-end speedup. Mode3 adds the production-style clock checks to regional mode1.','modes':[0,1,2,3],'repetitions':3})
  command=[sys.executable,str(Path(__file__).resolve()),'--execute','--output',str(out)]
  script=out/'job.sh';script.write_text('#!/bin/bash\nset -eu\nexec '+' '.join(shlex.quote(x) for x in command)+'\n')
  hosts=[31,38,44,46,50,52,57]
  submit=['qsub','-terse','-w','e','-cwd','-q',','.join('debian.q@research%d.grid.gsb'%h for h in hosts),'-pe','threaded','1','-binding','linear:1','-l','exclusive=true,h_rt=00:15:00,h_vmem=4G','-m','n','-N','kernel_profile','-j','y','-o',str(out/'job.log'),'-S','/bin/bash',str(script)]
  r=subprocess.run(submit,cwd=ROOT,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT);write(out/'submission.json',{'command':submit,'returncode':r.returncode,'response':r.stdout});print(r.stdout,end='');return r.returncode
 spec=json.loads((out/'spec.json').read_text())
 for name,want in spec['sources'].items(): assert sha(out/name)==want,name
 resources=cpu_resources();assert resources['cpu_model']=='AMD EPYC 9354 32-Core Processor' and resources['effective_cpu_quota'] is None
 cpu=resources['representative_cpus'][0];os.sched_setaffinity(0,{cpu});write(out/'allocation.json',{'resources':resources,'selected_cpu':cpu})
 cc=ROOT/'env/bin/x86_64-conda-linux-gnu-c++';commands={}
 for kind in ['plain','profile']:
  command=[str(cc),'-O3','-DNDEBUG','-std=c++17','-march=nocona','-mtune=haswell','-I'+str(out)]
  if kind=='profile':command+=['-pg','-gdwarf-4']
  command+=[str(out/'driver.cpp'),'-o',str(out/kind)]
  subprocess.run(command,cwd=out,check=True);commands[kind]={'command':command,'binary_sha256':sha(out/kind)}
 write(out/'binaries.json',commands);rows=[]
 for repeat in range(spec['repetitions']):
  modes=spec['modes'] if repeat%2==0 else list(reversed(spec['modes']))
  for mode in modes:
   r=subprocess.run([str(out/'plain'),str(mode)],cwd=out,text=True,stdout=subprocess.PIPE,check=True)
   row=json.loads(r.stdout);row.update(implementation='plain',repeat=repeat);rows.append(row);write(out/'observations.json',rows)
 for mode in spec['modes']:
  target=out/('profile-'+str(mode));target.mkdir()
  r=subprocess.run([str(out/'profile'),str(mode)],cwd=target,text=True,stdout=subprocess.PIPE,check=True)
  row=json.loads(r.stdout);row.update(implementation='profile',repeat=0);rows.append(row);write(out/'observations.json',rows)
  for kind,flags in [('flat',['-b','-p']),('lines',['-b','-p','-l'])]:
   r=subprocess.run(['/usr/bin/gprof']+flags+[str(out/'profile'),str(target/'gmon.out')],cwd=target,text=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
   (target/(kind+'.txt')).write_text(r.stdout);(target/(kind+'-stderr.txt')).write_text(r.stderr)
   assert r.returncode==0,(mode,kind,r.stderr)
 for mode in spec['modes']:
  group=[x for x in rows if x['mode']==mode];assert len({x['checksum'] for x in group})==1
  for key in ['candidates','recursion','repairs']:assert len({x[key] for x in group})==1
 assert {x['checksum'] for x in rows if x['mode']==1}=={x['checksum'] for x in rows if x['mode']==3},'Clock checks altered search'
 write(out/'completion.json',{'completed':True,'finished_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'observations':len(rows),'checksums_and_work_identical':True})
 return 0
if __name__=='__main__':raise SystemExit(main())
