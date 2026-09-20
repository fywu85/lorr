#!/usr/bin/env python3
"""Refresh the physical work decomposition of the verified Warehouse best."""
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
BASE = ROOT/'experiments/construction-20260918'

def read(p):
    return json.loads(p.read_text())

def write(p, value):
    p.write_text(json.dumps(value, indent=2)+'\n')

def sha(p):
    h = hashlib.sha256()
    with p.open('rb') as f:
        for data in iter(lambda:f.read(4*1024*1024), b''):
            h.update(data)
    return h.hexdigest()

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--raw', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--execute', action='store_true')
    args = parser.parse_args()
    raw, out = args.raw.resolve(), args.output.resolve()
    if not args.execute:
        raw.mkdir(parents=True, exist_ok=False)
        copies = {'run.py':Path(__file__), 'decompose.py':BASE/'decompose.py', 'decompose.cpp':BASE/'decompose.cpp',
                  'verification.json':BASE/'results/match-horizon-native-full-v110/verification.json',
                  'work-budget.json':BASE/'results/match-horizon-native-full-v110/work-budget.json',
                  'metrics.json':BASE/'results/match-horizon-native-full-v110/metrics.json',
                  'outcomes.json':BASE/'results/match-horizon-native-full-v110-outcomes/accounting.json',
                  'nms-accounting.json':BASE/'results/nms-task-pool-audit/accounting.json',
                  'warehouse.map':ROOT/'mr24/warehouse.domain/maps/warehouse_large.map',
                  'cpu_resources.py':ROOT/'tools/cpu_resources.py'}
        for name, source in copies.items():
            shutil.copy2(source, raw/name)
        cases = {r['case']:str(Path(r['raw_case'])/'WAREHOUSE.json') for r in read(raw/'verification.json')['rows'] if r['variant']=='trick_matchguard1'}
        write(raw/'cases.json', cases)
        files = {name:sha(raw/name) for name in list(copies)+['cases.json']}
        write(raw/'request.json', dict(created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(), files=files,
              output=str(out), json_header_sha256=sha(ROOT/'cgar/inc/nlohmann/json.hpp'), offline_only=True))
        command = ['/usr/bin/python3', str(raw/'run.py'), '--raw', str(raw), '--output', str(out), '--execute']
        job = raw/'job.sh'
        job.write_text('#!/bin/bash\nset -eu\nexec '+' '.join(map(shlex.quote, command))+'\n')
        submit = ['qsub','-h','-terse','-w','e','-cwd','-q','debian.q@research44.grid.gsb,debian.q@research57.grid.gsb',
                  '-pe','threaded','1','-binding','linear:1','-l','exclusive=false,h_rt=00:20:00,h_vmem=8G',
                  '-m','n','-N','current_work_audit','-j','y','-o',str(raw/'job.log'),'-S','/bin/bash',str(job)]
        result = subprocess.run(submit, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        write(raw/'submission.json', dict(command=submit, returncode=result.returncode, response=result.stdout))
        result.check_returncode()
        assert re.fullmatch(r'\d+\s*', result.stdout)
        print(result.stdout, end='', flush=True)
        subprocess.run(['qrls', result.stdout.strip()], check=True)
        return
    request = read(raw/'request.json')
    assert request['output'] == str(out)
    def verify_sources():
        for name, expected in request['files'].items():
            assert sha(raw/name) == expected, name
        assert sha(ROOT/'cgar/inc/nlohmann/json.hpp') == request['json_header_sha256']
    verify_sources()
    sys.path.insert(0, str(raw))
    from cpu_resources import cpu_resources
    cpu = cpu_resources()
    assert cpu['effective_cpu_quota'] is None and cpu['physical_cores_visible'] == 1
    os.sched_setaffinity(0, cpu['representative_cpus'])
    write(raw/'allocation.json', cpu)
    subprocess.run(['/usr/bin/python3', str(raw/'decompose.py'), '--output', str(raw/'analysis'),
                    '--cases', str(raw/'cases.json'), '--map', str(raw/'warehouse.map')], check=True)
    provenance = read(raw/'analysis/provenance.json')
    for key in ['fixture_passed','loaded_action_fixture_passed','opposite_turn_fixture_passed','phase_boundary_fixture_passed','initial_gap_fixture_passed']:
        assert provenance[key]
    assert provenance['source_sha256'] == request['files']['decompose.cpp']
    assert provenance['script_sha256'] == request['files']['decompose.py']
    assert provenance['map_sha256'] == request['files']['warehouse.map']
    v, work = read(raw/'verification.json'), read(raw/'work-budget.json')
    assert v['all_valid_within_deadline_and_memory'] and not v['failures'] and work['all_valid']
    references = {r['case']:r for r in v['rows']}
    amounts_by_case = {r['case']:r for r in work['rows']}
    metrics = {r['case']:r for r in read(raw/'metrics.json')}
    outcomes = read(raw/'outcomes.json')
    assert outcomes['all_valid'] and outcomes['verification_sha256'] == request['files']['verification.json']
    rows = []
    for name in read(raw/'cases.json'):
        report = read(raw/'analysis'/(name+'.json'))
        ref, physical = references[name], amounts_by_case[name]
        assert report['completed_tasks'] == ref['tasks'] == physical['tasks']
        assert report['team_size'] == 10000 and report['steps'] == 5000
        assert report['planner_errors'] == report['schedule_errors'] == report['entry_timeouts'] == report['assignments_after_pickup'] == 0
        for k, phase in enumerate(['idle','empty','loaded']):
            observed = report['full_phase_actions'][phase]
            for action in ['fw','cr','ccr','wait']:
                assert observed[action] == metrics[name]['movement_phases'][str(k)][action]
            assert observed['other'] == 0
        a = report['completed_loaded_action_totals']
        parts = dict(completed_chain_lower_bound=report['completed_chain_shortest']['sum'],
            completed_forward_detour=report['completed_loaded_forward_excess_over_shortest'],
            completed_turns=a['cr']+a['ccr'], completed_waits=a['wait'],
            empty_including_unfinished=report['empty_robot_steps_including_reassignments'],
            loaded_unfinished=report['loaded_robot_steps_including_unfinished_tasks']-sum(a.values()),
            idle=report['unassigned_robot_steps'])
        assert parts['completed_chain_lower_bound']+parts['completed_forward_detour'] == a['fw']
        assert sum(parts.values()) == 50000000 and min(parts.values()) >= 0
        assert parts['loaded_unfinished'] == physical['unfinished_task_steps']['loaded']
        assert parts['empty_including_unfinished'] == physical['phases']['empty']['total']
        raw_hash = provenance['inputs'][name]['sha256']
        assert raw_hash == outcomes['reports'][name]['input_sha256']
        rows.append(dict(case=name, seed=ref['seed'], tasks=ref['tasks'], trajectory_sha256=ref['trajectory_sha256'],
                         raw_sha256=raw_hash, components=parts, per_completed_task={k:x/ref['tasks'] for k,x in parts.items()},
                         robot_steps_per_completed_task=50000000/ref['tasks']))
    verify_sources()
    total = sum(r['tasks'] for r in rows)
    sums = {k:sum(r['components'][k] for r in rows) for k in rows[0]['components']}
    pooled = {k:x/total for k,x in sums.items()}
    nms = read(raw/'nms-accounting.json')
    result = dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(), all_valid=True,
                  offline_only=True, source_commit=v['exact_production_source_commit'], binary_sha256=v['binary_sha256'],
                  all_five_fixtures_pass=True, all_phase_counters_exact=True, raw_hashes_exact=True,
                  rows=rows, pooled_per_completed_task=pooled,
                  nms_diagnostic_per_completed_task=nms['per_completed_task'],
                  cgar_minus_nms_diagnostic={k:pooled[k]-nms['per_completed_task'][k] for k in pooled},
                  limits='Different task cohorts and resources; observational decomposition, not causal savings. NMS saved run had one timeout and RSS above32GB and is not a qualifying score. No new solver benchmark or throughput record.')
    out.mkdir(parents=True, exist_ok=False)
    for name in ['request.json','submission.json','allocation.json','nms-accounting.json']:
        shutil.copy2(raw/name, out/name)
    for path in (raw/'analysis').glob('*.json'):
        shutil.copy2(path, out/path.name)
    write(out/'accounting.json', result)
    print('CURRENT_WORK_AUDIT_VERIFIED', json.dumps(pooled), flush=True)

if __name__ == '__main__':
    main()
