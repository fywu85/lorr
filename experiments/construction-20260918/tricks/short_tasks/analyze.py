#!/usr/bin/env python3
"""Verify and replay the flagged short-task preference/lane factorial on GRID."""
import argparse
import datetime
import hashlib
import importlib.util
import json
import os
from pathlib import Path
import re
import shlex
import shutil
import subprocess
import sys

ROOT = next(p for p in Path(__file__).resolve().parents if (p / 'tools/cpu_resources.py').is_file())
BASE = ROOT / 'experiments/construction-20260918'
KEYS = ['CGAR_TRICK_LANES', 'CGAR_TRICK_SHORT_TASKS']


def read(p):
    return json.loads(p.read_text())


def write(p, value):
    p.write_text(json.dumps(value, indent=2) + '\n')


def digest(p):
    return hashlib.sha256(p.read_bytes()).hexdigest()


def module(name, p):
    spec = importlib.util.spec_from_file_location(name, p)
    result = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(result)
    return result


def numbers(line):
    return {k: float(v) if '.' in v else int(v) for k, v in re.findall(r'(\w+)=([-\d.]+)', line)}


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--raw', type=Path, required=True)
    p.add_argument('--output', type=Path, required=True)
    p.add_argument('--commit', required=True)
    p.add_argument('--hold-job')
    p.add_argument('--execute', action='store_true')
    a = p.parse_args()
    raw, out = a.raw.resolve(), a.output.resolve()
    support = raw / 'factorial-support'
    if not a.execute:
        support.mkdir(exist_ok=False)
        own = support / 'analyze.py'
        shutil.copy2(Path(__file__), own)
        files = [own]
        for name in ['sequences-20260918/analyze.py', 'motion-20260918/analyze.py',
                     'assignment-20260918/analyze.py', 'throughput-20260918-next/analyze_matrix.py',
                     'throughput-20260918-strict/analyze.py', 'construction-20260918/verify_full.py',
                     'construction-20260918/collect_cold.py']:
            dest = support / 'experiments' / name
            dest.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(ROOT / 'experiments' / name, dest)
            files.append(dest)
        references = {
            'profile.json': BASE / 'warehouse-regional-reference-variants.json',
            'generic-full.json': BASE / 'results/pickup-full-regions-six-seed-v44.json',
            'lane-full.json': BASE / 'results/trick-nms-warehouse-full-v54/trick-verification.json',
            'lane-screen.json': BASE / 'results/trick-nms-warehouse-screen-v54/trick-verification.json',
        }
        for name, path in references.items():
            shutil.copy2(path, support / name)
            files.append(support / name)
        pool = module('pool_source', BASE / 'bottleneck_review/pool_audit.py')
        shutil.copy2(BASE / 'decompose.py', raw / 'decompose.py')
        source = pool.augmented_source((BASE / 'decompose.cpp').read_text())
        marker = '    std::ofstream(argv[3]) << report.dump(2)'
        extra = r'''    report["chain_windows"] = json::array();
    long long previous_completed = 0;
    for (const auto& snapshot : report["pool_snapshots"]) {
        const int at = snapshot.at("step");
        long long revealed = 0, completed = 0;
        std::vector<long long> window;
        const int width = std::max(1, report.at("steps").get<int>() / 5);
        for (const auto& task : tasks) {
            if (task.revealed <= at) revealed += task.chain;
            if (task.finished >= 0 && task.finished <= at) completed += task.chain;
            if (task.finished > at - width && task.finished <= at && task.finished >= 0)
                window.push_back(task.chain);
        }
        const long long outstanding = snapshot.at("outstanding").at("chain").at("sum");
        if (revealed != completed + outstanding)
            throw std::runtime_error("revealed chain inventory does not conserve distance");
        report["chain_windows"].push_back({{"step", at}, {"revealed_chain_sum", revealed},
            {"completed_chain_sum", completed}, {"outstanding_chain_sum", outstanding},
            {"new_completed_chain_sum", completed - previous_completed}, {"completed_window", distribution(window)}});
        previous_completed = completed;
    }
'''
        assert source.count(marker) == 1
        (raw / 'decompose.cpp').write_text(source.replace(marker, extra + marker))
        files += [raw / 'decompose.py', raw / 'decompose.cpp']
        write(raw / 'factorial-request.json', dict(root=str(ROOT), commit=a.commit,
              files={str(f): digest(f) for f in files}, raw=str(raw), output=str(out)))
        command = ['/usr/bin/python3', str(own), '--execute', '--raw', str(raw), '--output', str(out), '--commit', a.commit]
        job = raw / 'factorial-analysis.sh'
        job.write_text('#!/bin/bash\nset -eu\nexec ' + ' '.join(map(shlex.quote, command)) + '\n')
        submit = ['qsub', '-h', '-terse', '-w', 'n', '-cwd', '-q', 'debian.q', '-pe', 'threaded', '1',
                  '-binding', 'linear:1', '-l', 'exclusive=false,h_rt=01:00:00,h_vmem=8G', '-m', 'n',
                  '-N', 'short_task_analysis', '-j', 'y', '-o', str(raw / 'factorial-analysis.log'), '-S', '/bin/bash']
        if a.hold_job:
            submit += ['-hold_jid', a.hold_job]
        submit.append(str(job))
        r = subprocess.run(submit, cwd=ROOT, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        receipt = dict(command=submit, returncode=r.returncode, response=r.stdout)
        write(raw / 'factorial-submission.json', receipt)
        write(raw / 'motion-analysis-submission.json', receipt)
        print(r.stdout, end='')
        if r.returncode:
            return r.returncode
        assert re.fullmatch(r'\d+\s*', r.stdout), r.stdout
        subprocess.run(['qrls', r.stdout.strip()], check=True)
        return 0

    request = read(raw / 'factorial-request.json')
    assert request['commit'] == a.commit and Path(request['root']) == ROOT
    for filename, sha in request['files'].items():
        assert digest(Path(filename)) == sha, filename
    sys.path.insert(0, str(ROOT / 'tools'))
    from cpu_resources import cpu_resources
    cpu = cpu_resources()
    assert cpu['effective_cpu_quota'] is None and cpu['representative_cpus']
    os.sched_setaffinity(0, cpu['representative_cpus'][:1])
    spec, build = read(raw / 'spec.json'), read(raw / 'build.json')
    assert spec['trick'] == 'WAREHOUSE' and spec['experiment_track'] == 'TRICK'
    assert spec['instances'] == ['WAREHOUSE'] and spec['time_limit_ms'] == 5000 and spec['cpus_per_instance'] == 4
    assert spec['benchmark_mode'] == 'relaxed_development' and not spec['exclusive_host']
    sources = dict(build['sources'], **build['test_sources'])
    for name, sha in sources.items():
        assert hashlib.sha256(subprocess.check_output(['git', 'show', a.commit + ':' + name], cwd=ROOT)).hexdigest() == sha, name
    assert digest(raw / 'lifelong') == build['binary_sha256']
    horizon = (spec['horizons'] or {'WAREHOUSE': 5000})['WAREHOUSE']
    assert horizon in (200, 5000)
    profile = next(iter(read(support / 'profile.json').values()))
    allocation, intervals, arms = read(raw / 'allocation.json'), [], {}
    initial_schedules, scheduling = {}, {}
    for case in spec['cases']:
        name, env, seed = case['name'], case['environment'], case['seed']
        lanes, short = (int(env[k]) for k in KEYS)
        assert lanes in (0, 1) and short in (0, 1)
        assert {k: v for k, v in env.items() if k not in KEYS} == profile
        arms.setdefault(seed, {})
        assert (lanes, short) not in arms[seed]
        arms[seed][lanes, short] = name
        meta, summary = read(raw / name / 'metadata.json'), read(raw / name / 'summary.json')[0]
        assert meta['trick_argv'] == ['--trick', 'WAREHOUSE'] and meta['experiment_track'] == 'TRICK'
        assert meta['trick'] == 'WAREHOUSE' and meta['build_provenance'] == build
        expected = dict(lanes=lanes, short_tasks=short, hrrn=1-short, oldest_admission=1-short)
        assert meta['expected_trick_components'] == expected
        assert meta['max_process_memory_bytes'] == 32000000000 and summary['memory_valid']
        assert summary['trick_receipt_valid'] and summary['experiment_track_valid']
        resources = meta['cpu_resources']
        assert resources['effective_cpu_quota'] is None and resources['cpu_model'] == 'AMD EPYC 9354 32-Core Processor'
        binding = summary['cpu']
        assert binding == meta['cpu_binding'] and len(binding) == 4 and set(binding).issubset(allocation['selected_cpus'])
        core_of = {c: tuple(group) for group in resources['logical_cpus_by_physical_core'] for c in group}
        cores = {(resources['hostname'], core_of[c]) for c in binding}
        assert len(cores) == 4
        begin, end = (datetime.datetime.fromisoformat(meta[k]) for k in ['started_utc', 'finished_utc'])
        for b, e, occupied in intervals:
            assert not (begin < e and b < end and cores.intersection(occupied))
        intervals.append((begin, end, cores))
        log = (raw / name / 'WAREHOUSE.log').read_text().splitlines()
        receipts = [line for line in log if line.startswith('[CGAR_TRICK_COMPONENTS]')]
        assert receipts == ['[CGAR_TRICK_COMPONENTS] instance=WAREHOUSE lanes=%d short_tasks=%d hrrn=%d oldest_admission=%d started_tasks=protected' % (lanes, short, 1-short, 1-short)]
        assert not any(line.startswith(('[cgar-chain-pricing]', '[cgar-temporal-score]')) for line in log)
        if summary['valid']:
            assert summary['makespan'] == summary['entry_compute_samples'] == horizon
            assert summary['peak_process_rss_bytes'] < 32000000000 and summary['entry_compute_max_seconds'] <= 5
            assert all(summary[k] == 0 for k in ['planner_errors', 'schedule_errors', 'timeouts', 'internal_timeouts', 'exit'])
            assert any(line.startswith('[cgar-flow]') for line in log) == (not lanes)
            globals_ = [numbers(line) for line in log if line.startswith('[cgar-temporal]')]
            regions = [numbers(line) for line in log if line.startswith('[cgar-temporal-regions]')]
            assert [r['step'] for r in globals_] == [r['step'] for r in regions] == list(range(200, horizon + 1, 200))
            assert all(r['candidate_limit'] == 4000000 and r['workers'] == r['threads'] == 1 for r in globals_)
            for r in regions:
                assert r['regions'] == r['threads'] == 4 and r['rounds'] == 2 and r['temperature_ppm'] == 1000
                assert r['repairs'] == (r['kept'] + r['reverted']) * 25000 and r['score_after'] + 1e-6 >= r['score_before']
            scheduler = [numbers(line) for line in log if line.startswith('[cgar-scheduler]')]
            scheduling[name] = scheduler[-1] if scheduler else None
            data = read(raw / name / 'WAREHOUSE.json')
            initial = []
            for encoded in data['actualSchedule']:
                first = [int(item.split(':')[1]) for item in encoded.split(',') if item and int(item.split(':')[0]) == 1]
                assert len(first) == 1 and first[0] >= 0
                initial.append(first[0])
            assert len(initial) == len(set(initial)) == 10000
            initial_schedules[name] = initial
            del data
            if short:
                scheduler = [numbers(line) for line in log if line.startswith('[cgar-scheduler]')]
                assert scheduler and all(r['fair'] == 0 for r in scheduler)
    assert sorted(arms) == ([0] if horizon == 200 else [0, 2])
    assert all(set(cases) == {(0, 0), (0, 1), (1, 0), (1, 1)} for cases in arms.values())
    for cases in arms.values():
        baseline = initial_schedules.get(cases[0, 0])
        for name in cases.values():
            if baseline is not None and name in initial_schedules:
                assert initial_schedules[name] == baseline, ('initial dispatch changed', name)
    helper = support / 'experiments/construction-20260918'
    if horizon == 200:
        subprocess.run(['/usr/bin/python3', str(helper / 'collect_cold.py'), '--input', str(raw), '--output', str(out)], check=True)
        summaries = read(out / 'run-summaries.json')
        fingerprints = read(out / 'trajectory-fingerprints.json')
        expected_lane = next(r['trajectory_sha256'] for r in read(support / 'lane-screen.json')['rows'] if r['seed'] == 0)
        for arm, sha in [((0, 0), 'a4bfe67f610e2451bb40e68da0e9fad0b75985d3f7701528334fee73f93cea8b'), ((1, 0), expected_lane)]:
            name = arms[0][arm]
            if summaries[name][0]['valid']:
                assert fingerprints[name] == sha, ('historical control changed', name)
        result = dict(full_run=False, all_valid=all(v[0]['valid'] for v in summaries.values()),
                      exact_generic_and_lane_controls=True)
        result['eligible_for_full'] = result['all_valid']
    else:
        subprocess.run(['/usr/bin/python3', str(support / 'experiments/sequences-20260918/analyze.py'),
                        '--input', str(raw), '--output', str(out), '--workers', '1'], check=True)
        verifier = module('full_verifier', helper / 'verify_full.py')
        verifier.ROOT = ROOT
        result = verifier.verify(raw, out, a.commit, allow_failed=True, decision_limit_ms=5000)
        rows = {r['case']: r for r in result['rows']}
        generic = {r['seed']: r for r in read(support / 'generic-full.json')['rows'] if r['environment']['CGAR_TEMPORAL_REGIONS'] == '4'}
        lanes = {r['seed']: r for r in read(support / 'lane-full.json')['rows']}
        for seed, cases in arms.items():
            for arm, refs in [((0, 0), generic), ((1, 0), lanes)]:
                name = cases[arm]
                if name in rows:
                    assert rows[name]['trajectory_sha256'] == refs[seed]['trajectory_sha256']
                    assert rows[name]['tasks'] == refs[seed]['tasks']
        cases = {name: str(raw / name / 'WAREHOUSE.json') for name in rows}
        write(raw / 'pool-cases.json', cases)
        subprocess.run(['/usr/bin/python3', str(raw / 'decompose.py'), '--output', str(raw / 'pool-replay'),
                        '--cases', str(raw / 'pool-cases.json')], check=True)
        # Independent ownership/reveal/pickup/completion boundary fixture.
        fixture = {'teamSize': 2, 'makespan': 5, 'numTaskFinished': 1,
            'numPlannerErrors': 0, 'numScheduleErrors': 0, 'numEntryTimeouts': 0,
            'start': [[0, 0, 'E'], [0, 4, 'W']],
            'actualPaths': ['F,F,F,W,W', 'W,W,W,W,F'],
            'actualSchedule': ['0:-1,2:0,4:-1', '1:0,2:-1,4:2'],
            'tasks': [[0, 0, [0, 2, 0, 3]], [1, 0, [0, 0, 0, 4]],
                      [2, 3, [0, 4, 0, 0]], [3, 5, [0, 1, 0, 2]]],
            'events': [[2, 0, 0, 1], [3, 0, 0, 2], [4, 1, 2, 1]]}
        write(out / 'pool-fixture.json', fixture)
        subprocess.run([str(raw / 'pool-replay/decompose'), str(raw / 'pool-replay/fixture.map'),
                        str(out / 'pool-fixture.json'), str(out / 'pool-fixture-result.json')], check=True)
        observed = read(out / 'pool-fixture-result.json')
        expected = [(2, 2, 0, 0), (2, 1, 1, 0), (2, 1, 0, 1), (2, 2, 0, 0), (2, 1, 0, 1), (3, 2, 0, 1)]
        assert len(observed['pool_snapshots']) == len(expected)
        for row, counts in zip(observed['pool_snapshots'], expected):
            assert tuple(row[k]['chain']['n'] for k in ['outstanding', 'unassigned', 'held_unopened', 'started']) == counts
        assert [r['new_completed_chain_sum'] for r in observed['chain_windows']] == [0, 0, 0, 1, 0, 0]
        assert observed['chain_windows'][-1]['revealed_chain_sum'] == 10
        cohorts = {}
        for name in rows:
            report = read(raw / 'pool-replay' / (name + '.json'))
            assert report['completed_tasks'] == rows[name]['tasks']
            assert len(report['pool_snapshots']) == 6
            assert all(s['outstanding']['chain']['n'] == 15000 for s in report['pool_snapshots'])
            cohorts[name] = report
        write(out / 'task-pool-and-travel.json', cohorts)
        effects = []
        for seed, cases in arms.items():
            if not all(name in rows for name in cases.values()):
                continue
            control, short, lane, both = (rows[cases[k]]['tasks'] for k in [(0, 0), (0, 1), (1, 0), (1, 1)])
            effects.append(dict(seed=seed, control=control, short_tasks=short, lanes=lane, both=both,
                short_effect_without_lanes=short-control, short_effect_with_lanes=both-lane,
                lane_effect_without_short=lane-control, lane_effect_with_short=both-short,
                interaction=both-lane-short+control))
        result.update(full_run=True, all_valid=not result['failures'], paired_effects=effects,
                      exact_generic_and_lane_controls=True)
        for source in (raw / 'pool-replay').glob('*fixture*.json'):
            shutil.copy2(source, out / source.name)
        shutil.copy2(raw / 'pool-replay/provenance.json', out / 'pool-provenance.json')
    result.update(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(), source_commit=a.commit,
                  source_files_verified=len(sources), binary_sha256=build['binary_sha256'], track='TRICK',
                  promoted=False, competition_budget_confirmed=False,
                  scope='Controlled CGAR policy ablation; no causal fraction of a competitor lead. Shared-host 5s development budget; fixed work and 32GB RSS. No generic frontier change.')
    for name in ['factorial-request.json', 'factorial-submission.json']:
        shutil.copy2(raw / name, out / name)
    write(out / 'scheduling-counters.json', dict(rows=scheduling, scope='Cumulative assignment estimates and fallback/fair counters through the last emitted diagnostic timestamp; not full-run totals.'))
    write(out / 'factorial-verification.json', result)
    for filename, sha in request['files'].items():
        assert digest(Path(filename)) == sha, filename
    print('TRICK_FACTORIAL_ANALYSIS_COMPLETE', json.dumps({k: result.get(k) for k in ['all_valid', 'full_run', 'eligible_for_full', 'paired_effects']}))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
