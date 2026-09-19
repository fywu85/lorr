#!/usr/bin/env python3
"""Validate travel accounting, then decompose completed reference trajectories."""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools'))
from cpu_resources import cpu_resources


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--output', type=Path, required=True)
    a = p.parse_args()
    out = a.output.resolve(); out.mkdir(parents=True, exist_ok=False)
    source = Path(__file__).with_suffix('.cpp')
    (out / source.name).write_bytes(source.read_bytes())
    binary = out / 'decompose'
    command = [str(ROOT / 'env/bin/x86_64-conda-linux-gnu-c++'), '-std=c++17', '-O2', '-I' + str(ROOT / 'cgar/inc'),
               str(out / source.name), '-Wl,-rpath,' + str(ROOT / 'env/lib'), '-o', str(binary)]
    subprocess.run(command, check=True)
    fixture = {'teamSize': 2, 'makespan': 5, 'numTaskFinished': 1,
               'numPlannerErrors': 0, 'numScheduleErrors': 0, 'numEntryTimeouts': 0,
               'start': [[0, 0, 'E'], [0, 4, 'W']],
               'actualPaths': ['F,F,F,W,W', 'W,W,W,W,W'],
               'actualSchedule': ['0:-1,2:0,4:-1', '1:0,2:-1'],
               'tasks': [[0, 0, [0, 2, 0, 3]]], 'events': [[2, 0, 0, 1], [3, 0, 0, 2]]}
    (out / 'fixture.map').write_text('type octile\nheight 2\nwidth 5\nmap\n..EE.\n.....\n')
    (out / 'fixture.json').write_text(json.dumps(fixture))
    subprocess.run([str(binary), str(out / 'fixture.map'), str(out / 'fixture.json'), str(out / 'fixture-result.json')], check=True)
    result = json.loads((out / 'fixture-result.json').read_text())
    expected = {'completed_chain_shortest': 1, 'first_assignment_pickup_shortest': 2,
                'last_assignment_pickup_shortest': 1, 'observed_first_assignment_pickup_delay': 2,
                'observed_last_assignment_pickup_delay': 1, 'completed_loaded_steps': 1}
    for key, value in expected.items():
        assert result[key]['n'] == 1 and result[key]['mean'] == value, (key, result[key])
    assert result['tasks_with_multiple_assignments'] == 1 and result['assignments_after_pickup'] == 0
    assert result['unassigned_robot_steps'] == 7 and result['empty_robot_steps_including_reassignments'] == 2
    assert result['empty_forward_actions'] == 1 and result['empty_wait_actions'] == 1
    assert result['loaded_robot_steps_including_unfinished_tasks'] == 1
    cases = {
        'cgar_cache': 'runs/cgar-temporal-full-v1-20260918/orientation_8192-s0-r0/WAREHOUSE.json',
        'cgar_equal_50000': 'runs/cgar-temporal-validation-v5-20260918/equal_50000-s0-r0/WAREHOUSE.json',
        'kittyknight': 'runs/warehouse-leaders-full-20260918/kk-WAREHOUSE/result.json',
        'nms_diagnostic': 'runs/warehouse-leaders-full-20260918/nms-WAREHOUSE/result.json',
    }
    inputs = {}
    for name, path in cases.items():
        target = ROOT / path
        digest = hashlib.sha256()
        with target.open('rb') as stream:
            for chunk in iter(lambda: stream.read(1024 * 1024), b''):
                digest.update(chunk)
        inputs[name] = {'path': path, 'sha256': digest.hexdigest()}
        subprocess.run([str(binary), str(ROOT / 'mr24/warehouse.domain/maps/warehouse_large.map'),
                        str(target), str(out / (name + '.json'))], check=True)
    (out / 'provenance.json').write_text(json.dumps({
        'source_sha256': hashlib.sha256(source.read_bytes()).hexdigest(),
        'binary_sha256': hashlib.sha256(binary.read_bytes()).hexdigest(),
        'command': command, 'fixture_passed': True, 'allocation': cpu_resources(), 'inputs': inputs}, indent=2) + '\n')


if __name__ == '__main__':
    main()
