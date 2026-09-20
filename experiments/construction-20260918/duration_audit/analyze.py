#!/usr/bin/env python3
"""Retrospective prefix-direction audit, with separately declared future checks."""
import argparse
import datetime
import hashlib
import json
from pathlib import Path

BASE=Path(__file__).resolve().parents[1]
CASES=[
 ('native-work-full-v95','trick_native_work4m_regions2','whole_run'),
 ('native-pickup-full-v97','trick_native_pickup5','whole_run'),
 ('p90-pickup-combined-v99','trick_p90_pickup5','whole_run'),
 ('p90-pickup-refine-v106','trick_p90_pickup8','whole_run'),
 ('native-workers-full-v103','trick_pickup8_workers1','whole_run'),
 ('native-neutral-tail-full-v101','trick_p90_pickup8_neutral0','whole_run'),
 ('match-cadence-native-full-v108','trick_match10','whole_run'),
 ('match-cadence-seeds13-v111','trick_match10','whole_run'),
 ('known-horizon-full-v86','trick_native_horizon0','cutoff'),
 ('horizon-margin-full-v92','trick_native_horizon5000_margin0','cutoff'),
 ('horizon-percentile-full-v96','trick_native_percentile0','cutoff'),
 ('horizon-percentile-refine-v98','trick_native_percentile90','cutoff'),
 ('match-horizon-native-full-v110','trick_matchguard0','cutoff'),
]
HELD_OUT=[('native-turn-full-v113','trick_turn1','whole_run'),
          ('match-horizon-seeds13-v114','trick_matchguard0','cutoff')]
CHECKPOINTS=[500,800,1000,1500,2000,2500,3000,3500,4000,4250,4500,4750,4900,4975,5000]


def read(p):return json.loads(p.read_text())
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def sign(v):return (v>0)-(v<0)


def load(cases):
    rows=[];inputs={}
    for name,control,scope in cases:
        root=BASE/'results'/name
        paths={k:root/(k+'.json') for k in ['verification','metrics']}
        before={str(p):sha(p) for p in paths.values()}
        v=read(paths['verification']);metrics={r['case']:r for r in read(paths['metrics'])}
        assert v['all_valid_within_deadline_and_memory'] and not v['failures']
        controls={r['seed']:r for r in v['rows'] if r['variant']==control}
        assert controls
        for r in v['rows']:
            m=metrics[r['case']]
            assert m['steps']==5000 and m['tasks']==r['tasks'] and m['trajectory_sha256']==r['trajectory_sha256']
            assert set(m['curve'])==set(str(t) for t in range(25,5001,25)) and m['curve']['5000']==r['tasks']
            if r['variant']==control:continue
            c=controls[r['seed']];cm=metrics[c['case']]
            delta={str(t):m['curve'][str(t)]-cm['curve'][str(t)] for t in range(25,5001,25)}
            final=delta['5000'];assert final==r['tasks']-c['tasks']
            # Earliest sampled prefix after which every later sampled sign agrees.
            wrong=[t for t in range(25,5001,25) if sign(delta[str(t)])!=sign(final)]
            stable=(max(wrong)+25) if wrong else 25
            assert stable<=5000
            rows.append(dict(experiment=name,scope=scope,seed=r['seed'],variant=r['variant'],
                             control_variant=control,final_difference=final,prefix_difference=delta,
                             stable_direction_from_sample=stable,control_trajectory=c['trajectory_sha256'],
                             candidate_trajectory=r['trajectory_sha256']))
        assert before=={str(p):sha(p) for p in paths.values()}
        inputs.update(before)
    return rows,inputs


def summary(rows):
    groups={}
    for scope in ['whole_run','cutoff','all']:
        rs=[r for r in rows if scope=='all' or r['scope']==scope]
        if not rs:continue
        # No family independence or confidence claim; some pairs share controls.
        assert all(r['final_difference']!=0 for r in rs)
        checkpoints=[]
        for t in CHECKPOINTS:
            ds=[(r['prefix_difference'][str(t)],r['final_difference']) for r in rs]
            checkpoints.append(dict(step=t,pairs=len(ds),same_direction=sum(sign(x)==sign(y) for x,y in ds),
                                    no_prefix_difference=sum(x==0 for x,y in ds),
                                    eventual_winners_rejected=sum(x<0 and y>0 for x,y in ds),
                                    eventual_losers_favored=sum(x>0 and y<0 for x,y in ds)))
        groups[scope]=dict(pairs=len(rs),checkpoints=checkpoints,
                           earliest_all_stably_correct_sample=max(r['stable_direction_from_sample'] for r in rs))
    return groups


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--output',type=Path,required=True)
    p.add_argument('--held-out',action='store_true')
    a=p.parse_args();rows,inputs=load(HELD_OUT if a.held_out else CASES)
    data=dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),all_valid=True,
              kind='predeclared_new_cases' if a.held_out else 'retrospective_development_cases',
              source_sha256=sha(Path(__file__)),inputs=inputs,groups=summary(rows),rows=rows,
              held_out_cases=HELD_OUT,checkpoints=CHECKPOINTS,
              limits=['Observed directions only, not confidence intervals or reliable stopping guarantees.',
                      'Pairs share fixed instances, seeds, controls and selected development history; they are not independent samples.',
                      'Prefixes come from full runs with their original configured horizon; this does not measure changing the horizon to the prefix length.',
                      'A 5000-step cutoff policy may first act late; identical early scores provide no evidence of no gain.',
                      'The two held-out directories were declared while their full runs were still pending. They are new solver cases, not new generated inputs.',
                      'All running full cases finish regardless of this audit; no prefix-only promotion or termination.'])
    a.output.mkdir(parents=True,exist_ok=False)
    (a.output/'analysis.json').write_text(json.dumps(data,indent=2)+'\n')
    lines=['# Evaluation duration: observed prefix-direction errors','',data['kind']+'; checked '+data['checked_utc']+'.','',
           'A prefix favors the candidate when its completed-task count is greater than the paired control. The full5000-step result supplies the eventual direction. Ties abstain. This is a retrospective diagnostic; no statistical reliability guarantee.','']
    for scope,g in data['groups'].items():
        lines += ['## '+scope,'','Pairs: '+str(g['pairs'])+'. Earliest sampled prefix after which all remaining sampled directions agree with the final result: '+str(g['earliest_all_stably_correct_sample'])+' steps. This observed value is not a validated stopping rule.','',
                  '| Prefix steps | Correct direction | Tied /no signal | Eventual winners rejected | Eventual losers favored |','|---:|---:|---:|---:|---:|']
        for r in g['checkpoints']:
            lines.append('| {step} | {same_direction}/{pairs} | {no_prefix_difference} | {eventual_winners_rejected} | {eventual_losers_favored} |'.format(**r))
        lines += ['']
    lines += ['## Limits','']+[x for x in data['limits']]+['','[Exact paired curves and input hashes](analysis.json).']
    (a.output/'summary.md').write_text('\n'.join(lines)+'\n')
    print(json.dumps(data['groups'],indent=2))


if __name__=='__main__':main()
