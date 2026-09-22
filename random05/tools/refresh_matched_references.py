#!/usr/bin/env python3
"""Refresh max(NMS,KK) references from explicitly supplied independent audits."""
import argparse
import datetime
import json
import math
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
def read(path):return json.loads((ROOT/path).read_text())
def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--extra-audit',action='append',default=[])
    args=p.parse_args()
    paths=list(dict.fromkeys([
        'random05/results/random-nms-kk-baselines-v1/audit.json',
        'random05/results/kk-random05-arena2-research35-v1/audit.json',
        'random05/results/kk-random05-arena2-research36-v1/audit.json',
    ]+args.extra_audit))
    rows=[]
    for path in paths:
        for row in read(path)['rows']:rows.append(dict(row,audit=path))
    front=read('random05/random-frontiers.json');old=read('random05/results/generalization-20260921/audit.json')['runs'];archived={}
    for instance,record in front.items():
        candidates={'nms':[],'kk':[]}
        for row in rows:
            if row['valid'] and row['input']==record['trick']['case']['input']:
                assert row['team'] in candidates
                candidates[row['team']].append(dict(tasks=row['tasks'],evidence=row['evidence'],audit=row['audit'],name=row['name']))
        for row in old:
            if row['valid'] and row['profile']=='nms' and row['instance']==instance:
                candidates['nms'].append(dict(tasks=row['tasks'],evidence=row['evidence'],audit='random05/results/generalization-20260921/audit.json',name=row['case']))
        if instance=='RANDOM-05':
            path='random05/results/generalization-20260921/reference-random05.json';ref=read(path);assert ref['same_inputs_and_allocation']
            candidates['nms'].append(dict(tasks=ref['summary']['result']['numTaskFinished'],evidence=path,audit=path,name=ref['summary']['name']))
        best={team:max(values,key=lambda v:v['tasks']) if values else None for team,values in candidates.items()}
        complete=all(best.values());target=max(v['tasks'] for v in best.values()) if complete else None
        archived[instance]=dict(nms=best['nms'],kk=best['kk'],complete=complete,target=target,
            team=max(best,key=lambda t:best[t]['tasks']) if complete else None,
            ten_percent_target=math.ceil(1.1*target) if complete else None,observations=candidates)
    fresh={}
    for instance,version in [('RANDOM-01',1),('RANDOM-02',1),('RANDOM-03',2)]:
        path='random05/results/'+instance.lower().replace('-','')+'-fresh-validation-v'+str(version)+'/audit.json'
        frozen=read(path);records=[]
        for comparison in frozen['comparisons']:
            kk=[r for r in rows if r['valid'] and r['name'].startswith(instance.lower()+'-seed'+str(comparison['seed'])+'-kk-repeat')]
            assert len(kk)==2,'missing or duplicated frozen KK fresh comparisons'
            target=max(max(r['tasks'] for r in kk),comparison['stronger_nms'])
            entry=dict(seed=comparison['seed'],ours=comparison['ours'],nms=comparison['nms_repeats'],kk=[r['tasks'] for r in kk],
                target=target,gain_percent=100*(comparison['ours']/target-1),evidence=path,kk_audit='random05/results/random-nms-kk-baselines-v1/audit.json')
            if instance in ('RANDOM-01','RANDOM-02'):entry['general']=comparison['baseline']
            records.append(entry)
        total=sum(r['ours'] for r in records);target=sum(r['target'] for r in records)
        result=dict(rows=records,candidate_total=total,reference_total=target,gain_percent=100*(total/target-1))
        if all('general' in r for r in records):
            result['general_total']=sum(r['general'] for r in records);result['general_gain_percent']=100*(result['general_total']/target-1)
        fresh[instance]=result
    fresh_history={}
    v3_path='random05/results/random03-fresh-validation-v3/audit.json'
    if (ROOT/v3_path).exists():
        frozen=read(v3_path)
        replay=read('random05/results/random03-fresh-validation-v3/replay-audit.json')
        assert frozen['all_valid'] and frozen['reference']=='max(NMS, Kitty Knight)'
        assert replay['complete'] and replay['all_attempts_valid'] and not replay['pending']
        assert frozen['protocol_commit']==replay['protocol_commit']
        records=[]
        for c in frozen['comparisons']:
            target=max(c['nms_repeats']+c['kk_repeats'])
            assert target==c['matched_max'] and len(c['nms_repeats'])==len(c['kk_repeats'])==2
            records.append(dict(seed=c['seed'],ours=c['ours'],baseline=c['baseline'],
                nms=c['nms_repeats'],kk=c['kk_repeats'],target=target,
                gain_percent=100*(c['ours']/target-1),evidence=v3_path,kk_audit=v3_path))
        total=sum(r['ours'] for r in records);target=sum(r['target'] for r in records)
        baseline=sum(r['baseline'] for r in records)
        result=dict(rows=records,candidate_total=total,reference_total=target,
            gain_percent=100*(total/target-1),baseline_total=baseline,
            gain_over_baseline_percent=100*(total/baseline-1),version=3,
            candidate_development_tasks=2646,baseline_development_tasks=2620,
            protocol_commit=frozen['protocol_commit'],candidate_source_commit=frozen['candidate_source_commit'])
        fresh_history['RANDOM-03']={'v2':fresh['RANDOM-03'],'v3':result}
        fresh['RANDOM-03']=result
    report=dict(updated_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
        policy='Strongest retained valid full local run of each team; max(NMS,KK) requires both. Published results remain separate. Original failures remain in the source audits.',
        input_audits=paths,archived=archived,fresh=fresh,fresh_history=fresh_history,pending=read(paths[0])['pending'])
    (ROOT/'random05/references/matched-nms-kk-combined.json').write_text(json.dumps(report,indent=2)+'\n')
    for instance,result in archived.items():print(instance,result['target'],result['ten_percent_target'])
if __name__=='__main__':main()
