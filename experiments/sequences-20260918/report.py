#!/usr/bin/env python3
"""Regenerate warehouse comparison tables and optional standalone figures."""
import argparse
import collections
import json
from pathlib import Path
import statistics

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
OUT = HERE / 'results'
LIMIT = 32_000_000_000


def read(path):
    return json.loads(path.read_text())


def write(name, value):
    (OUT / name).write_text(json.dumps(value, indent=2) + '\n')


def table(head, rows):
    return '\n'.join(['| ' + ' | '.join(head) + ' |', '|' + '|'.join(['---'] * len(head)) + '|'] +
                     ['| ' + ' | '.join(map(str, row)) + ' |' for row in rows])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--plots', action='store_true', help='Requires matplotlib; numeric report has no third-party dependencies')
    a = parser.parse_args()
    matrices = {}
    for path in sorted(OUT.glob('warehouse-*/metrics.json')):
        label = path.parent.name
        matrices[label] = {name: read(path.parent / (name + '.json'))
                           for name in ['metrics', 'run-summaries', 'spec', 'allocation']}
    records = []
    metrics = {}
    for label, matrix in matrices.items():
        by_case = {r['case']: r for r in matrix['metrics']}
        for case in matrix['spec']['cases']:
            name = case['name']
            rows = matrix['run-summaries'].get(name, [])
            if not rows:
                continue
            result, = rows
            row = by_case.get(name)
            peak = result['process_resources']['peak_rss_kib'] * 1024
            record = dict(matrix=label, case=name, variant=case['variant'], seed=case['seed'],
                          environment=case['environment'], valid=result['valid'], outcome=result['outcome'],
                          tasks=result['after'], makespan=result['makespan'], wall_seconds=result['wall_seconds'],
                          peak_process_bytes=peak, within_32_decimal_GB=peak <= LIMIT,
                          decision_limit_ms=matrix['spec']['time_limit_ms'],
                          cpu_model=matrix['allocation']['resources']['cpu_model'])
            if row:
                record.update(max_wrapper_seconds=row['max_decision_seconds'], trajectory_sha256=row['trajectory_sha256'],
                              completed_per_1000=row['completed_per_1000'])
                metrics[label, name] = row
            records.append(record)
    write('all-full-runs.json', records)
    control_label = 'warehouse-prerotation-full-v12'
    controls = {r['case']: r for r in matrices[control_label]['metrics']}
    old = {}
    for label in ['warehouse-cache-full-v3', 'warehouse-cache-confirmation-v4']:
        for r in matrices[label]['metrics']:
            if r['case'].startswith('orientation_512-'):
                old[int(r['case'].split('-s')[1].split('-')[0])] = r
    paired = []
    for seed in range(6):
        baseline = controls[f'baseline-s{seed}-r0']
        candidate = controls[f'orientation_8192-s{seed}-r0']
        pre = controls[f'pre_rotate-s{seed}-r0']
        paired.append(dict(seed=seed, default=baseline['tasks'], demand_512=old[seed]['tasks'],
                           lru_8192=candidate['tasks'], pre_rotation=pre['tasks'],
                           effect_vs_default_percent=100 * (candidate['tasks'] / baseline['tasks'] - 1),
                           effect_vs_demand_512_percent=100 * (candidate['tasks'] / old[seed]['tasks'] - 1),
                           pre_rotation_effect_percent=100 * (pre['tasks'] / candidate['tasks'] - 1)))
    write('cache-paired.json', paired)
    # Check all repeated default and recommended-policy trajectories, including
    # one- versus two-second decision caps. This compares complete schedules,
    # actions, task events and task definitions, not just completion totals.
    equivalents = []
    for r in records:
        env = r['environment']
        if not r['valid'] or int(env.get('CGAR_SEQUENCE_HORIZON', 0)) or int(env.get('CGAR_PRE_ROTATE', 0)):
            continue
        if int(env.get('CGAR_ORIENTATION_GUIDANCE', 0)) == 0:
            kind = 'baseline'
        elif int(env.get('CGAR_ORIENTATION_GUIDANCE', 0)) == 1 and int(env.get('CGAR_TURN_TABLE_MB', 512)) >= 8192:
            kind = 'orientation_8192'
        else:
            continue
        expected = controls[f'{kind}-s{r["seed"]}-r0']['trajectory_sha256']
        match = r['trajectory_sha256'] == expected
        equivalents.append(dict(matrix=r['matrix'], case=r['case'], reference=f'{control_label}/{kind}-s{r["seed"]}-r0',
                                identical=match, decision_limit_ms=r['decision_limit_ms']))
        assert match, equivalents[-1]
    previous = read(ROOT / 'experiments/motion-20260918/results/admission/metrics.json')
    for r in previous:
        if r['instance'] != 'WAREHOUSE' or not r['case'].startswith('baseline-'):
            continue
        match = r['trajectory_sha256'] == controls[r['case']]['trajectory_sha256']
        equivalents.append(dict(matrix='prior-motion-admission', case=r['case'], identical=match))
        assert match
    screen = []
    first = {r['case'] + '/' + r['instance']: r for r in read(OUT / 'screen-v1/metrics.json')}
    for r in read(OUT / 'screen-v2/metrics.json'):
        key = r['case'] + '/' + r['instance']
        same = r['trajectory_sha256'] == first[key]['trajectory_sha256']
        screen.append(dict(case=r['case'], instance=r['instance'], identical=same))
        assert same
    write('trajectory-equivalence.json', dict(full_runs=equivalents, historical_screen_memoization=screen))
    resources = []
    groups = collections.defaultdict(list)
    for r in records:
        groups[r['matrix'], r['variant']].append(r)
    for (label, variant), group in sorted(groups.items()):
        valid = [r for r in group if r['valid']]
        resources.append(dict(matrix=label, variant=variant, seeds=[r['seed'] for r in group],
                              runs=len(group), valid=len(valid),
                              mean_tasks=statistics.mean(r['tasks'] for r in valid) if valid else None,
                              min_tasks=min((r['tasks'] for r in valid), default=None),
                              max_tasks=max((r['tasks'] for r in valid), default=None),
                              max_wall_seconds=max(r['wall_seconds'] for r in group),
                              max_wrapper_seconds=max((r['max_wrapper_seconds'] for r in valid), default=None),
                              peak_process_bytes=max(r['peak_process_bytes'] for r in group),
                              decision_limit_ms=group[0]['decision_limit_ms']))
    write('profile-resources.json', resources)
    baseline_mean = statistics.mean(r['default'] for r in paired)
    candidate_mean = statistics.mean(r['lru_8192'] for r in paired)
    summary = dict(seeds=list(range(6)), horizon=5000, default_mean=baseline_mean,
                   demand_512_mean=statistics.mean(r['demand_512'] for r in paired),
                   lru_8192_mean=candidate_mean,
                   ratio_of_means_improvement_percent=100 * (candidate_mean / baseline_mean - 1),
                   mean_paired_improvement_percent=statistics.mean(r['effect_vs_default_percent'] for r in paired),
                   mean_paired_vs_demand_512_percent=statistics.mean(r['effect_vs_demand_512_percent'] for r in paired),
                   memory_limit_bytes=LIMIT, full_records=len(records), valid_records=sum(r['valid'] for r in records))
    write('summary.json', summary)
    text = ['# Full WAREHOUSE results', '',
            'Generated by `report.py` from compact evidence. Every run targets 5,000 simulation steps with 10,000 robots; invalid and interrupted attempts remain explicit in the record. A valid simulator result is not a liveness proof.', '',
            '## Confirmed cache profile', '',
            table(['Seed', 'Default', '512 MiB demand cache', '8 GiB LRU cache', 'Change vs default', 'With pre-rotation'],
                  [[r['seed'], f"{r['default']:,}", f"{r['demand_512']:,}", f"{r['lru_8192']:,}",
                    f"{r['effect_vs_default_percent']:+.2f}%", f"{r['pre_rotation']:,}"] for r in paired]), '',
            f"Mean completions: default **{baseline_mean:,.0f}**, 8 GiB profile **{candidate_mean:,.1f}**. Ratio-of-means improvement: **{summary['ratio_of_means_improvement_percent']:.2f}%**. These are planner seeds on one fixed input, not independent maps or confidence intervals.", '',
            '## All tested profiles', '',
            'Reported task means include valid full runs only; the valid/runs column exposes failures. Profiles with different decision caps are diagnostic comparisons. Runtime depends on the recorded CPU model and batch. Earlier sequence prototypes also have the intent-protection defects described in the README.', '',
            table(['Matrix / profile', 'Seeds', 'Valid / runs', 'Mean tasks', 'Max wall, s', 'Max wrapper, s', 'Peak RSS, GiB', 'Cap, ms'],
                  [[r['matrix'].replace('warehouse-', '') + ' / ' + r['variant'], ','.join(map(str,r['seeds'])),
                    f"{r['valid']}/{r['runs']}", f"{r['mean_tasks']:,.1f}" if r['mean_tasks'] is not None else '—',
                    f"{r['max_wall_seconds']:.1f}", f"{r['max_wrapper_seconds']:.3f}" if r['max_wrapper_seconds'] is not None else '—',
                    f"{r['peak_process_bytes']/1024**3:.3f}", r['decision_limit_ms']] for r in resources]), '']
    leader_path = OUT / 'leaders-full/results.json'
    leaders = read(leader_path) if leader_path.exists() else []
    text += ['## Archived leader references', '']
    if leaders:
        text += [table(['Snapshot', 'Completed tasks', 'Simulator valid', 'Entry timeouts', 'Wall, min', 'Peak RSS, GiB', 'Fits 32 GB'],
                       [[r['case']['team'], f"{r['summary']['result']['numTaskFinished']:,}", r['summary']['valid'],
                         r['summary']['result']['numEntryTimeouts'], f"{r['summary']['wall_seconds']/60:.1f}",
                         f"{r['summary']['usage']['peak_rss_kib']/1024**2:.3f}",
                         r['summary']['usage']['peak_rss_kib']*1024 <= LIMIT] for r in leaders]), '',
                 'Each reference has 32 logical CPUs / 16 physical cores and a 128 GiB allocation, with its supplied policies intact. CGAR uses one physical core per instance and the stricter 32 GB target. These are local reproductions, not official leaderboard scores or equal-compute comparisons.', '']
    else:
        text += ['Reference analysis is pending.', '']
    study = read(OUT / 'study.json')
    interruptions = [(r['label'], name) for r in study['matrices'] for name in r.get('interrupted_cases', [])]
    if interruptions:
        text += ['## Interrupted cases with no accepted score', '',
                 table(['Matrix', 'Case', 'Status'], [[label, name, 'Scheduler termination; incomplete'] for label, name in interruptions]), '',
                 'These cases are excluded from task means and remain explicitly listed in the study ledger and scheduler accounting.', '']
    (HERE / 'RESULTS.md').write_text('\n'.join(text))
    if a.plots:
        plot(matrices, leaders)
    print(json.dumps(summary, indent=2))


