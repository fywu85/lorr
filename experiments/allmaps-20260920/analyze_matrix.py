#!/usr/bin/env python3
"""Freeze and verify controlled CGAR factor and explicit-trick matrices."""
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
    p.add_argument('--variants', type=Path, required=True)
    p.add_argument('--inputs', type=Path, required=True)
    p.add_argument('--control', required=True)
    p.add_argument('--seeds',type=int,nargs='+',default=[0])
    p.add_argument('--trick', choices=['WAREHOUSE','SORTATION','CITY-01','CITY-02','GAME','RANDOM-01','RANDOM-02','RANDOM-03','RANDOM-04','RANDOM-05'])
    p.add_argument('--allow-random05', action='store_true', help='Explicitly include CGAR runs on RANDOM-05 without modifying the separate solver')
    p.add_argument('--hold-job')
    p.add_argument('--cpus-per-instance',type=int,choices=[4,8],default=4,help='Required physical-core count for this declared comparison')
    p.add_argument('--analysis-tag', default='factor-analysis', help='Separate immutable verification attempt label')
    p.add_argument('--execute', action='store_true')
    a=p.parse_args()
    if not re.fullmatch(r'[a-zA-Z0-9_-]+',a.analysis_tag):p.error('analysis tag must be a simple label')
    tag=a.analysis_tag;raw=a.raw.resolve();out=a.output.resolve();support=raw/(tag+'-support')
    if not a.execute:
        # Reject declaration mistakes before creating an immutable attempt or
        # reserving an analysis slot. Full artifact/hash checks still run on GRID.
        declared=read(raw/'spec.json');profiles=read(a.variants);inputs=read(a.inputs)
        if set(declared['instances'])!=set(inputs['instances']):
            p.error('input manifest instances must exactly match the matrix instances')
        if 'RANDOM-05' in declared['instances'] and not a.allow_random05:
            p.error('CGAR RANDOM-05 analysis requires --allow-random05')
        expected={(name,seed,0) for name in profiles for seed in a.seeds}
        actual={(c['variant'],c['seed'],c['repeat']) for c in declared['cases']}
        if len(set(a.seeds))!=len(a.seeds) or a.control not in profiles or actual!=expected or len(declared['cases'])!=len(expected):
            p.error('variants, control and seeds must match the frozen matrix declaration')
        if declared['trick']!=a.trick or declared['cpus_per_instance']!=a.cpus_per_instance:
            p.error('trick and physical-core count must match the frozen matrix declaration')
        if any(c['environment']!=profiles[c['variant']] for c in declared['cases']):
            p.error('variant environments differ from the frozen matrix declaration')
        support.mkdir(exist_ok=False)
        copies={'analyze_matrix.py':Path(__file__), 'variants.json':a.variants.resolve(),
                'inputs.json':a.inputs.resolve(), 'cpu_resources.py':ROOT/'tools/cpu_resources.py',
                'warehouse_waiting.py':ROOT/'experiments/construction-20260918/native_work/analyze.py'}
        for name in ['sequences-20260918/analyze.py','motion-20260918/analyze.py',
                     'assignment-20260918/analyze.py','throughput-20260918-next/analyze_matrix.py',
                     'throughput-20260918-strict/analyze.py']:
            copies['experiments/'+name]=ROOT/'experiments'/name
        for name,source in copies.items():
            target=support/name;target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(source,target)
        write(raw/(tag+'-request.json'),dict(commit=a.commit,control=a.control,trick=a.trick,seeds=a.seeds,allow_random05=a.allow_random05,cpus_per_instance=a.cpus_per_instance,files={n:sha(support/n) for n in copies}))
        command=['/usr/bin/python3',str(support/'analyze_matrix.py'),'--raw',str(raw),'--output',str(out),'--commit',a.commit,'--variants',str(support/'variants.json'),'--inputs',str(support/'inputs.json'),'--control',a.control,'--analysis-tag',tag,'--cpus-per-instance',str(a.cpus_per_instance),'--execute']
        if a.trick:command+=['--trick',a.trick]
        if a.allow_random05:command+=['--allow-random05']
        command+=['--seeds']+list(map(str,a.seeds))
        job=raw/(tag+'.sh');job.write_text('#!/bin/bash\nset -eu\nexec '+' '.join(map(shlex.quote,command))+'\n')
        submit=['qsub','-h','-terse','-w','e','-cwd','-q','debian.q@research43.grid.gsb,debian.q@research44.grid.gsb,debian.q@research57.grid.gsb','-pe','threaded','2','-binding','linear:1',
                '-l','exclusive=false,h_rt=00:45:00,h_vmem=6G','-m','n','-N','cgar_crossmap_analysis','-j','y','-o',str(raw/(tag+'.log')),'-S','/bin/bash']
        if a.hold_job:submit+=['-hold_jid',a.hold_job]
        submit.append(str(job));r=subprocess.run(submit,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True)
        write(raw/(tag+'-submission.json'),dict(command=submit,returncode=r.returncode,response=r.stdout))
        r.check_returncode();assert re.fullmatch(r'\d+\s*',r.stdout);print(r.stdout,end='',flush=True)
        subprocess.run(['qrls',r.stdout.strip()],check=True);return
    request=read(raw/(tag+'-request.json'));assert request['commit']==a.commit and request['control']==a.control and request['trick']==a.trick and request.get('allow_random05',False)==a.allow_random05 and request.get('seeds',[0])==a.seeds and request.get('cpus_per_instance',4)==a.cpus_per_instance
    for name,expected in request['files'].items():assert sha(support/name)==expected,name
    sys.path.insert(0,str(support));from cpu_resources import cpu_resources
    from warehouse_waiting import waiting_audit
    resources=cpu_resources();write(raw/(tag+'-allocation.json'),resources)
    assert resources['effective_cpu_quota'] is None and resources['physical_cores_visible']==1,resources
    os.sched_setaffinity(0,resources['representative_cpus'])
    build,spec,allocation=(read(raw/name) for name in ['build.json','spec.json','allocation.json'])
    source=dict(build['sources'],**build['test_sources'])
    for name,expected in source.items():
        assert hashlib.sha256(subprocess.check_output(['git','show',a.commit+':'+name],cwd=ROOT)).hexdigest()==expected,name
    assert sha(raw/'lifelong')==build['binary_sha256']
    profiles=read(support/'variants.json');inputs=read(support/'inputs.json')
    for path,expected in inputs['sha256'].items():assert sha(Path(path))==expected,path
    assert set(spec['instances'])==set(inputs['instances']) and (a.allow_random05 or 'RANDOM-05' not in spec['instances'])
    assert len(spec['cases'])==len(profiles)*len(a.seeds) and a.control in profiles
    assert len(set(a.seeds))==len(a.seeds)
    assert {(c['variant'],c['seed'],c['repeat']) for c in spec['cases']}=={(v,seed,0) for v in profiles for seed in a.seeds}
    track='TRICK' if a.trick else 'GENERIC'
    assert spec['trick']==a.trick and spec['experiment_track']==track
    assert spec['time_limit_ms'] in (1000,5000) and spec['cpus_per_instance']==a.cpus_per_instance
    assert not spec['exclusive_host'] and spec['benchmark_mode']=='relaxed_development'
    core_count=spec['parallel_suites']*spec['jobs_per_suite']*spec['cpus_per_instance']
    assert len(allocation['selected_cpus'])==core_count and len(set(allocation['selected_cpus']))==core_count
    subprocess.run(['/usr/bin/python3',str(support/'experiments/sequences-20260918/analyze.py'),
                    '--input',str(raw),'--output',str(out),'--control',a.control,'--workers','1'],check=True)
    summaries=read(out/'run-summaries.json');metadata=read(out/'run-metadata.json')
    metrics={(r['case'],r['instance']):r for r in read(out/'metrics.json')}
    horizons=spec['horizons'] or read(ROOT/'mr24/simulation_time.json');completion=read(raw/'completion.json')
    rows=[];fairness={};work={};suite_intervals=[]
    for case in spec['cases']:
        label=case['name'];meta=metadata[label]
        assert case['environment']==profiles[case['variant']] and case['seed'] in a.seeds and case['repeat']==0
        assert meta['environment']==dict(case['environment'],CGAR_SEED=str(case['seed'])) and meta['build_provenance']==build
        assert meta['binary_sha256']==build['binary_sha256'] and meta['max_process_memory_bytes']==32000000000
        assert meta['plan_time_limit_ms']==spec['time_limit_ms'] and meta['preprocess_time_limit_ms']==30000
        assert meta['trick']==a.trick and meta['trick_argv']==(['--trick',a.trick] if a.trick else []) and meta['experiment_track']==track
        cpu=meta['cpu_resources'];assert cpu['effective_cpu_quota'] is None
        assert cpu['cpu_model']=='AMD EPYC 9354 32-Core Processor'
        core_of={c:tuple(group) for group in cpu['logical_cpus_by_physical_core'] for c in group}
        suite_cores={(cpu['hostname'],core_of[c]) for c in meta['cpu_binding']}
        assert len(suite_cores)==len(meta['cpu_binding'])==spec['jobs_per_suite']*spec['cpus_per_instance'] and set(meta['cpu_binding']).issubset(allocation['selected_cpus'])
        begin,end=(datetime.datetime.fromisoformat(meta[k]) for k in ['started_utc','finished_utc'])
        for first,last,cores in suite_intervals:
            assert not(begin<last and first<end and suite_cores.intersection(cores))
        suite_intervals.append((begin,end,suite_cores))
        bindings=set()
        for summary in summaries[label]:
            name=summary['instance'];key=label+'/'+name;binding=summary['cpu'];bindings.add(tuple(binding))
            assert len(binding)==a.cpus_per_instance and set(binding).issubset(meta['cpu_binding']) and len({core_of[c] for c in binding})==a.cpus_per_instance
            assert meta['instances'][name]==dict(input=inputs['instances'][name]['input'],steps=horizons[name])
            assert summary['experiment_track_valid']
            if summary['valid']:assert summary['trick_receipt_valid']
            assert summary['max_process_memory_bytes']==32000000000
            logs=(raw/label/(name+'.log')).read_text().splitlines()
            if summary['valid']:
                if a.trick:assert any(l.startswith('[CGAR_TRICK] instance='+a.trick+' ') for l in logs)
                else:assert not any(l.startswith('[CGAR_TRICK') for l in logs)
            usage=summary['process_resources']
            row=dict(case=label,variant=case['variant'],instance=name,seed=case['seed'],steps=horizons[name],
                     robots=inputs['instances'][name]['team_size'],valid=summary['valid'],outcome=summary['outcome'],tasks=None,
                     peak_rss_bytes=summary['peak_process_rss_bytes'],wall_seconds=summary['wall_seconds'],
                     mean_cpu_cores=(usage['user_seconds']+usage['system_seconds'])/usage['wall_seconds'],reserved_physical_cores=a.cpus_per_instance,
                     raw_result=str(raw/label/(name+'.json')))
            if summary['valid']:
                m=metrics[label,name]
                assert m['steps']==summary['makespan']==summary['entry_compute_samples']==horizons[name]
                assert summary['after']==m['tasks'] and all(summary[k]==0 for k in ['planner_errors','schedule_errors','timeouts','internal_timeouts','exit'])
                assert summary['memory_valid'] and summary['peak_process_rss_bytes']<32000000000 and summary['entry_timing_valid']
                assert summary['entry_compute_max_seconds']==m['max_decision_seconds']<=spec['time_limit_ms']/1000
                assert m['movement_diagnostics']['complete']
                assert sum(m['movement_phases'][str(k)][action] for k in range(3) for action in ['fw','cr','ccr','wait'])==row['robots']*row['steps']
                cap=int(case['environment']['CGAR_TEMPORAL_REGION_CANDIDATE_LIMIT'])
                sampled=[fields(l) for l in logs if l.startswith('[cgar-regional-work] ')]
                assert [int(x['step']) for x in sampled]==list(range(200,row['steps']+1,200))
                assert all(int(x['candidate_limit'])==cap for x in sampled)
                regional_batches=int(case['environment'].get('CGAR_TEMPORAL_REGIONS','4'))*int(case['environment'].get('CGAR_TEMPORAL_REGION_ROUNDS','2'))
                assert regional_batches>=1
                assert all(0<=int(x['limited_batches'])<=regional_batches for x in sampled)
                assert all(not int(x['limited_batches']) or int(x['max_batch_candidates'])>=cap for x in sampled)
                priority_noise=int(case['environment'].get('CGAR_TEMPORAL_PRIORITY_NOISE','0'))
                priority_samples=[fields(l) for l in logs if l.startswith('[cgar-priority-portfolio] ')]
                if priority_noise:
                    assert [int(x['step']) for x in priority_samples]==list(range(200,row['steps']+1,200))
                    workers=int(case['environment']['CGAR_TEMPORAL_WORKERS'])
                    assert all(int(x['workers'])==workers and int(x['noise'])==priority_noise for x in priority_samples)
                    assert all(0<=int(x['selected_worker'])<workers and 0<=int(x['changed_orders'])<=workers for x in priority_samples)
                    assert all(int(x['reused'])==int(case['environment'].get('CGAR_TEMPORAL_PRIORITY_PERSIST','0')) for x in priority_samples)
                else:
                    assert not priority_samples
                squared=int(case['environment'].get('CGAR_TRICK_RANK_SQUARED','0'))
                rank_samples=[fields(l) for l in logs if l.startswith('[cgar-trick-rank] ')]
                rank_config=[fields(l) for l in logs if l.startswith('[CGAR_TRICK_RANK] ')]
                if squared:
                    assert a.trick in ('GAME','RANDOM-04','RANDOM-05') and case['environment']['CGAR_TEMPORAL_EQUAL_WEIGHT']=='0'
                    assert rank_config==[dict(squared='1',base='linear',rank='base_order',protected='unchanged',candidates='unchanged')]
                    assert [int(x['step']) for x in rank_samples]==list(range(200,row['steps']+1,200))
                    totals=[int(x['changed_total']) for x in rank_samples]
                    assert totals==sorted(totals) and totals[-1]>0
                    assert all(int(x['squared'])==1 and 0<=int(x['changed_total'])<=row['robots']*int(x['step']) for x in rank_samples)
                else:
                    assert not rank_samples and not rank_config
                retarget_budget=int(case['environment'].get('CGAR_REASSIGN_MATCH_TASK_BUDGET','1'))
                assert m['declared_task_retarget_budget']==retarget_budget and m['max_changes_per_task']<=retarget_budget
                retarget_config=[fields(l) for l in logs if l.startswith('[cgar-match-retarget-config] ')]
                retarget_samples=[fields(l) for l in logs if l.startswith('[cgar-match-retarget] ')]
                if retarget_budget!=1:
                    assert retarget_config==[dict(task_budget=str(retarget_budget),finite='1',cooldown='20',started='protected',primary='protected',recovery='protected',fair='protected',fixed_work='1')]
                    assert retarget_samples and all(int(s['task_budget'])==retarget_budget and 0<=int(s['max_task_moves'])<=retarget_budget for s in retarget_samples)
                    counts=[int(s['repeated_moves']) for s in retarget_samples];assert counts==sorted(counts)
                else:
                    assert not retarget_config and not retarget_samples
                if int(case['environment'].get('CGAR_TRICK_HORIZON_MANHATTAN','0')):
                    known=int(case['environment']['CGAR_TRICK_KNOWN_HORIZON'])
                    assert known==row['steps'], 'known horizon must be the predeclared full run length'
                    receipts=[fields(l) for l in logs if l.startswith('[CGAR_TRICK_HORIZON] ')]
                    assert receipts==[dict(known_horizon=str(known),assumption='configured',lower_bound='manhattan_plus_service',core='unrestricted_bound',assignments='new_only',fair='unchanged',held='unchanged',all_impossible='assign',after_horizon='ordinary')]
                    horizon_samples=[fields(l) for l in logs if l.startswith('[cgar-horizon] ')]
                    assert horizon_samples and int(horizon_samples[-1]['pairs'])>0
                    assert all(int(x['known_horizon'])==known and 0<=int(x['impossible_pairs'])<=int(x['pairs']) for x in horizon_samples)
                    if int(case['environment'].get('CGAR_TRICK_HORIZON_MARGIN','0')):
                        margin_samples=[fields(l) for l in logs if l.startswith('[cgar-horizon-margin] ')]
                        assert margin_samples and all(int(x['bound_violations'])==0 for x in margin_samples)
                chain_mode=int(case['environment'].get('CGAR_TEMPORAL_CHAIN_MODE','0'))
                chain_config=[fields(l) for l in logs if l.startswith('[cgar-chain-config] ')]
                chain_samples=[fields(l) for l in logs if l.startswith('[cgar-chain] ')]
                if chain_mode:
                    assert len(chain_config)==1
                    chain=chain_config[0];cells=int(chain['cells'])
                    assert chain==dict(mode=str(chain_mode),score=str(int(bool(chain_mode&1))),order=str(int(bool(chain_mode&2))),complete='1',cells=str(cells),stored_bytes=str(64*cells*cells),threads=case['environment'].get('CGAR_TEMPORAL_CHAIN_THREADS','1'),service='after_action',domain='core_goal_pocket_escape',fixed_work='1')
                    assert 0<int(chain['stored_bytes'])<=int(case['environment'].get('CGAR_TEMPORAL_CHAIN_MB','512'))*1024*1024
                    assert [int(x['step']) for x in chain_samples]==list(range(200,row['steps']+1,200))
                    for x in chain_samples:
                        assert int(x['mode'])==chain_mode
                        assert 0<=int(x['robots'])+int(x['fallback'])<=row['robots']*int(x['step'])
                        assert 0<=int(x['multi_service'])<=int(x['service'])<=int(x['choices'])
                        assert 0<=int(x['completed'])<=int(x['service'])
                    for counter in ('robots','fallback','choices','service','multi_service','completed'):
                        counts=[int(x[counter]) for x in chain_samples];assert counts==sorted(counts)
                    assert int(chain_samples[-1]['robots'])>0
                    assert (int(chain_samples[-1]['choices'])>0)==bool(chain_mode&1)
                else:
                    assert not chain_config and not chain_samples
                window_horizon=int(case['environment'].get('CGAR_WINDOW','0'))
                window_config=[fields(l) for l in logs if l.startswith('[cgar-window-config] ')]
                window_samples=[fields(l) for l in logs if l.startswith('[cgar-window] ')]
                if window_horizon:
                    assert not chain_mode and len(window_config)==1
                    cfg=window_config[0];cells=int(cfg['cells'])
                    for field,key,default in [('horizon','CGAR_WINDOW',0),('keep','CGAR_WINDOW_KEEP',6),
                            ('iterations','CGAR_WINDOW_ITERS',128),('nodes','CGAR_WINDOW_NODES',2048),
                            ('group','CGAR_WINDOW_GROUP',4),('workers','CGAR_WINDOW_WORKERS',4),
                            ('threads','CGAR_WINDOW_THREADS',4),('table_threads','CGAR_TEMPORAL_CHAIN_THREADS',1)]:
                        assert int(cfg[field])==int(case['environment'].get(key,str(default)))
                    rollout=int(case['environment'].get('CGAR_WINDOW_SEED_ROLLOUT','0'))
                    assert int(cfg.get('seed_rollout','0'))==rollout
                    progress_ties=int(case['environment'].get('CGAR_WINDOW_PROGRESS_TIES','0'));assert int(cfg.get('progress_ties','0'))==progress_ties
                    assert int(cfg.get('protected_prefix','0'))==int(case['environment'].get('CGAR_WINDOW_PROTECTED_PREFIX','0'))
                    assert cfg['seed']=='cgar' and cfg['protected'] in ('immutable','immutable_first_action') and cfg['objective']=='paid_plus_chain'
                    assert cfg['service']=='after_action' and cfg['fixed_work']==cfg['timeout_is_failure']=='1'
                    assert int(cfg['stored_bytes'])==64*cells*cells<=int(case['environment'].get('CGAR_TEMPORAL_CHAIN_MB','512'))*1024*1024
                    assert 1<=int(cfg['turn_cost'])<=255 and 1<=int(cfg['wait_cost'])<=255
                    if int(case['environment'].get('CGAR_WINDOW_WAIT_COST','0')):
                        assert cfg['wait_cost']==case['environment']['CGAR_WINDOW_WAIT_COST']
                    assert [int(x['step']) for x in window_samples]==list(range(200,row['steps']+1,200))
                    attempts=int(cfg['iterations'])*int(cfg['workers'])
                    for x in window_samples:
                        assert x['complete']=='1' and int(x['attempts'])==attempts
                        assert int(x.get('rollout_batches','0'))==((window_horizon-1)//5 if rollout else 0)
                        assert int(x['calls'])==int(x['step']) and int(x['total_attempts'])==attempts*int(x['step'])
                        assert 0<=int(x['improved'])<=int(x['accepted'])<=attempts
                        assert 0<=int(x['expanded'])<=int(x['searches'])*int(cfg['nodes'])
                        assert int(x['capped'])+int(x['failed'])<=int(x['searches'])
                        assert 0<=int(x['final_cost'])<=int(x['initial_cost'])<=int(x['seed_cost'])
                        if progress_ties and x['final_cost']==x['initial_cost']:
                            assert int(x['final_remaining'])<=int(x['initial_remaining'])
                        if progress_ties and x['seed_cost']==x['initial_cost']:
                            assert int(x['initial_remaining'])<=int(x['seed_remaining'])
                        assert 0<=int(x['selected_worker'])<int(cfg['workers'])
                        assert 0<=int(x['changed_first'])<=row['robots']-int(x['protected'])
                        assert 0<=int(x['retained'])<=row['robots'] and 0<=int(x['history_resets'])<=row['robots']
                    for counter in ('total_attempts','total_changed_first','total_retained','total_history_resets'):
                        counts=[int(x[counter]) for x in window_samples];assert counts==sorted(counts)
                    row['rolling_window']=dict(configuration=cfg,last_sample=window_samples[-1])
                else:
                    assert not window_config and not window_samples
                peak_samples=[fields(l) for l in logs if l.startswith('[cgar-regional-peaks] ')]
                if int(case['environment'].get('CGAR_TEMPORAL_REGION_PEAK_AUDIT','0')):
                    assert [int(x['step']) for x in peak_samples]==list(range(200,row['steps']+1,200))
                    assert all(0<=int(x['lost_improvements'])<=int(x['lost_peaks'])<=int(x['batches']) for x in peak_samples)
                    assert all(float(x['discarded_gain'])>=0 for x in peak_samples)
                else:assert not peak_samples
                keep_peak=int(case['environment'].get('CGAR_TEMPORAL_REGION_KEEP_PEAK','0'))
                retention_config=[fields(l) for l in logs if l.startswith('[cgar-regional-keep-peak-config] ')]
                retention_samples=[fields(l) for l in logs if l.startswith('[cgar-regional-keep-peak] ')]
                if keep_peak:
                    assert retention_config==[dict(enabled='1',fixed_work='1',timeout_is_failure='1')]
                    assert [int(x['step']) for x in retention_samples]==list(range(200,row['steps']+1,200))
                    assert all(0<=int(x['restored_batches'])<=regional_batches for x in retention_samples)
                    counts=[int(x['total_restored_batches']) for x in retention_samples]
                    assert counts==sorted(counts)
                    assert all(0<=int(x['total_restored_batches'])<=regional_batches*int(x['step']) for x in retention_samples)
                else:
                    assert not retention_config and not retention_samples
                fleet_audit=None
                fleet_limit=int(case['environment'].get('CGAR_TRICK_GAME_ACTIVE_LIMIT','0'))
                fleet=[fields(l) for l in logs if l.startswith('[CGAR_TRICK_GAME_FLEET] ')]
                if fleet_limit:
                    assert a.trick=='GAME' and len(fleet)==1
                    f=fleet[0];tabu=int(case['environment'].get('CGAR_TRICK_GAME_TABU','0'))
                    assert int(f['nominal_active'])==fleet_limit and int(f['tabu'])==tabu
                    assert int(f['disabled'])==min(row['robots']-fleet_limit,int(f['eligible_pool']))
                    assert int(f['active'])==row['robots']-int(f['disabled'])
                    assert f['selection']=='once' and f['rng']=='independent_splitmix' and f['idle_motion']=='cgar' and f['held_tasks']=='protected'
                    assert f['asset_sha256']==('15e86aa9a3f5d45a822bd699db8a7adc2a34ca76e1bcca7e8e18640d28103681' if tabu else 'none')
                    assert 0<=int(f['tabu_kept'])<=int(f['active']) and 0<=int(f['held_kept'])<=int(f['active'])
                    assert int(f['mask_fnv1a64'])>0
                    # Reconstruct the mask independently from the archived starts
                    # and committed asset, then audit every actual assignment.
                    inp=Path(inputs['instances']['GAME']['input']);inp_data=read(inp)
                    agent_path=inp.parent/inp_data['agentFile']
                    numbers=[int(l) for l in agent_path.read_text().splitlines() if l.strip() and not l.lstrip().startswith('#')]
                    assert numbers[0]==row['robots'] and len(numbers)==row['robots']+1
                    starts=numbers[1:];exempt=set()
                    if tabu:
                        asset=subprocess.check_output(['git','show',a.commit+':cgar/tricks/game_fleet_tabu.hpp'],cwd=ROOT,text=True)
                        body=asset.split('game_tabu_cells[] = {',1)[1].split('};',1)[0]
                        exempt=set(map(int,re.findall(r'\d+',body)));assert len(exempt)==10882
                    mask64=(1<<64)-1
                    def priority(robot):
                        x=(int(case['seed'])&mask64)^((robot+0x9e3779b97f4a7c15)&mask64)
                        x=((x^(x>>30))*0xbf58476d1ce4e5b9)&mask64
                        x=((x^(x>>27))*0x94d049bb133111eb)&mask64
                        return x^(x>>31)
                    pool=[r for r,c in enumerate(starts) if c not in exempt]
                    pool.sort(key=lambda r:(priority(r),r))
                    excluded=set(pool[:min(row['robots']-fleet_limit,len(pool))])
                    fingerprint=14695981039346656037
                    for robot in range(row['robots']):
                        fingerprint=((fingerprint^int(robot in excluded))*1099511628211)&mask64
                    assert int(f['held_kept'])==0 and int(f['eligible_pool'])==len(pool)
                    assert int(f['tabu_kept'])==row['robots']-len(pool)
                    assert int(f['mask_fnv1a64'])==fingerprint and int(f['disabled'])==len(excluded)
                    actual=read(raw/label/(name+'.json'))
                    assert [p[0]*530+p[1] for p in actual['start']]==starts
                    assert len(actual['actualSchedule'])==row['robots']
                    checked=0
                    for robot in excluded:
                        for item in actual['actualSchedule'][robot].split(','):
                            if item:
                                assert int(item.split(':')[1])<0,(robot,item)
                                checked+=1
                    del actual
                    fleet_audit=dict(start_file=str(agent_path),mask_independently_reconstructed=True,
                        mask_fnv1a64=fingerprint,excluded_robots=len(excluded),
                        excluded_schedule_entries_checked=checked,excluded_positive_assignments=0)

                else:
                    assert not fleet
                after_turn=int(case['environment'].get('CGAR_TEMPORAL_PROMISE_AFTER_TURN','0'))
                promise_samples=[fields(l) for l in logs if l.startswith('[cgar-temporal-promise] ')]
                promise_config=[fields(l) for l in logs if l.startswith('[cgar-temporal-promise-config] ')]
                if after_turn:
                    assert promise_config==[dict(after_turn='1',actions='1',protected_priority='1',fixed_work='1')]
                    assert [int(x['step']) for x in promise_samples]==list(range(200,row['steps']+1,200))
                    assert all(int(x['after_turn'])==1 and int(x['calls'])==int(x['step']) for x in promise_samples)
                    assert all(0<=int(x['retained'])<=row['robots'] and 0<=int(x['collision_resets'])<=row['robots'] for x in promise_samples)
                    totals=[int(x['retained_total']) for x in promise_samples]
                    resets=[int(x['collision_resets_total']) for x in promise_samples]
                    assert totals==sorted(totals) and resets==sorted(resets) and totals[-1]>0
                    assert totals[-1]+resets[-1]<=row['robots']*row['steps']
                else:
                    assert not promise_config and not promise_samples
                fairness[key]=waiting_audit(raw/label/(name+'.json'),m,retarget_budget=retarget_budget)
                work[key]=dict(chain_config=chain_config,chain_samples=chain_samples,regional_peak_retention=retention_samples,regional_peaks=peak_samples,retarget_budget=retarget_budget,retarget_samples=retarget_samples,game_fleet=fleet,game_fleet_audit=fleet_audit,rank_squared=rank_samples,after_turn_promises=promise_samples,priority_portfolio=priority_samples,regional_budget=sampled,regional=[fields(l) for l in logs if l.startswith('[cgar-temporal-regions] ')],timing=[fields(l) for l in logs if l.startswith('[cgar-temporal-timing] ')])
                row.update(tasks=m['tasks'],mean_entry_ms=1000*m['total_decision_seconds']/row['steps'],
                           max_entry_seconds=m['max_decision_seconds'],trajectory_sha256=m['trajectory_sha256'],
                           outstanding_age_p90=m['outstanding_task_age']['p90'],competition_budget_confirmed=False)
            else:
                assert summary['after'] is None, 'failed run must not have an accepted partial score'
                row['failure_lines']=[l for l in logs if 'CGAR_TIMEOUT' in l or 'CGAR_ERROR' in l or 'CGAR_INITIALIZATION_ERROR' in l]
                row['trick_receipt_valid']=summary['trick_receipt_valid']
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
                disjoint_physical_core_groups=True,no_cpu_quota=True,decision_limit_ms=spec['time_limit_ms'],strict_one_second_limit=spec['time_limit_ms']==1000,
                competition_budget_confirmed=False,random05_excluded='RANDOM-05' not in spec['instances'],independent_random05_solver_untouched=True,trick=a.trick,control=a.control,full_horizons=spec['horizons'] is None,
                scope='Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.')
    write(out/'verification.json',report);write(out/'fairness.json',fairness);write(out/'regional-work.json',work)
    for name in ['completion.json','submission.json',tag+'-request.json',tag+'-submission.json']:
        shutil.copy2(raw/name,out/name)
    lines=['# Controlled CGAR factor matrix','',report['scope'],'','Enforced entry limit: '+str(spec['time_limit_ms'])+'ms. Planner seeds: '+str(a.seeds)+'.','',
           '| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |',
           '|---|---:|---:|---:|---:|---:|']
    for r in rows:
        lines.append('| {} / {} | {} | {} | {} | {:.3f} | {} |'.format(r['instance'],r['variant']+' / seed'+str(r['seed']),r['tasks'] if r['valid'] else r['outcome'],round(r['mean_entry_ms'],2) if r['valid'] else 'n/a',round(r['max_entry_seconds']*1000,2) if r['valid'] else 'n/a',r['peak_rss_bytes']/1e9,r.get('outstanding_age_p90','n/a')))
    lines+=['','Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.','',
            '[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).']
    (out/'summary.md').write_text('\n'.join(lines)+'\n')
    print('CGAR_FACTOR_MATRIX_VERIFIED',report['valid_runs'],'valid',report['failed_runs'],'failed',flush=True)


if __name__=='__main__':main()
