#!/usr/bin/env python3
"""Audit full RANDOM density experiments, including action replay and order latency."""
import argparse
import datetime
import hashlib
import json
from pathlib import Path
import shutil
from audit_generalization import ROOT, source_check, sha
from action_audit import audit as replay
from audit_task_waits import audit as waiting


def read(path):
    return json.loads(path.read_text())


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--batch', required=True)
    parser.add_argument('--build')
    parser.add_argument('--source')
    parser.add_argument('--source-map', type=Path, help='JSON mapping binary SHA256 to build/source for a mixed-source batch')
    parser.add_argument('--allow-pending', action='store_true')
    args = parser.parse_args()
    if args.source_map:
        if args.build or args.source:
            parser.error('Use --source-map or --build/--source, not both')
        sources = {}
        for digest, declaration in read(args.source_map).items():
            checked = source_check(declaration['build'], declaration['source'])
            assert checked['binary_sha256'] == digest, 'declared build digest mismatch'
            sources[digest] = checked
        assert sources, 'empty source map'
    else:
        if not args.build or not args.source:
            parser.error('--build and --source are required without --source-map')
        checked = source_check(args.build, args.source)
        sources = {checked['binary_sha256']: checked}
    directory = ROOT / 'runs/random05' / args.batch
    archived = ROOT / 'random05/results' / args.batch
    frontiers = read(ROOT / 'random05/random-frontiers.json')
    for path in list(directory.glob('*.json')) + list(directory.glob('*/*.json')):
        if path.name in ('summary.json','completion.json','spec.json','allocation.json','submission.json','batch.json'):
            target = archived / path.relative_to(directory)
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(str(path), str(target))
    rows, pending = [], []
    for job in read(directory / 'batch.json')['jobs']:
        case_dir = Path(job['output'])
        if not (case_dir / 'summary.json').exists():
            pending.append(job['name'])
            continue
        case = read(case_dir / 'spec.json')['cases'][0]
        assert case['binary_sha256'] in sources, 'undeclared case binary'
        source = sources[case['binary_sha256']]
        summary = read(case_dir / 'summary.json')[0]
        instance = Path(case['input']).stem
        reference = frontiers[instance]['general']['case']
        work = case_dir / case['name']
        assert summary == read(work / 'summary.json')
        assert case['cores'] == 16 and case['smt'] == 2 and int(case['env']['R05_THREADS']) == 32
        assert case['steps'] == reference['steps']
        assert case['limit_ms'] == 1000 and case['preprocess_ms'] == 30000
        allocation = read(case_dir / 'allocation.json')['resources']
        assert allocation['cpu_model'] == 'AMD EPYC 9354 32-Core Processor'
        assert allocation['physical_cores_visible'] == 16 and len(allocation['logical_cpu_affinity']) == 32
        assert allocation['effective_cpu_quota'] is None and len(read(work / 'case.json')['cpus']) == 32
        assert sha(work / 'lifelong') == case['binary_sha256'] == summary['binary_sha256'] == source['binary_sha256']
        for filename, digest in case['input_hashes'].items():
            assert sha(Path(filename)) == digest, filename
        for filename, digest in reference['input_hashes'].items():
            if '/assets/' not in filename:
                assert case['input_hashes'][filename] == digest, filename
        if case.get('trick'):
            assert case['trick'] == instance or (case['trick'] is True and instance == 'RANDOM-05')
            profile = 'trick'
        else:
            assert case['env']['R05_GUIDANCE'] == 'none' and int(case['env']['R05_HORIZON']) == 0
            profile = 'general'
        row = dict(instance=instance, profile=profile, name=case['name'], valid=summary['valid'],
                   source_commit=source['commit'], finished_utc=summary['finished_utc'], exit=summary['exit'],
                   evidence=str((archived / case['name'] / 'summary.json').relative_to(ROOT)),
                   peak_rss_bytes=summary['usage']['peak_rss_kib']*1024,
                   latency_seconds=summary.get('latency_seconds'))
        assert row['peak_rss_bytes'] <= 32000000000
        if summary['valid']:
            assert summary['exit'] == 0 and summary['result']['makespan'] == case['steps']
            assert summary['result']['entryComputeSamples'] == case['steps']
            assert all(summary['result'][k] == 0 for k in ('numPlannerErrors','numScheduleErrors','numEntryTimeouts'))
            assert summary['latency_seconds']['max'] <= 1
            map_path = next(Path(p) for p in case['input_hashes'] if p.endswith('.map'))
            result = replay(work / 'result.json', map_path)
            result.pop('completed_loaded_work')
            waits, data = waiting(work / 'result.json')
            assert hashlib.sha256(json.dumps(data['actualPaths'], separators=(',', ':')).encode()).hexdigest() == summary['trajectory_sha256']
            assert result['tasks'] == summary['result']['numTaskFinished'] == waits['tasks_finished']
            assert result['steps'] == waits['horizon_steps'] == case['steps']
            row.update(tasks=result['tasks'], replay=result, waiting=waits)
        else:
            row.update(result=summary.get('result'), note='Original unsuccessful attempt retained; not a frontier candidate.')
        rows.append(row)
        print(instance, case['name'], row.get('tasks', 'failed'), 'audited', flush=True)
    report = dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                  source=next(iter(sources.values())) if len(sources)==1 else None,
                  sources_by_binary=sources, rows=rows, pending=pending, complete=not pending,
                  caveat='Development comparisons on one archived layout across densities; no independent task/start or geometry validation.')
    (archived / 'audit.json').write_text(json.dumps(report, indent=2)+'\n')
    assert args.allow_pending or not pending, pending


if __name__ == '__main__':
    main()