def plot(matrices, leaders):
    import matplotlib
    matplotlib.use('Agg')
    import matplotlib.pyplot as plt
    plt.rcParams.update({'svg.fonttype': 'none', 'svg.hashsalt': 'lorr-warehouse-sequences-20260918', 'font.size': 10})
    fig, axes = plt.subplots(1, 2, figsize=(12, 4.2), constrained_layout=True)
    fig.suptitle('CGAR seeds 0–2; 10,000 robots; 5,000 simulation steps', fontsize=11)
    profiles = [('Default', 'warehouse-prerotation-full-v12', 'baseline', '#666666'),
                ('8 GiB orientation cache', 'warehouse-prerotation-full-v12', 'orientation_8192', '#0072B2'),
                ('Early sequence refinement (rejected)', 'warehouse-full-v2', 'sequence5_two', '#D55E00')]
    for label, matrix, variant, color in profiles:
        rows = [r for r in matrices[matrix]['metrics'] if r['case'].startswith(variant+'-s') and int(r['case'].split('-s')[1].split('-')[0]) < 3]
        steps = [int(t) for t in rows[0]['curve']]
        values = [statistics.mean(r['curve'][str(t)] for r in rows) for t in steps]
        axes[0].plot(steps, values, label=label, color=color, linewidth=2)
        rates = [statistics.mean(r['completed_per_1000'][i] for r in rows) for i in range(5)]
        axes[1].plot(range(1,6), rates, marker='o', color=color, label=label)
    axes[0].set(xlabel='Simulation step', ylabel='Mean completed tasks', title='Full-run ranking can reverse an early result')
    axes[1].set(xlabel='1,000-step window', ylabel='Mean tasks completed in window', title='Cache profile sustains its completion rate', xticks=list(range(1,6)))
    for ax in axes:
        ax.grid(alpha=.2); ax.set_ylim(bottom=0)
    axes[0].legend(loc='upper left', fontsize=8)
    fig.savefig(HERE / 'warehouse-full-curves.svg', metadata={'Date': None})
    plt.close(fig)
    if leaders:
        fig, ax = plt.subplots(figsize=(7,4.2), constrained_layout=True)
        control = [r for r in matrices['warehouse-prerotation-full-v12']['metrics'] if r['case'].startswith('orientation_8192-')]
        steps = [int(t) for t in control[0]['curve']]
        ax.plot(steps, [statistics.mean(r['curve'][str(t)] for r in control) for t in steps], label='CGAR 8 GiB profile, six-seed mean', color='#0072B2', linewidth=2)
        for r in leaders:
            row = r['metrics']; status = 'no timeout' if r['summary']['valid'] else 'timeout'
            memory = r['summary']['usage']['peak_rss_kib'] / 1024**2
            ax.plot([int(t) for t in row['curve']], list(row['curve'].values()), linestyle='-' if r['summary']['valid'] else '--',
                    color={'kk': '#009E73', 'nms': '#D55E00'}[r['case']['team']],
                    label=f"{r['case']['team'].upper()} ({status}; {memory:.1f} GiB)")
        ax.set(xlabel='Simulation step', ylabel='Completed tasks', title='WAREHOUSE local reference gap: unequal compute')
        ax.grid(alpha=.2); ax.set_ylim(bottom=0); ax.legend(fontsize=8)
        fig.savefig(HERE / 'warehouse-reference-curves.svg', metadata={'Date': None})
        plt.close(fig)
    for path in HERE.glob('warehouse-*-curves.svg'):
        path.write_text('\n'.join(line.rstrip() for line in path.read_text().splitlines()) + '\n')


if __name__ == '__main__':
    main()
