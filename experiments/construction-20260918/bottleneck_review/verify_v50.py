"""Recheck the full V50 validation and every sampled search-counter frame."""
import json
from pathlib import Path
import sys

BASE = Path(__file__).resolve().parents[1]
ROOT = BASE.parents[1]
sys.path.insert(0, str(BASE))
from compare_regional import fields
from verify_full import verify, read


def main():
    raw = ROOT / 'runs/cgar-deadline-duplicate-full-v50-20260919'
    out = BASE / 'results/deadline-duplicate-full-v50'
    report = verify(raw, out, 'cb6a666da8aee461d5a34cab01d7b7ccaa76a201')
    references = {r['seed']: r for r in read(BASE / 'results/pickup-full-regions-six-seed-v44.json')['rows']
                  if r['environment']['CGAR_TEMPORAL_REGIONS'] == '4'}
    tags = ['cgar-temporal', 'cgar-temporal-starts', 'cgar-temporal-rotation',
            'cgar-temporal-regions', 'cgar-temporal-region-round', 'cgar-orientation']
    checked = {}
    for row in report['rows']:
        ref = references[row['seed']]
        assert row['tasks'] == ref['tasks']
        assert row['trajectory_sha256'] == ref['trajectory_sha256']
        actual = (Path(row['raw_case']) / 'WAREHOUSE.log').read_text().splitlines()
        previous = (Path(ref['raw_case']) / 'WAREHOUSE.log').read_text().splitlines()
        counts = {}
        for tag in tags:
            prefix = '[' + tag + ']'
            a = [fields(line) for line in actual if line.startswith(prefix)]
            b = [fields(line) for line in previous if line.startswith(prefix)]
            expected = 50 if tag == 'cgar-temporal-region-round' else 25
            assert len(a) == len(b) == expected, (row['seed'], tag, len(a), len(b))
            assert a == b, (row['seed'], tag)
            counts[tag] = len(a)
        checked[row['case']] = counts
    report.update(
        exact_reference_trajectory_seeds=sorted(r['seed'] for r in report['rows']),
        all_sampled_non_timing_search_counters_identical=True,
        counter_frames_checked=checked,
        throughput_claim='Existing reference trajectories reproduced exactly; no throughput improvement.',
        timing_limit='Different full-run allocations; not a controlled speedup estimate.',
        throughput_goal_complete=False)
    (out / 'verification.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps({k: report[k] for k in ['full_cases', 'complete_entry_samples',
        'exact_reference_trajectory_seeds', 'all_sampled_non_timing_search_counters_identical']}, indent=2))


if __name__ == '__main__':
    main()
