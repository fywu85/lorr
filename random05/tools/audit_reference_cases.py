#!/usr/bin/env python3
"""Audit frozen NMS/KK benchmark attempts without treating failures as scores."""
import argparse
import datetime
import hashlib
import json
import math
import shutil
import subprocess
from pathlib import Path
from action_audit import audit as replay
from audit_task_waits import audit as waiting
from result_horizon import summary_steps

ROOT=Path(__file__).resolve().parents[2]

def read(path):
    return json.loads(Path(path).read_text())

def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--batch',type=Path,required=True)
    p.add_argument('--manifest',required=True)
    p.add_argument('--protocol-commit',required=True)
    p.add_argument('--output',type=Path,required=True)
    p.add_argument('--allow-pending',action='store_true')
    args=p.parse_args()
    expected=json.loads(subprocess.check_output(['git','show',args.protocol_commit+':'+args.manifest],cwd=ROOT))
    directory=args.batch.resolve();archive=args.output.resolve().parent
    split=(directory/'batch.json').exists();rows=[];pending=[]
    for path in list(directory.glob('*.json'))+list(directory.glob('*/*.json')):
        if path.name in ('summary.json','completion.json','spec.json','allocation.json','submission.json','batch.json'):
            target=archive/'runs'/path.relative_to(directory);target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(str(path),str(target))
    for frozen in expected:
        name=frozen['name'];base=directory/name if split else directory
        if not (base/'summary.json').exists():pending.append(name);continue
        spec=read(base/'spec.json');case=next(c for c in spec['cases'] if c['name']==name)
        for key,value in frozen.items():assert case[key]==value,(name,'changed frozen case',key)
        summary=next(r for r in read(base/'summary.json') if r['name']==name)
        work=base/name;assert summary==read(work/'summary.json')
        assert sha(work/'lifelong')==summary['binary_sha256']==case['binary_sha256']
        for path,digest in case['input_hashes'].items():assert sha(path)==digest,path
        team=case['team'];assert team in ('nms','kk')
        if team=='kk':
            build=Path(case['original_binary']).parent.parent
            frozen_build=read(build/'spec.json');completion=read(build/'completion.json')
            assert frozen_build['kind']=='kk-build' and frozen_build['change'].startswith('Unmodified')
            assert completion['exit']==0 and completion['binary_sha256']==case['binary_sha256']
            assert case['runtime_hashes']==frozen_build['source_hashes']
            for path,digest in case['runtime_hashes'].items():assert sha(work/'cwd'/path)==digest,(name,'changed runtime source',path)
        resources=read(base/'allocation.json')['resources']
        assert resources['cpu_model']=='AMD EPYC 9354 32-Core Processor'
        assert case['cores']==resources['physical_cores_visible']==16 and case['smt']==2
        assert len(resources['logical_cpu_affinity'])==32 and resources['effective_cpu_quota'] is None
        assert len(read(work/'case.json')['cpus'])==32 and case['limit_ms']==1000 and case['preprocess_ms']==30000
        row=dict(name=name,team=team,input=case['input'],input_hashes=case['input_hashes'],valid=False,
                 summary_valid=summary['valid'],exit=summary['exit'],binary_sha256=case['binary_sha256'],
                 finished_utc=summary['finished_utc'],evidence=str((archive/'runs'/base.relative_to(directory)/'summary.json').relative_to(ROOT)),
                 usage=summary.get('usage'),latency_seconds=summary.get('latency_seconds'))
        if summary['valid']:
            data=read(work/'result.json');timing=data.get('entryComputeTimes',data.get('plannerTimes',[]))
            time_ok=len(timing)==case['steps'] and all(math.isfinite(t) and 0<=t<=1 for t in timing)
            memory_ok=summary['usage']['peak_rss_kib']*1024<=32000000000
            assert summary_steps(summary)==case['steps']
            assert all(data.get(k,0)==0 for k in ('numPlannerErrors','numScheduleErrors','numEntryTimeouts'))
            map_path=next(Path(path) for path in case['input_hashes'] if path.endswith('.map'))
            actions=replay(work/'result.json',map_path);actions.pop('completed_loaded_work')
            waits,_=waiting(work/'result.json')
            assert actions['tasks']==waits['tasks_finished']==summary['result']['numTaskFinished']
            assert actions['steps']==case['steps'] and actions['replay_valid'] and waits['event_accounting_valid']
            row.update(valid=time_ok and memory_ok,timing_valid=time_ok,memory_valid=memory_ok,
                       observed_tasks=actions['tasks'],replay=actions,waiting=waits)
            if row['valid']:row['tasks']=actions['tasks']
        if not row['valid']:
            log=work/'native.log'
            if log.exists():row['failure_log_tail']='\n'.join(log.read_text(errors='replace').splitlines()[-18:])
            row['note']='Original failed or invalid attempt retained; excluded from throughput comparisons.'
        resource_sample=work/'process-resources.json'
        if resource_sample.exists():
            monitor=read(resource_sample)
            row['process_peaks']={pid:dict(name=v['name'],peaks=v['peaks']) for pid,v in monitor['processes'].items()}
            target=archive/'runs'/base.relative_to(directory)/name/'process-resources.json';target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(str(resource_sample),str(target))
        rows.append(row);print(name,row.get('tasks','invalid'),'audited',flush=True)
    report=dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),protocol_commit=args.protocol_commit,
                manifest=args.manifest,complete=not pending,all_attempts_valid=not pending and all(r['valid'] for r in rows),rows=rows,pending=pending,
                interpretation='Use max(NMS,KK) only with valid measured results for both teams on the same input/allocation. Keep published scores separate.')
    args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps(report,indent=2)+'\n')
    assert args.allow_pending or not pending,pending

if __name__=='__main__':main()
