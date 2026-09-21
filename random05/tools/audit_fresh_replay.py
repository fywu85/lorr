#!/usr/bin/env python3
"""Independently replay frozen fresh-input cases and report order latency.

Use audit_fresh.py separately for source, allocation, deadline and protocol checks.
This report preserves unsuccessful attempts and explicitly labels pending cases.
"""
import argparse
import datetime
import hashlib
import json
from pathlib import Path
import subprocess
from action_audit import audit as replay
from audit_task_waits import audit as waiting

ROOT = Path(__file__).resolve().parents[2]


def read(path):
    return json.loads(path.read_text())


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--batch', type=Path, required=True)
    parser.add_argument('--protocol-commit', required=True)
    parser.add_argument('--protocol-json', required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--allow-pending', action='store_true')
    args = parser.parse_args()
    def frozen(path):
        return json.loads(subprocess.check_output(['git', 'show', args.protocol_commit+':'+path], cwd=ROOT))
    protocol = frozen(args.protocol_json)
    manifest = frozen(protocol['manifest'])
    runs, pending = {}, []
    for expected in manifest:
        name = expected['name']
        directory = args.batch / name
        summary_path = directory / 'summary.json'
        if not summary_path.exists():
            pending.append(name)
            continue
        case = read(directory / 'spec.json')['cases'][0]
        for key, value in expected.items():
            assert case[key] == value, (name, 'changed frozen case', key)
        summaries = read(summary_path)
        assert len(summaries) == 1
        summary = summaries[0]
        row = dict(valid=summary['valid'], exit=summary['exit'], finished_utc=summary['finished_utc'])
        if summary['valid']:
            work = directory / name
            assert read(work / 'summary.json') == summary
            map_path = next(Path(p) for p in case['input_hashes'] if p.endswith('.map'))
            result = replay(work / 'result.json', map_path)
            result.pop('completed_loaded_work')
            waits, data = waiting(work / 'result.json')
            trajectory = hashlib.sha256(json.dumps(data['actualPaths'], separators=(',', ':')).encode()).hexdigest()
            assert trajectory == summary['trajectory_sha256']
            assert result['tasks'] == waits['tasks_finished'] == summary['result']['numTaskFinished']
            assert result['steps'] == waits['horizon_steps'] == case['steps']
            assert result['result_sha256'] == waits['result_sha256']
            row.update(tasks=result['tasks'], replay=result, waiting=waits)
        else:
            row.update(result=summary.get('result'), note='Original failed attempt retained; not a successful validation.')
        runs[name] = row
        print(name, row.get('tasks', 'failed'), 'replayed', flush=True)
    result = dict(created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                  protocol_commit=args.protocol_commit, protocol_json=args.protocol_json,
                  complete=not pending, all_attempts_valid=not pending and all(r['valid'] for r in runs.values()),
                  runs=runs, pending=pending,
                  scope='Independent full movement, collision, scheduling, task-event and order-latency checks; source/resource/protocol checks are in audit_fresh.py.')
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2)+'\n')
    assert args.allow_pending or not pending, pending


if __name__ == '__main__':
    main()
