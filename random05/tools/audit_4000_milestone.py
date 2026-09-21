#!/usr/bin/env python3
"""Cross-check the frozen 4,000-task milestone and its independently replayed evidence."""
import datetime
import hashlib
import json
from pathlib import Path
import re
import subprocess

ROOT = Path(__file__).resolve().parents[2]
OUTPUT = ROOT / 'random05/results/completion-audit-4000/audit.json'


def read(path):
    return json.loads((ROOT / path).read_text())


def sha(path):
    return hashlib.sha256((ROOT / path).read_bytes()).hexdigest()


def main():
    evidence = {}
    requirements = []

    def proof(path):
        evidence[path] = sha(path)
        return read(path)

    def verified(requirement, paths, **details):
        requirements.append(dict(requirement=requirement, status='verified',
                                 evidence=paths, **details))

    original_path = 'random05/results/record3990-coupling-split-full-v80/audit.json'
    repeat_path = 'random05/results/record4011-repeat-split-full-v80/audit.json'
    exact_path = 'random05/results/record4011-repeat-split-full-v80/exact-equivalence.json'
    original = proof(original_path)
    record = next(r for r in original['rows'] if r['name'] == '32-record3990-mix1')
    repeat = proof(repeat_path)
    reproduced = repeat['rows'][0]
    equality = proof(exact_path)
    assert original['complete'] and repeat['complete'] and len(repeat['rows']) == 1
    assert record['valid'] and reproduced['valid']
    assert record['tasks'] == reproduced['tasks'] == equality['tasks'] == 4011
    assert record['tasks'] >= 4000
    fields = {'actualPaths', 'plannerPaths', 'actualSchedule', 'plannerSchedule', 'events', 'tasks'}
    assert set(equality['exact_fields']) == fields and all(equality['exact_fields'].values())
    for row in (record, reproduced):
        assert row['replay']['replay_valid'] and row['replay']['steps'] == 2000
        assert row['replay']['agents'] == 800 and row['waiting']['event_accounting_valid']
        assert sha(row['replay']['result']) == row['replay']['result_sha256']
    verified('Full 2,000-step RANDOM-05 combined record >=4,000 and exact independent repeat',
             [original_path, repeat_path, exact_path], tasks=4011, exact_fields=sorted(fields))

    fresh_path = 'random05/results/fresh-validation-v5/audit.json'
    replay_path = 'random05/results/fresh-validation-v5/replay-audit.json'
    order_path = 'random05/results/fresh-validation-v5/declaration-order.json'
    fresh, replay, order = proof(fresh_path), proof(replay_path), proof(order_path)
    protocol_path = 'random05/experiments/fresh-validation-v5-protocol.json'
    protocol = json.loads(subprocess.check_output(['git', 'show', 'e46f57b:' + protocol_path], cwd=str(ROOT)))
    assert fresh['protocol_commit'] == replay['protocol_commit'] == order['protocol_commit'] == 'e46f57b'
    assert order['generation_after_commit']
    assert datetime.datetime.fromisoformat(order['generated_utc']) > datetime.datetime.fromisoformat(order['protocol_committed_utc'])
    assert fresh['all_valid'] and replay['complete'] and replay['all_attempts_valid'] and not replay['pending']
    expected = {'seed{}-{}'.format(seed, kind) for seed in (50009, 50010)
                for kind in ('ours', 'baseline', 'nms-repeat1', 'nms-repeat2')}
    assert set(fresh['runs']) == set(replay['runs']) == expected
    for name, row in replay['runs'].items():
        assert row['valid'] and row['tasks'] == fresh['runs'][name]['tasks']
        assert row['replay']['replay_valid'] and row['replay']['steps'] == 2000
        assert row['waiting']['event_accounting_valid']
        assert sha(row['replay']['result']) == row['replay']['result_sha256']
    comparisons = fresh['comparisons']
    assert [c['seed'] for c in comparisons] == protocol['seeds'] == [50009, 50010]
    assert all(c['ours'] > max(c['nms_repeats']) == c['stronger_nms'] for c in comparisons)
    candidate_total = sum(c['ours'] for c in comparisons)
    baseline_total = sum(c['baseline'] for c in comparisons)
    nms_total = sum(c['stronger_nms'] for c in comparisons)
    assert candidate_total > baseline_total
    verified('Frozen fresh inputs: beat stronger NMS on each and previous configuration in aggregate',
             [fresh_path, replay_path, order_path, protocol_path], original_attempts=8,
             candidate_total=candidate_total, previous_total=baseline_total, stronger_nms_total=nms_total,
             gain_over_nms_percent=100 * (candidate_total / nms_total - 1),
             gain_over_previous_percent=100 * (candidate_total / baseline_total - 1))

    best = proof('random05/best-32-workers.json')
    assert proof('random05/best.json') == best
    four = proof('random05/best-four-cores.json')
    assert four['case']['cores'] == int(four['case']['env']['R05_THREADS']) == 4
    assert best['case']['cores'] == 16 and best['case']['smt'] == 2
    assert int(best['case']['env']['R05_THREADS']) == 32
    assert best['source_commit'] == record['source_commit'] == protocol['candidate_source_commit'] == 'a2ff2b2'
    assert best['case']['binary_sha256'] == protocol['candidate_binary_sha256']
    four_summary = next(r for r in read(four['evidence']) if r['name'] == four['case']['name'])
    assert four_summary['valid'] and four_summary['result']['numTaskFinished'] == 3770
    verified('Separate four-core and 32-worker records and matching source/binary identities',
             ['random05/best-four-cores.json', 'random05/best-32-workers.json'],
             four_core_tasks=3770, workers32_tasks=4011)

    resource_checks = []
    raw_runs = [(record['name'], Path(record['replay']['result']).parent),
                (reproduced['name'], Path(reproduced['replay']['result']).parent)]
    raw_runs += [(name, Path(row['replay']['result']).parent) for name, row in replay['runs'].items()]
    for name, work in raw_runs:
        case = read(work / 'case.json')
        summary = read(work / 'summary.json')
        allocation = read(work.parent / 'allocation.json')['resources']
        runner = (ROOT / work.parent / 'runner.py').read_text()
        assert case['steps'] == summary['result']['makespan'] == 2000
        raw_result = read(work / 'result.json')
        timing_field = 'plannerTimes' if '-nms-' in name else 'entryComputeTimes'
        times = raw_result[timing_field]
        assert len(times) == 2000 and all(0 <= value < 1 for value in times)
        assert max(times) == summary['latency_seconds']['max']
        if '-nms-' not in name:
            assert summary['result']['entryComputeSamples'] == 2000
        assert case['limit_ms'] == 1000 and case['preprocess_ms'] == 30000
        assert summary['valid'] and summary['exit'] == 0 and summary['latency_seconds']['max'] < 1
        assert summary['usage']['peak_rss_kib'] * 1024 < 32000000000
        assert all(summary['result'][k] == 0 for k in ('numPlannerErrors', 'numScheduleErrors', 'numEntryTimeouts'))
        assert allocation['physical_cores_visible'] == 16 and allocation['effective_cpu_quota'] is None
        assert len(case['cpus']) == 32 and set(case['cpus']) == set(allocation['logical_cpu_affinity'])
        assert allocation['cpu_model'] == 'AMD EPYC 9354 32-Core Processor'
        assert len(allocation['logical_cpus_by_physical_core']) == 16
        assert all(len(pair) == 2 for pair in allocation['logical_cpus_by_physical_core'])
        assert 'min(32000000000,inherited_as)' in runner
        assert "command=['/usr/bin/prlimit','--as='+str(as_limit),'--']+command" in runner
        if '-nms-' not in name:
            assert case['command'][case['command'].index('--trick') + 1] == 'RANDOM-05'
            assert int(case['env']['R05_THREADS']) == 32
        resource_checks.append(dict(name=name, timing_source=timing_field, timing_samples=len(times),
                                    maximum_entry_seconds=summary['latency_seconds']['max'],
                                    peak_rss_bytes=summary['usage']['peak_rss_kib'] * 1024,
                                    cpus=case['cpus'], runner_sha256=sha(work.parent / 'runner.py'),
                                    process_address_space_guard_bytes=32000000000))
    verified('Actual 16-core / 32-SMT allocation, strict timing and preserved 32 GB process guard',
             [original_path, repeat_path, fresh_path], original_runs_checked=len(resource_checks))

    gates_path = 'random05/results/trick-gates-v80/audit.json'
    gates = proof(gates_path)
    assert gates['binary_sha256'] == best['case']['binary_sha256']
    assert len(gates['cases']) == 3 and all(c['properly_rejected'] for c in gates['cases'])
    verified('Explicit instance trick flag on selected runs; frozen binary rejects missing/wrong flag', [gates_path])
    prefix = 'random05/results/completion-audit-4000/'
    compiled = proof(prefix + 'compiled-and-test-sources.json')
    assert compiled['regression_pass'] and compiled['source_commit'] == 'a2ff2b2'
    assert compiled['binary_sha256'] == best['case']['binary_sha256']
    assert len(compiled['checked_files']) == 35 and 'tests/engine_tests.cpp' in compiled['checked_files']
    verified('Compiled solver, simulator and test sources match frozen commit; regressions pass',
             [prefix + 'compiled-and-test-sources.json'], source_files_checked=35)
    inputs = proof(prefix + 'input-trace-audit.json')
    assert inputs['all_match'] and len(inputs['runs']) == 10
    assert set(inputs['runs']) == {name for name, _ in raw_runs}
    for row in inputs['runs'].values():
        assert row['all_match'] and row['starts_checked'] == 800
        assert sha(row['result']) == row['result_sha256']
        assert all(sha(path) == digest for path, digest in row['input_hashes'].items())
    verified('Independent replay traces use actual input starts and every revealed task chain',
             [prefix + 'input-trace-audit.json'], runs_checked=10)
    difference = proof(prefix + 'frozen-config-difference.json')
    assert all(p['only_environment_difference'] == {'R05_TRIAGE_GUIDED_MIX': ['0.75', '1']}
               for p in difference['pairs'])
    verified('Candidate and previous solver differ only in the declared cutoff mix',
             [prefix + 'frozen-config-difference.json'])

    preservation = proof(prefix + 'preservation.json')
    assert preservation['heldout_hashes_match'] and not preservation['tracked_result_or_experiment_deletions']
    assert [h['seed'] for h in preservation['heldout_inputs']] == list(range(50001, 50011))
    for bundle in preservation['heldout_inputs']:
        for filename, digest in bundle['hashes'].items():
            assert sha(Path(bundle['input']).parent / filename) == digest
    failures = proof(prefix + 'failure-inventory.json')
    for item in failures['preserved_unsuccessful_benchmark_attempts']:
        assert (ROOT / item['summary']).exists()
        if item.get('native_log'):
            assert (ROOT / item['native_log']).exists()
    for item in failures['preserved_failed_builds']:
        assert (ROOT / item['completion']).exists() and (ROOT / item['log']).exists()
    verified('Held-out inputs preserved and excluded from tuning; unsuccessful evidence retained',
             [prefix + 'preservation.json', prefix + 'failure-inventory.json'], heldout_seeds=list(range(50001, 50011)),
             unsuccessful_attempts=len(failures['preserved_unsuccessful_benchmark_attempts']),
             failed_builds=len(failures['preserved_failed_builds']))

    progress_path = 'random05/results/progress-audit.json'
    progress = proof(progress_path)
    assert progress['all_valid'] and len(progress['checks']) == 85
    assert progress['checks'][-1]['tasks'] == 4011
    text = (ROOT / 'RANDOM05_PROGRESS.md').read_text()
    table = text.split('| Completed UTC', 1)[1].split('## Reference evidence supplied', 1)[0]
    rows = [line for line in table.splitlines() if line.startswith('| 20')]
    assert len(rows) == len(progress['checks'])
    for line, check in zip(rows, progress['checks']):
        cells = [c.strip() for c in line.split('|')[1:-1]]
        assert cells[0] == check['utc'] and int(cells[3]) == check['tasks']
        commit = re.search(r'/commit/([0-9a-f]+)', cells[1]).group(1)
        assert commit == check['source_commit']
        assert check['waiting_metrics']['event_accounting_valid']
    consolidated = (ROOT / 'random05/RANDOM_PROGRESS.md').read_text()
    assert '4,011' in consolidated and '26.63%' in consolidated and 'a2ff2b2' in consolidated
    verified('85 timestamped commit-linked frontier rows and completed/censored waiting metrics',
             [progress_path, 'RANDOM05_PROGRESS.md', 'random05/RANDOM_PROGRESS.md',
              'random05/results/fresh-validation-v5/WAITING.md'], frontier_rows=85)

    report = dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                  objective='Reach and independently validate at least4,000 completed tasks on full2,000-step RANDOM-05 combined.',
                  all_required_work_verified=True, requirements=requirements,
                  source_commit=best['source_commit'], binary_sha256=best['case']['binary_sha256'],
                  archived_tasks=record['tasks'], archived_finished_utc=record['finished_utc'],
                  fresh_comparisons=comparisons, resource_checks=resource_checks, evidence_sha256=evidence,
                  limitations=[
                      'Selected archived-input maximum with declared guidance and known-horizon tricks.',
                      'Two fresh task/start streams on one layout; no unseen-layout or universal4,000 claim.',
                      'Fresh4,047 is a different input and does not replace archived4,011.',
                      'Completed-only waiting distributions are conditional; oldest unfinished orders are censored at2,000.',
                      'Failure inventory includes unsuccessful earlier attempts; those are not promoted.',
                      'RANDOM-01..04 development is separately authorized and is not required to claim this4,000 milestone.'
                  ])
    OUTPUT.write_text(json.dumps(report, indent=2) + '\n')
    print('All {} milestone requirements verified; archived4011, fresh7959 vsNMS6285.'.format(len(requirements)))


if __name__ == '__main__':
    main()
