#!/usr/bin/env python3
"""Refresh the general PILOT progress dashboard from verified record manifests."""
import datetime
import json
import math
import re
from pathlib import Path
from result_horizon import summary_steps

ROOT = Path(__file__).resolve().parents[2]
INSTANCES = ('WAREHOUSE', 'SORTATION', 'CITY-01', 'CITY-02', 'GAME',
             'RANDOM-01', 'RANDOM-02', 'RANDOM-03', 'RANDOM-04', 'RANDOM-05')


def read(path):
    return json.loads((ROOT / path).read_text())


def render():
    records = read('random05/random-frontiers.json')
    published = read('random05/references/published-nms-kk-combined-2024.json')
    matched=read('random05/references/matched-nms-kk-combined.json')
    local03=matched['archived']['RANDOM-03']['target']
    local04=matched['archived']['RANDOM-04']['target']
    goal03=math.ceil(local03*1.1);goal04=math.ceil(local04*1.1)
    selected = {}
    for instance, profiles in records.items():
        profile = max(('general', 'trick'), key=lambda key: profiles[key]['tasks'])
        row = profiles[profile]
        case = row['case']
        summary = next(s for s in read(row['evidence']) if s['name'] == case['name'])
        assert summary['valid'] and summary['result']['numTaskFinished'] == row['tasks']
        assert summary['finished_utc'] == row['finished_utc']
        assert summary_steps(summary) == case['steps']
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
        'competition instances remain placeholders for future work. The reference is',
        '**max(NMS, Kitty Knight)** for each instance;',
        'throughput is primary, with order waiting times tracked as a secondary metric.', '',
        '**Active objective:** maximize verified throughput across all five RANDOM instances.',
        '[Full goal and constraints](random05/ACTIVE_GOAL.md). RANDOM-03/04 are milestones within this campaign.', '',
        '**Current qualification milestones:** RANDOM-03 at least **{:,}** tasks and RANDOM-04'.format(goal03),
        'at least **{:,}**, each 10% above matched max(NMS,KK), with robust subsecond runtime.'.format(goal04),
        'Selected configurations must pass repeated full runs and fresh-input checks.',
        '[Campaign and qualification rules](random05/RANDOM34_CAMPAIGN.md).', '',
        'Selected scores are complete combined-track runs with enforced **1,000 ms**',
        'entry limits, **30 s** preprocessing, and a **32 decimal GB** process limit.',
        'These rows use **16 physical EPYC9354 cores / 32 SMT workers**, with bound',
        'affinity and no CPU quota on shared GRID hosts. A deadline overrun fails the',
        'run; PILOT completes its declared fixed work instead of returning a partial search.', '',
        '| Instance | PILOT tasks | Matched max(NMS, KK) | Reference | Difference | Profile | Seed | Max step (ms) |',
        '|---|---:|---:|---|---:|---|---:|---:|']
    published_rows=['| Instance | PILOT | Published max(NMS, KK) | Reference | Difference |',
                    '|---|---:|---:|---|---:|']
    for instance in INSTANCES:
        nms_target = published['teams']["No Man's Sky"]['score_details'][instance]['my_metric']
        kk_target = published['teams']['Kitty Knight']['score_details'][instance]['my_metric']
        target = max(nms_target, kk_target)
        reference = 'NMS' if nms_target >= kk_target else 'KK'
        historical_tasks=selected[instance][1]['tasks'] if instance in selected else None
        published_rows.append('| {} | {} | {:,} | {} | {} |'.format(instance,
            '{:,}'.format(historical_tasks) if historical_tasks is not None else '—',target,reference,
            '{:+.2f}%'.format(100*(historical_tasks/target-1)) if historical_tasks is not None else '—'))
        if instance not in selected:
            lines.append('| {} | — | — | — | — | Not evaluated | — | — |'.format(instance))
            continue
        profile, row, summary = selected[instance]
        local=matched['archived'].get(instance)
        target=local['target'] if local else None
        reference='Pending'
        if local and local['nms'] and local['kk']:
            reference='NMS' if local['nms']['tasks']>=local['kk']['tasks'] else 'KK'
        lines.append('| {} | {:,} | {} | {} | {} | {} | {} | {:.2f} |'.format(
            instance,row['tasks'],'{:,}'.format(target) if target is not None else 'Pending',reference,
            '{:+.2f}%'.format(100*(row['tasks']/target-1)) if target is not None else 'Pending',
            'GENERAL' if profile == 'general' else 'TRICK',row['case']['env']['R05_SEED'],
            1000*summary['latency_seconds']['max']))
    lines += ['',
        '**Headline comparisons use the stronger matched local result from NMS and Kitty Knight.**',
        'KK sets the RANDOM-01/02 references; NMS sets RANDOM-03/04/05.',
        'Both teams have valid measurements on all five RANDOM instances.',
        'RANDOM-05 KK uses its unchanged binary with `MALLOC_ARENA_MAX=2`;',
        'both allocator-only repeats score2,085 and pass strict limits and replay.',
        'Original virtual-address exhaustion failures remain in the [baseline audit](random05/NMS_KK_COMPARISON.md).',
        'Retain the strongest historical and new valid baseline; a missing team is never zero.',
        '[Comparison policy](random05/COMPARISON_POLICY.md).',
        'A dash means no valid PILOT throughput result, not zero completed tasks.',
        'The frozen large-map distance representation was estimated at 95–189 GB,',
        'so those maps are deferred; no large-map throughput is claimed.',
        '[Capacity assessment](random05/GENERALIZATION.md#large-map-limits).', '',
        'The local comparisons use identical archived inputs and matched CPU',
        'allocations. All selected runs pass independent movement, collision,',
        'assignment and task-event replay checks.', '',
        '| Instance | PILOT | Local NMS32 | Local KK32 | Matched max(NMS, KK) | Difference |',
        '|---|---:|---:|---:|---:|---:|']
    for instance in INSTANCES:
        if instance in selected:
            _, row, _ = selected[instance]
            reference=matched['archived'][instance]
            nms=reference['nms']['tasks'] if reference['nms'] else None
            kk=reference['kk']['tasks'] if reference['kk'] else None
            target=reference['target']
            lines.append('| {} | {:,} | {} | {} | {} | {} |'.format(instance,row['tasks'],
                '{:,}'.format(nms) if nms is not None else 'Pending',
                '{:,}'.format(kk) if kk is not None else 'Pending',
                '{:,}'.format(target) if target is not None else 'Pending',
                '{:+.2f}%'.format(100*(row['tasks']/target-1)) if target is not None else 'Pending'))
    lines += ['', '**Published scores below are historical orientation, not matched local comparisons.**',
        'NMS reported timeout labels for WAREHOUSE, SORTATION and GAME are preserved',
        'in the [target snapshot](random05/references/published-nms-kk-combined-2024.json).', '']+published_rows
    lines += ['',
        'These are selected individual bests, not an average or one universal preset.',
        'GENERAL means no map-specific guidance or known-horizon rule was enabled;',
        'the general algorithm settings were still selected during development.',
        'TRICK results require the corresponding `--trick INSTANCE` flag.',
        'Selected tuned guidance: {}.'.format(', '.join(i for i, (_, r, _) in selected.items()
            if r['case']['env'].get('R05_GUIDANCE', 'none') != 'none') or 'none'),
        'Selected known-horizon rules: {}.'.format(', '.join(i for i, (_, r, _) in selected.items()
            if int(r['case']['env'].get('R05_HORIZON', 0))) or 'none'),
        'Selected task-admission caps: {}. Opened tasks remain protected; all robots remain movable.'.format(', '.join('{}={}'.format(i,r['case']['env']['R05_ACTIVE_TASK_CAP']) for i,(_,r,_) in selected.items() if int(r['case']['env'].get('R05_ACTIVE_TASK_CAP',0))) or 'none'),
        'All five RANDOM cases share one layout: this is density transfer,',
        'not unseen-map validation.', '',
        '| Instance | General best | General vs max | Explicit-trick best | Trick vs max | Matched max(NMS, KK) |',
        '|---|---:|---:|---:|---:|---:|']
    for instance in INSTANCES:
        if instance in records:
            general=records[instance]['general']['tasks']
            trick=records[instance]['trick']['tasks']
            target=matched['archived'][instance]['target']
            lines.append('| {} | {:,} | {} | {:,} | {} | {} |'.format(instance,general,
                '{:+.2f}%'.format(100*(general/target-1)) if target else 'Pending',trick,
                '{:+.2f}%'.format(100*(trick/target-1)) if target else 'Pending',
                '{:,}'.format(target) if target else 'Pending'))
        else:
            lines.append('| {} | — | — | — | — | — |'.format(instance))
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
        '2,359 matched max(NMS,KK) (+10.30%). Its original NMS-only fresh comparison gives',
        '2,599/2,557 versus the stronger NMS repetitions 2,327/2,343:',
        '**+{:.2f}% aggregate**, with individual gains +11.69% and +9.13%.'.format(fresh03['aggregate_gain_percent']),
        'KK was not measured on those V1 streams; this is not a fresh two-team maximum.',
        'All eight fresh runs passed timing/resource checks and independent replay.',
        'The candidate stays below 701 ms on both fresh inputs; its archived exact',
        'repeat and two other planner seeds peak below 675 ms.',
        'The earlier 2,606-task routing-cost refinement is a development result;',
        'the preceding fresh-input validation does not qualify that change.',
        'The 2,606 profile repeats exactly. Paired seeds5/0/3 score2,606/2,572/2,590',
        'versus2,602/2,548/2,566, +0.674% in aggregate and positive on each;',
        'all pass full replay and peak below767ms.',
        'The later heuristic-priority search reaches **2,620** and repeats exactly.',
        'Five paired development seeds total13,016 versus12,974 (+0.324%), with',
        'three gains and two losses. The new same-source seed3 weight1 control',
        'failed at1046.391ms; its earlier valid source132 baseline is separately',
        'identified in the [paired report](random05/results/random03-record2620-split-full-v153/paired-comparison.json).',
        'Frozen fresh V2 finishes2,617/2,609 versus the previous search2,612/2,620:',
        '**-0.115% aggregate versus the previous search**, with improved mean',
        'latency455.6/480.2ms versus553.1/545.7ms. The supplemented fresh',
        'max(NMS,KK) comparison is **+8.852%**, below10%. NMS is stronger on both',
        'streams; the four later unmodified KK repeats also pass replay and timing.',
        '[Fresh V2 report](random05/RANDOM03_FRESH_VALIDATION_V2.md).',

        'RANDOM-01 reaches739 with observed-progress horizon correction0.25/span32,',
        'added to the735profile. The exact repeat passes all six fields. Five paired',
        'planner seeds736/739/726/732/732 versus732/735/725/729/730 give+0.3835%,',
        'all positive. Maximum qualification entry118.656ms. This is a modest',
        'development gain; fresh01V1still qualifies the earlier729profile only.',
        '[Paired evidence](random05/results/random01-record739-split-full-v162/REPORT.md).', '',
        'The preceding2,621 physical-deadline gate record repeats exactly. Five paired',
        'development seeds total13,029 versus12,999 (+0.231%, four gains/one loss).',
        'The gate alone has not received standalone fresh-input qualification.',
        '[Paired evidence](random05/results/random03-record2621-split-full-v162/paired-seeds.json).', '',
        'The2,646 selected result adds observed-progress horizon correction',
        '0.25/span32 to the2,621profile. Full replay and strict limits pass:',
        'mean484.103/max676.507ms, RSS469.467MB. The unchanged2,621control is',
        'exact in all six fields, as is the2,646repeat. Five paired planner seeds',
        '2611/2606/2611/2646/2610 total13,084versus13,029 (+0.4221%, all positive).',
        'Maximum qualification entry730.523ms. Selectingseed3is included.',
        '[Paired evidence](random05/results/random03-record2646-split-full-v162/REPORT.md).', '',
        '**Frozen fresh V3 validates the2,646 profile on two new task/start inputs:**',
        '2,660/2,622 versus the prior2,620 recipe2,639/2,612 (+0.590% aggregate,',
        'positive on both). Both teams were frozen in the original protocol:',
        'NMS2,461/2,444 and2,361/2,380; KK2,084/2,093 and2,094/2,130.',
        '**+{:.3f}% aggregate versus max(NMS,KK)**, with individual gains8.086%/10.168%.'.format(matched['fresh']['RANDOM-03']['gain_percent']),
        'This still misses the10% fresh-input milestone. All12runs pass strict',
        'source/input/runtime/bound-affinity checks and independent full replay.',
        'Candidate mean494.489/478.907ms, max727.685/677.620ms; RSS below465MB.',
        'These are same-layout validation streams, not new archived records.',
        '[Fresh V3 report](random05/RANDOM03_FRESH_VALIDATION_V3.md).', '',
        'RANDOM-04 currently reaches **{:,}** ({:+.2f}% above matched max(NMS,KK)),'.format(selected['RANDOM-04'][1]['tasks'],100*(selected['RANDOM-04'][1]['tasks']/local04-1)),
        '**{} tasks short** of{:,}. Its record peaks at{:.1f}ms;'.format(max(0,goal04-selected['RANDOM-04'][1]['tasks']),goal04,1000*selected['RANDOM-04'][2]['latency_seconds']['max']),
        'The earlier 2,777-task profile repeated exactly. Eight planner seeds score',
        '2,726–2,777; all original, repeat and seed checks peak below 791 ms.',
        'Frozen RANDOM-01/02 task/start checks are supplemented with KK repeats.',
        'The frozen selected profiles beat **max(NMS,KK) by4.37% and10.17%** in',
        'aggregate; their frozen general controls are ahead by3.86% and9.52%.',
        'Every input is positive. The later general R01record727 is not yet fresh-validated.',
        'These two streams per density use the same layout, not unseen maps.',
        '[RANDOM-01](random05/RANDOM01_FRESH_VALIDATION_V1.md),',
        '[RANDOM-02](random05/RANDOM02_FRESH_VALIDATION_V1.md).', '',
        'The new2,782 record uses task-specific observed progress to adjust the',
        'known-horizon estimate. Exact repetition passes; the new profile improves',
        "the eight-seed aggregate by0.340% (six gains, two losses). Seed11's later",
        'extension fails the strict first-step deadline; it has no valid score.',
        '[Replication](random05/results/random124-seed-extension-split-full/paired-comparison.json).',
        'Fresh RANDOM-04 validation is still pending.',
        'The earlier 2,661-task configuration passed four planner seeds and exact',
        'repetitions below 491 ms; a later equivalent source control peaked at',
        '755.1 ms. That slower control remains in the timing evidence.',
        '[Full timing evidence](random05/results/random34-runtime/REPORT.md).',
        '[Frozen RANDOM-03 comparison](random05/RANDOM03_FRESH_VALIDATION_V1.md).', '']
    lines += ['',
        '**The 4,175-task admission profile passes frozen fresh validation V6:**',
        '4,182/4,177 versus the stronger NMS repetitions 3,155/3,178,',
        '**+31.99% in aggregate** and +5.25% over the preceding 4,011 recipe.',
        'All eight runs pass strict timing/resource checks and independent replay;',
        'candidate maxima are 774/799 ms. Its archived exact repeat and four',
        'planner seeds also pass. This validates the frozen 4,175 profile,',
        'not the later cutoff/startup refinements or unseen layouts.',
        'KK was not measured on those V6 streams; the31.99% is NMS-only.',
        '[Frozen V6 comparison](random05/FRESH_VALIDATION_V6.md).',
        'The 4,197 profile now repeats exactly in all six trajectory fields;',
        'planner seeds0/1/2/3 score4,197/4,168/4,179/4,143. All full qualification',
        'runs pass independent replay, with maximum870.036ms and RSS below491MB.',
        '[Qualification](random05/results/random05-record4197-split-full-v132/audit.json).', '',
        'The preceding4,236 selected best adds mild startup task weighting and repeats exactly.',
        'Seeds0/1/2/3 score4,236/4,051/4,172/4,096: the aggregate is0.791% below',
        'the preceding4,197 profile, so this is a selected-seed record, not a',
        'replicated improvement. Fresh validation remains pending.',
        '[Replication](random05/results/random05-record4236-split-full-v132/paired-comparison.json).', '',
        'The preceding4,242 record adds observed-progress correction0.125/span32;',
        'mean589.171/max780.487ms, RAM489.562MB. It repeats exactly; four seeds',
        "score4,242/4,028/4,183/4,128 (+0.157% over4,236's profile, three gains",
        "and one loss; still0.635% below4,197's aggregate). Fresh validation is pending.",
        '[Evidence](random05/results/random05-startup-progress-split-full-v144/audit.json).', '',
        'The new4,254 selected record refines traffic-assignment guidance with80',
        'updates instead of20, keeping the same per-step work: mean579.781ms,',
        'max874.301ms, RSS490.107MB. All2,000steps pass independent replay and',
        'strict limits. The exact repeat matches all six trace fields. Four planner',
        'seeds4254/4189/4252/4253 improve all paired controls, +2.2134%aggregate,',
        'and average4237.0. Maximum qualification entry919.594ms; all pass.',
        'Selectingseed0is included; no fresh-input qualification is claimed.',
        '[Paired evidence](random05/results/random05-record4254-split-full-v162/REPORT.md).', '',
        '**The earlier RANDOM-05 4,000-task milestone is independently verified.** The archived',
        '4,011-task configuration repeated exactly. Frozen fresh task/start inputs',
        '50009 and 50010 produced 3,912 and 4,047 versus the stronger NMS repetitions',
        '3,095 and 3,190: **+{:.2f}% in aggregate**. The improvement over the immediately'.format(fresh['aggregate_gain_percent']),
        'previous configuration is +0.442%, positive on both fresh inputs. The fresh',
        '4,047 belongs to a different input. This validation applies to the frozen4,011',
        'configuration; see the detailed history for qualification of newer records.',
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
        '[Completed and censored waits](random05/WAITING_PROGRESS.md),',
        '[fresh-input waits](random05/results/fresh-validation-v5/WAITING.md).', '',
        'Windowed LNS provides the current general gains on RANDOM-01 and RANDOM-02.',
        'The 1,397 general RANDOM-02 record uses paired repair orders. Seven',
        'development seeds improve +0.36% in aggregate over one order at the same',
        'potential repair work; five improve, one ties, one loses. All pass full',
        'replay below739ms; the selected best repeats exactly. No horizon rule or',
        'guidance is enabled in that general profile.',
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
    campaign_path=ROOT/'random05/RANDOM34_CAMPAIGN.md'
    if campaign_path.exists():
        campaign=campaign_path.read_text()
        for instance in ('RANDOM-03','RANDOM-04'):
            target=matched['archived'][instance]['target'];threshold=(11*target+9)//10
            record=selected[instance][1]
            row='| {} | {:,} | {:,} | {:,} | {:,} |'.format(instance,record['tasks'],target,threshold,record['case']['steps'])
            pattern=r'^\| '+re.escape(instance)+r' \| [0-9,]+ \| [0-9,]+ \| [0-9,]+ \| [0-9,]+ \|$'
            campaign,count=re.subn(pattern,row,campaign,count=1,flags=re.MULTILINE)
            assert count==1,'missing campaign status row'
        campaign_path.write_text(campaign)
    (ROOT / 'PILOT_PROGRESS.md').write_text('\n'.join(lines))
    print('Updated PILOT_PROGRESS.md: {} evaluated instances, {} placeholders.'.format(len(selected), len(INSTANCES)-len(selected)))


if __name__ == '__main__':
    render()
