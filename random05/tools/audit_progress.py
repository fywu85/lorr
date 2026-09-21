#!/usr/bin/env python3
"""Check each recorded frontier against its frozen full run and source commit."""
import argparse
import datetime
import hashlib
import json
from pathlib import Path
from result_horizon import summary_steps, executed_steps
import re
import subprocess
from audit_task_waits import audit as audit_task_waits

ROOT = Path(__file__).resolve().parents[2]
SOURCES = ('src/engine.cpp', 'src/engine.hpp', 'src/Entry.cpp', 'CMakeLists.txt')


def read(path):
    return json.loads(path.read_text())


def audit():
    raw_spec_index = None

    def raw_result(spec, result, evidence):
        nonlocal raw_spec_index
        raw_path = ROOT / 'runs/random05' / Path(evidence).relative_to('random05/results').parent / result['name'] / 'result.json'
        if not raw_path.exists():
            # Early archived batch names differ from their original run paths.
            # Resolve by the complete frozen specification, never by makespan.
            if raw_spec_index is None:
                raw_spec_index = {}
                run_root = ROOT / 'runs/random05'
                for candidate in list(run_root.glob('*/spec.json')) + list(run_root.glob('*/*/spec.json')):
                    raw_spec = read(candidate)
                    if raw_spec.get('kind') == 'benchmark':
                        key = raw_spec['created_utc']
                        assert key not in raw_spec_index, ('ambiguous frozen specification', key)
                        raw_spec_index[key] = candidate
            raw_spec_path = raw_spec_index[spec['created_utc']]
            assert read(raw_spec_path) == spec, (evidence, 'archived/raw specification mismatch')
            raw_path = raw_spec_path.parent / result['name'] / 'result.json'
        assert read(raw_path.parent / 'summary.json') == result, (evidence, 'archived/raw summary mismatch')
        return raw_path

    def horizon(result, spec, evidence):
        if 'actual_path_steps' in result:
            return summary_steps(result)
        # Preserve historical summaries verbatim. Their original full trace
        # supplies the horizon metadata absent from the early runner schema.
        data = read(raw_result(spec, result, evidence))
        steps = executed_steps(data)
        assert result.get('steps', steps) == steps
        assert data['numTaskFinished'] == result['result']['numTaskFinished']
        times = data.get('entryComputeTimes', data.get('plannerTimes'))
        if times is not None:
            assert len(times) == steps, 'legacy timing horizon mismatch'
        return steps

    builds = {}
    for path in (ROOT / 'runs/random05').glob('build-*/completion.json'):
        completion = read(path)
        if completion.get('exit') == 0 and 'binary_sha256' in completion:
            builds[completion['binary_sha256']] = read(path.parent / 'spec.json')
    references = {}
    for name in ('nms4-full-v1', 'nms-original-full-v1', 'nms4-repeats-full-v3'):
        directory = ROOT / 'random05/results' / name
        specification = read(directory / 'spec.json')
        for reference in read(directory / 'summary.json'):
            assert reference['valid'] and horizon(reference, specification, str((directory / 'summary.json').relative_to(ROOT))) == 2000
            reference_case = next(c for c in specification['cases'] if c['name'] == reference['name'])
            references[reference['result']['numTaskFinished']] = (reference_case, read(directory / 'allocation.json'), name)
    report = []
    waiting_cache = {}
    text = (ROOT / 'RANDOM05_PROGRESS.md').read_text()
    table = text.split('| Completed UTC', 1)[1].split('## Reference evidence supplied', 1)[0]
    for row in table.splitlines():
        if not row.startswith('| 20'):
            continue
        fields = [x.strip() for x in row.split('|')[1:-1]]
        utc, commit_link, _, tasks, compute, nms_reference, gain, evidence_link = fields
        commit = re.search(r'/commit/([0-9a-f]+)', commit_link).group(1)
        evidence = re.search(r'\]\(([^)]+)\)', evidence_link).group(1)
        summaries = read(ROOT / evidence)
        matches = [s for s in summaries if s['finished_utc'] == utc
                   and s.get('result', {}).get('numTaskFinished') == int(tasks)]
        assert len(matches) == 1, (utc, evidence, 'missing or ambiguous result')
        result = matches[0]
        assert result['valid'] and result['exit'] == 0, (utc, 'invalid run')
        spec = read((ROOT / evidence).parent / 'spec.json')
        assert horizon(result, spec, evidence) == 2000, (utc, 'partial run')
        for key in ('numPlannerErrors', 'numScheduleErrors', 'numEntryTimeouts'):
            assert result['result'][key] == 0, (utc, key)
        spec = read((ROOT / evidence).parent / 'spec.json')
        case = next(c for c in spec['cases'] if c['name'] == result['name'])
        allocation = read((ROOT / evidence).parent / 'allocation.json')
        assert case.get('limit_ms', 1000) == 1000, (utc, 'frontier entry budget is not 1s')
        assert result['latency_seconds']['max'] <= 1.0, (utc, 'frontier entry exceeds 1s')
        assert result['usage']['peak_rss_kib'] * 1024 <= 32000000000, (utc, 'frontier exceeds 32GB RSS')
        cpu = allocation['resources']['cpu_model']
        workers = int(case['env']['R05_THREADS'])
        expected = '{} / {} / {}'.format(workers, case['cores'], cpu.split(' 32-Core')[0].replace('AMD ', ''))
        assert compute == expected, (utc, 'compute metadata mismatch')
        nms_tasks = int(nms_reference.split()[0].replace(',', ''))
        reference_case, reference_allocation, reference_name = references[nms_tasks]
        assert case.get('limit_ms', 1000) == reference_case.get('limit_ms', 1000), (utc, 'NMS entry-budget mismatch')
        assert case.get('preprocess_ms', 30000) == reference_case.get('preprocess_ms', 30000), (utc, 'NMS preprocessing-budget mismatch')
        for path, value in reference_case['input_hashes'].items():
            assert case['input_hashes'][path] == value, (utc, 'NMS input mismatch', path)
        expected_gain = (int(tasks) / nms_tasks - 1) * 100
        assert abs(float(gain.rstrip('%')) - expected_gain) <= 0.051, (utc, 'incorrect gain')
        matched = (cpu == reference_allocation['resources']['cpu_model']
                   and case['cores'] == reference_case['cores']
                   and case.get('smt', 1) == reference_case.get('smt', 1))
        digest = result['binary_sha256']
        assert digest == case['binary_sha256'], (utc, 'binary mismatch')
        build = builds[digest]
        sources = set(SOURCES) | {s for s in build['source_hashes'] if s.startswith('src/') and Path(s).suffix in ('.cpp', '.hpp', '.h')}
        for source in sorted(sources):
            content = subprocess.check_output(['git', 'show', commit + ':random05/' + source], cwd=ROOT)
            assert hashlib.sha256(content).hexdigest() == build['source_hashes'][source], (utc, commit, source)
        # The archived summary mirrors the raw runner directory. Keep waiting
        # metrics tied to this exact throughput-selected result, including the
        # unfinished tail rather than only orders that managed to finish.
        raw_path = raw_result(spec, result, evidence)
        if raw_path not in waiting_cache:
            waiting, raw_data = audit_task_waits(raw_path)
            trajectory = hashlib.sha256(json.dumps(raw_data['actualPaths'], separators=(',', ':')).encode()).hexdigest()
            if 'trajectory_sha256' in result:
                assert trajectory == result['trajectory_sha256'], (utc, 'raw trajectory mismatch')
            waiting_cache[raw_path] = waiting
        waiting = waiting_cache[raw_path]
        assert waiting['tasks_finished'] == int(tasks) and waiting['horizon_steps'] == 2000
        report.append(dict(utc=utc, source_commit=commit, tasks=int(tasks),
                           evidence=evidence, binary_sha256=digest, workers=workers,
                           physical_cores=case['cores'], cpu_model=cpu, nms_tasks=nms_tasks,
                           waiting_metrics=waiting,
                           nms_evidence='random05/results/' + reference_name + '/summary.json',
                           matched_cpu_model_and_allocation=matched,
                           entry_limit_ms=case.get('limit_ms', 1000),
                           maximum_entry_seconds=result['latency_seconds']['max'],
                           peak_rss_bytes=result['usage']['peak_rss_kib'] * 1024))
    assert report, 'no frontier rows found'
    return dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                checks=report, all_valid=True)


