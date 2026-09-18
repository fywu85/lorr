#!/usr/bin/env python3
"""Build the study report from retained metrics, without selecting only favorable runs."""
import json
from pathlib import Path

HERE = Path(__file__).resolve().parent
RESULTS = HERE / 'results'


def read(path):
    return json.loads((RESULTS / path).read_text())


def write(path, value):
    (RESULTS / path).write_text(json.dumps(value, indent=2) + '\n')


def main():
    scheduler = read('scheduler-full/metrics.json')
    estimates = read('estimates-full/metrics.json')
    sp = read('scheduler-full/paired-results.json')
    ep = read('estimates-full/paired-results.json')
    instances = read('scheduler-full/spec.json')['instances']
    old = {(r['case'],r['instance']): r for r in scheduler if r['case'].startswith('baseline-')}
    equivalence = [{'case':r['case'],'instance':r['instance'],
                    'identical_trajectories':r['trajectory_sha256']==old[r['case'],r['instance']]['trajectory_sha256']}
                   for r in estimates if r['case'].startswith('baseline-')]
    assert len(equivalence)==30 and all(r['identical_trajectories'] for r in equivalence)
    write('baseline-equivalence.json',equivalence)
    horizons = []
    for variant, dataset in [('hrrn_off',scheduler),('samples256',scheduler),('refine',estimates)]:
        keyed={(r['case'],r['instance']):r for r in dataset}
        for name in instances:
            pairs=[(keyed['baseline-s%d-r0'%seed,name],keyed[variant+'-s%d-r0'%seed,name]) for seed in range(3)]
            full=[100*(b['tasks']/a['tasks']-1) for a,b in pairs]
            if all(a['trajectory_sha256']==b['trajectory_sha256'] for a,b in pairs):
                continue
            grid=list(map(int,pairs[0][0]['curve']))
            def category(v): return 1 if v>=1 else (-1 if v<=-1 else 0)
            def stable(magnitude):
                flags=[]
                for step in grid:
                    counts=[(a['curve'][str(step)],b['curve'][str(step)]) for a,b in pairs]
                    if any(min(a,b)<100 for a,b in counts):
                        flags.append(False);continue
                    effects=[100*(b/a-1) for a,b in counts]
                    flags.append(all(category(e)==category(f) and (not magnitude or abs(e-f)<=5)
                                     for e,f in zip(effects,full)))
                bad=max((i for i,good in enumerate(flags) if not good),default=-1)
                return grid[bad+1] if bad+1<len(grid) else None
            horizons.append({'variant':variant,'instance':name,'full_steps':pairs[0][0]['steps'],
                             'directional_steps_all_seeds':stable(False),'within_5pp_steps_all_seeds':stable(True),
                             'full_effect_percent_by_seed':full})
    write('horizons.json',horizons)
    profile={name:min(1000,steps) for name,steps in json.loads((HERE.parents[1]/'mr24/simulation_time.json').read_text()).items()}
    (HERE/'screen-1000.json').write_text(json.dumps(profile,indent=2)+'\n')
    def spread(rows,variant,name):
        values=[r['effect_percent'] for r in rows if r['variant']==variant and r['instance']==name]
        return '{:+.1f} to {:+.1f}%'.format(min(values),max(values))
    lines=['# Scheduler and distance-consistency experiments, 18 September 2026','',
           'The first follow-up batch did not establish a new default that reliably improves throughput. Keep HRRN enabled and 64 fallback samples. The three new distance-consistency switches remain experimental and default off. No map-specific CGAR policy was introduced. See [NMS transfer priorities](NMS-transfer.md) for the next assignment and planning experiments.','',
           'All 120 short scheduler runs, 90 full scheduler runs and 150 full distance runs completed with zero planner errors, schedule errors or timeouts. The new binary with all switches off reproduces the old baseline trajectory in all 30 instance/seed pairs, including paths, assignments, task events and revealed tasks. [Equivalence evidence](results/baseline-equivalence.json).','',
           '## Full-horizon effects','',
           'Changes below are relative to the repaired baseline at `87322af`, paired by seeds 0, 1 and 2. Ranges show the smallest and largest paired completion change; they are not confidence intervals. Each instance uses its archived horizon.','',
           '| Instance | HRRN disabled | Samples 64 → 256 | Refine chain costs |','|---|---:|---:|---:|']
    for name in instances:
        lines.append('| {} | {} | {} | {} |'.format(name,spread(sp,'hrrn_off',name),spread(sp,'samples256',name),spread(ep,'refine',name)))
    lines += ['', 'Disabling HRRN improves many smaller instances but reduces full SORTATION completions by 11.4–17.7%. Its outstanding-task age at the 90th percentile rises from 1,660–1,955 to 4,630–4,710 steps on SORTATION, and from 3,098–3,446 to 5,000 on WAREHOUSE. Increasing samples to 256 also reduces SORTATION throughput. Those changes are not promoted. Task-age figures include every revealed unfinished task, including capacity-excluded tasks on RANDOM-05; they are descriptive and do not establish starvation freedom.','',
              'Refining chain costs produces mixed effects on smaller instances and no completion change on WAREHOUSE or SORTATION. In seed 0, the last SORTATION counter at step 4,800 reports 1,689 refined legs and zero changed scalar costs: the original approximations already had the same numerical value. WAREHOUSE reports 2,168 refined legs but only two changed costs. Improved estimate provenance alone cannot close this throughput gap.','']
    for variant in ['peek','stable_stall','combined']:
        rows=[r for r in ep if r['variant']==variant]
        lines.append('`{}`: {}/30 trajectories identical to baseline; completion changes range from {:+.2f}% to {:+.2f}%.'.format(variant,sum(r['identical_trajectories'] for r in rows),min(r['effect_percent'] for r in rows),max(r['effect_percent'] for r in rows)))
        lines.append('')
    lines += ['The switches are independent: `CGAR_REFINE_CHAIN_COSTS`, `CGAR_SCHEDULER_CACHE_PEEK`, and `CGAR_STABLE_STALL_BASIS`. Refinement upgrades only cached approximate legs, retains scalar values after table eviction, and invalidates changed task epochs. Peek avoids scheduler-driven LRU promotion. Stable stall observations reset the measurement window when the potential changes while preserving the fair-primary ticket. Unit tests cover these failure modes; all existing regression fixtures also pass with all three switches enabled. [Build/test log](results/validation/validation.txt), [source and binary hashes](results/validation/source-hashes.json).','',
              '## How short can evaluation be?','',
              'A 1,000-step cap is a fast screen, not a reliable universal prediction of final throughput. On SORTATION, disabling HRRN looks 2.36–2.37% better at 1,000 steps but is 11.4–17.7% worse by 5,000. The task population and congestion evolve; large early completion counts do not remove that bias. The previous fallback repair calibration was specific to that repair.','',
              'We recalibrated on a 25-step grid. The directional criterion requires at least 100 completions in both policies for every seed and the same full-run classification (above +1%, below −1%, or between) at that prefix and every later tested prefix. The stricter criterion also stays within five percentage points of the full effect. These are retrospective cutoffs for tested changes, not guarantees for future mechanisms.','',
              '| Instance | Full steps | HRRN directional cutoff, all 3 seeds | Within 5 pp |','|---|---:|---:|---:|']
    for name in instances:
        r=next(r for r in horizons if r['variant']=='hrrn_off' and r['instance']==name)
        lines.append('| {} | {} | {} | {} |'.format(name,r['full_steps'],r['directional_steps_all_seeds'],r['within_5pp_steps_all_seeds']))
    lines += ['', 'Use [screen-1000.json](screen-1000.json) for quick runtime/correctness and early-quality checks. Promote candidate defaults only after paired full-horizon validation. Some small instances can be screened much earlier; WAREHOUSE and GAME can require nearly their full horizons under this criterion. [All calibrated cutoffs](results/horizons.json), [raw compact curves](results/scheduler-full/metrics.json).','',
              '## Reconstructed original CGAR core','',
              'The preserved pre-fix `cgar.cpp` and `cgar.hpp` were rebuilt with the earliest recorded wrappers from commit `c2f0e5b` and the existing simulator objects. The original complete executable and wrappers were not preserved. This is a core reconstruction, not an exact replay of the historical draft binary. Its source files and build recipe are retained in `draft-source/` and `build_draft.py`.','',
              'Both runs used reserved physical cores and 1,000 ms decisions on EPYC 9354 hosts. The reconstructed draft had ten concurrent instances; the baseline was in the 30-core matrix. The draft still contains deadline-sensitive policies, so its behavior may vary with execution conditions.','',
              '| Instance | Reconstructed draft, seed 0 | Repaired baseline, seed 0 | Change |','|---|---:|---:|---:|']
    for r in read('draft/comparison.json'):
        lines.append('| {} | {} | {} | {:+.1f}% |'.format(r['instance'],r['draft'],r['baseline'],r['effect_percent']))
    lines += ['', 'The remaining deficits on WAREHOUSE and SORTATION are real in this reconstruction comparison: baseline throughput has not fully recovered on every instance. RANDOM-05 is not an unrestricted gain: the current certified mode parks 31 robots and excludes tasks outside its supported core. The draft does not use the same restriction. [Provenance and comparison](results/draft/comparison.json).','',
              '## KittyKnight and NMS references','',
              'Each reference had an exclusive GRID allocation, 32 logical CPUs on 16 physical cores, 128 GiB requested memory, a 1,000 ms decision deadline and the documented 30-minute preprocessing allowance. CGAR uses one physical core and 30 seconds preprocessing, which it satisfies. These are resource-aware reference measurements, not official competition scores: the cluster mixes EPYC 7532 and 9354 CPUs, the original competition used different hardware, and each leader was run once with its supplied configuration. Leader map-specific policies remain in their own sources; none were copied into CGAR.','',
              'All columns use the same horizon for each row, capped at 1,000 steps. A dagger marks a reference that completed the horizon but recorded a timeout; it is not a clean success and is retained rather than silently excluded.','',
              '| Instance | Steps | CGAR seed 0 | KittyKnight | NMS |','|---|---:|---:|---:|---:|']
    for r in read('leaders/comparison.json'):
        lines.append('| {} | {} | {} | {}{} | {}{} |'.format(r['instance'],r['steps'],r['cgar_seed0'],r['kk'],'' if r['kk_valid'] else '†',r['nms'],'' if r['nms_valid'] else '†'))
    lines += ['', 'NMS recorded one timeout at timestep 0 on each of WAREHOUSE, SORTATION and GAME. Other recorded errors and any further failures are preserved in the [run summaries](results/leaders/run-summaries.json) and [timeout diagnostics](results/leaders/timeout-diagnostics.json). Short-horizon results establish a substantial gap on several instances; they do not establish a full-horizon ranking.','',
              '## Resources, reproducibility and retained evidence','',
              'CGAR matrices ran at most six suites of five single-threaded instances concurrently, each pinned to a distinct physical core. `exclusive=true` prevents other scheduled jobs sharing the host, and recorded cgroup CPU quotas were unlimited. Jobs still share host memory bandwidth and normal system activity. Grid Engine granted 8 GiB per slot; a 30-slot job has a 240 GiB process address-space limit, not a separate 8 GiB limit per child. The earlier strict study separately validated a one-slot 8 GiB run.','',
              'The scheduler matrix used an EPYC 9354 host; the distance matrix used EPYC 7532. Comparisons within each matrix use the same allocation. Cross-matrix elapsed times are not implementation speedups. Executable hashes and build manifests identify frozen binaries independently of later working-tree edits.','',
              '```sh',
              'python3 tools/benchmark_matrix.py --output runs/new-estimate-matrix \\',
              '  --variants experiments/throughput-20260918-next/estimate-variants.json \\',
              '  --seeds 0 1 2 --parallel-suites 6 --jobs-per-suite 5',
              '# Add --horizon-profile experiments/throughput-20260918-next/screen-1000.json for a short screen.',
              '# For a frozen binary, also pass --binary PATH --source-manifest BUILD_JSON.',
              'python3 experiments/throughput-20260918-next/analyze_matrix.py \\',
              '  --input runs/new-estimate-matrix --output runs/new-estimate-analysis',
              '```','',
              'Build `lifelong` and `cgar_regression` before using the current executable. The reference launcher is `run_references.py`; the historical-core reconstruction uses `build_draft.py`. `analyze_matrix.py`, `package_references.py` and `report.py` produce the retained evidence and this report. Raw trajectories, binaries, generated leader assets and logs stay in ignored `runs/`. Compact results include every scheduled case, resource records and source hashes.','']
    (HERE/'README.md').write_text('\n'.join(lines))
    print('REPORT_COMPLETE',len(equivalence),'baseline equivalences')


if __name__ == '__main__':
    main()
