"""Run one named diagnostic variant at a matched horizon, sequentially."""
from pathlib import Path
import argparse,hashlib,json,os,re,subprocess,time
ROOT=Path(__file__).resolve().parents[2]
HERE=Path(__file__).resolve().parent
VARIANTS={'fixed':{},'hybrid':{'CGAR_DIAG_HYBRID':'1'},'draft_scheduler':{'CGAR_DIAG_DRAFT_SCHED':'1'},'no_recovery':{'CGAR_TXN':'0'},'shortest_tasks':{'CGAR_HRRN':'0'},'hybrid_shortest':{'CGAR_DIAG_HYBRID':'1','CGAR_HRRN':'0'},'hybrid_upgrade':{'CGAR_DIAG_HYBRID':'1','CGAR_DIAG_UPGRADE_CHAIN':'1'},'sample':{'CGAR_DIAG_GLOBAL_SAMPLES':'64'},'hybrid_sample':{'CGAR_DIAG_HYBRID':'1','CGAR_DIAG_GLOBAL_SAMPLES':'64'},'hybrid_defer':{'CGAR_DIAG_HYBRID':'1','CGAR_DIAG_DEFER_FALLBACK':'1'},'hybrid_sample_upgrade':{'CGAR_DIAG_HYBRID':'1','CGAR_DIAG_GLOBAL_SAMPLES':'64','CGAR_DIAG_UPGRADE_CHAIN':'1'}}
p=argparse.ArgumentParser();p.add_argument('variant',choices=VARIANTS);p.add_argument('instance');p.add_argument('--steps',type=int,default=1000);p.add_argument('--probe',type=Path,default=HERE/'build/v2');a=p.parse_args()
out=ROOT/'runs/cgar-diagnosis-20260917'/('{}-{}-{}'.format(a.variant,a.instance,a.steps));out.mkdir(parents=True,exist_ok=False)
binary=a.probe.resolve()/'lifelong';input_file=next((ROOT/'mr24').glob('*/'+a.instance+'.json')).resolve()
env={k:v for k,v in os.environ.items() if not k.startswith('CGAR_')};env.update(VARIANTS[a.variant])
cmd=[str(binary),'-i',str(input_file),'-o',str(out/'result.json'),'-s',str(a.steps),'-t','1000','-p','30000']
(out/'metadata.json').write_text(json.dumps({'variant':a.variant,'environment':VARIANTS[a.variant],'command':cmd,'binary_sha256':hashlib.sha256(binary.read_bytes()).hexdigest(),'probe_build':json.loads((a.probe/'metadata.json').read_text())},indent=2)+'\n')
start=time.monotonic()
with (out/'run.log').open('w') as log:r=subprocess.run(cmd,cwd=str(ROOT/'cgar'),env=env,stdout=log,stderr=subprocess.STDOUT)
with (out/'result.json').open() as f:s=f.read(2048)
fields=['numTaskFinished','numPlannerErrors','numScheduleErrors','numEntryTimeouts','makespan']
row={key:int(re.search(r'"'+key+r'"\s*:\s*(\d+)',s).group(1)) for key in fields}
row.update(variant=a.variant,instance=a.instance,steps=a.steps,wall_seconds=round(time.monotonic()-start,3),exit_code=r.returncode)
log=(out/'run.log').read_text();diags=[]
for line in log.splitlines():
 if line.startswith('[diag] '):diags.append({k:int(v) for k,v in re.findall(r'(\w+)=(\d+)',line)})
row['diagnostics']=diags
row['valid']=(r.returncode == 0 and row['makespan'] == a.steps and all(row[key] == 0 for key in ['numPlannerErrors','numScheduleErrors','numEntryTimeouts']))
(out/'summary.json').write_text(json.dumps(row,indent=2)+'\n')
print(json.dumps(row),flush=True)
if not row['valid']: raise SystemExit(1)
