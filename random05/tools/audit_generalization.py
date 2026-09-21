#!/usr/bin/env python3
"""Audit the frozen RANDOM density transfer and preserve large-map limits."""
import argparse
import datetime
import hashlib
import json
from pathlib import Path
from result_horizon import summary_steps
import shutil
import subprocess
from action_audit import audit as replay

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / 'random05/results/generalization-20260921'
NMS_SHA = '2dfe6f35b3ae532d075aa487de88ec8d436a07ddef97e4dec24e94b550820539'
FIELDS = ('actualPaths', 'plannerPaths', 'actualSchedule', 'plannerSchedule', 'events', 'tasks')


def read(path):
    return json.loads(path.read_text())


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def source_check(build_name, commit):
    build = ROOT / 'runs/random05' / build_name
    spec, done = read(build / 'spec.json'), read(build / 'completion.json')
    assert done['exit'] == 0
    names = [n for n in spec['source_hashes'] if n == 'CMakeLists.txt' or
             ((n.startswith('src/') or n.startswith('simulator/')) and Path(n).suffix in ('.cpp', '.h', '.hpp'))]
    for name in names:
        content = subprocess.check_output(['git', 'show', commit + ':random05/' + name], cwd=ROOT)
        assert hashlib.sha256(content).hexdigest() == spec['source_hashes'][name], name
    return dict(commit=commit, binary_sha256=done['binary_sha256'], checked_files=names)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--trick-source', required=True)
    parser.add_argument('--allow-pending', action='store_true')
    args = parser.parse_args()
    sources = dict(generic=source_check('build-v69', '233f5bf'),
                   frozen_trick=source_check('build-v78', args.trick_source))
    batches = [('generic', 'generalization-random-generic-split-full-v69'),
               ('frozen_trick', 'generalization-random-frozen-trick-split-full-v78'),
               ('nms', 'generalization-nms-split-full')]
    inventory = {r['instance']:r for r in read(OUT / 'capacity-inventory.json')['instances']}
    runs, pending, paired_inputs = [], [], {}
    for profile, batch in batches:
        directory = ROOT / 'runs/random05' / batch
        archived = ROOT / 'random05/results' / batch
        for path in list(directory.glob('*.json')) + list(directory.glob('*/*.json')):
            if path.name not in ('summary.json','completion.json','spec.json','allocation.json','submission.json','batch.json'):
                continue
            target = archived / path.relative_to(directory)
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(str(path), str(target))
        for job in read(directory / 'batch.json')['jobs']:
            case_dir = Path(job['output'])
            if not (case_dir / 'summary.json').exists():
                pending.append(job['name'])
                continue
            spec = read(case_dir / 'spec.json')
            case = spec['cases'][0]
            summary = read(case_dir / 'summary.json')[0]
            work = case_dir / case['name']
            instance = Path(case['input']).stem
            item = inventory[instance]
            allocation = read(case_dir / 'allocation.json')['resources']
            assert spec == read(archived / case['name'] / 'spec.json')
            assert summary == read(work / 'summary.json')
            assert case['cores'] == 16 and case['smt'] == 2
            assert case['steps'] == item['steps']
            assert case['limit_ms'] == 1000 and case['preprocess_ms'] == 30000
            assert allocation['cpu_model'] == 'AMD EPYC 9354 32-Core Processor'
            assert allocation['physical_cores_visible'] == 16
            assert len(allocation['logical_cpu_affinity']) == 32
            assert allocation['effective_cpu_quota'] is None
            assert len(read(work / 'case.json')['cpus']) == 32
            assert sha(work / 'lifelong') == case['binary_sha256'] == summary['binary_sha256']
            expected = NMS_SHA if profile == 'nms' else sources[profile]['binary_sha256']
            assert case['binary_sha256'] == expected
            for filename, digest in case['input_hashes'].items():
                assert sha(Path(filename)) == digest, filename
            assert case['input_hashes'][case['input']] == item['input_sha256']
            map_path = next(Path(p) for p in case['input_hashes'] if p.endswith('.map'))
            assert sha(map_path) == item['map_sha256']
            free_cells = sum(c not in '@T' for line in map_path.read_text().splitlines()[4:] for c in line)
            assert free_cells == item['free_cells']
            assert (4*free_cells)**2*4 == item['oriented_distance_table_bytes']
            assert item['table_alone_fits_32gb'] == (item['oriented_distance_table_bytes'] <= 32000000000)
            common = {k:v for k,v in case['input_hashes'].items() if '/assets/' not in k}
            if instance in paired_inputs:
                assert paired_inputs[instance] == common, ('unmatched inputs', instance)
            paired_inputs[instance] = common
            if profile == 'generic':
                assert not case.get('trick') and case['env']['R05_GUIDANCE'] == 'none'
                assert int(case['env']['R05_HORIZON']) == 0
            elif profile == 'frozen_trick':
                assert case['trick'] == instance
                assert case['env']['R05_GUIDANCE'] == 'flow'
                assert int(case['env']['R05_HORIZON']) == item['steps']
            row = dict(profile=profile, instance=instance, case=case['name'],
                       source=sources.get(profile), binary_sha256=expected,
                       evidence=str((archived / case['name'] / 'summary.json').relative_to(ROOT)),
                       valid=summary['valid'], exit=summary['exit'],
                       peak_rss_bytes=summary['usage']['peak_rss_kib']*1024,
                       latency_seconds=summary.get('latency_seconds'), usage=summary['usage'])
            assert row['peak_rss_bytes'] <= 32000000000
            if summary['valid']:
                assert summary_steps(summary) == item['steps']
                assert summary['latency_seconds']['max'] <= 1
                data = read(work / 'result.json')
                assert hashlib.sha256(json.dumps(data['actualPaths'], separators=(',', ':')).encode()).hexdigest() == summary['trajectory_sha256']
                map_path = next(Path(p) for p in case['input_hashes'] if p.endswith('.map'))
                result = replay(work / 'result.json', map_path)
                result.pop('completed_loaded_work')
                assert result['tasks'] == summary['result']['numTaskFinished']
                row.update(tasks=result['tasks'], replay=result)
            else:
                log = (work / 'native.log').read_text()
                assert profile == 'nms' and summary['exit'] == 124 and 'Preprocessing timeout' in log
                row.update(failure='30-second preprocessing deadline', native_log_sha256=sha(work / 'native.log'),
                           diagnostic=log.strip())
            runs.append(row)
            print(profile, instance, row.get('tasks', row.get('failure')), 'audited', flush=True)
    controls = {}
    record = ROOT / 'runs/random05/branch-seeds-split-full-v69/32-branch-seed-b18-seed5/32-branch-seed-b18-seed5/result.json'
    check = ROOT / 'runs/random05/generalization-random-frozen-trick-split-full-v78/pipeline-frozen-trick-random-05/pipeline-frozen-trick-random-05/result.json'
    if check.exists():
        a, b = read(record), read(check)
        controls = {f:a[f] == b[f] for f in FIELDS}
        assert all(controls.values()) and b['numTaskFinished'] == 3928
    assert args.allow_pending or not pending, pending
    report = dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                  complete=not pending, pending=pending, sources=sources,
                  frozen_random05_control_equivalence=controls, runs=runs,
                  caveat='One archived input per density, identical map layout; selected RANDOM05 settings, no per-density tuning. Unsupported pipeline large maps are analytical capacity results, not executed failures.')
    (OUT / 'audit.json').write_text(json.dumps(report, indent=2) + '\n')
    print('Audited', len(runs), 'completed attempts;', len(pending), 'pending.')


if __name__ == '__main__':
    main()
