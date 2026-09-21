#!/usr/bin/env python3
"""Append verified full-run timing rows and render the RANDOM-03/04 timing table."""
import argparse
import datetime
import json
import math
from pathlib import Path
from result_horizon import summary_steps

ROOT = Path(__file__).resolve().parents[2]
DEST = ROOT / 'random05/results/random34-runtime'


def read(path):
    return json.loads(path.read_text())


def percentile(values, fraction):
    position = (len(values) - 1) * fraction
    low = int(position)
    high = min(low + 1, len(values) - 1)
    return values[low] + (values[high] - values[low]) * (position - low)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--add', nargs=3, action='append', default=[], metavar=('LABEL', 'BATCH', 'CASE'))
    args = parser.parse_args()
    report = read(DEST / 'timing.json')
    for label, batch, case in args.add:
        archived = ROOT / 'random05/results' / batch
        audit = read(archived / 'audit.json')
        audited = next(row for row in audit['rows'] if row['name'] == case)
        assert audited['valid'] and audited['replay']['tasks'] == audited['tasks']
        evidence = archived / case / 'summary.json'
        row = next(row for row in read(evidence) if row['name'] == case)
        raw = read(ROOT / 'runs/random05' / batch / case / case / 'result.json')
        values = sorted(float(v) * 1000 for v in raw['entryComputeTimes'])
        assert len(values) == row['steps'] == summary_steps(row)
        assert all(math.isfinite(v) and 0 <= v < 1000 for v in values)
        assert abs(max(values) - row['latency_seconds']['max'] * 1000) < 1e-6
        assert row['valid'] and row['result']['numTaskFinished'] == audited['tasks']
        assert row['usage']['peak_rss_kib'] * 1024 < 32000000000
        item = dict(label=label, batch=batch, case=case, tasks=audited['tasks'],
                    finished_utc=row['finished_utc'], binary_sha256=row['binary_sha256'],
                    seed=row['environment']['R05_SEED'], mean_ms=sum(values) / len(values),
                    p50_ms=percentile(values, .5), p95_ms=percentile(values, .95),
                    p99_ms=percentile(values, .99), max_ms=max(values),
                    peak_rss_mb=row['usage']['peak_rss_kib'] * 1024 / 1e6,
                    evidence=str(evidence.relative_to(ROOT)),
                    mean_logical_cpus=(row['usage']['user_seconds'] + row['usage']['system_seconds']) / row['usage']['wall_seconds'])
        old = next((r for r in report['rows'] if r['label'] == label), None)
        assert old is None or old == item, 'refusing to overwrite a different historical timing row'
        if old is None:
            report['rows'].append(item)
        print(label, audited['tasks'], 'max_ms', max(values), flush=True)
    report['checked_utc'] = datetime.datetime.now(datetime.timezone.utc).isoformat()
    (DEST / 'timing.json').write_text(json.dumps(report, indent=2) + '\n')
    lines = ['# RANDOM-03/04 runtime evidence', '',
             'All listed runs finish their full 800/1000-step horizons under a strict one-second limit. These observations do not establish a worst-case bound for unseen inputs.', '',
             '| Configuration | Tasks | Mean ms | p95 ms | p99 ms | Max ms | RAM MB | Mean logical CPUs |',
             '|---|---:|---:|---:|---:|---:|---:|---:|']
    for r in report['rows']:
        lines.append('| {label} | {tasks} | {mean_ms:.1f} | {p95_ms:.1f} | {p99_ms:.1f} | {max_ms:.1f} | {peak_rss_mb:.0f} | {mean_logical_cpus:.1f} |'.format(**r))
    current04 = [r for r in report['rows'] if r['label'] == 'R04-record2661' or r['label'].startswith('R04-2661-')]
    peak04 = math.ceil(max(r['max_ms'] for r in current04))
    lines += ['',
              'The 2,602-task RANDOM-03 recipe has an exact repetition and three planner seeds (5/0/3): 2,602/2,548/2,566 tasks. The original, repeat and both other seeds all peak below 710 ms. Frozen fresh inputs give 2,599/2,557 tasks: +11.69%/+9.13% against the stronger NMS repetition, +10.41% aggregate. [Frozen validation](../../RANDOM03_FRESH_VALIDATION_V1.md).', '',
              'The 2,661-task RANDOM-04 recipe has exact repetitions and four planner seeds (0/3/4/5): 2,542/2,576/2,661/2,605 tasks. Across the listed originals, repetitions and equivalent controls, the maximum is below {} ms. The original build96 record and its seed checks peak below 490 ms; newer source controls are listed separately. Runtime has measured margin; throughput remains 177 tasks short of the 2,838 target. The older 2,641-task profile and its five-seed qualification are retained above as history.'.format(peak04), '',
              'The 2,698-task admission profile is checked on seeds0/3/4/5:2701/2654/2698/2705. All original-source qualification runs peak below671 ms. Every seed improves over the corresponding uncapped profile; initial unfinished orders increase on the record seed. The exact8byte-cache and fast-matching controls are listed separately.', '',
              'The 2,738-task profile (cap560, cutoff1) has an exact repetition and four seeds0/3/4/5:2718/2689/2738/2735. Original-source qualification peaks below671 ms; equivalent newer-source controls peak below685 ms. The 2,741-task cutoff0.875 record and its subsequent seed checks remain separately listed.', '',
              'The contrast2.2, dispersion0.4, cap560, cutoff0.875 profile has four seeds0/3/4/5:2730/2711/2776/2776, all maxima below630 ms. Its original-binary seed4 repetition preserves all six trace fields and peaks at622.264 ms; newer equivalent controls are separately listed. The preceding contrast2.4 profile has an exact repetition at729.537 ms; that slower observation is retained.', '',
              'The 2,777-task horizon-matching profile repeats exactly in all six trace fields. Seeds0/1/2/3/4/5/6/7 score2726/2757/2766/2742/2777/2771/2753/2753; original, repeat and all seven additional seed runs peak below791 ms. It still needs a higher throughput candidate and frozen fresh-input validation.', '',
              'The current throughput frontier and remaining target are maintained in [PILOT_PROGRESS.md](../../../PILOT_PROGRESS.md) and [the campaign](../../RANDOM34_CAMPAIGN.md). A higher score does not inherit the timing qualification of a different cutoff or admission profile.', '',
              'CPU averages count occupied logical CPUs over the process lifetime, including initialization and OpenMP waiting. Each allocation contains 16 physical cores / 32 logical CPUs. RAM is peak RSS in decimal MB.', '',
              'Higher-work and other timed-out configurations remain in the campaign audits and are not represented as successful configurations here. [Machine-readable timings and evidence](timing.json).']
    (DEST / 'REPORT.md').write_text('\n'.join(lines) + '\n')


if __name__ == '__main__':
    main()
