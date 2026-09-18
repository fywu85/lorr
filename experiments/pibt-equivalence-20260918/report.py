#!/usr/bin/env python3
"""Regenerate tables and paired effects from committed compact PIBT study results."""
import collections
import json
from pathlib import Path
import statistics

HERE = Path(__file__).resolve().parent
RESULTS = HERE / 'results'
INSTANCES = ['WAREHOUSE', 'SORTATION', 'CITY-01', 'CITY-02', 'GAME',
             'RANDOM-01', 'RANDOM-02', 'RANDOM-03', 'RANDOM-04', 'RANDOM-05']


def read(name):
    return json.loads((RESULTS / name).read_text())


def save(name, value):
    (RESULTS / name).write_text(json.dumps(value, indent=2) + '\n')


def table(headers, rows):
    return '\n'.join(['| ' + ' | '.join(headers) + ' |', '| ' + ' | '.join(['---'] * len(headers)) + ' |'] +
                     ['| ' + ' | '.join(map(str, row)) + ' |' for row in rows])


def main():
    rows, summaries, sources = [], {}, {}
    for matrix in ['full', 'controls']:
        spec = read(matrix + '/spec.json')
        assert spec['time_limit_ms'] == 10000
        metadata = {r['name']: r for r in spec['cases']}
        summaries.update(read(matrix + '/run-summaries.json'))
        for r in read(matrix + '/metrics.json'):
            c = metadata[r['case']]
            row = dict(r, variant=c['variant'], seed=c['seed'], repeat=c['repeat'], matrix=matrix)
            rows.append(row)
            sources[c['variant']] = matrix
    assert len(rows) == 300
    assert all(r['valid'] and r['exit'] == 0 and r['timeouts'] == 0 and
               r['planner_errors'] == 0 and r['schedule_errors'] == 0 and r['internal_timeouts'] == 0
               for suite in summaries.values() for r in suite)
    indexed = {(r['variant'], r['instance'], r['seed']): r for r in rows}
    variants = list(dict.fromkeys(r['variant'] for r in rows))
    means = {i: {v: statistics.mean(indexed[v, i, s]['tasks'] for s in range(3)) for v in variants}
             for i in INSTANCES}
    save('means.json', means)
    contrasts = [('commitments', 'pibt', 'pibt_commitments'),
                 ('tickets', 'pibt', 'pibt_tickets'),
                 ('tickets_with_commitments', 'pibt_commitments', 'pibt_tickets_commitments'),
                 ('certificate', 'pibt', 'pibt_certificate'),
                 ('certificate_with_tickets', 'pibt_tickets', 'pibt_tickets_certificate'),
                 ('certificate_with_tickets_and_commitments', 'pibt_tickets_commitments', 'pibt_tickets_certificate_commitments'),
                 ('recovery_without_ordinary_commitments', 'pibt_tickets_certificate', 'pibt_recovery'),
                 ('recovery_with_commitments', 'pibt_tickets_certificate_commitments', 'pibt_all'),
                 ('commitments_with_certificate_and_tickets', 'pibt_tickets_certificate', 'pibt_tickets_certificate_commitments'),
                 ('commitments_with_recovery', 'pibt_recovery', 'pibt_all')]
    contrasts += [('versus_baseline_' + v, 'baseline', v) for v in variants if v != 'baseline']
    effects = []
    for label, control, treatment in contrasts:
        for instance in INSTANCES:
            pairs = []
            for seed in range(3):
                a, b = indexed[control, instance, seed], indexed[treatment, instance, seed]
                assert a['steps'] == b['steps']
                pairs.append({'seed': seed, 'control': a['tasks'], 'treatment': b['tasks'],
                              'effect_percent': 100 * (b['tasks'] / a['tasks'] - 1),
                              'identical_trajectories': a['trajectory_sha256'] == b['trajectory_sha256'],
                              'control_outstanding_task_age_p90': a['outstanding_task_age']['p90'],
                              'treatment_outstanding_task_age_p90': b['outstanding_task_age']['p90'],
                              'control_parked': int(a['last_planner_counters']['parked']),
                              'treatment_parked': int(b['last_planner_counters']['parked'])})
            changes = [p['effect_percent'] for p in pairs]
            effects.append({'contrast': label, 'control': control, 'treatment': treatment, 'instance': instance,
                            'mean_percent': statistics.mean(changes), 'min_percent': min(changes),
                            'max_percent': max(changes), 'pairs': pairs})
    save('effects.json', effects)
    effect = {(r['contrast'], r['instance']): r for r in effects}
    movement = []
    for variant in variants:
        for instance in INSTANCES:
            counters = collections.Counter()
            for seed in range(3):
                for phase in indexed[variant, instance, seed]['movement_phases'].values():
                    counters.update({k: phase[k] for k in ['fw', 'cr', 'ccr', 'wait', 'planned_wait',
                                     'blocked_forward', 'safety_cancel', 'turn_dependency', 'commitment', 'recovery']})
            total = sum(counters[k] for k in ['fw', 'cr', 'ccr', 'wait'])
            movement.append({'variant': variant, 'instance': instance, 'agent_steps': total,
                             'counts': dict(counters), 'percent': {k: 100 * v / total for k, v in counters.items()}})
    save('movement.json', movement)
    runtime = []
    for variant in variants:
        selected = [r for r in rows if r['variant'] == variant]
        resource_rows = [r for name, suite in summaries.items() if name.startswith(variant + '-s') for r in suite]
        runtime.append({'variant': variant, 'matrix': sources[variant], 'valid': len(selected),
                        'max_decision_seconds': max(r['max_decision_seconds'] for r in selected),
                        'runs_exceeding_one_second': sum(r['max_decision_seconds'] > 1 for r in selected),
                        'total_decision_seconds': sum(r['total_decision_seconds'] for r in selected),
                        'peak_rss_gib': max(r['process_resources']['peak_rss_kib'] for r in resource_rows) / 1024 ** 2})
    save('runtime.json', runtime)
    base_time = next(r['total_decision_seconds'] for r in runtime if r['variant'] == 'baseline')
    cost_rows = []
    for instance in INSTANCES:
        items = [r for r in read('snapshots/results.json') if r['topology'] == instance]
        cost = {'instance': instance}
        for team in ['pibt_reference', 'cgar_reference']:
            cost[team] = {'preprocess_seconds': sum(r[team]['preprocess_seconds'] for r in items),
                          'decision_seconds': sum(r[team]['total_decision_seconds'] for r in items),
                          'max_decision_seconds': max(r[team]['max_decision_seconds'] for r in items)}
        cost_rows.append(cost)
    save('reference-cost.json', cost_rows)
    small, snapshots = read('small/completion.json'), read('snapshots/completion.json')
    assert small['identical'] == small['cases'] == 288
    assert snapshots['identical'] == snapshots['cases'] == 20
    comparisons = read('baseline-equivalence.json')['comparisons']
    assert len(comparisons) == 30 and all(r['identical'] for r in comparisons)
    main_table = table(['Instance', 'Steps', 'PIBT + turns', '+ commitments', '+ all tested CGAR layers', 'Current CGAR'],
       [[i, indexed['baseline', i, 0]['steps']] + [f'{means[i][v]:,.1f}' for v in
          ['pibt', 'pibt_commitments', 'pibt_all', 'baseline']] for i in INSTANCES])
    all_table = table(['Variant'] + INSTANCES,
                      [[v] + [f'{means[i][v]:.1f}' for i in INSTANCES] for v in variants])
    layer_labels = [('commitments', 'Commitments alone'), ('tickets', 'Tickets alone'),
                    ('certificate', 'Certificate alone'),
                    ('certificate_with_tickets_and_commitments', 'Certificate with tickets + commitments'),
                    ('recovery_with_commitments', 'Recovery with tickets + certificate + commitments')]
    layer_table = table(['Instance'] + [label for _, label in layer_labels],
        [[i] + [f"{effect[name, i]['mean_percent']:+.1f}%" for name, _ in layer_labels] for i in INSTANCES])
    counts = {(r['variant'], r['instance']): r['percent'] for r in movement}
    movement_table = table(['Instance', 'Profile', 'Forward', 'Turning', 'Wait', 'Turn-blocked forward'],
        [[i, v, f"{counts[v,i]['fw']:.1f}%", f"{counts[v,i]['cr'] + counts[v,i]['ccr']:.1f}%",
          f"{counts[v,i]['wait']:.1f}%", f"{counts[v,i]['turn_dependency']:.1f}%"]
         for i in ['WAREHOUSE', 'SORTATION', 'GAME', 'RANDOM-04', 'RANDOM-05']
         for v in ['pibt', 'pibt_commitments', 'baseline']])
    runtime_table = table(['Profile', 'Valid/runs', 'Max wrapper s', 'Runs >1 s', 'Total wrapper/control', 'Peak RSS GiB'],
        [[r['variant'], str(r['valid']) + '/30', f"{r['max_decision_seconds']:.3f}", r['runs_exceeding_one_second'],
          f"{r['total_decision_seconds']/base_time:.2f}x" if r['matrix'] == 'full' else 'separate allocation',
          f"{r['peak_rss_gib']:.3f}"] for r in runtime])
    cost_table = table(['Instance', 'Upstream preparation s', 'Upstream spatial planning s', 'CGAR preparation s', 'CGAR full planning s'],
        [[r['instance'], f"{r['pibt_reference']['preprocess_seconds']:.4f}", f"{r['pibt_reference']['decision_seconds']:.4f}",
          f"{r['cgar_reference']['preprocess_seconds']:.4f}", f"{r['cgar_reference']['decision_seconds']:.4f}"] for r in cost_rows])
    result = f'''# PIBT specialization and CGAR layer diagnosis, 18 September 2026

CGAR now has an executable PIBT compatibility profile that matches the pinned upstream on **308 cases and {small['agent_decisions'] + snapshots['agent_decisions']:,} robot decisions**. The shared recursive assignment code serves both this profile and the existing CGAR policy. All 30 full-horizon default runs reproduce the previous baseline exactly.

The throughput result is that **ordinary spatial PIBT plus a direct turn adapter is substantially weaker than current CGAR**. Persistent movement commitments account for much of the recovery. Certificate-related routing and capacity handling also help the dense random cases when combined with commitments. The default remains the strongest of the tested profiles by the three-seed mean on every instance. No production policy is promoted by this study.

## Reference and the scope of equivalence

The reference is [Kei18/pibt2](https://github.com/Kei18/pibt2), the AIJ-2022 implementation, pinned at [`faab5b916649549f1cd563df8dbf6e4f6382f631`](https://github.com/Kei18/pibt2/tree/faab5b916649549f1cd563df8dbf6e4f6382f631). Its grid-pathfinding dependency is pinned at `f444df84459258d7b4d8ceffdbbf2e201b042a68`. Selected upstream files are vendored with both MIT license notices; [upstream.json](upstream.json) records every file hash. The algorithm in `pibt.cpp` is unchanged. CMake adds only a friend declaration to a generated header copy to expose stepping for the harness.

`CGAR_PIBT_REFERENCE=1` selects upstream's elapsed/initial-distance/random priority ordering, exact spatial distances, west/east/north/south/wait candidate order, shuffled ties, and empty-cell preference. It disables ordinary commitments and defaults certificates and transactions to off. This profile includes every traversable component, with no certificate-based capacity pruning. The production default still uses its existing policy. Both call [the shared recursion](../../cgar/cgar_planner/pibt_kernel.hpp).

There are two movement models. Native PIBT moves to an adjacent cell or waits in one step; it has no orientation. The native harness compares CGAR's spatial proposals with upstream, including checking the stepping wrapper against the original complete `PIBT::run` on all 144 native cases. The rotation harness adapts upstream proposals independently: turn toward the target, then cancel forward dependency chains that terminate at a stationary robot. CGAR uses its existing action conversion and safety code. Matching this adapter is not a claim that upstream PIBT itself handles LoRR rotations.

The 288 small cases span six 8-by-8 topologies, 25%/75%/100% occupancy, eight seeds and both movement models, for up to 256 decisions. Twenty full-fleet probes replay the existing MR24 snapshots at steps 100 and 500 for 100 decisions. Fixed goals remain fixed even after arrival; idle snapshot robots receive their start cell as a goal. This differs from the earlier NMS replay's goal-retirement protocol, so arrival totals are not directly comparable. Obstacles, adjacency, vertex collisions and edge swaps are independently validated. [Small results](results/small/results.json), [snapshot results](results/snapshots/results.json).

These are exact behavioral checks of a specialization, not a general liveness proof. Full occupancy, bridges and disconnected inputs deliberately test behavior outside usual progress assumptions. Switching off CGAR's certificates and recovery does not preserve their progress guarantees automatically. For lifelong use the existing CGAR scheduler remains; a changed or retired target cell resets native priority age and initial distance. That is an explicit LoRR extension, not a reproduction of upstream's MAPD scheduler. There is no map-name or category-specific rule.

## Full-horizon results

All **300/300** full-horizon runs completed with zero planner errors, schedule errors or timeouts. Ten instances, seeds 0–2 and ten profiles are retained. Values below are mean completed tasks over the same three seeds. Horizons are the archived per-instance horizons, not uniformly 1,000 steps.

**The decision limit is 10,000 ms for every profile in this diagnostic study.** Exact initial distance construction failed at 1,000 ms on CITY-01, CITY-02 and GAME in the earlier screen. Thus these are not competition-budget results. A timeout remains a failed run; no partial solution is returned. Default CGAR's 30 full trajectories match its previous 1,000 ms runs, and every default wrapper measurement here is below one second. [Baseline equivalence](results/baseline-equivalence.json).

{main_table}

`PIBT + turns` is CGAR's compatible spatial policy with the direct LoRR adapter and CGAR scheduling. It is not an official pibt2 competition submission. `+ commitments` adds only persistence of chosen movement targets. `+ all tested CGAR layers` also adds ticket ordering, certificates and recovery; it still retains reference candidate ordering and exact distances. Its residual differences from default include turn tie-breaking, random-number consumption and the distance-build policy, so the remaining gap cannot be attributed to one of these alone.

## What individual layers contribute

Each entry is the arithmetic mean of three paired percentage changes against the named control, not a confidence interval or a competition aggregate. Effects are not additive. Full per-seed values, ranges, outstanding-task ages and parked counts are in [effects.json](results/effects.json).

{layer_table}

The first three columns add one mechanism to bare PIBT. The fourth compares `pibt_tickets_certificate_commitments` against `pibt_tickets_commitments`. The fifth compares `pibt_all` against `pibt_tickets_certificate_commitments`. The two certificate controls were run in a separate reserved allocation after the first 240 runs to isolate recovery; they use the same executable, horizons, inputs, seeds and 10-second cap. No runtime speedup is inferred across those allocations.

Certificates here include topology-based routing, pocket locks and capacity handling, not merely a boolean validity check. On RANDOM-05, certificate-enabled variants park 31 robots and exclude tasks with stops outside the retained core; bare variants keep the full fleet and task set. Its certificate effect therefore combines motion changes with different service coverage. The code applies this capacity rule generically to the graph, not by map name. Recovery-disabled certificate controls still perform the capacity bootstrap required by that mode; the recovery contrast toggles the optional `CGAR_TXN` mechanism, not every possible vacancy shift.

## Movement explains much of the difference

Without commitments, spatial PIBT may choose a different destination after each physical turn. Commitments retain the target long enough to execute the turn-and-move sequence. The matched intervention produces much more actual forward movement and less turning on the large instances. This supports persistence as a useful mechanism; we did not directly count abandoned turn targets, and not every wait is avoidable.

{movement_table}

Percentages pool all robot steps across the three seeds. Forward, turning and wait sum to 100%; turn-blocked forward is a subset of wait. Commitment and recovery participation overlap actions and are retained separately in [movement.json](results/movement.json). The existing trajectory analyzer independently checks diagnostic action counts against simulator paths.

The practical next experiment is a fixed amount of search over short action sequences, including turns and waits, with occupancy checked at every future step. Retain persistent destinations, primary/recovery protection and the generic capacity rules while improving coordination. Ordinary PIBT equivalence is now a regression reference; it does not reproduce NMS's orientation-aware multi-step search or establish parity with NMS.

## Runtime, failures and CPU allocation

The one-second, 200-step screen retained **24 successes and six explicit failures**: both `pibt` and `pibt_all` timed out on CITY-01, CITY-02 and GAME at timestep 0 in `distance_table`. All six exited 124 at approximately 1,000 ms; unavailable throughput is not recorded as zero or used as a successful partial run. [Failure excerpts](results/screen/failures.json), [all screen outcomes](results/screen/run-summaries.json).

{runtime_table}

Wrapper time includes scheduling, planning and simulator wrapper overhead. Each reference profile exceeded one second on nine full runs, the three affected instances across three seeds; all stayed below the explicit ten-second cap. A 10-second allowance does not cause extra search or early return: the prescribed computation either finishes or raises `Timeout`. The production default limit is unchanged.

For WAREHOUSE, the current default completed all 5,000 steps with 10,000 robots in 114.2–118.5 seconds per seed (mean 116.3 seconds), using about 6.2 GiB RSS on one reserved physical core. These are process wall times, excluding GRID queue time and compilation. Three such seeds can run concurrently when three cores and sufficient memory are reserved.

Each full matrix used 30 distinct GRID physical cores, six simultaneous suites of five instances, an exclusive allocation and no CPU quota. Each CGAR process was pinned to one physical core; it did not use 30 cores itself. Both matrices used AMD EPYC 9354 processors. Requested memory was 8 GiB per slot; the aggregate process address-space limit is not an independent 8 GiB limit for each child. [Full allocation](results/full/allocation.json), [control allocation](results/controls/allocation.json).

The reference comparison used exclusive four-core and ten-core allocations on AMD EPYC 7532 processors, with each upstream/CGAR pair run sequentially on the same physical core. Upstream precomputes a distance table per robot before its timed steps; CGAR caches tables by goal and builds them in its first planning calls. Summed costs for the two snapshots of each instance are:

{cost_table}

Do not compare upstream's warm step time with CGAR's cold step time alone. The columns also have different scopes: the upstream step timer covers its spatial solver, while CGAR's includes progress bookkeeping, conversion to actions and safety checks. Graph/input creation, trace validation and JSON I/O are excluded. These measurements locate costs, not a claim of equal whole-program speed. They show why exact-distance preprocessing must be considered separately when enforcing a one-second decision limit. [Per-case timings](results/snapshots/results.json).

## Every tested profile

All rows below use the same three seeds. The full flag definitions are in [variants.json](variants.json) and [control-variants.json](control-variants.json).

{all_table}

## Reproduction

The harness builds two separate executables so the upstream and LoRR type definitions cannot collide. It verifies vendored hashes, captures the CGAR/driver source hashes, and checks that sources do not change during execution. The existing CGAR regression suite also passes. [Build and regression evidence](results/build-provenance/), [reference build provenance](results/small/build.json).

```sh
python3 experiments/pibt-equivalence-20260918/equivalence.py \\
  --output runs/new-pibt-equivalence
# After the first GRID job completes:
python3 experiments/pibt-equivalence-20260918/equivalence.py \\
  --output runs/new-pibt-snapshots --snapshots \\
  --build-dir runs/new-pibt-equivalence/build --jobs 10
python3 experiments/assignment-20260918/build.py --output runs/new-pibt-build
# After the build job completes:
python3 tools/benchmark_matrix.py --output runs/new-pibt-full \\
  --binary runs/new-pibt-build/lifelong --source-manifest runs/new-pibt-build/build.json \\
  --variants experiments/pibt-equivalence-20260918/variants.json \\
  --seeds 0 1 2 --parallel-suites 6 --jobs-per-suite 5 --time-limit-ms 10000
python3 experiments/motion-20260918/submit_analysis.py \\
  --input runs/new-pibt-full --output runs/new-pibt-full-analysis --hold-job JOB_ID
```

Repeat the matrix with `control-variants.json` for the 60 additional controls. The shorter failed/successful screen uses `screen-variants.json`, `--horizon-profile experiments/pibt-equivalence-20260918/screen-200.json`, seed 0 and `--time-limit-ms 1000`. Every output directory must be new. Submission commands return before their jobs finish. The configured workspace compiler/environment is required by the GRID runners; the experiment CMake project itself is also usable directly.

`python3 experiments/pibt-equivalence-20260918/report.py` regenerates this report and derived JSON using only committed compact results. `collect.py` packages the local run evidence after GRID analysis has produced `results/full` and `results/controls`; its path arguments support alternate raw run locations. Binaries, full trajectories and raw logs remain ignored under `runs/`.
'''
    (HERE / 'README.md').write_text(result)
    print('Report regenerated: 308 equivalence cases, 300 full runs, 30 screen outcomes.')


if __name__ == '__main__':
    main()
