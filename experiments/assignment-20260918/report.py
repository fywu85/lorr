#!/usr/bin/env python3
"""Summarize all assignment experiments, including regressions and task waiting."""
import json
from pathlib import Path
import statistics
HERE=Path(__file__).resolve().parent
ROOT=HERE.parents[1]
RESULTS=HERE/'results'


def read(path):return json.loads((RESULTS/path).read_text())
def write(path,value):(RESULTS/path).write_text(json.dumps(value,indent=2)+'\n')


def main():
    folders=['weights','swaps','nms-cost','cost10']
    metrics={folder:read(folder+'/metrics.json') for folder in folders}
    pairs={folder:read(folder+'/paired-results.json') for folder in folders}
    instances=read('weights/spec.json')['instances']
    summaries={folder:read(folder+'/run-summaries.json') for folder in folders}
    all_runs=[r for suites in summaries.values() for rows in suites.values() for r in rows]
    assert len(all_runs)==360 and all(r['valid'] for r in all_runs)
    lookup={folder:{(r['case'],r['instance']):r for r in rows} for folder,rows in metrics.items()}
    historical=json.loads((HERE.parent/'throughput-20260918-next/results/estimates-full/metrics.json').read_text())
    historical={(r['case'],r['instance']):r for r in historical}
    equivalences=[]
    for folder in folders:
        for seed in range(3):
            for name in instances:
                case='baseline-s%d-r0'%seed;a=lookup[folder][case,name];b=historical[case,name]
                equivalences.append({'matrix':folder,'case':case,'instance':name,'identical_trajectories':a['trajectory_sha256']==b['trajectory_sha256']})
    for seed in range(3):
        for name in instances:
            case='weight5-s%d-r0'%seed
            equivalences.append({'matrix':'weight5 repeated in swaps','case':case,'instance':name,
                                 'identical_trajectories':lookup['weights'][case,name]['trajectory_sha256']==lookup['swaps'][case,name]['trajectory_sha256']})
    assert all(r['identical_trajectories'] for r in equivalences)
    write('equivalence.json',equivalences)
    definitions=[('weight2','weights'),('weight5','weights'),('reassign','swaps'),('reassign_weight5','swaps'),('weighted_cost','nms-cost'),('weighted_cost_reassign','nms-cost'),('weighted_cost10','cost10')]
    effects=[]
    for variant,folder in definitions:
        for name in instances:
            rows=[r for r in pairs[folder] if r['variant']==variant and r['instance']==name]
            values=[r['effect_percent'] for r in rows]
            effects.append({'variant':variant,'instance':name,'mean_effect_percent':statistics.mean(values),
                            'min_effect_percent':min(values),'max_effect_percent':max(values),'seeds':rows})
    write('effects.json',effects)
    old_direct=json.loads((HERE.parent/'throughput-20260918-next/results/scheduler-full/metrics.json').read_text())
    old_direct={(r['case'],r['instance']):r for r in old_direct}
    direct_comparison=[]
    for name in instances:
        rows=[]
        for seed in range(3):
            a=old_direct['hrrn_off-s%d-r0'%seed,name]
            b=lookup['nms-cost']['weighted_cost-s%d-r0'%seed,name]
            rows.append({'seed':seed,'weight1':a['tasks'],'weight5':b['tasks'],'effect_percent':100*(b['tasks']/a['tasks']-1)})
        direct_comparison.append({'instance':name,'mean_effect_percent':statistics.mean(r['effect_percent'] for r in rows),'seeds':rows})
    write('direct-cost-weight-comparison.json',direct_comparison)
    def effect(variant,name):return next(r for r in effects if r['variant']==variant and r['instance']==name)
    def mean(variant,name):return '{:+.1f}%'.format(effect(variant,name)['mean_effect_percent'])
    resource=[]
    for folder in folders:
        rows=[r for suite in summaries[folder].values() for r in suite]
        resource.append({'matrix':folder,'allocation':read(folder+'/allocation.json'),
                         'max_decision_seconds':max(r['max_decision_seconds'] for r in metrics[folder]),
                         'max_rss_gib':max(r['process_resources']['peak_rss_kib']/1048576 for r in rows),
                         'max_native_wall_seconds':max(r['wall_seconds'] for r in rows)})
    write('resources.json',resource)
    max_changes=max(r['max_changes_per_task'] for ms in metrics.values() for r in ms)
    late=sum(r['assignments_after_pickup'] for ms in metrics.values() for r in ms)
    assert max_changes<=1 and late==0
    write('reassignment-audit.json',{'runs':len(all_runs),'max_observed_changes_per_task':max_changes,'assignments_after_pickup':late})
    lines=['# NMS-inspired task assignment experiments, 18 September 2026','',
           'Implemented global pickup weighting and bounded swaps of unopened tasks. All 360 full-horizon runs completed with zero planner errors, schedule errors or timeouts. The changes are available as independent switches; the production defaults remain `CGAR_PICKUP_WEIGHT=1`, `CGAR_REASSIGN=0`, and `CGAR_HRRN=1`. There is no map-specific CGAR configuration.','',
           '## Results and default decision','',
           'Every comparison pairs seeds 0, 1 and 2 across all ten archived horizons. Tables report the arithmetic mean of the three paired percentage changes, not a confidence interval or official competition score. Per-seed counts and ranges are retained in [effects.json](results/effects.json); per-matrix metrics retain completion curves and trajectory hashes.','',
           '| Instance | Pickup weight 2 | Pickup weight 5 | Reassignment | Reassignment + weight 5 |','|---|---:|---:|---:|---:|']
    for name in instances:lines.append('| {} | {} | {} | {} | {} |'.format(name,*[mean(v,name) for v in ['weight2','weight5','reassign','reassign_weight5']]))
    r=effect('reassign','SORTATION');w=effect('weight5','SORTATION')
    lines += ['', 'Weight 5 with the existing HRRN objective reduces SORTATION throughput in every seed ({:+.1f}% to {:+.1f}%). Reassignment alone changes SORTATION by {:+.1f}% to {:+.1f}%, so the seed-0 gain did not repeat. These results do not justify enabling either setting by default.'.format(w['min_effect_percent'],w['max_effect_percent'],r['min_effect_percent'],r['max_effect_percent']),'',
              'NMS ranks by weighted travel cost directly. To test that interaction, a third matrix disables HRRN while retaining CGAR\'s independent oldest-task admission. This configuration still uses a single global weight 5. The direct-cost profile is a throughput/waiting-time tradeoff; it is not enabled by default. One final global weight-10 setting tests whether greater pickup emphasis improves that tradeoff.','',
              '| Instance | Direct cost, weight 5 | Direct cost, weight 5 + reassignment | Direct cost, weight 10 |','|---|---:|---:|---:|']
    for name in instances:lines.append('| {} | {} | {} | {} |'.format(name,mean('weighted_cost',name),mean('weighted_cost_reassign',name),mean('weighted_cost10',name)))
    lines += ['', 'Direct cost with weight 5 improves the three-seed mean on nine of ten instances, including CITY-01 (+6.3%), CITY-02 (+10.3%) and RANDOM-03 (+11.5%). SORTATION loses 5.7% on average and its unfinished-task age p90 rises from 1,660–1,955 to 4,508–4,600 steps. Weight 10 increases the SORTATION loss to 10.1%. The useful outcome is an optional global throughput profile with explicit costs; the conservative default is retained.', '',
              'The previous study already found gains on the eight smaller instances from disabling HRRN alone. The new weight-5 result should not be credited with all those gains. Compared with that earlier direct-cost weight-1 policy, weight 5 adds about 2.0% on WAREHOUSE and 9.3% on SORTATION (three-seed means), although SORTATION remains below the HRRN baseline. [Comparison with the prior controlled runs](results/direct-cost-weight-comparison.json).', '', 'To run that profile on reserved cores:', '', '```sh',
              'python3 tools/benchmark_gridengine.py --output runs/cgar-direct-cost --jobs 10 \\',
              '  --env CGAR_HRRN=0 --env CGAR_PICKUP_WEIGHT=5', '```', '',
              'These are adaptations of two NMS ideas inside CGAR, not a port of NMS\'s complete scheduler. NMS also maintains broader sorted candidate lists and couples its choices to a different, parallel motion planner. A weak or negative result here does not isolate the value of those mechanisms in NMS.','',
              '## Pickup travel and waiting','',
              'Completed-task throughput is the adoption criterion. Reducing an estimated pickup distance or even total empty movement does not by itself establish a gain: congestion, the selected task mix, and unfinished tasks also matter. The following counts come from actual simulator actions and pickup events.','',
              '| SORTATION, seed | Policy | Tasks | Empty forward moves | Mean time from first assignment to observed pickup | Unfinished-task age p90 |','|---|---|---:|---:|---:|---:|']
    for seed in range(3):
        for variant,folder in [('baseline','weights'),('weight5','weights'),('reassign','swaps'),('weighted_cost','nms-cost')]:
            r=lookup[folder][variant+'-s%d-r0'%seed,'SORTATION']
            lines.append('| {} | {} | {} | {} | {:.1f} | {} |'.format(seed,variant,r['tasks'],r['empty_forward_actions'],r['observed_pickup_delay']['mean'],r['outstanding_task_age']['p90']))
    lines += ['', 'Pickup delay is measured from the first assignment, including time spent with an earlier owner after a swap. It includes only tasks whose pickup was observed; the metrics also retain unpicked assignment ages and counts. Empty forward/turn/wait actions cover all assigned, unopened task intervals through the horizon, including unfinished pickups. Lower total movement with fewer completions is not automatically higher efficiency. Unfinished-task age includes every revealed unfinished task. In RANDOM-05 it includes tasks excluded by the existing capacity mode (31 parked robots), so this is not a claim of unrestricted service.','',
              '## Reassignment contract and checks','',
              '- Both tasks must be unstarted and already assigned. A swap retains both assignments; it never drops a task or changes its reveal time.',
              '- Each task can change robot at most once before pickup; each robot has a 20-step cooldown.',
              '- Current/next pending primary tickets, recovery participants and reserved cells, pocket service, unfinished turns/moves, started tasks and fairness-admitted tasks are protected.',
              '- Both robots and pickups must be in the core. Existing capacity eligibility remains enforced for every remaining task stop.',
              '- Every ten steps, process at most 256 eligible sources. Each source gets at most 16 local candidates from a 2,048-node search and 16 global samples. Accept only savings of at least four estimated moves and 10% of the old pair cost.',
              '- Use cached tables for all four distances if both goal tables exist, otherwise Manhattan for all four. Build no speculative tables and leave routing-cache recency unchanged.',
              '- Prescribed work must complete before the shared deadline. Failure raises a timeout; elapsed time never truncates a successful pass.','',
              'Regression fixtures verify a swap reducing pickup distance from 20 to 2, a one-change cap, started-task protection, a minimum saving, primary protection while another pair swaps, unfinished-turn protection, recovery-witness protection, and preservation of a fair admission even when swapping would be cheaper. Weighting has a fixture that chooses a nearby longer task over a distant short task while preserving waiting-time priority. The full existing regression suite also passed. [Validation](results/swap-validation/validation.txt).','',
              'Actual assignment histories across all 360 runs contain no task reassigned after pickup and no task with more than one observed ownership change. Swaps made before a task\'s first simulator-visible assignment are absent from ownership-change counts; planner counters retain accepted swaps. These checks preserve the intended invariants but are not an end-to-end liveness proof. [Audit](results/reassignment-audit.json).','',
              '## Resources and reproducibility','',
              'Each matrix used an exclusive GRID allocation with up to six concurrent suites of five native instances, pinned to 30 distinct physical cores, unlimited CPU quota, and 8 GiB requested per slot. CGAR remains single-threaded per instance. GRID scales the process address-space limit with slots; this is not an independent 8 GiB limit for each child. CPU models differ across matrices, so wall-time differences between matrices are not implementation speedups.','',
              '| Matrix | CPU | Maximum decision seconds | Peak RSS GiB | Longest native run seconds |','|---|---|---:|---:|---:|']
    for r in resource:
        lines.append('| {} | {} | {:.3f} | {:.3f} | {:.1f} |'.format(r['matrix'],r['allocation']['resources']['cpu_model'],r['max_decision_seconds'],r['max_rss_gib'],r['max_native_wall_seconds']))
    lines += ['', 'All 120 baseline instance/seed runs reproduce the prior committed baseline trajectories exactly. Repeated weight-5 runs agree across both builds/matrices in all 30 pairs. The equivalence hash covers paths, assignments, events and tasks, excluding runtime measurements. [Evidence](results/equivalence.json).','',
              'Full horizons were used because the preceding experiment demonstrated that a 1,000-step screen can reverse the eventual ranking. `metrics.json` retains completion curves and `paired-results.json` retains retrospective prefix cutoffs; they do not validate one universal short horizon for future assignment changes.','',
              '```sh',
              'python3 experiments/assignment-20260918/build.py --output runs/new-assignment-build',
              '# After the build and regression job completes:',
              'python3 tools/benchmark_matrix.py --output runs/new-assignment-matrix \\',
              '  --binary runs/new-assignment-build/lifelong \\',
              '  --source-manifest runs/new-assignment-build/build.json \\',
              '  --variants experiments/assignment-20260918/reassign-variants.json \\',
              '  --seeds 0 1 2 --parallel-suites 6 --jobs-per-suite 5',
              '# Analyze after completion, or use submit_analysis.py --hold-job JOB_ID:',
              'python3 experiments/assignment-20260918/analyze.py \\',
              '  --input runs/new-assignment-matrix --output runs/new-assignment-analysis',
              '```','',
              'The variant files cover weights, reassignment and direct-cost ranking. Weight 10 was added after the weight-5 direct-cost tradeoff emerged; it is exploratory tuning on the same ten instances, not a held-out confirmation. Source and binary hashes, build logs, allocation metadata and all compact results are retained. Binaries and raw trajectories remain in ignored `runs/`. The default remains unchanged while the measured alternatives are available for further work.','']
    (HERE/'README.md').write_text('\n'.join(lines))
    print('REPORT_COMPLETE',len(all_runs),'runs;',len(equivalences),'matching equivalences')


if __name__=='__main__':main()