def waiting_markdown(report):
    lines = [
        '# RANDOM-05 order latency alongside throughput', '',
        'Throughput is the primary objective. These secondary metrics use each',
        'throughput-frontier run from RANDOM05_PROGRESS.md, without selecting a',
        'different run for latency. All durations are simulation steps.', '',
        'Completed latency is release to final waypoint; its mean and percentiles',
        'exclude unfinished orders. Percentiles use the nearest-rank convention.',
        'The oldest unfinished age and initial-cohort counts expose that censored',
        'tail. An age of 2,000 is the observation horizon, not a waiting-time bound.',
        'The initial cohort contains 1,200 orders. Later release times depend on',
        'throughput because completed orders trigger replacements.', '',
        'Source, input, allocation and raw-result hashes are checked by',
        '`tools/audit_progress.py` and recorded in [the audit](results/progress-audit.json).',
        'See [matched NMS waiting times](results/task-waiting-frontiers-20260920T1612/REPORT.md).', '',
        '| Completed UTC | Source | Tasks | Workers / physical cores | Completed mean | Completed p95 | Completed max | Oldest unfinished | Initial unfinished / unopened |',
        '|---|---|---:|---|---:|---:|---:|---:|---|']
    for row in report['checks']:
        waiting = row['waiting_metrics']
        def maximum(key):
            return waiting[key]['steps'] if waiting[key] else '-'
        lines.append('| {} | [{}](https://github.com/fywu85/lorr/commit/{}) | {} | {} / {} | {:.1f} | {} | {} | {} | {} / {} |'.format(
            row['utc'], row['source_commit'], row['source_commit'], row['tasks'],
            row['workers'], row['physical_cores'], waiting['completed_latency']['mean_steps'],
            waiting['completed_latency']['p95_steps'], maximum('maximum_release_to_completion'),
            maximum('oldest_unfinished'), waiting['initial_still_unfinished'], waiting['initial_still_unopened']))
    return '\n'.join(lines) + '\n'


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, default=ROOT / 'random05/results/progress-audit.json')
    args = parser.parse_args()
    report = audit()
    args.output.write_text(json.dumps(report, indent=2) + '\n')
    (ROOT / 'random05/WAITING_PROGRESS.md').write_text(waiting_markdown(report))
    print('Verified {} frontier rows against full runs and source hashes.'.format(len(report['checks'])))


if __name__ == '__main__':
    main()
