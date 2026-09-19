#!/usr/bin/env python3
"""Preserve deadline-screen evidence and exact trajectory fingerprints."""
import argparse
import hashlib
import json
from pathlib import Path
import shutil


def write(path, value):
    path.write_text(json.dumps(value, indent=2) + '\n')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--input', required=True, type=Path)
    parser.add_argument('--output', required=True, type=Path)
    args = parser.parse_args()
    raw, out = args.input.resolve(), args.output.resolve()
    spec = json.loads((raw / 'spec.json').read_text())
    assert isinstance(spec['horizons'], dict) and set(spec['horizons']) == {'WAREHOUSE'}
    horizon = spec['horizons']['WAREHOUSE']
    assert horizon in (50, 200), horizon
    assert (raw / 'completion.json').exists(), 'screen is still running'
    out.mkdir(parents=True, exist_ok=False)
    for name in ['spec.json', 'allocation.json', 'completion.json', 'submission.json']:
        shutil.copy2(raw / name, out / name)
    summaries, fingerprints = {}, {}
    for case in spec['cases']:
        name = case['name']
        source = raw / name
        rows = json.loads((source / 'summary.json').read_text())
        assert len(rows) == 1 and rows[0]['instance'] == 'WAREHOUSE'
        summaries[name] = rows
        dest = out / name
        dest.mkdir()
        shutil.copy2(source / 'metadata.json', dest / 'metadata.json')
        row = rows[0]
        diagnostics = [line for log in sorted(source.glob('*.log')) for line in log.read_text(errors='replace').splitlines()
                       if line.startswith(('[cgar-temporal', '[cgar-orientation]', '[cgar-flow]'))]
        write(dest / 'diagnostics.json', diagnostics)
        if row['valid']:
            assert row['makespan'] == row['entry_compute_samples'] == horizon
            assert row['entry_compute_max_seconds'] <= spec['time_limit_ms'] / 1000
            assert all(row[key] == 0 for key in ['planner_errors', 'schedule_errors', 'timeouts', 'internal_timeouts', 'exit'])
            data = json.loads((source / 'WAREHOUSE.json').read_text())
            digest = hashlib.sha256()
            encoder = json.JSONEncoder(separators=(',', ':'))
            for key in ['actualPaths', 'actualSchedule', 'events', 'tasks']:
                digest.update(key.encode())
                for chunk in encoder.iterencode(data[key]):
                    digest.update(chunk.encode())
            fingerprints[name] = digest.hexdigest()
        else:
            fingerprints[name] = None
            (dest / 'failure.txt').write_text('\n'.join(
                p.name + '\n' + p.read_text(errors='replace') for p in sorted(source.glob('*.log'))))
    write(out / 'purpose.json', {'purpose': 'Cold-start deadline screen only; prefix throughput is not a performance ranking.',
                               'horizon_steps': horizon, 'full_run_validation_required': True})
    write(out / 'run-summaries.json', summaries)
    write(out / 'trajectory-fingerprints.json', fingerprints)
    print(out, 'cases', len(summaries), 'valid', sum(bool(row[0]['valid']) for row in summaries.values()))


if __name__ == '__main__':
    main()
