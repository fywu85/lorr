#!/usr/bin/env python3
"""Regenerate the movement diagnosis and all paired comparison tables."""
import collections
import hashlib
import json
from pathlib import Path
import statistics
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1];OUT=HERE/'results'
names=list(json.loads((ROOT/'mr24/simulation_time.json').read_text()))
def read(name):return json.loads((OUT/name).read_text())
def write(name,data):(OUT/name).write_text(json.dumps(data,indent=2)+'\n')
def mean(xs):return statistics.mean(xs)
def table(headers,rows):return '\n'.join(['| '+' | '.join(headers)+' |','|'+'|'.join(['---']*len(headers))+'|']+['| '+' | '.join(map(str,row))+' |' for row in rows])

def main():
    matrices={name:{kind:read(name+'/'+kind+'.json') for kind in ['metrics','paired-results','run-summaries','allocation']} for name in ['diagnostics','orientation','portfolio','rotation','admission']}
    old={(r['case'],r['instance']):r for r in json.loads((ROOT/'experiments/assignment-20260918/results/swaps/metrics.json').read_text())}
    equivalence=[]
    for label,matrix in matrices.items():
        for r in matrix['metrics']:
            if not r['case'].startswith('baseline-') or (r['case'],r['instance']) not in old:continue
            equivalence.append({'matrix':label,'case':r['case'],'instance':r['instance'],'identical_trajectories':r['trajectory_sha256']==old[r['case'],r['instance']]['trajectory_sha256']})
    assert all(r['identical_trajectories'] for r in equivalence);write('baseline-equivalence.json',equivalence)
    effects=[]
    for label,m in matrices.items():
        for v in sorted({r['variant'] for r in m['paired-results']}):
            for name in names:
                for group,seeds in [('development',[0,1,2]),('confirmation',[3,4,5]),('all',[0,1,2,3,4,5])]:
                    rows=[r for r in m['paired-results'] if r['variant']==v and r['instance']==name and r['seed'] in seeds]
                    if not rows or (label!='admission' and group!='development'):continue
                    xs=[r['effect_percent'] for r in rows]
                    effects.append({'matrix':label,'variant':v,'instance':name,'seed_group':group,'seeds':[r['seed'] for r in rows],'mean_percent':mean(xs),'min_percent':min(xs),'max_percent':max(xs),'wins':sum(x>0 for x in xs)})
    write('effects.json',effects)
    def effects_table(variants,group='development'):
        return table(['Instance']+variants,[[n]+['{:+.1f}%'.format(next(r['mean_percent'] for r in effects if r['instance']==n and r['variant']==v and r['seed_group']==group)) for v in variants] for n in names])
    resources=[]
    for label,m in matrices.items():
        controls={r['case']:r for r in m['metrics'] if r['case'].startswith('baseline-')}
        for v in sorted({r['case'].split('-s')[0] for r in m['metrics']}):
            rows=[r for r in m['metrics'] if r['case'].startswith(v+'-s')]
            seeds={r['case'].split('-s')[1].split('-')[0] for r in rows}
            baseline=[r for r in m['metrics'] if r['case'].startswith('baseline-') and r['case'].split('-s')[1].split('-')[0] in seeds]
            summaries=[r for case,rs in m['run-summaries'].items() if case.startswith(v+'-s') for r in rs]
            resource={'matrix':label,'variant':v,'runs':len(summaries),'valid':sum(r['valid'] for r in summaries),'max_decision_seconds':max(r['max_decision_seconds'] for r in rows),'decision_time_ratio':sum(r['total_decision_seconds'] for r in rows)/sum(r['total_decision_seconds'] for r in baseline),'peak_rss_gib':max(r['process_resources']['peak_rss_kib'] for r in summaries)/1024**2,'cpu_model':m['allocation']['resources']['cpu_model']}
            if any(r.get('orientation') for r in rows):
                builds=sum(r.get('orientation',{}).get('builds',0) for r in rows);guided=sum(r.get('orientation',{}).get('guided',0) for r in rows);fallback=sum(r.get('orientation',{}).get('fallback',0) for r in rows)
                resource.update(oriented_builds=builds,oriented_guided=guided,oriented_fallback=fallback,guided_fraction=guided/(guided+fallback) if guided+fallback else None)
            resources.append(resource)
    write('resources.json',resources)
    counters=read('movement-counters.json');motion_rows=[];phase_rows=[]
    for name in names:
        rows=[r for r in counters if r['instance']==name];den=sum(r['agent_steps'] for r in rows)
        value=lambda k:100*sum(r['total'][k] for r in rows)/den
        motion_rows.append([name]+['{:.1f}%'.format(x) for x in [value('fw'),value('cr')+value('ccr'),value('planned_wait'),value('blocked_forward'),value('farther'),value('recovery')]])
        if name in ['WAREHOUSE','SORTATION','GAME']:
            for phase in ['1','2']:
                p=[r['phases'][phase] for r in rows];steps=sum(sum(x[k] for k in ['fw','cr','ccr','wait']) for x in p)
                phase_rows.append([name,'before pickup' if phase=='1' else 'after pickup','{:.1f}%'.format(100*steps/den)]+['{:.1f}%'.format(100*sum(x[k] for x in p)/steps) for k in ['fw','wait','turn_dependency']])
    snapshot_rows=[];snapshots=read('snapshots.json')
    for name in names:
        r=[x for x in snapshots if x['teams']['cgar']['instance']==name]
        supplied=[x for x in r if x['reference']=='supplied'];uniform=[x for x in r if x['reference']=='uniform']
        c=sum(x['teams']['cgar']['goals_reached'] for x in supplied);n=sum(x['teams']['nms']['goals_reached'] for x in supplied);u=sum(x['teams']['nms']['goals_reached'] for x in uniform)
        snapshot_rows.append([name,c,n,u,'{:.2f}x'.format(u/c)])
    total=sum(r['runs'] for r in resources);valid=sum(r['valid'] for r in resources)
    candidate=[r for r in resources if r['variant']=='orientation_admission'][0]
    timing=read('timing-audit.json')
    repeat_max=max(r['repeat_max_wrapper_seconds'] for r in timing['repeat_comparisons'])
    old_builds=next(r['oriented_builds'] for r in resources if r['variant']=='orientation_turn_first')
    revised_builds=sum(r['orientation']['builds'] for r in matrices['admission']['metrics'] if r['case'] in ['orientation_admission-s0-r0','orientation_admission-s1-r0','orientation_admission-s2-r0'])
    def rtable():return table(['Matrix/profile','Recorded valid / runs','Max wrapper time, s','Wrapper time / control','Peak RSS, GiB'],[[r['matrix']+'/'+r['variant'],str(r['valid'])+'/'+str(r['runs']),'{:.3f}'.format(r['max_decision_seconds']),'{:.2f}x'.format(r['decision_time_ratio']),'{:.3f}'.format(r['peak_rss_gib'])] for r in resources])
    text=f'''# CGAR movement diagnosis, 18 September 2026

The optional orientation-and-turn profile improves WAREHOUSE by 10.7%, CITY-01 by 6.2%, and RANDOM-02 by 9.9% across six seeds. SORTATION averages -1.5%, with individual effects from -9.1% to +6.5%, so the default remains unchanged.

The matched-goal experiment establishes a substantial movement gap even when NMS's map-specific guidance and fleet limits are removed. CGAR loses substantial agent time to waiting, including forward proposals whose dependency chain ends at a turning robot. This identifies an opportunity; it does not make every observed wait avoidable.

The retained changes are observational diagnostics, optional turn-first tie-breaking and optional orientation-aware route costs. Two unsuccessful movement mechanisms are archived as reproducible patches and removed from the active planner. No map-specific CGAR rule was introduced. The default motion policy remains unchanged; the switches and measured tradeoffs below are explicit.

## 1. Where CGAR spends time

Thirty full-horizon instrumented runs cover all ten instances and seeds 0, 1 and 2. All reproduce the previous baseline's paths, assignments, task events and revealed tasks exactly. The counters use read-only cached distances: they build no tables, promote no cache entries and consume no randomness. [Equivalence](results/diagnostic-equivalence.json), [counters](results/movement-counters.json).

Percentages below pool the three seeds within each instance. Forward, turning, planned waiting and blocked forward waiting are disjoint, apart from the separately retained tiny final-safety category. Farther moves are a subset of forward actions; recovery participation overlaps actions. A move farther from the goal may be necessary to unblock traffic.

{table(['Instance','Forward','Turn','Planned wait','Forward blocked','Forward farther','In recovery'],motion_rows)}

A blocked forward action was originally offered as FW and converted to W. Almost all these dependency chains end at a robot offering a turn. Planned waits include reservation pressure and congestion; they are not all voluntarily idle robots. Recovery means direct membership of an installed witness, including future waves. It does not measure all traffic effects caused by recovery or priority rules. The rare recoveries on WAREHOUSE/SORTATION do not support blaming recovery alone for their large gap.

The pickup/delivery split is measured from actual task-stop state, with action counts independently checked against simulator trajectories. Phase-specific percentages use only robot steps in that phase; the phase share uses all robot steps.

{table(['Instance','Phase','Share of agent time','Forward in phase','Wait in phase','Turn-blocked in phase'],phase_rows)}

## 2. Same positions and goals, different motion planners

Twenty inputs are taken immediately before decisions 101 and 501 of the seed-0 CGAR baseline: two snapshots per instance. Both planners start fresh on identical positions, orientations and current goals. Each robot has at most one fixed goal, no scheduling or reassignment runs, and no new work is revealed. Reached goals retire; those robots remain movable. The 100-step probe measures goal arrivals and remaining exact spatial distance, not lifelong throughput. Both planners lose their original internal history.

The supplied NMS reference keeps the archived configuration. A second reference replaces only environment initialization with unit costs for every legal action, a generic map/test type, and no category-specific active-fleet limit. It retains NMS's orientation graph, 129 global action sequences, PIBTS search and 32-worker portfolio. The original submission is unchanged. The replacement is [nms_uniform_environment.cpp](nms_uniform_environment.cpp).

Counts below sum the two 100-step probes. Goals already occupied at the start are excluded from arrivals. Every one of the 80 replay runs completed without a collision or late decision. The 20 repeated CGAR cases have identical per-agent arrival times and movement summaries across the two reference matrices; this check is not a full path hash.

{table(['Instance','CGAR arrivals','Supplied NMS','Uniform NMS','Uniform / CGAR'],snapshot_rows)}

This controls task assignment and shows that the motion stack matters. It does not attribute the difference solely to orientation, lookahead or parallel search. NMS had 32 logical CPUs on 16 physical cores and 128 GiB requested memory; CGAR was pinned to one physical core of that same allocation. Both had a 1,000 ms decision limit, with their existing preprocessing allowances. Thus this is a resource-aware diagnostic, not an equal-compute comparison or an official competition score. Each NMS snapshot was run once; its deadline-driven search can vary between runs.

Supplied NMS deactivates up to 4,000 pending GAME robots and 300 RANDOM-04 robots; the uniform reference deactivates zero pending robots. RANDOM-05 retains CGAR's existing capacity restriction and 31 parked robots. The snapshots originate from CGAR and cover two fixed times, not a random sample of all encountered states. [Inputs](results/snapshot-inputs.json), [results and allocations](results/snapshots.json), [reference build provenance](results/snapshot-uniform-build.json).

## 3. Full-horizon motion changes

Effects are arithmetic means of paired completion changes, not confidence intervals or an aggregate competition score. Guidance experiments use seeds 0–2. The final cache policy is also tested on seeds 3–5, chosen before those results were observed. Those are additional planner seeds on the same maps and task inputs, not held-out maps.

`turn_first` changes only the order of equal-distance candidates: fewer required turns before empty-cell preference. `orientation` caches exact unit-cost reverse BFS distances over (cell, orientation), then ranks complete turn-and-forward macros against a wait. The original spatial potential, persistent primary, pocket locks, commitments and recovery witness remain in control of progress. The combination applies both changes globally.

{effects_table(['turn_first','orientation','orientation_turn_first'])}

The first oriented cache holds 512 MiB and permits 32 complete new tables per decision. Its LRU working set churns on large instances: most eligible PIBT calls still use spatial guidance, while table construction consumes about four times the baseline's total decision time. A prescribed count, not elapsed time, decides when guidance falls back.

The revised cache ranks currently requested goals every 32 steps, preferring higher demand and retaining resident entries on ties. It admits only as many goals as fit in the same 512 MiB. Both the number of table builds and the refresh interval are fixed policy bounds. This avoids repeatedly rebuilding goals that cannot remain resident. Neither policy uses map names, map categories or supplied guidance weights.

{table(['Instance','Seeds 0–2','Seeds 3–5','All six seeds'],[[n]+['{:+.1f}%'.format(next(r['mean_percent'] for r in effects if r['variant']=='orientation_admission' and r['instance']==n and r['seed_group']==g)) for g in ['development','confirmation','all']] for n in names])}

On the same first three seeds, the revised cache builds {revised_builds:,} tables versus {old_builds:,} for the original combined profile: {100*(1-revised_builds/old_builds):.1f}% fewer. The revised profile used {candidate['decision_time_ratio']:.2f} times the control's total recorded wrapper time within its six-seed matrix, compared with 4.27 times for the earlier combined profile within its own matrix. Different CPU models prevent treating the two matrices as a direct speed comparison. The simulator's wrapper includes logging after planning returns; four measurements exceeded one second, discussed below. It remains optional as `CGAR_ORIENTATION_GUIDANCE=2 CGAR_TURN_FIRST=1`. Full per-seed effects and waiting-task ages are retained, including regressions. [Effects](results/effects.json), [full metrics](results/admission/metrics.json).

## 4. Rejected experiments

A four-proposal experiment varies ordinary robot priorities, completes all four valid action sets and scores their actual actions after cancellation. It preserves the primary identity/priority and fixed recovery constraints, and returns only after all prescribed work finishes. Its immediate-progress objective and priority perturbation reduce full-run throughput. This rejects this implementation, not fixed-work search in general.

A second experiment reserves a turning robot's current cell during PIBT and reports that it cannot vacate that cell to its requesting parent. It preserves the turn intent. This produces much more restrictive planning and severe throughput losses; changing the accounting point for a blocked action does not remove the underlying coordination problem.

{effects_table(['portfolio4','turn_first_portfolio4','rotation','rotation_turn_first'])}

Both mechanisms are removed from active CGAR. Their source changes are retained in [prototypes/](prototypes/), as patches against commit `83fc931`, with frozen build hashes and every completed run. Both archived patches apply cleanly to that commit and reproduce their frozen source hashes ([check](results/prototype-reproduction.json)). They have no live environment switch in the final planner. Regression fixtures exercised primary persistence, pocket evacuation, capacity bootstrap and recovery protection, in addition to simulator validation.

The next larger mechanism to investigate is a fixed amount of search over short **action sequences**, including turns, waits and forward moves, with simultaneous occupancy at each future step. NMS searches a pool of 129 five-action sequences; a one-step spatial proposal plus later repair cannot express that same coordination. Any CGAR integration still needs the primary and recovery reservations represented in every future step. These results do not yet prove that this mechanism alone closes the gap.

## Validation, resources and reproduction

The main matrices contain {total} full-horizon CGAR runs, with {valid} recorded valid by the simulator and no reported action errors or entry timeouts. Twelve additional full-horizon runs audit the timing outliers, for 462 total, plus 80 matched-snapshot replays. Recorded validity does not imply that every simulator wrapper measurement stayed below one second. All archived horizons are retained because short lifelong prefixes can reverse the final policy ranking. The 100-step motion probes isolate a different question; they are not a proposed replacement benchmark horizon. Every retained baseline with seeds 0–2 reproduces the prior trajectory: {len(equivalence)} comparisons. [Evidence](results/baseline-equivalence.json).

The main CGAR matrices use 30 distinct reserved physical cores, one per native instance, unlimited CPU quota, exclusive GRID allocation, and 8 GiB requested per slot. The scheduler's process address-space limit scales with all slots; this is not an independent 8 GiB limit for each child. CPU models differ between matrices, so runtime ratios are computed only against the control within each matrix.

{rtable()}

Four `plannerTimes` entries in the admission matrix exceed the nominal 1,000 ms decision limit: 3.725 s and 3.718 s on two separate WAREHOUSE processes, and 1.183 s / 1.231 s on another WAREHOUSE/SORTATION pair. The corresponding log timestamps show each pause after the `planner returns` record, at nearly the same wall-clock time in the two processes. `BaseSystem::plan` joins the planner thread before this log call; its caller measures elapsed time after that call returns. The entry's final deadline check occurs inside the already completed planner thread. Thus these outliers include time after CGAR planning finished. Logging or a host pause is the supported interpretation; the precise system cause was not instrumented.

The repeat reserves 12 physical cores on GRID and runs both profiles on WAREHOUSE and SORTATION for seeds 3–5. All 12 reproduce the original full trajectories exactly; the largest wrapper measurement is {repeat_max:.3f} s. Original measurements remain in every timing table and runtime ratio. The strict CGAR timeout path is unchanged, and no partial plan was accepted to recover performance. [Timing excerpts and comparisons](results/timing-audit.json), [repeat metrics](results/timing-repeat/metrics.json).

Orientation tests compare every source/orientation/goal in a small topology against an independent forward action search, check cache eviction/retention, explicit deadline failure and diagnostic attribution of a turn dependency. Existing safety, scheduling and liveness-mechanism fixtures pass with the new guidance enabled. These are regression checks, not a new end-to-end liveness proof.

```sh
python3 experiments/assignment-20260918/build.py --output runs/new-motion-build
# After its GRID build finishes:
python3 tools/benchmark_matrix.py --output runs/new-motion-matrix \\
  --binary runs/new-motion-build/lifelong \\
  --source-manifest runs/new-motion-build/build.json \\
  --variants experiments/motion-20260918/admission-variants.json \\
  --seeds 0 1 2 3 4 5 --parallel-suites 6 --jobs-per-suite 5
python3 experiments/motion-20260918/submit_analysis.py \\
  --input runs/new-motion-matrix --output runs/new-motion-analysis --hold-job JOB_ID
```

Snapshot drivers link the original planner objects while replacing the simulator main with [snapshot_driver.cpp](snapshot_driver.cpp). Its action validator rejects obstacles, wrapped rows, duplicate destinations and edge swaps before applying a joint action. [snapshot_jobs.py](snapshot_jobs.py) records source/object/binary hashes and allocates the requested reference CPUs. The frozen inputs are retained under `snapshots/`; rebuilding the drivers uses those committed inputs and does not require historical raw trajectories. Configure both `cgar/build` and `nms/build` with the repository environment first; the snapshot build refreshes their planner objects before linking. For example, after activating the environment and configuring CGAR as in the root README:

```sh
cmake -S nms -B nms/build -DCMAKE_BUILD_TYPE=Release \\
  -DCMAKE_PREFIX_PATH="$CONDA_PREFIX" -DPYTHON=OFF
python3 experiments/motion-20260918/snapshot_jobs.py build \\
  --output runs/new-snapshot-build
# After build-completion.json reports success:
python3 experiments/motion-20260918/snapshot_jobs.py run \\
  --build-dir runs/new-snapshot-build --output runs/new-snapshots
python3 experiments/motion-20260918/build_uniform_reference.py \\
  --base runs/new-snapshot-build --output runs/new-uniform-build
# After the uniform build completes:
python3 experiments/motion-20260918/snapshot_jobs.py run \\
  --build-dir runs/new-uniform-build --output runs/new-uniform-snapshots
```

`python3 experiments/motion-20260918/report.py` regenerates this report and its comparison tables from committed compact results. Rebuilt supplied and uniform drivers also pass a RANDOM-01 snapshot smoke check ([evidence](results/reproducer-validation.json)).

Binaries, full trajectories and raw logs remain ignored under `runs/`. Each build/run directory must be new; GRID submission commands return before the scheduled job finishes.
'''
    (HERE/'README.md').write_text(text)

if __name__=='__main__':main()
