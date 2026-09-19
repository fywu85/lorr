#!/usr/bin/env python3
"""Verify replay inputs and evaluate chain-price information across held-out seeds."""
import argparse,concurrent.futures,datetime,hashlib,json,math,os,random,shlex,statistics,subprocess,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[3]
sys.path.insert(0,str(ROOT/'tools'))
from cpu_resources import cpu_resources

def read(p):return json.loads(p.read_text())
def write(p,x):p.write_text(json.dumps(x,indent=2)+'\n')
def sha(p):
 h=hashlib.sha256()
 with p.open('rb') as f:
  for b in iter(lambda:f.read(1<<20),b''):h.update(b)
 return h.hexdigest()
def verify_case(args):
 out,label,expected,cpu=args;os.sched_setaffinity(0,{cpu});folder=out/label;meta=read(folder/'input-metadata.json');raw=Path(meta['raw_case'])/'WAREHOUSE.json'
 assert sha(raw)==meta['raw_trajectory_file_sha256']
 data=read(raw);h=hashlib.sha256();encoder=json.JSONEncoder(separators=(',',':'))
 for key in ['actualPaths','actualSchedule','events','tasks']:
  h.update(key.encode())
  for chunk in encoder.iterencode(data[key]):h.update(chunk.encode())
 assert h.hexdigest()==expected,(label,'trajectory changed')
 assert sha(folder/'input.json')==meta['input_sha256']
 return {'case':label,'trajectory_sha256':h.hexdigest(),'input_sha256':meta['input_sha256'],'input_file_verified':True}
def dot(a,b):return sum(x*y for x,y in zip(a,b))
class QR:
 def __init__(self,x):
  self.ncols=len(x[0]);self.q=[];self.r=[];self.active=[]
  for j in range(self.ncols):
   v=[r[j] for r in x];coef=[]
   for basis in self.q:
    c=dot(basis,v);coef.append(c);v=[a-c*b for a,b in zip(v,basis)]
   norm=math.sqrt(dot(v,v))
   if norm<1e-9:continue
   self.q.append([a/norm for a in v]);self.r.append(coef+[norm]);self.active.append(j)
 def fit(self,y):
  b=[dot(q,y) for q in self.q];beta=[0.0]*len(b)
  for j in range(len(b)-1,-1,-1):beta[j]=(b[j]-sum(self.r[k][j]*beta[k] for k in range(j+1,len(b))))/self.r[j][j]
  out=[0.0]*self.ncols
  for j,v in zip(self.active,beta):out[j]=v
  return out

def evaluation(rows,permutations):
 x=[[1.0,r['unit_chain']/100.0,(r['assignment_step']-r['revealed_step'])/1000.0,r['assignment_step']/5000.0,len(r['locations'])-1] for r in rows]
 y=[float(r['loaded_steps']) for r in rows]
 z=[(r['oriented_chain']/r['unit_scale']-r['unit_chain'])/100.0 for r in rows]
 folds=[]
 for seed in range(6):
  train=[i for i,r in enumerate(rows) if r['seed']!=seed];test=[i for i,r in enumerate(rows) if r['seed']==seed]
  q=QR([x[i] for i in train]);coef=q.fit([y[i] for i in train]);base={i:dot(x[i],coef) for i in train+test}
  residual=[y[i]-base[i] for i in train]
  folds.append((seed,train,test,q,base,residual))
 def score(feature,detail=False):
  all_old=[];all_new=[];result=[]
  for seed,train,test,q,base,residual in folds:
   coef_z=q.fit([feature[i] for i in train]);zt=[feature[i]-dot(x[i],coef_z) for i in train]
   variance=dot(zt,zt);beta=dot(zt,residual)/variance if variance>1e-12 else 0.0
   old=[y[i]-base[i] for i in test];new=[y[i]-(base[i]+beta*(feature[i]-dot(x[i],coef_z))) for i in test]
   all_old+=old;all_new+=new
   mse0=statistics.mean(v*v for v in old);mse1=statistics.mean(v*v for v in new)
   result.append({'test_seed':seed,'train_n':len(train),'test_n':len(test),'base_mse':mse0,'augmented_mse':mse1,'mse_reduction_percent':100*(1-mse1/mse0),'base_mae':statistics.mean(abs(v) for v in old),'augmented_mae':statistics.mean(abs(v) for v in new),'oriented_excess_coefficient_steps_per_step':beta/100.0})
  m0=statistics.mean(v*v for v in all_old);m1=statistics.mean(v*v for v in all_new)
  return {'n':len(y),'base_mse':m0,'augmented_mse':m1,'mse_reduction_percent':100*(1-m1/m0),'base_mae':statistics.mean(abs(v) for v in all_old),'augmented_mae':statistics.mean(abs(v) for v in all_new),'folds':result}
 result=score(z,True);groups={}
 for i,r in enumerate(rows):groups.setdefault((r['seed'],r['publication']),[]).append(i)
 rng=random.Random(391781);null=[]
 for repeat in range(permutations):
  shuffled=z[:]
  for indices in groups.values():
   values=[z[i] for i in indices];rng.shuffle(values)
   for i,v in zip(indices,values):shuffled[i]=v
  null.append(score(shuffled)['mse_reduction_percent'])
 result['permutation_mse_reductions_percent']=null
 result['permutation_p_one_sided']=(1+sum(v>=result['mse_reduction_percent'] for v in null))/(1+len(null)) if null else None
 result['permutation_scope']='Oriented excess shuffled within seed/publication among completed sampled tasks; baseline features and labels fixed.'
 return result

