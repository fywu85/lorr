#!/usr/bin/env python3
"""Package motion controls, input snapshots and reproducibility records."""
import hashlib
import datetime
import re
import json
from pathlib import Path
import shutil
ROOT=Path(__file__).resolve().parents[2];HERE=Path(__file__).resolve().parent;OUT=HERE/'results'
OUT.mkdir(exist_ok=True)
def write(name,data):(OUT/name).write_text(json.dumps(data,indent=2)+'\n')
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()

def snapshots():
    rows=[]
    for kind,folder in [('supplied','cgar-motion-snapshots-20260918'),('uniform','cgar-motion-uniform-snapshots-20260918')]:
        raw=ROOT/'runs'/folder;spec=json.loads((raw/'spec.json').read_text())
        for case in spec['cases']:
            work=raw/case['name'];allocation=json.loads((work/'allocation.json').read_text())
            row={'case':case['name'],'reference':kind,'snapshot_sha256':case['sha256'],'allocation':allocation,'teams':{}}
            for team in ['cgar','nms']:
                p=work/(team+'.json');r=json.loads(p.read_text());u=json.loads((work/(team+'-usage.json')).read_text())
                d={k:r[k] for k in ['valid','instance','source_timestep','initial_goals','initially_at_goal','steps_requested','steps_completed','time_limit_ms','preprocess_seconds','actions_all','actions_pending','checkpoints'] if k in r}
                d['goals_reached']=r.get('reached_curve',[0])[-1]
                d['reached_checkpoints']={str(k):r['reached_curve'][k-1] for k in [25,50,75,100]} if 'reached_curve' in r else {}
                d['max_decision_seconds']=max(r.get('decision_seconds',[0]));d['total_decision_seconds']=sum(r.get('decision_seconds',[]))
                d['late_decisions']=r.get('late_decisions',[]);d['error']=r.get('error');d['parked']=r.get('parked')
                d['max_disabled_pending']=max(r.get('reference_disabled_pending',[]) or [0])
                d['resource_usage']=u;d['raw_result_sha256']=sha(p)
                behavior={k:r[k] for k in ['arrival_steps','reached_curve','actions_all','actions_pending','checkpoints'] if k in r}
                d['arrival_and_summary_sha256']=hashlib.sha256(json.dumps(behavior,sort_keys=True).encode()).hexdigest()
                row['teams'][team]=d
            rows.append(row)
        write('snapshot-'+kind+'-build.json',spec['build'])
    assert len(rows)==40 and all(t['valid'] and t['resource_usage']['exit']==0 for r in rows for t in r['teams'].values())
    write('snapshots.json',rows)
    inputs=HERE/'snapshots';inputs.mkdir(exist_ok=True);manifest=[]
    source=ROOT/'runs/cgar-motion-snapshot-build-v2-20260918/snapshots'
    for item in json.loads((source/'manifest.json').read_text()):
        p=Path(item['snapshot']);shutil.copy2(p,inputs/p.name)
        manifest.append(dict(item,snapshot='snapshots/'+p.name))
    write('snapshot-inputs.json',manifest)
    # The two reference treatments use the same CGAR executable and inputs.
    first={r['case']:r for r in rows if r['reference']=='supplied'}
    equivalence=[{'case':r['case'],'identical_arrivals_and_movement_summaries':r['teams']['cgar']['arrival_and_summary_sha256']==first[r['case']]['teams']['cgar']['arrival_and_summary_sha256']} for r in rows if r['reference']=='uniform']
    assert all(x['identical_arrivals_and_movement_summaries'] for x in equivalence)
    write('snapshot-cgar-equivalence.json',equivalence)

def validation():
    for label,name in [('diagnostics','cgar-motion-diagnostics-build-20260918'),('orientation','cgar-motion-orientation-build-20260918'),('portfolio','cgar-motion-portfolio-build-20260918'),('portfolio-protection','cgar-motion-final-validation-20260918'),('rotation','cgar-motion-rotation-build-20260918'),('admission','cgar-motion-admission-build-20260918')]:
        raw=ROOT/'runs'/name;dest=OUT/('validation-'+label);dest.mkdir(exist_ok=True)
        for old,new in [('build.json','manifest.json'),('allocation.json','allocation.json'),('build.log','validation.txt')]:shutil.copy2(raw/old,dest/new)
        if (raw/'variant-validation.log').exists():shutil.copy2(raw/'variant-validation.log',dest/'variant-validation.txt')

