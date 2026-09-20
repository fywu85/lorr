#!/usr/bin/env python3
"""Exercise only pre-initialization CLI rejection paths; performs no simulation."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[4]


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--binary', type=Path, required=True)
    p.add_argument('--output', type=Path, required=True)
    a = p.parse_args()
    binary = a.binary.resolve()
    env = {k: v for k, v in os.environ.items() if not k.startswith('CGAR_')}
    warehouse = ROOT / 'mr24/warehouse.domain/WAREHOUSE.json'
    rows = []
    with tempfile.TemporaryDirectory(prefix='lorr-trick-cli-') as temp:
        folder = Path(temp)
        # Distinct wrong geometry and same-size one-obstacle mutation.
        (folder / 'tiny.map').write_text('type octile\nheight 2\nwidth 2\nmap\n..\n..\n')
        original = (ROOT / 'mr24/warehouse.domain/maps/warehouse_large.map').read_text().splitlines()
        original[4] = ('.' if original[4][0] == '@' else '@') + original[4][1:]
        (folder / 'mutated.map').write_text('\n'.join(original) + '\n')
        for name in ['tiny', 'mutated']:
            (folder / (name + '.json')).write_text(json.dumps({'mapFile': name + '.map'}))
        cases = [
            ('unknown_name', warehouse, 'RANDOM-01', {}, 'unknown --trick instance'),
            ('empty_name', warehouse, '', {}, 'unknown --trick instance'),
            ('wrong_dimensions', folder / 'tiny.json', 'WAREHOUSE', {}, 'requires the exact MR24 warehouse'),
            ('changed_obstacle', folder / 'mutated.json', 'WAREHOUSE', {}, 'layout mismatch'),
            ('wrong_backend', warehouse, 'WAREHOUSE', {'CGAR_PLANNER': 'default'}, 'native CGAR backend'),
        ]
        for label, instance, trick, overrides, message in cases:
            command = [str(binary), '-i', str(instance), '-o', str(folder / 'unexpected.json'), '--trick', trick]
            result = subprocess.run(command, cwd=str(ROOT), env=dict(env, **overrides),
                                    stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, timeout=10)
            assert result.returncode == 2 and message in result.stdout, (label, result.returncode, result.stdout)
            assert not (folder / 'unexpected.json').exists()
            assert '[CGAR_TRICK]' not in result.stdout
            rows.append({'case': label, 'returncode': result.returncode, 'message': result.stdout.strip(),
                         'preinitialization_rejection': True, 'no_result_written': True})
    report = {'binary_sha256': hashlib.sha256(binary.read_bytes()).hexdigest(), 'cases': rows,
              'scope': 'Negative CLI checks only; positive and no-flag behavior require the GRID simulations.'}
    a.output.parent.mkdir(parents=True, exist_ok=True)
    a.output.write_text(json.dumps(report, indent=2) + '\n')
    print('TRICK_CLI_REJECTIONS_PASS cases=' + str(len(rows)))


if __name__ == '__main__':
    main()
