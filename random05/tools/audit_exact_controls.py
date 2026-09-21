#!/usr/bin/env python3
"""Compare six complete trace fields for explicitly declared, audited controls."""
import argparse
import datetime
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
FIELDS = ('actualPaths', 'plannerPaths', 'actualSchedule', 'plannerSchedule', 'events', 'tasks')


def read(path):
    return json.loads(path.read_text())


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--batch', required=True)
    parser.add_argument('--references', required=True, type=Path,
                        help='JSON mapping audited candidate names to reference result paths')
    args = parser.parse_args()
    archive = ROOT / 'random05/results' / args.batch
    audit = read(archive / 'audit.json')
    assert audit['complete'], 'finish the full independent audit first'
    rows = {r['name']: r for r in audit['rows']}
    requested = read(args.references)
    assert requested, 'no declared comparisons'
    checks = []
    for name, reference in requested.items():
        row = rows[name]
        assert row['valid'] and row['replay']['replay_valid'], name
        candidate = Path(row['replay']['result'])
        reference = ROOT / reference
        assert candidate.resolve() != reference.resolve(), 'self-comparison is not evidence'
        actual, expected = read(candidate), read(reference)
        equal = {key: actual[key] == expected[key] for key in FIELDS}
        assert all(equal.values()), (name, equal)
        checks.append(dict(candidate=str(candidate.relative_to(ROOT)),
                           reference=str(reference.relative_to(ROOT)), tasks=row['tasks'],
                           equal=equal, result_sha256=digest(candidate), reference_sha256=digest(reference)))
        print(name, row['tasks'], 'all six fields exact', flush=True)
    result = dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                  references=str(args.references), checks=checks)
    (archive / 'control-equivalence.json').write_text(json.dumps(result, indent=2) + '\n')


if __name__ == '__main__':
    main()