def timing_audit():
    original = json.loads((OUT / 'admission/metrics.json').read_text())
    repeats = json.loads((OUT / 'timing-repeat/metrics.json').read_text())
    by_case = {(r['case'], r['instance']): r for r in original}
    anomalous = []
    timestamp = re.compile(r'^\[(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}\.\d+)\]')
    for row in original:
        if row['max_decision_seconds'] <= 1:
            continue
        raw = ROOT / 'runs/cgar-motion-admission-full-20260918' / row['case']
        result = json.loads((raw / (row['instance'] + '.json')).read_text())
        log = raw / (row['instance'] + '.log')
        events = [(datetime.datetime.strptime(m.group(1), '%Y-%m-%d %H:%M:%S.%f'), line)
                  for line in log.read_text().splitlines()
                  for m in [timestamp.match(line)] if m]
        for step, seconds in enumerate(result['plannerTimes']):
            if seconds <= 1:
                continue
            matches = [i for i, (_, line) in enumerate(events)
                       if '[timestep={}] planner returns'.format(step) in line]
            assert len(matches) == 1
            i = matches[0]
            previous, returned, following = events[i - 1:i + 2]
            anomalous.append({
                'case': row['case'], 'instance': row['instance'], 'decision_index': step,
                'plannerTimes_seconds': seconds, 'simulator_entry_timeouts': result['numEntryTimeouts'],
                'previous_log_record': previous[1], 'planner_return_log_record': returned[1],
                'following_log_record': following[1],
                'previous_to_return_timestamp_seconds': (returned[0] - previous[0]).total_seconds(),
                'return_to_following_timestamp_seconds': (following[0] - returned[0]).total_seconds(),
                'source_log_sha256': sha(log)})
    comparisons = []
    for row in repeats:
        first = by_case[row['case'], row['instance']]
        comparisons.append({
            'case': row['case'], 'instance': row['instance'],
            'identical_trajectory': row['trajectory_sha256'] == first['trajectory_sha256'],
            'trajectory_sha256': row['trajectory_sha256'],
            'original_max_wrapper_seconds': first['max_decision_seconds'],
            'repeat_max_wrapper_seconds': row['max_decision_seconds']})
    assert len(comparisons) == 12 and all(r['identical_trajectory'] for r in comparisons)
    write('timing-audit.json', {
        'measurement': 'plannerTimes times BaseSystem::plan, including planner-return logging after Entry::compute finishes.',
        'interpretation': 'Pauses coincide across separate processes after the planner-return log timestamp. This supports a logging or host pause, not a longer CGAR search; the specific system cause was not instrumented.',
        'anomalies': anomalous, 'repeat_comparisons': comparisons,
        'repeat_allocation': json.loads((OUT / 'timing-repeat/allocation.json').read_text())})

def reproducer_validation():
    archived = json.loads((OUT / 'snapshots.json').read_text())
    rows = []
    for label in ['supplied', 'uniform']:
        raw = ROOT / ('runs/cgar-motion-reproducer-smoke-' + label + '-20260918')
        spec = json.loads((raw / 'spec.json').read_text())
        case = spec['cases'][0]
        work = raw / case['name']
        completion = json.loads((work / 'completion.json').read_text())
        assert all(item['exit'] == 0 for item in completion)
        prior = next(r for r in archived if r['reference'] == label and r['case'] == case['name'])
        row = {'reference': label, 'case': case['name'], 'snapshot_sha256': case['sha256'],
               'allocation': json.loads((work / 'allocation.json').read_text()),
               'build': spec['build'], 'teams': {}}
        for team in ['cgar', 'nms']:
            result = json.loads((work / (team + '.json')).read_text())
            assert result['valid'] and result['steps_completed'] == 100 and not result.get('late_decisions')
            behavior = {k: result[k] for k in ['arrival_steps', 'reached_curve', 'actions_all', 'actions_pending', 'checkpoints']}
            digest = hashlib.sha256(json.dumps(behavior, sort_keys=True).encode()).hexdigest()
            row['teams'][team] = {
                'valid': result['valid'], 'steps_completed': result['steps_completed'],
                'goals_reached': result['reached_curve'][-1],
                'max_decision_seconds': max(result['decision_seconds']),
                'arrival_and_summary_sha256': digest,
                'raw_result_sha256': sha(work / (team + '.json')),
                'resource_usage': json.loads((work / (team + '-usage.json')).read_text())}
            if team == 'cgar':
                assert digest == prior['teams'][team]['arrival_and_summary_sha256']
                row['teams'][team]['matches_archived_arrivals_and_summaries'] = True
        rows.append(row)
    write('reproducer-validation.json', rows)

if __name__=='__main__':snapshots();validation();timing_audit();reproducer_validation()
