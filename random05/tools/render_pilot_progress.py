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
        'TRICK results require the corresponding `--trick INSTANCE` flag. RANDOM-03',
        'uses a tuned guidance field with the horizon rule off. RANDOM-04 and',
        'RANDOM-05 use both guidance and the known-horizon rule. All five RANDOM',
        'cases share one layout: this is density transfer, not unseen-map validation.', '',
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
    fresh = read('random05/results/fresh-validation-v5/audit.json')
    assert fresh['all_valid']
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
        'The four-core RANDOM-05 record stays separate: **3,770 versus matched NMS4',
        '2,914 (+29.4%)**. Its earlier frozen fresh comparison was +25.42%.',
        'The current 32-worker archived record averages 534 ms per step, peaks at',
        '622 ms, and uses 559 MB peak RSS. Its longest completed order takes 1,944',
        'steps; some initial orders remain unfinished at 2,000, so the eventual',
        'maximum wait is unknown. Throughput, rather than fairness, selected these runs.',
        '[Completed and censored waits](random05/results/task-waiting-frontiers-20260920T1612/REPORT.md),',
        '[fresh-input waits](random05/results/fresh-validation-v5/WAITING.md).', '',
        'Windowed LNS provides the current general gains on RANDOM-01 and RANDOM-02.',
        'On guided RANDOM-03, keeping 14 instead of 10 previous plan steps improves',
        'all three paired planner seeds by 3.27% in aggregate. Reactive planning',
        'remains stronger on the two crowded cases; the windowed transfer trials',
        'there were substantially worse. RANDOM-04 is still close to the local NMS',
        'baseline; deeper look-ahead and seed sensitivity are the current follow-up.', '',
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
