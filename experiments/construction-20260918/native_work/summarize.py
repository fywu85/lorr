#!/usr/bin/env python3
"""Render a compact report from complete, independently verified full runs."""
import argparse
import hashlib
import json
from pathlib import Path


def read(p):
    return json.loads(p.read_text())


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--results',type=Path,required=True)
    p.add_argument('--title',required=True)
    p.add_argument('--assessment',required=True)
    a=p.parse_args();root=a.results
    v=read(root/'verification.json');f=read(root/'fairness.json');w=read(root/'work-budget.json')
    assert v['all_valid_within_deadline_and_memory'] and not v['failures'] and w['all_valid']
    assert w['inputs']['verification']['sha256']==hashlib.sha256((root/'verification.json').read_bytes()).hexdigest()
    work={r['case']:r for r in w['rows']};controls={r['seed']:r for r in v['rows'] if r['variant']==v['control_variant']}
    core_counts={r['reserved_physical_cores'] for r in v['rows']}
    assert len(core_counts)==1
    cores=next(iter(core_counts))
    deadline_ms=v['decision_limit_ms']
    assert deadline_ms in (1000,5000)
    assert all(r['max_entry_seconds'] <= deadline_ms/1000 for r in v['rows'])
    if v.get('competition_budget_confirmed'):
        assert deadline_ms==1000 and v['exclusive_host_requested']
        timing_scope='The complete runs pass the enforced competition1s entry limit on an exclusive host. This validates these runs, not every seed or machine; timing differences do not isolate individual optimizations.'
    else:
        timing_scope='Development timings do not certify the competition1s limit or isolate speedups. Host exclusivity requested: '+str(v.get('exclusive_host_requested'))+'.'
    lines=['# '+a.title,'',a.assessment,'',
           'Verified '+v['checked_utc']+'. Source ['+v['exact_production_source_commit'][:7]+'](https://github.com/fywu85/lorr/commit/'+v['exact_production_source_commit']+'); binary `'+v['binary_sha256']+'`.', '',
           'Every case completes5000steps with10000robots, zero planner/scheduler errors and timeouts, and independently reconciled50million robot actions. All'+str(v['verified_source_and_test_files'])+'source/test hashes, fixed work, distinct '+str(cores)+'-core bindings,'+str(deadline_ms)+'ms entry deadline and32decimalGB RSS checks pass. Exact prior controls reproduce their complete trajectory hashes. '+timing_scope,'',
           '| Variant / seed | Tasks | Paired change | Final1000 | Mean entry ms | Max entry s | Peak RSS GB | Mean CPU cores /'+str(cores)+' | Full minutes |',
           '|---|---:|---:|---:|---:|---:|---:|---:|---:|']
    for r in v['rows']:
        assert r['tasks']==f[r['case']]['tasks_completed']==work[r['case']]['tasks']
        assert r['trajectory_sha256']==work[r['case']]['trajectory_sha256']
        diff=r['tasks']-controls[r['seed']]['tasks']
        lines.append('| {variant} /{seed} | {tasks:,} | {diff:+,} | {final1000:,} | {mean_entry_ms:.3f} | {max_entry_seconds:.6f} | {ram:.3f} | {average_cpu_cores:.3f} | {minutes:.2f} |'.format(diff=diff,ram=r['peak_rss_bytes']/1e9,minutes=r['wall_seconds']/60,**r))
    lines += ['', '| Variant / seed | Oldest never assigned | Outstanding age p90 /max | First-half tasks incomplete | Initial never assigned | Max retargets | After-pickup assignments |', '|---|---:|---:|---:|---:|---:|---:|']
    for r in v['rows']:
        x=f[r['case']]
        lines.append('| {} /{} | {} | {} /{} | {} | {} | {} | {} |'.format(r['variant'],r['seed'],x['never_assigned_age']['max'],x['outstanding_age']['p90'],x['outstanding_age']['max'],x['first_half_revealed_still_incomplete'],x['initial_never_assigned'],x['max_reassignments'],x['assignments_after_pickup']))
    lines += ['', 'Ordinary HRRN and forced-oldest admission remain enabled, short preference remains OFF, and held/started/primary/recovery protections remain. Finite observations on solver seeds of one fixed input do not prove starvation freedom. Tail regressions are retained in the table.', '', '| Candidate / seed | Empty robot-work change | Unfinished task-work change | Forward change | Turn change | Wait change |', '|---|---:|---:|---:|---:|---:|']
    for r in v['rows']:
        if r['variant']==v['control_variant']: continue
        c=controls[r['seed']];x,y=work[c['case']],work[r['case']]
        values=[y['phases']['empty']['total']-x['phases']['empty']['total'],y['unfinished_task_total']-x['unfinished_task_total']]+[y['actions'][k]-x['actions'][k] for k in ['forward','turn','wait']]
        lines.append('| {} /{} | {} |'.format(r['variant'],r['seed'],' | '.join(format(z,'+,') for z in values)))
    lines += ['', 'Unfinished-task work overlaps the empty/loaded and action partitions; never add these as separate costs. Changed task cohorts and trajectories prevent a causal savings interpretation. The assumed5000-step horizon and all Warehouse policies require `--trick WAREHOUSE`.', '', '[Full verification and exact per-case configuration](verification.json), [fairness](fairness.json), [physical work](work-budget.json), [sampled policy work](work-samples.json).']
    (root/'summary.md').write_text('\n'.join(lines)+'\n')


if __name__=='__main__':main()
