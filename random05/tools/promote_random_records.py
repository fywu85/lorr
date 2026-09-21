#!/usr/bin/env python3
"""Promote independently audited RANDOM01-04 records into the density ledger."""
import argparse
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('audits', nargs='+', type=Path)
    args = parser.parse_args()
    index_path = ROOT / 'random05/random-frontiers.json'
    ledger_path = ROOT / 'random05/RANDOM_PROGRESS.md'
    index = json.loads(index_path.read_text())
    text = ledger_path.read_text()
    candidates = []
    for path in args.audits:
        audit = json.loads(path.read_text())
        for row in audit['rows']:
            if row['valid']:
                candidates.append((row, path))
    records = []
    for row, audit_path in sorted(candidates, key=lambda pair:pair[0]['finished_utc']):
        instance, profile = row['instance'], row['profile']
        if instance == 'RANDOM-05':
            if row['tasks'] > index[instance][profile]['tasks']:
                print('RANDOM-05 requires the main frontier/waiting promotion:', row['tasks'], row['evidence'])
            continue
        if row['tasks'] <= index[instance][profile]['tasks']:
            continue
        summary_path = ROOT / row['evidence']
        summary = json.loads(summary_path.read_text())[0]
        case = json.loads((summary_path.parent / 'spec.json').read_text())['cases'][0]
        assert summary['valid'] and summary['result']['numTaskFinished'] == row['tasks']
        assert row['replay']['replay_valid'] and row['waiting']['event_accounting_valid']
        assert summary['finished_utc'] == row['finished_utc']
        assert row['latency_seconds']['max'] <= 1 and row['peak_rss_bytes'] <= 32000000000
        assert summary['result']['makespan'] == case['steps']
        index[instance][profile] = dict(source_commit=row['source_commit'], evidence=row['evidence'],
                                       case=case, tasks=row['tasks'], finished_utc=row['finished_utc'],
                                       audit=str(audit_path.resolve().relative_to(ROOT)))
        commit = row['source_commit']
        records.append('| {} | {} | {} | {} | [{}](https://github.com/fywu85/lorr/commit/{}) | [Full run]({}) |'.format(
            row['finished_utc'], instance, profile, row['tasks'], commit, commit,
            row['evidence'].replace('random05/', '', 1)))
        print('Promoted', instance, profile, row['tasks'])
    if records:
        start = text.index('## Record evidence')
        end = text.index('\n## ', start+1)
        text = text[:end].rstrip()+'\n'+'\n'.join(records)+'\n\n'+text[end:]
    for i, robots, steps in [(1,100,600),(2,200,600),(3,400,800),(4,700,1000),(5,800,2000)]:
        instance = 'RANDOM-0'+str(i)
        prefix = '| {} | {} | {} | '.format(instance, robots, steps)
        old = next(line for line in text.splitlines() if line.startswith(prefix))
        row = index[instance]
        new = prefix+'{} | {} | {} |'.format(row['general']['tasks'], row['trick']['tasks'], row['nms32_tasks'])
        text = text.replace(old, new, 1)
    index_path.write_text(json.dumps(index, indent=2)+'\n')
    ledger_path.write_text(text)
    if (ROOT / 'PILOT_PROGRESS.md').exists():
        from render_pilot_progress import render
        render()


if __name__ == '__main__':
    main()
