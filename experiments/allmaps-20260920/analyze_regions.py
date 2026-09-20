#!/usr/bin/env python3
"""Freeze and verify the full generic cross-instance regional-work comparison."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import re
import shlex
import shutil
import subprocess
import sys

ROOT = next(p for p in Path(__file__).resolve().parents if (p/'tools/cpu_resources.py').is_file())
HERE = ROOT/'experiments/allmaps-20260920'


def read(path):
    return json.loads(path.read_text())


def write(path, data):
    path.write_text(json.dumps(data, indent=2)+'\n')


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def fields(line):
    return dict(item.split('=', 1) for item in line.split()[1:])


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--raw', type=Path, required=True)
    p.add_argument('--output', type=Path, required=True)
    p.add_argument('--commit', required=True)
    p.add_argument('--hold-job')
    p.add_argument('--execute', action='store_true')
    a=p.parse_args();raw=a.raw.resolve();out=a.output.resolve();support=raw/'cross-map-analysis-support'
    if not a.execute:
        support.mkdir(exist_ok=False)
        copies={'analyze_regions.py':Path(__file__), 'variants.json':HERE/'regional-variants.json',
                'inputs.json':HERE/'regional-inputs.json', 'cpu_resources.py':ROOT/'tools/cpu_resources.py',
                'warehouse_waiting.py':ROOT/'experiments/construction-20260918/native_work/analyze.py'}
        for name in ['sequences-20260918/analyze.py','motion-20260918/analyze.py',
                     'assignment-20260918/analyze.py','throughput-20260918-next/analyze_matrix.py',
                     'throughput-20260918-strict/analyze.py']:
            copies['experiments/'+name]=ROOT/'experiments'/name
        for name,source in copies.items():
            target=support/name;target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(source,target)
        write(raw/'cross-map-analysis-request.json',dict(commit=a.commit,files={n:sha(support/n) for n in copies}))
        command=['/usr/bin/python3',str(support/'analyze_regions.py'),'--raw',str(raw),'--output',str(out),'--commit',a.commit,'--execute']
        job=raw/'cross-map-analysis.sh';job.write_text('#!/bin/bash\nset -eu\nexec '+' '.join(map(shlex.quote,command))+'\n')
        submit=['qsub','-h','-terse','-w','e','-cwd','-q','debian.q','-pe','threaded','1','-binding','linear:1',
                '-l','exclusive=false,h_rt=00:45:00,h_vmem=12G','-m','n','-N','cgar_crossmap_analysis','-j','y','-o',str(raw/'cross-map-analysis.log'),'-S','/bin/bash']
        if a.hold_job:submit+=['-hold_jid',a.hold_job]
        submit.append(str(job));r=subprocess.run(submit,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True)
        write(raw/'cross-map-analysis-submission.json',dict(command=submit,returncode=r.returncode,response=r.stdout))
        r.check_returncode();assert re.fullmatch(r'\d+\s*',r.stdout);print(r.stdout,end='',flush=True)
        subprocess.run(['qrls',r.stdout.strip()],check=True);return
    request=read(raw/'cross-map-analysis-request.json');assert request['commit']==a.commit
    for name,expected in request['files'].items():assert sha(support/name)==expected,name
    sys.path.insert(0,str(support));from cpu_resources import cpu_resources
    from warehouse_waiting import waiting_audit
    resources=cpu_resources();assert resources['effective_cpu_quota'] is None and resources['physical_cores_visible']==1
    os.sched_setaffinity(0,resources['representative_cpus'])
    build,spec,allocation=(read(raw/name) for name in ['build.json','spec.json','allocation.json'])
    source=dict(build['sources'],**build['test_sources'])
    for name,expected in source.items():
        assert hashlib.sha256(subprocess.check_output(['git','show',a.commit+':'+name],cwd=ROOT)).hexdigest()==expected,name
    assert sha(raw/'lifelong')==build['binary_sha256']
    profiles=read(support/'variants.json');inputs=read(support/'inputs.json')
    for path,expected in inputs['sha256'].items():assert sha(Path(path))==expected,path
    assert set(spec['instances'])==set(inputs['instances']) and 'RANDOM-05' not in spec['instances']
    assert len(spec['cases'])==len(profiles)==2 and spec['horizons'] is None
    assert spec['trick'] is None and spec['experiment_track']=='GENERIC'
    assert spec['time_limit_ms']==5000 and spec['cpus_per_instance']==4 and spec['jobs_per_suite']==4 and spec['parallel_suites']==2
    assert not spec['exclusive_host'] and spec['benchmark_mode']=='relaxed_development'
    assert len(allocation['selected_cpus'])==32 and len(set(allocation['selected_cpus']))==32
    subprocess.run(['/usr/bin/python3',str(support/'experiments/sequences-20260918/analyze.py'),
                    '--input',str(raw),'--output',str(out),'--control','generic_regions_4m','--workers','1'],check=True)
    summaries=read(out/'run-summaries.json');metadata=read(out/'run-metadata.json')
    metrics={(r['case'],r['instance']):r for r in read(out/'metrics.json')}
    horizons=read(ROOT/'mr24/simulation_time.json');completion=read(raw/'completion.json')
    rows=[];fairness={};work={};suite_intervals=[]
    for case in spec['cases']:
        label=case['name'];meta=metadata[label]
        assert case['environment']==profiles[case['variant']] and case['seed']==case['repeat']==0
        assert meta['environment']==dict(case['environment'],CGAR_SEED='0') and meta['build_provenance']==build
        assert meta['binary_sha256']==build['binary_sha256'] and meta['max_process_memory_bytes']==32000000000
        assert meta['plan_time_limit_ms']==5000 and meta['preprocess_time_limit_ms']==30000
        assert meta['trick'] is None and meta['trick_argv']==[] and meta['experiment_track']=='GENERIC'
        cpu=meta['cpu_resources'];assert cpu['effective_cpu_quota'] is None
        assert cpu['cpu_model']=='AMD EPYC 9354 32-Core Processor'
        core_of={c:tuple(group) for group in cpu['logical_cpus_by_physical_core'] for c in group}
        suite_cores={(cpu['hostname'],core_of[c]) for c in meta['cpu_binding']}
        assert len(suite_cores)==len(meta['cpu_binding'])==16 and set(meta['cpu_binding']).issubset(allocation['selected_cpus'])
        begin,end=(datetime.datetime.fromisoformat(meta[k]) for k in ['started_utc','finished_utc'])
        for first,last,cores in suite_intervals:
            assert not(begin<last and first<end and suite_cores.intersection(cores))
        suite_intervals.append((begin,end,suite_cores))
        bindings=set()
        for summary in summaries[label]:
            name=summary['instance'];key=label+'/'+name;binding=summary['cpu'];bindings.add(tuple(binding))
            assert len(binding)==4 and set(binding).issubset(meta['cpu_binding']) and len({core_of[c] for c in binding})==4
            assert meta['instances'][name]==dict(input=inputs['instances'][name]['input'],steps=horizons[name])
            assert summary['experiment_track_valid'] and summary['trick_receipt_valid']
            assert summary['max_process_memory_bytes']==32000000000
            logs=(raw/label/(name+'.log')).read_text().splitlines()
            assert not any(l.startswith('[CGAR_TRICK') for l in logs)
            usage=summary['process_resources']
            row=dict(case=label,variant=case['variant'],instance=name,seed=0,steps=horizons[name],
                     robots=inputs['instances'][name]['team_size'],valid=summary['valid'],outcome=summary['outcome'],tasks=None,
                     peak_rss_bytes=summary['peak_process_rss_bytes'],wall_seconds=summary['wall_seconds'],
                     mean_cpu_cores=(usage['user_seconds']+usage['system_seconds'])/usage['wall_seconds'],reserved_physical_cores=4,
                     raw_result=str(raw/label/(name+'.json')))
            if summary['valid']:
                m=metrics[label,name]
                assert m['steps']==summary['makespan']==summary['entry_compute_samples']==horizons[name]
                assert summary['after']==m['tasks'] and all(summary[k]==0 for k in ['planner_errors','schedule_errors','timeouts','internal_timeouts','exit'])
                assert summary['memory_valid'] and summary['peak_process_rss_bytes']<32000000000 and summary['entry_timing_valid']
                assert summary['entry_compute_max_seconds']==m['max_decision_seconds']<=5
                assert m['movement_diagnostics']['complete']
                assert sum(m['movement_phases'][str(k)][action] for k in range(3) for action in ['fw','cr','ccr','wait'])==row['robots']*row['steps']
                cap=int(case['environment']['CGAR_TEMPORAL_REGION_CANDIDATE_LIMIT'])
                sampled=[fields(l) for l in logs if l.startswith('[cgar-regional-work] ')]
                assert [int(x['step']) for x in sampled]==list(range(200,row['steps']+1,200))
                assert all(int(x['candidate_limit'])==cap for x in sampled)
                assert all(0<=int(x['limited_batches'])<=8 for x in sampled)
                assert all(not int(x['limited_batches']) or int(x['max_batch_candidates'])>=cap for x in sampled)
                fairness[key]=waiting_audit(raw/label/(name+'.json'),m)
                work[key]=dict(regional_budget=sampled,regional=[fields(l) for l in logs if l.startswith('[cgar-temporal-regions] ')],timing=[fields(l) for l in logs if l.startswith('[cgar-temporal-timing] ')])
                row.update(tasks=m['tasks'],mean_entry_ms=1000*m['total_decision_seconds']/row['steps'],
                           max_entry_seconds=m['max_decision_seconds'],trajectory_sha256=m['trajectory_sha256'],
                           outstanding_age_p90=m['outstanding_task_age']['p90'],competition_budget_confirmed=False)
            else:
                assert summary['after'] is None, 'failed run must not have an accepted partial score'
                row['failure_lines']=[l for l in logs if 'CGAR_TIMEOUT' in l or 'CGAR_ERROR' in l]
            rows.append(row)
        assert len(summaries[label])==len(spec['instances'])
        for first in bindings:
            for second in bindings:
                assert first==second or not set(first).intersection(second)
        assert completion['case_returncodes'][spec['cases'].index(case)]==int(any(not x['valid'] for x in summaries[label]))
    assert completion['returncode']==int(any(not r['valid'] for r in rows))
    report=dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),source_commit=a.commit,
                source_and_test_files=len(source),binary_sha256=build['binary_sha256'],rows=rows,
                valid_runs=sum(r['valid'] for r in rows),failed_runs=sum(not r['valid'] for r in rows),
                disjoint_physical_core_groups=True,no_cpu_quota=True,decision_limit_ms=5000,
                competition_budget_confirmed=False,random05_excluded=True,
                scope='Full archived inputs, one planner seed, identical generic settings except regional candidate budget. Shared-host5s development,32decimalGB. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.')
    write(out/'verification.json',report);write(out/'fairness.json',fairness);write(out/'regional-work.json',work)
    for name in ['completion.json','submission.json','cross-map-analysis-request.json','cross-map-analysis-submission.json']:
        shutil.copy2(raw/name,out/name)
    lines=['# Generic regional work budgets across eight instances','',report['scope'],'',
           '| Instance / regional limit | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |',
           '|---|---:|---:|---:|---:|---:|']
    for r in rows:
        lines.append('| {} / {} | {} | {} | {} | {:.3f} | {} |'.format(r['instance'],r['variant'],r['tasks'] if r['valid'] else r['outcome'],round(r['mean_entry_ms'],2) if r['valid'] else 'n/a',round(r['max_entry_seconds']*1000,2) if r['valid'] else 'n/a',r['peak_rss_bytes']/1e9,r.get('outstanding_age_p90','n/a')))
    lines+=['','Throughput is primary; fairness is a secondary reported metric. RANDOM-05 was excluded.','',
            '[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).']
    (out/'summary.md').write_text('\n'.join(lines)+'\n')
    print('CROSS_MAP_REGIONS_VERIFIED',report['valid_runs'],'valid',report['failed_runs'],'failed',flush=True)


if __name__=='__main__':main()
