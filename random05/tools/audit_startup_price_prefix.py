#!/usr/bin/env python3
"""Verify that expiring price runs preserve the constant-price prefix exactly."""
import argparse
import datetime
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
REFERENCE_BATCH = 'random04-priced-admission-split-full-v121'
REFERENCE_CASE = 'trick-random-04-priced-admission-price12'


def read(path):
    return json.loads(path.read_text())


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def trace(batch, case):
    return ROOT / 'runs/random05' / batch / case / case / 'result.json'


def prefix(data, steps):
    fields = {key: [row.split(',')[:steps] for row in data[key]]
              for key in ('actualPaths', 'plannerPaths')}
    for key in ('actualSchedule', 'plannerSchedule'):
        fields[key] = [[entry for entry in row.split(',') if entry and int(entry.split(':')[0]) <= steps]
                       for row in data[key]]
    fields['events'] = [event for event in data['events'] if event[0] <= steps]
    fields['tasks'] = [task for task in data['tasks'] if task[1] <= steps]
    return fields


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--batch', required=True)
    args = parser.parse_args()
    base = ROOT / 'random05/results' / REFERENCE_BATCH
    reference_audit = next(row for row in read(base / 'audit.json')['rows'] if row['name'] == REFERENCE_CASE)
    assert reference_audit['valid'] and reference_audit['replay']['replay_valid']
    reference_spec = read(base / REFERENCE_CASE / 'spec.json')['cases'][0]
    original_path = trace(REFERENCE_BATCH, REFERENCE_CASE)
    original = read(original_path)
    assert digest(original_path) == reference_audit['replay']['result_sha256']
    destination = ROOT / 'random05/results' / args.batch
    audit = read(destination / 'audit.json')
    assert audit['complete'], 'finish the full independent replay audit first'
    rows = []
    for candidate in audit['rows']:
        if not candidate['valid']:
            continue
        spec = read(destination / candidate['name'] / 'spec.json')['cases'][0]
        env = dict(spec['env'])
        duration = int(env.pop('R05_ADMISSION_PRICE_STEPS', 0))
        if duration <= 0 or env != reference_spec['env']:
            continue  # Other guidance/dispersion profiles need their own reference.
        assert spec['input_hashes'] == reference_spec['input_hashes']
        path = trace(args.batch, candidate['name'])
        current = read(path)
        assert digest(path) == candidate['replay']['result_sha256']
        left, right = prefix(original, duration), prefix(current, duration)
        equal = {key: left[key] == right[key] for key in left}
        assert all(equal.values()), (candidate['name'], equal)
        first_difference = min((next((step for step, (a, b) in enumerate(zip(x.split(','), y.split(',')), 1)
                                      if a != b), 1001)
                                for x, y in zip(original['actualPaths'], current['actualPaths'])), default=1001)
        assert first_difference > duration
        rows.append(dict(case=candidate['name'], prefix_steps=duration,
                         result=str(path.relative_to(ROOT)), result_sha256=digest(path),
                         exact_prefix_fields=equal, first_action_difference=first_difference,
                         full_tasks=candidate['tasks']))
        print(candidate['name'], 'exact prefix', duration, 'first action difference', first_difference, flush=True)
    assert rows, 'no audited cases match the frozen constant-price reference'
    output = dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                  reference=str(original_path.relative_to(ROOT)), reference_sha256=digest(original_path),
                  rows=rows, caveat='Only the declared startup prefix is identical. Later state, task reveals and outcomes diverge.')
    (destination / 'startup-prefix-equivalence.json').write_text(json.dumps(output, indent=2) + '\n')


if __name__ == '__main__':
    main()
