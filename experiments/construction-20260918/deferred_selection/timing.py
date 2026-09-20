#!/usr/bin/env python3
"""Frozen same-core A/B/A warehouse timing and exact-work comparison."""
import argparse,datetime,hashlib,json,os,shlex,shutil,subprocess,sys
from pathlib import Path
ROOT=next(p for p in Path(__file__).resolve().parents if (p/'tools/cpu_resources.py').is_file())
BASE=ROOT/'experiments/construction-20260918'
def read(p):return json.loads(p.read_text())
def write(p,v):p.write_text(json.dumps(v,indent=2)+'\n')
def digest(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,required=True);p.add_argument('--archive',type=Path,required=True);p.add_argument('--original',type=Path);p.add_argument('--optimized',type=Path);p.add_argument('--execute',action='store_true');a=p.parse_args();raw=a.output.resolve();out=a.archive.resolve()
 if not a.execute:
  assert a.original and a.optimized;raw.mkdir(parents=True,exist_ok=False);support=raw/'support';support.mkdir();files=[]
  for source,name in [(Path(__file__),'timing.py'),(ROOT/'tools/cpu_resources.py','cpu_resources.py'),(ROOT/'tools/benchmark_cgar.py','benchmark_cgar.py'),(BASE/'collect_cold.py','collect_cold.py')]:
   dest=support/name;shutil.copy2(source,dest)
   if name=='benchmark_cgar.py':
    s=dest.read_text();old='ROOT = Path(__file__).resolve().parents[1]';assert s.count(old)==1;dest.write_text(s.replace(old,'ROOT = Path('+repr(str(ROOT))+')',1))
   files.append(dest)
  profile=read(BASE/'tricks/horizon_margin/full-variants.json')['trick_native_horizon5000_margin0']
  builds={}
  for label,folder in [('original',a.original.resolve()),('optimized',a.optimized.resolve())]:
   build=read(folder/'build.json');dest=raw/label;dest.mkdir();shutil.copy2(folder/'lifelong',dest/'lifelong');shutil.copy2(folder/'build.json',dest/'build.json');assert digest(dest/'lifelong')==build['binary_sha256'];builds[label]=build
   for name,sha in dict(build['sources'],**build['test_sources']).items():assert hashlib.sha256(subprocess.check_output(['git','show',build['source_commit']+':'+name],cwd=ROOT)).hexdigest()==sha,name
  expected=read(BASE/'results/horizon-margin-screen-v92/trajectory-fingerprints.json')['trick_native_horizon5000_margin0-s0-r0']
  write(raw/'request.json',dict(root=str(ROOT),archive=str(out),profile=profile,builds=builds,expected_trajectory_sha256=expected,support_hashes={str(f):digest(f) for f in files}))
  labels=['original_a','optimized','original_b'];write(raw/'spec.json',dict(horizons={'WAREHOUSE':200},time_limit_ms=5000,cpus_per_instance=4,cases=[dict(name=x,variant=x,seed=0,environment=profile) for x in labels],experiment_track='TRICK',trick='WAREHOUSE',scope='Sequential same-core A/B/A exact-work timing; no quality ranking.'))
  command=['/usr/bin/python3',str(support/'timing.py'),'--execute','--output',str(raw),'--archive',str(out)];job=raw/'job.sh';job.write_text('#!/bin/bash\nset -eu\nexec '+' '.join(map(shlex.quote,command))+'\n')
  submit=['qsub','-terse','-w','e','-cwd','-q',','.join('debian.q@research%d.grid.gsb'%h for h in [44,46,50,52,57]),'-pe','threaded','4','-binding','linear:4','-l','exclusive=false,h_rt=00:30:00,h_vmem=8G','-m','n','-N','cgar_store_timing','-j','y','-o',str(raw/'job.log'),'-S','/bin/bash',str(job)]
  r=subprocess.run(submit,cwd=ROOT,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT);write(raw/'submission.json',dict(command=submit,response=r.stdout,returncode=r.returncode));r.check_returncode();print(r.stdout,end='');return
 req=read(raw/'request.json');assert Path(req['root'])==ROOT and Path(req['archive'])==out
 for name,sha in req['support_hashes'].items():assert digest(Path(name))==sha,name
 support=raw/'support';sys.path.insert(0,str(support));from cpu_resources import cpu_resources
 resources=cpu_resources();assert resources['physical_cores_visible']==4 and resources['effective_cpu_quota'] is None and resources['cpu_model']=='AMD EPYC 9354 32-Core Processor';cpus=resources['representative_cpus'];assert len(cpus)==4;os.sched_setaffinity(0,set(cpus));write(raw/'allocation.json',dict(resources=resources,selected_cpus=cpus,job_id=os.environ['JOB_ID']))
 env={k:v for k,v in os.environ.items() if not k.startswith('CGAR_')};env.update(req['profile']);codes=[]
 for label,which in [('original_a','original'),('optimized','optimized'),('original_b','original')]:
  folder=raw/which;assert read(folder/'build.json')==req['builds'][which] and digest(folder/'lifelong')==req['builds'][which]['binary_sha256']
  command=['/usr/bin/python3',str(support/'benchmark_cgar.py'),'--output',str(raw/label),'--binary',str(folder/'lifelong'),'--source-manifest',str(folder/'build.json'),'--jobs','1','--cpus-per-instance','4','--log-detail-level','2','--cpu-list',','.join(map(str,cpus)),'--seed','0','--steps','200','--plan-time-limit-ms','5000','--instances','WAREHOUSE','--trick','WAREHOUSE']
  # Binary storage must not collide with the benchmark output directory.
  target=raw/('case_'+label);command[command.index('--output')+1]=str(target)
  codes.append(subprocess.run(command,env=env,cwd=ROOT).returncode)
  # collect_cold expects the case name directly under raw.
  if label=='optimized':
   (raw/'optimized').rename(raw/'optimized_build')
  target.rename(raw/label)
  assert codes[-1]==0,(label,codes[-1])
 write(raw/'completion.json',dict(completed=True,returncode=0,case_returncodes=codes,finished_utc=datetime.datetime.now(datetime.timezone.utc).isoformat()))
 subprocess.run(['/usr/bin/python3',str(support/'collect_cold.py'),'--input',str(raw),'--output',str(out)],check=True)
 fingerprints=read(out/'trajectory-fingerprints.json');assert set(fingerprints.values())=={req['expected_trajectory_sha256']}
 summaries=read(out/'run-summaries.json');records={};diagnostics={};last_end=None
 for label,which in [('original_a','original'),('optimized','optimized'),('original_b','original')]:
  row=summaries[label][0];meta=read(raw/label/'metadata.json');build=req['builds'][which]
  assert row['valid'] and row['memory_valid'] and row['trick_receipt_valid'] and row['experiment_track_valid'] and row['peak_process_rss_bytes']<32000000000
  assert row['cpu']==meta['cpu_binding']==cpus and meta['build_provenance']==build and meta['trick_argv']==['--trick','WAREHOUSE']
  begin,end=(datetime.datetime.fromisoformat(meta[k]) for k in ['started_utc','finished_utc']);assert last_end is None or begin>=last_end;last_end=end
  lines=read(out/label/'diagnostics.json');diagnostics[label]=[x for x in lines if x.startswith(('[cgar-temporal]','[cgar-temporal-regions]','[cgar-temporal-rotation]','[cgar-orientation]'))];assert len(diagnostics[label])==4
  cpu=row['process_resources'];records[label]=dict(wall_seconds=row['wall_seconds'],process_cpu_seconds=cpu['user_seconds']+cpu['system_seconds'],max_entry_seconds=row['entry_compute_max_seconds'],peak_rss_bytes=row['peak_process_rss_bytes'],source_commit=build['source_commit'],binary_sha256=build['binary_sha256'])
 assert diagnostics['original_a']==diagnostics['optimized']==diagnostics['original_b']
 changes={k:100*(records['optimized'][k]/((records['original_a'][k]+records['original_b'][k])/2)-1) for k in ['wall_seconds','process_cpu_seconds']}
 write(out/'equivalence.json',dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),all_trajectories_exact=True,all_sampled_search_counts_and_scores_exact=True,all600_entries_valid=True,identical_four_physical_cores=True,rows=records,diagnostics=diagnostics,candidate_percent_change_vs_mean_brackets=changes,scope='Shared-host5s A/B/A200-step exact equivalence/timing, no strict1s certification or whole-run speedup/quality claim.'))
 shutil.copy2(raw/'request.json',out/'request.json');print('DEFERRED_SELECTION_TIMING_VERIFIED',changes,flush=True)
if __name__=='__main__':main()
