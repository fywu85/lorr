#!/usr/bin/env python3
"""Combine two independently verified, disjoint full-seed remaining-flow studies."""
import datetime
import hashlib
import json
from pathlib import Path
import statistics
import subprocess

BASE = Path(__file__).resolve().parents[1]

def main():
    paths = [BASE / 'results' / name / 'comparison.json' for name in
             ['remaining-flow-full-v56-shared', 'remaining-flow-confirm-v56-shared']]
    studies = [json.loads(p.read_text()) for p in paths]
    resolve = lambda value: subprocess.check_output(['git', 'rev-parse', value + '^{commit}'], cwd=BASE, text=True).strip()
    source = resolve(studies[0]['source_commit'])
    binary = studies[0]['binary_sha256']
    rows, pairs = [], []
    for study in studies:
        assert resolve(study['source_commit']) == source and study['binary_sha256'] == binary
        assert study['full_run'] and not study['failures']
        assert study['decision_limit_ms'] == 5000 and study['benchmark_mode'] == 'relaxed_development'
        assert study['all_valid_within_deadline_and_memory'] and not study['competition_budget_confirmed']
        assert study['source_files_verified'] == 26
        assert study['complete_entry_samples'] == 5000 * len(study['rows'])
        assert sorted(study['exact_control_seeds']) == sorted(p['seed'] for p in study['pairs'])
        rows.extend(study['rows']); pairs.extend(study['pairs'])
    pairs.sort(key=lambda r: r['seed'])
    assert [r['seed'] for r in pairs] == list(range(6)) and len(rows) == 12
    assert {(r['seed'], int(r['environment']['CGAR_TEMPORAL_REMAINING_FLOW'])) for r in rows} == {(s,m) for s in range(6) for m in [0,1]}
    base_mean = statistics.mean(p['control_tasks'] for p in pairs)
    candidate_mean = statistics.mean(p['tasks'] for p in pairs)
    differences = [p['task_difference'] for p in pairs]
    result = dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
        source_commit=source, binary_sha256=binary, inputs={str(p.relative_to(BASE)):hashlib.sha256(p.read_bytes()).hexdigest() for p in paths},
        full_run=True, seeds=list(range(6)), all_valid=True, exact_all_controls=True,
        decision_limit_ms=5000, benchmark_mode='relaxed_development', competition_budget_confirmed=False,
        control_mean=base_mean, candidate_mean=candidate_mean,
        mean_difference=statistics.mean(differences), mean_effect_percent=(candidate_mean/base_mean-1)*100,
        wins=sum(d>0 for d in differences), ties=sum(d==0 for d in differences), losses=sum(d<0 for d in differences),
        holdout_mean_difference=statistics.mean(p['task_difference'] for p in pairs if p['seed'] in [1,3,4,5]),
        pairs=pairs, rows=rows, promoted=False,
        scope='Six paired planner seeds using one fixed instance/task stream; no confidence claim over independent maps or task streams. Shared-host5s development, not strict1s certification. Promotion requires separate assessment.')
    out=BASE/'results/remaining-flow-six-seed-v56-shared.json'
    out.write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps({k:result[k] for k in ['control_mean','candidate_mean','mean_difference','mean_effect_percent','wins','losses','holdout_mean_difference']}))

if __name__=='__main__':
    main()