def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--input',type=Path,required=True);p.add_argument('--execute',action='store_true');a=p.parse_args();out=a.input.resolve()
 if not a.execute:
  cmd=[sys.executable,str(Path(__file__).resolve()),'--input',str(out),'--execute'];script=out/'analysis.sh';script.write_text('#!/bin/bash\nset -eu\nexec '+' '.join(shlex.quote(x) for x in cmd)+'\n')
  submit=['qsub','-terse','-w','e','-cwd','-q','debian.q','-pe','threaded','4','-binding','linear:4','-l','exclusive=true,h_rt=00:15:00,h_vmem=4G','-m','n','-N','chain_analysis','-j','y','-o',str(out/'analysis.log'),'-S','/bin/bash',str(script)]
  r=subprocess.run(submit,cwd=ROOT,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT);write(out/'analysis-submission.json',{'command':submit,'response':r.stdout,'returncode':r.returncode});print(r.stdout,end='');return r.returncode
 assert read(out/'completion.json')['completed'];spec=read(out/'spec.json');res=cpu_resources();cpus=res['representative_cpus'][:4];assert len(cpus)==4 and res['effective_cpu_quota'] is None
 write(out/'analysis-allocation.json',{'resources':res,'selected_cpus':cpus})
 for name,want in spec['source_hashes'].items():assert sha(out/name)==want,name
 cases=[('prefix',spec['prefix_case'])]+[('seed'+str(c['seed']),c) for c in spec['cases']]
 args=[(out,label,c['trajectory_sha256'],cpus[i%4]) for i,(label,c) in enumerate(cases)]
 with concurrent.futures.ProcessPoolExecutor(max_workers=4) as pool:verified=list(pool.map(verify_case,args))
 os.sched_setaffinity(0,{cpus[0]});records=[];cohorts=[]
 completion=read(out/'completion.json')
 for label,c in cases:
  prices=read(out/label/'prices.json');assert sha(out/label/'prices.json')==next(r['prices_sha256'] for r in completion['cases'] if r['case']==label)
  assert prices['all_publication_counters_exact'];rows=prices['queries']
  if label=='prefix':continue
  assert len(rows)==640 and len(prices['publications'])==10
  for r in rows:assert r['oriented_chain']>=r['unit_scale']*r['unit_chain'] and r['seed']==c['seed']
  cohorts.append({'seed':c['seed'],'sampled':len(rows),'completed':sum(r['completion_step'] is not None for r in rows),'picked_unfinished':sum(r['completion_step'] is None and r['pickup_step'] is not None for r in rows),'unpicked':sum(r['pickup_step'] is None for r in rows)})
  records+=rows
 complete=[r for r in records if r['completion_step'] is not None]
 result={'scope':'Offline predictive information only; no causal throughput, production cache availability or counterfactual assignment claim. Sampled before outcomes; primary model is conditional on completion, with censoring reported.','checked_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'verified_inputs':verified,'cohorts':cohorts,'features':['intercept','unit_chain','age_at_assignment','assignment_time','remaining_leg_count'],'extra_feature':'oriented_chain / unit_scale - unit_chain, using the field available at assignment','primary':evaluation(complete,200),'early_assignment_sensitivity':evaluation([r for r in complete if r['assignment_step']<=4000],0),'sampled_task_records':records,'all60_full_publications_match_logged_counters':True,'throughput_goal_complete':False}
 write(out/'prediction.json',result);write(out/'analysis-completion.json',{'completed':True,'finished_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'sampled':len(records),'completed_labels':len(complete)})
 print(json.dumps({'cohorts':cohorts,'primary':{k:v for k,v in result['primary'].items() if k!='permutation_mse_reductions_percent'},'early_mse_reduction':result['early_assignment_sensitivity']['mse_reduction_percent']},indent=2))
 return 0
if __name__=='__main__':raise SystemExit(main())
