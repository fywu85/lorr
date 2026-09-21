#!/usr/bin/env python3
"""Refresh the general PILOT progress dashboard from verified record manifests."""
import datetime
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
INSTANCES = ('WAREHOUSE', 'SORTATION', 'CITY-01', 'CITY-02', 'GAME',
             'RANDOM-01', 'RANDOM-02', 'RANDOM-03', 'RANDOM-04', 'RANDOM-05')


def read(path):
    return json.loads((ROOT / path).read_text())


def render():
    records = read('random05/random-frontiers.json')
    published = read('random05/references/published-nms-combined-2024.json')
    selected = {}
    for instance, profiles in records.items():
        profile = max(('general', 'trick'), key=lambda key: profiles[key]['tasks'])
        row = profiles[profile]
        case = row['case']
        summary = next(s for s in read(row['evidence']) if s['name'] == case['name'])
        assert summary['valid'] and summary['result']['numTaskFinished'] == row['tasks']
        assert summary['finished_utc'] == row['finished_utc']
        assert summary['result']['makespan'] == case['steps']
        assert summary['latency_seconds']['max'] <= 1
        assert summary['usage']['peak_rss_kib'] * 1024 < 32000000000
        if profile == 'general':
            assert not case['trick'] and case['env']['R05_GUIDANCE'] == 'none'
            assert case['env']['R05_HORIZON'] == '0'
        else:
            assert case['trick'] == instance or (case['trick'] is True and instance == 'RANDOM-05')
        selected[instance] = (profile, row, summary)
    now = datetime.datetime.now(datetime.timezone.utc).strftime('%Y-%m-%d %H:%M UTC')
    lines = ['# PILOT competition progress', '',
        'Updated {}. **PILOT** means **Pipelined Look-ahead with Task matching**.'.format(now),
        'It is the independent planner/scheduler developed from the colleague\'s log,',
        'with pipelined PIBT and parallel look-ahead for crowded traffic, plus optional',
        'windowed LNS for lighter traffic. Its results are separate from CGAR.', '',
        'Current development covers RANDOM-01 through RANDOM-05. The other five',
        'competition instances remain placeholders for future work. NMS is the target;',
        'throughput is primary, with order waiting times tracked as a secondary metric.', '',
        '**Active target:** RANDOM-03 at least **2,595** tasks and RANDOM-04 at least',
        '**2,838**, each 10% above matched local NMS, with robust subsecond runtime.',
        'Selected configurations must pass repeated full runs and fresh-input checks.',
        '[Campaign and qualification rules](random05/RANDOM34_CAMPAIGN.md).', '',
        'Selected scores are complete combined-track runs with enforced **1,000 ms**',
        'entry limits, **30 s** preprocessing, and a **32 decimal GB** process limit.',
        'These rows use **16 physical EPYC9354 cores / 32 SMT workers**, with bound',
        'affinity and no CPU quota on shared GRID hosts. A deadline overrun fails the',
        'run; PILOT completes its declared fixed work instead of returning a partial search.', '',
        '| Instance | PILOT tasks | Published NMS | Difference | Profile | Seed | Max step (ms) |',
        '|---|---:|---:|---:|---|---:|---:|']
    for instance in INSTANCES:
        target = published['instances'][instance]['tasks']
        if instance not in selected:
            lines.append('| {} | — | {:,} | — | Not evaluated | — | — |'.format(instance, target))
            continue
        profile, row, summary = selected[instance]
        lines.append('| {} | {:,} | {:,} | {:+.2f}% | {} | {} | {:.2f} |'.format(
            instance, row['tasks'], target, 100*(row['tasks']/target-1),
            'GENERAL' if profile == 'general' else 'TRICK', row['case']['env']['R05_SEED'],
            1000*summary['latency_seconds']['max']))
    lines += ['',
        '**Published NMS scores are historical targets, not matched local baselines.**',
        'Their reported timeout labels for WAREHOUSE, SORTATION and GAME are preserved',
        'in the [target snapshot](random05/references/published-nms-combined-2024.json).',
        'A dash means no valid PILOT throughput result, not zero completed tasks.',
        'The frozen large-map distance representation was estimated at 95–189 GB,',
        'so those maps are deferred; no large-map throughput is claimed.',
        '[Capacity assessment](random05/GENERALIZATION.md#large-map-limits).', '',
        'The primary local comparison uses identical archived inputs and matched CPU',
        'allocations. All selected runs pass independent movement, collision,',
        'assignment and task-event replay checks.', '',
        '| Instance | PILOT | Matched local NMS32 | Difference |',
        '|---|---:|---:|---:|']
    for instance in INSTANCES:
        if instance in selected:
            _, row, _ = selected[instance]
            nms = records[instance]['nms32_tasks']
            lines.append('| {} | {:,} | {:,} | {:+.2f}% |'.format(instance, row['tasks'], nms, 100*(row['tasks']/nms-1)))
    lines += ['',
        'These are selected individual bests, not an average or one universal preset.',
        'GENERAL means no map-specific guidance or known-horizon rule was enabled;',
        'the general algorithm settings were still selected during development.',
        'TRICK results require the corresponding `--trick INSTANCE` flag.',
        'Selected tuned guidance: {}.'.format(', '.join(i for i, (_, r, _) in selected.items()
            if r['case']['env'].get('R05_GUIDANCE', 'none') != 'none') or 'none'),
        'Selected known-horizon rules: {}.'.format(', '.join(i for i, (_, r, _) in selected.items()
            if int(r['case']['env'].get('R05_HORIZON', 0))) or 'none'),
        'All five RANDOM cases share one layout: this is density transfer,',
        'not unseen-map validation.', '',
        '| Instance | General best | Explicit-trick best |',
        '|---|---:|---:|']
    for instance in INSTANCES:
        if instance in records:
            lines.append('| {} | {:,} | {:,} |'.format(instance, records[instance]['general']['tasks'], records[instance]['trick']['tasks']))
        else:
            lines.append('| {} | — | — |'.format(instance))
    lines += ['',
        'Current selected records are pinned to their completion timestamps and source commits:', '',
        '| Instance | Completed UTC | Source | Full-run evidence |',
        '|---|---|---|---|']
    for instance in INSTANCES:
        if instance not in selected:
            continue
        _, row, _ = selected[instance]
        commit = row['source_commit']
        lines.append('| {} | {} | [{}](https://github.com/fywu85/lorr/commit/{}) | [Run]({}) |'.format(
            instance, row['finished_utc'], commit, commit, row['evidence']))
    current05 = selected['RANDOM-05'][2]
    history = read('random05/results/progress-audit.json')['checks']
    current05_check = next(c for c in reversed(history)
                           if c['utc'] == selected['RANDOM-05'][1]['finished_utc']
                           and c['tasks'] == selected['RANDOM-05'][1]['tasks'])
    waits05 = current05_check['waiting_metrics']
    four = read('random05/best-four-cores.json')
    four_summary = next(r for r in read(four['evidence']) if r['name'] == four['case']['name'])
    nms4 = 2914
    fresh = read('random05/results/fresh-validation-v5/audit.json')
    assert fresh['all_valid']
    fresh03 = read('random05/results/random03-fresh-validation-v1/audit.json')
    assert fresh03['all_valid']
    lines += ['',
        '**RANDOM-03 has crossed the archived ten-percent target:** 2,602 versus',
        '2,359 matched NMS (+10.30%). The frozen fresh-input comparison gives',
        '2,599/2,557 versus the stronger NMS repetitions 2,327/2,343:',
        '**+{:.2f}% aggregate**, with individual gains +11.69% and +9.13%.'.format(fresh03['aggregate_gain_percent']),
        'All eight fresh runs passed timing/resource checks and independent replay.',
        'The candidate stays below 701 ms on both fresh inputs; its archived exact',
        'repeat and two other planner seeds peak below 675 ms. RANDOM-04 remains',
        'short of its throughput target. Its 2,661-task configuration has four',
        'planner seeds and exact repetitions; the selected qualification runs',
        'stay below 491 ms per step. A later equivalent source control peaked',
        'at 756 ms; all observed controls still meet the one-second limit.',
        '[Full timing evidence](random05/results/random34-runtime/REPORT.md).',
        '[Frozen RANDOM-03 comparison](random05/RANDOM03_FRESH_VALIDATION_V1.md).', '']
    lines += ['',
        '**RANDOM-05\'s 4,000-task milestone is independently verified.** The archived',
        '4,011-task configuration repeated exactly. Frozen fresh task/start inputs',
        '50009 and 50010 produced 3,912 and 4,047 versus the stronger NMS repetitions',
        '3,095 and 3,190: **+{:.2f}% in aggregate**. The improvement over the immediately'.format(fresh['aggregate_gain_percent']),
        'previous configuration is +0.442%, positive on both fresh inputs. The fresh',
        '4,047 belongs to a different input and does not replace the archived 4,011.',
        'All eight original fresh runs passed strict timing, resource and replay checks.',
        '[Frozen comparison](random05/FRESH_VALIDATION_V5.md),',
        '[milestone audit](random05/results/completion-audit-4000/audit.json).', '',
        'The four-core RANDOM-05 record stays separate: **{:,} versus matched NMS4'.format(four_summary['result']['numTaskFinished']),
        '{:,} ({:+.1f}%)**. Its earlier frozen fresh comparison was +25.42%.'.format(nms4, 100*(four_summary['result']['numTaskFinished']/nms4-1)),
        'The current 32-worker archived record averages {:.0f} ms per step, peaks at'.format(1000*current05['latency_seconds']['mean']),
        '{:.0f} ms, and uses {:.0f} MB peak RSS. Its longest completed order takes {:,}'.format(
            1000*current05['latency_seconds']['max'], current05['usage']['peak_rss_kib']*1024/1e6,
            waits05['maximum_release_to_completion']['steps']),
        'steps; some initial orders remain unfinished at 2,000, so the eventual',
        'maximum wait is unknown. Throughput, rather than fairness, selected these runs.',
        '[Completed and censored waits](random05/results/task-waiting-frontiers-20260920T1612/REPORT.md),',
        '[fresh-input waits](random05/results/fresh-validation-v5/WAITING.md).', '',
        'Windowed LNS provides the current general gains on RANDOM-01 and RANDOM-02.',
        'On guided RANDOM-03, keeping 14 instead of 10 previous plan steps improves',
        'all three paired planner seeds by 3.27% in aggregate. Reactive planning',
        'remains stronger on the two crowded cases; the windowed transfer trials',
        'there were substantially worse. RANDOM-04 is still close to the local NMS',
        'baseline; improving throughput and timing headroom there is the next priority.',
        'An earlier depth16 profile reached 2,565 on planner seed4, while seed6',
        'gives 2,536. Seeds0 and3 fail the strict deadline (1,197 and 1,007 ms);',
        'the original failures remain recorded. Depth18 gives 2,525. An invalid',
        'depth20 work count is rejected at initialization and has no throughput score.',
        '[All six original outcomes](random05/results/random04-depth16-validation-split-full-v80/audit.json).', '',
        'Recent exact controls reduce runtime without changing the selected actions:',
        'the shared task-ranking cache preserves 2,634 tasks on RANDOM-04 at a',
        'measured 447 ms mean / 485 ms maximum. The four-way A-star heap preserves',
        '2,551 and 2,552 on RANDOM-03, with mean reductions of about 4–7%; maxima',
        'vary across those comparisons. These are measured full-run results, not',
        'a universal latency guarantee. Strict timing failures remain in the logs.',
        '[Runtime evidence](random05/results/random34-runtime/REPORT.md),',
        '[heap comparisons](random05/results/random03-heap-runtime-split-full-v97/audit.json).', '',

        '[Detailed RANDOM-05 throughput history](RANDOM05_PROGRESS.md) remains the',
        'authoritative RANDOM-05 log, as requested. This file is the general PILOT',
        'dashboard and will gain non-RANDOM entries when valid results exist.',
        '[Full density experiment history](random05/RANDOM_PROGRESS.md),',
        '[exact configurations](random05/random-frontiers.json),',
        '[continuation notes](random05/NEXT.md).', '',
        'Refresh this dashboard after promoting a verified record with',
        '`python3 random05/tools/render_pilot_progress.py`. Evidence and historical',
        'failures stay in the detailed logs; missing map results remain placeholders.', '']
    (ROOT / 'PILOT_PROGRESS.md').write_text('\n'.join(lines))
    print('Updated PILOT_PROGRESS.md: {} evaluated instances, {} placeholders.'.format(len(selected), len(INSTANCES)-len(selected)))


if __name__ == '__main__':
    render()
