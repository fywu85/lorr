"""Verify current-reference motion replay and construct an exact work identity."""
from pathlib import Path
import datetime
import hashlib
import json
import shutil

ROOT = Path(__file__).resolve().parents[3]
BASE = ROOT / 'experiments/construction-20260918'
RAW = ROOT / 'runs/cgar-current-bottleneck-audit-v50-20260920'
OUT = BASE / 'results/current-bottleneck-audit-v50'

def read(p):
    return json.loads(p.read_text())

def main():
    OUT.mkdir(exist_ok=True)
    spec = read(RAW / 'spec.json')
    refs = {}
    for cohort in ['full', 'confirm']:
        for row in read(BASE / ('results/pickup-full-regions-%s-v44/metrics.json' % cohort)):
            if 'regions4_rounds2' in row['case']:
                seed = int(row['case'].rsplit('-s', 1)[1].split('-')[0]); refs[seed] = row
    rows, reports = [], []
    def accounting(d):
        n = d['completed_tasks']; a = d['completed_loaded_action_totals']; phases = d['full_phase_actions']
        completed_loaded = sum(a.values()); total = d['team_size'] * d['steps']
        assert sum(sum(p.values()) for p in phases.values()) == total
        assert completed_loaded == d['completed_loaded_steps']['sum']
        amounts = {'completed_chain_lower_bound':d['completed_chain_shortest']['sum'],
                   'completed_forward_detour':d['completed_loaded_forward_excess_over_shortest'],
                   'completed_turns':a['cr'] + a['ccr'], 'completed_waits':a['wait'],
                   'empty_including_unfinished':d['empty_robot_steps_including_reassignments'],
                   'loaded_unfinished':d['loaded_robot_steps_including_unfinished_tasks'] - completed_loaded,
                   'idle':d['unassigned_robot_steps']}
        assert amounts['completed_chain_lower_bound'] + amounts['completed_forward_detour'] == a['fw']
        assert a['other'] == 0 and sum(amounts.values()) == total and all(v >= 0 for v in amounts.values())
        return {'completed_tasks':n,'robot_steps':total,'components':amounts,
                'per_completed_task':{k:v/n for k,v in amounts.items()},'robot_steps_per_completed_task':total/n,
                'completed_loaded_steps':completed_loaded/n,'full_phase_actions':phases}
    for seed in range(6):
        name='seed%d'%seed; path=RAW/name; d=read(path/(name+'.json')); p=read(path/'provenance.json')
        completion=read(RAW/(name+'-completion.json')); ref=refs[seed]
        old=read(BASE/('results/chain-price-audit-v50/'+name+'/input-metadata.json'))
        assert completion['exit_code']==0
        assert p['inputs'][name]['sha256']==old['raw_trajectory_file_sha256']
        assert old['expected_trajectory_fingerprint']==ref['trajectory_sha256']
        assert d['completed_tasks']==ref['tasks']==old['tasks_finished']
        assert d['team_size']==10000 and d['steps']==5000
        assert d['planner_errors']==d['schedule_errors']==d['entry_timeouts']==0
        for field in ['fixture_passed','loaded_action_fixture_passed','opposite_turn_fixture_passed','phase_boundary_fixture_passed']: assert p[field]
        assert p['source_sha256']==spec['source_sha256']['experiments/construction-20260918/decompose.cpp']
        assert p['script_sha256']==spec['source_sha256']['experiments/construction-20260918/decompose.py']
        for k,phase in enumerate(['idle','empty','loaded']):
            for action in ['fw','cr','ccr','wait']:
                assert d['full_phase_actions'][phase][action]==ref['movement_phases'][str(k)][action]
        target=OUT/name;target.mkdir(exist_ok=True)
        for filename in [name+'.json','provenance.json']:
            shutil.copyfile(str(path/filename),str(target/filename))
        shutil.copyfile(str(RAW/(name+'-completion.json')),str(target/'completion.json'))
        row=accounting(d); row.update(seed=seed,trajectory_sha256=ref['trajectory_sha256'],finished_utc=completion['finished_utc'],
                                     final1000=ref['completed_per_1000'][-1],reassigned_tasks=ref['reassigned_tasks'],
                                     snapshot_protected={key:sum(ref['movement_phases'][str(phase)][key] for phase in range(3)) for key in ['recovery','primary','safety_cancel','blocked_forward']})
        rows.append(row);reports.append(d)
    kk=read(BASE/'results/loaded-motion-audit-v1/kittyknight.json');leader=accounting(kk)
    total_tasks=sum(r['completed_tasks'] for r in rows);total_steps=sum(r['robot_steps'] for r in rows)
    components={k:sum(r['components'][k] for r in rows) for k in leader['components']}
    pooled={k:v/total_tasks for k,v in components.items()}
    delta={k:pooled[k]-leader['per_completed_task'][k] for k in pooled}
    assert abs(sum(delta.values())-(total_steps/total_tasks-leader['robot_steps_per_completed_task']))<1e-9
    report={'checked_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),
            'scope':'Accounting identity for six current reference runs versus one local KittyKnight run; observational, different completed cohorts, unfinished work retained explicitly.',
            'reference_commit':spec['reference_commit'],'verified_raw_hashes_against_previous_independent_replay':6,
            'all_full_phase_action_counters_match_production':True,'all_fixture_checks_pass':True,
            'rows':rows,'kittyknight':leader,'pooled_cgar':{'tasks':total_tasks,'robot_steps':total_steps,'components':components,'per_completed_task':pooled,'robot_steps_per_completed_task':total_steps/total_tasks},
            'cgar_minus_kittyknight_steps_per_completed_task':delta,
            'mean_cgar_tasks':total_tasks/6,'target_tasks':leader['completed_tasks'],
            'required_throughput_lift_percent':100*(leader['completed_tasks']/(total_tasks/6)-1),
            'limitations':['No component delta is a causal or additive recoverable saving.','Completed chain length depends on assignment, completion order and censoring.','The KittyKnight reference used 38.858 GB RSS, above our 32 GB cap, and more CPU cores.','Necessary turns, collision-avoiding waits and detours are included.','No matched policy counterfactual or new throughput improvement is established.']}
    (OUT/'accounting.json').write_text(json.dumps(report,indent=2)+'\n')
    for name in ['spec.json','submission.json']:shutil.copyfile(str(RAW/name),str(OUT/name))
    for p in (RAW/'seed0').glob('*fixture*.json'):shutil.copyfile(str(p),str(OUT/p.name))
    print(json.dumps({k:report[k] for k in ['mean_cgar_tasks','required_throughput_lift_percent','cgar_minus_kittyknight_steps_per_completed_task']},indent=2))
    print('Pooled CGAR per task:',json.dumps(pooled)); print('Leader per task:',json.dumps(leader['per_completed_task']))

if __name__=='__main__':main()
