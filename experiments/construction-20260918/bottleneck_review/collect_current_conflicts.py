"""Validate the current-reference blocker audit and preserve its observational limits."""
import json
from pathlib import Path
import statistics
import sys

BASE = Path(__file__).resolve().parents[1]
ROOT = BASE.parents[1]
sys.path.insert(0, str(BASE))
from compare_regional import fields
from verify_full import verify, read


def main():
    raw = ROOT / 'runs/cgar-current-conflict-audit-full-v50-20260920'
    out = BASE / 'results/current-conflict-audit-full-v50'
    report = verify(raw, out, 'cb6a666da8aee461d5a34cab01d7b7ccaa76a201')
    reference = {r['seed']: r for r in read(BASE / 'results/pickup-full-regions-six-seed-v44.json')['rows']
                 if r['environment']['CGAR_TEMPORAL_REGIONS'] == '4'}
    categories = ['no_lower_forward', 'unblocked', 'one_movable', 'two_movable', 'many_movable', 'protected_blocker']
    audits = {}
    for row in report['rows']:
        ref = reference[row['seed']]
        assert row['tasks'] == ref['tasks'] and row['trajectory_sha256'] == ref['trajectory_sha256']
        lines = (Path(row['raw_case']) / 'WAREHOUSE.log').read_text().splitlines()
        frames = {
            'best_scalar': [fields(x) for x in lines if x.startswith('[cgar-temporal-forward-audit]')],
            'easiest_scalar': [fields(x) for x in lines if x.startswith('[cgar-temporal-forward-options]') and 'basis=scalar ' in x],
            'easiest_physical': [fields(x) for x in lines if x.startswith('[cgar-temporal-forward-options]') and 'basis=physical ' in x]}
        for name, rows in frames.items():
            assert [r['step'] for r in rows] == list(range(67, 5001, 67)), (row['case'], name)
            for r in rows:
                assert sum(r[k] for k in categories) == r['stationary']
                assert all(r[k] >= 0 for k in categories)
                if name != 'best_scalar':
                    assert r['distance_scale'] == 50 and r['unit_cost'] == 4
        assert all(a['stationary'] == b['stationary'] == c['stationary'] for a, b, c in
                   zip(frames['best_scalar'], frames['easiest_scalar'], frames['easiest_physical']))
        samples = {}
        for name, rows in frames.items():
            samples[name] = {}
            for label, start in [('all', 0), ('after1000', 1000), ('final1000', 4000)]:
                selected = [r for r in rows if r['step'] > start]
                samples[name][label] = {'frames': len(selected),
                    'mean_per_sample': {k: statistics.mean(r[k] for r in selected) for k in ['stationary'] + categories}}
        previous = (Path(ref['raw_case']) / 'WAREHOUSE.log').read_text().splitlines()
        for tag in ['cgar-temporal', 'cgar-temporal-starts', 'cgar-temporal-rotation',
                    'cgar-temporal-regions', 'cgar-temporal-region-round', 'cgar-orientation']:
            prefix = '[' + tag + ']'
            a = [fields(x) for x in lines if x.startswith(prefix)]
            b = [fields(x) for x in previous if x.startswith(prefix)]
            assert a == b and len(a) == (50 if tag == 'cgar-temporal-region-round' else 25), tag
        audits[row['case']] = {'frames': frames, 'windows': samples}
    report.update(audits=audits, all_reference_trajectories_and_sampled_search_counters_exact=True,
        scope='Current four-region reference; completed post-repair reservation snapshots, not failed-search traces.',
        limitations=['Stationary includes useful turns.', 'Movable blockers need not have a feasible displacement.',
                     'Physical gain is heuristic gain, not guaranteed future travel saving.',
                     'This audit does not classify region-boundary crossings or establish causal throughput loss.'],
        throughput_goal_complete=False)
    (out / 'audit.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps({case: data['windows']['easiest_physical']['final1000'] for case, data in audits.items()}, indent=2))


if __name__ == '__main__':
    main()
