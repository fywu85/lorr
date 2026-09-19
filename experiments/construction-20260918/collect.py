#!/usr/bin/env python3
"""Retain compact provenance and construction/motion measurements."""
import argparse
import hashlib
import json
from pathlib import Path
import shutil
import statistics


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def distribution(values):
    values = sorted(values)
    if not values:
        return None
    return {'min': values[0], 'median': statistics.median(values),
            'mean': statistics.mean(values), 'p95': values[min(len(values) - 1, int(len(values) * .95))], 'max': values[-1]}


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--input', required=True, type=Path)
    p.add_argument('--output', required=True, type=Path)
    p.add_argument('--allow-incomplete', action='store_true')
    a = p.parse_args(); source = a.input.resolve(); out = a.output.resolve(); out.mkdir(parents=True, exist_ok=True)
    spec = json.loads((source / 'spec.json').read_text()); rows = []; pending = []
    for case in spec['cases']:
        work = source / case['name']
        if not (work / 'completion.json').exists():
            pending.append(case['name']); continue
        result = json.loads((work / 'result.json').read_text()) if (work / 'result.json').exists() else {}
        usage = json.loads((work / 'usage.json').read_text())
        workers = [worker for decision in result.get('construction', []) for worker in decision['workers']]
        row = {'case': case['name'], 'profile': case['profile'], 'snapshot_sha256': case['sha256'],
               'valid': result.get('valid', False), 'error': result.get('error'), 'resource_usage': usage,
               'source_timestep': result.get('source_timestep'), 'initial_goals': result.get('initial_goals'),
               'goals_reached': result.get('reached_curve', [None])[-1], 'checkpoints': result.get('checkpoints'),
               'reached_checkpoints': {str(i + 1): v for i, v in enumerate(result.get('reached_curve', [])) if (i + 1) % 25 == 0},
               'decision_seconds': distribution(result.get('decision_seconds', [])),
               'preprocess_seconds': result.get('preprocess_seconds'), 'plan_affinity': result.get('plan_affinity'),
               'late_decisions': result.get('late_decisions', []), 'actions_pending': result.get('actions_pending'),
               'worker_constructions': len(workers), 'construction_complete': sum(w['construction_complete'] for w in workers),
               'construction_clock_cut': sum(w['clock_cut'] for w in workers),
               'max_disabled_pending': max(result.get('reference_disabled_pending', []) or [0]),
               'per_worker': {k: distribution([w[k] for w in workers]) for k in
                              ['order_visited', 'roots', 'roots_accepted', 'recursive_calls', 'candidates', 'max_depth',
                               'construction_seconds', 'construction_score', 'final_score', 'search_steps']},
               'prepare_seconds': distribution([d['prepare_seconds'] for d in result.get('construction', [])]),
               'worker_affinities': sorted({tuple(w['affinity']) for w in workers}),
               'helper_counts': sorted({w['helpers'] for w in workers}),
               'raw_result_sha256': sha(work / 'result.json') if (work / 'result.json').exists() else None,
               'first_decision': result.get('construction', [None])[0]}
        rows.append(row)
        dest = out / case['name']; dest.mkdir(exist_ok=True)
        for filename in ['allocation.json', 'usage.json', 'command.json', 'completion.json']:
            shutil.copy2(work / filename, dest / filename)
    build = Path(spec['build_dir'])
    for filename in ['build.json', 'requested.json', 'submitted-scripts.json', 'allocation.json', 'nms-instrumentation.patch']:
        if (build / filename).exists(): shutil.copy2(build / filename, out / ('build-' + filename))
    shutil.copy2(source / 'spec.json', out / 'spec.json')
    result = {'complete': not pending, 'pending': pending, 'rows': rows}
    (out / 'summary.json').write_text(json.dumps(result, indent=2) + '\n')
    print('case | reached | decision mean/max ms | complete constructions | RSS GiB | valid')
    for r in rows:
        d = r['decision_seconds'] or {'mean': 0, 'max': 0}
        print(f"{r['case']} | {r['goals_reached']} | {d['mean']*1000:.1f}/{d['max']*1000:.1f} | "
              f"{r['construction_complete']}/{r['worker_constructions']} | {r['resource_usage']['peak_rss_kib']/2**20:.2f} | {r['valid']}")
    print('pending:', len(pending))
    if pending and not a.allow_incomplete:
        raise SystemExit(1)


if __name__ == '__main__':
    main()
