#!/usr/bin/env python3
"""Audit complete entry-time quantiles and process resources from frozen full runs."""
import argparse
import datetime
import hashlib
import json
from pathlib import Path
from dwell_audit import latency

ROOT = Path(__file__).resolve().parents[2]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--report', type=Path, required=True)
    parser.add_argument('--case-contains', required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    report = json.loads(args.report.read_text())
    selected = [r for r in report['rows'] if args.case_contains in r['case']]
    assert selected and not args.output.exists()
    rows = []
    for row in selected:
        evidence = ROOT / row['evidence']
        metadata = json.loads((evidence / 'run-metadata.json').read_text())[row['case']]
        summaries = json.loads((evidence / 'run-summaries.json').read_text())
        summary = summaries[row['case']][0]
        assert summary['valid'] and summary['entry_timing_valid'] and summary['memory_valid']
        raw_case = Path(metadata['source_binary']).parent / row['case']
        raw_file = raw_case / 'WAREHOUSE.json'
        with raw_file.open() as source:
            data = json.load(source)
        values = data['entryComputeTimes']
        assert len(values) == summary['entry_compute_samples'] == data['makespan'] == 5000
        assert data['teamSize'] == 10000 and data['numTaskFinished'] == row['tasks']
        assert all(0 <= value <= 1 for value in values)
        timing = latency(values)
        assert abs(timing['max_ms'] / 1000 - summary['entry_compute_max_seconds']) < 1e-10
        assert abs(timing['mean_ms'] - row['mean_entry_ms']) < 1e-8
        resource = summary['process_resources']
        cpu = (resource['user_seconds'] + resource['system_seconds']) / resource['wall_seconds']
        assert abs(cpu - row['average_cpu_cores']) < 1e-10
        assert resource['peak_rss_kib'] * 1024 == row['peak_rss_bytes'] < 32000000000
        rows.append(dict(case=row['case'], seed=row['seed'], tasks=row['tasks'],
                         evidence=str(evidence.relative_to(ROOT)), raw_case=str(raw_case),
                         binary_sha256=metadata['binary_sha256'],
                         latency_nearest_rank=timing,
                         slowest_step_zero_based=max(range(len(values)), key=values.__getitem__),
                         steps_over_500ms=sum(value > .5 for value in values),
                         average_cpu_cores=cpu, reserved_physical_cores=metadata['cpus_per_instance'],
                         allocation_utilization_percent=100 * cpu / metadata['cpus_per_instance'],
                         peak_rss_bytes=row['peak_rss_bytes'], wall_seconds=row['wall_seconds']))
        del data
        print(row['case'], timing, flush=True)
    result = dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                  source_report=str(args.report),
                  source_report_sha256=hashlib.sha256(args.report.read_bytes()).hexdigest(),
                  script_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
                  scope='Complete scheduler plus planner wall latency, 5000 samples per run. CPU is whole-process user+system time divided by wall time, including simulator, startup and logging; not a sampled instantaneous CPU value. RSS is the whole-process high-water mark. Quantiles use nearest rank.',
                  rows=rows)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2) + '\n')


if __name__ == '__main__':
    main()
