#!/usr/bin/env python3
"""Freeze and submit independent remaining-flow screen/full verification to GRID."""
import argparse
import datetime
import hashlib
import importlib
import json
import os
from pathlib import Path
import re
import shlex
import shutil
import statistics
import subprocess
import sys

ROOT = next(p for p in Path(__file__).resolve().parents if (p / 'tools/cpu_resources.py').is_file())
BASE = ROOT / 'experiments/construction-20260918'
KEY = 'CGAR_TEMPORAL_REMAINING_FLOW'


def read(p):
    return json.loads(p.read_text())


def write(p, data):
    p.write_text(json.dumps(data, indent=2) + '\n')


def digest(p):
    return hashlib.sha256(p.read_bytes()).hexdigest()


def fields(line):
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
    support = raw / 'remaining-analysis-support'
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
        for name, source in [('reference.json', BASE / 'results/pickup-full-regions-six-seed-v44.json'),
                             ('reference-profile.json', BASE / 'warehouse-reference-variants.json')]:
            shutil.copy2(source, support / name)
            files.append(support / name)
        write(raw / 'remaining-analysis-request.json', {'root': str(ROOT), 'commit': a.commit,
              'files': {str(f): digest(f) for f in files}, 'raw': str(raw), 'output': str(out)})
        command = ['/usr/bin/python3', str(own), '--execute', '--raw', str(raw), '--output', str(out), '--commit', a.commit]
        job = raw / 'remaining-analysis.sh'
        job.write_text('#!/bin/bash\nset -eu\nexec ' + ' '.join(map(shlex.quote, command)) + '\n')
        submit = ['qsub', '-h', '-terse', '-w', 'n', '-cwd', '-q', 'debian.q', '-pe', 'threaded', '1',
                  '-binding', 'linear:1', '-l', 'exclusive=true,h_rt=00:30:00,h_vmem=8G', '-m', 'n',
                  '-N', 'remaining_analysis', '-j', 'y', '-o', str(raw / 'remaining-analysis.log'), '-S', '/bin/bash']
        if a.hold_job:
            submit += ['-hold_jid', a.hold_job]
        submit.append(str(job))
        result = subprocess.run(submit, cwd=ROOT, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        receipt = dict(command=submit, returncode=result.returncode, response=result.stdout)
        write(raw / 'remaining-analysis-submission.json', receipt)
        write(raw / 'motion-analysis-submission.json', receipt)
        print(result.stdout, end='')
        if result.returncode:
            return result.returncode
        assert re.fullmatch(r'\d+\s*', result.stdout), result.stdout
        subprocess.run(['qrls', result.stdout.strip()], check=True)
        return 0

    request = read(raw / 'remaining-analysis-request.json')
    assert request['commit'] == a.commit and Path(request['root']) == ROOT
    for filename, sha in request['files'].items():
        assert digest(Path(filename)) == sha, filename
    sys.path.insert(0, str(ROOT / 'tools'))
    from cpu_resources import cpu_resources
    cpu = cpu_resources()
    assert cpu['effective_cpu_quota'] is None and cpu['representative_cpus']
    os.sched_setaffinity(0, cpu['representative_cpus'][:1])
    spec, build = read(raw / 'spec.json'), read(raw / 'build.json')
    assert spec['trick'] is None and spec['experiment_track'] == 'GENERIC'
    assert spec['instances'] == ['WAREHOUSE'] and spec['time_limit_ms'] == 1000 and spec['cpus_per_instance'] == 4
    sources = dict(build['sources'], **build['test_sources'])
    for name, sha in sources.items():
        assert hashlib.sha256(subprocess.check_output(['git', 'show', a.commit + ':' + name], cwd=ROOT)).hexdigest() == sha, name
    assert digest(raw / 'lifelong') == build['binary_sha256']
    horizon = (spec['horizons'] or {'WAREHOUSE': 5000})['WAREHOUSE']
    assert horizon in (200, 5000)
    profile = next(iter(read(support / 'reference-profile.json').values()))
    cases = spec['cases']
    seeds = sorted({c['seed'] for c in cases})
    assert seeds == ([0] if horizon == 200 else [0, 2]) and len(cases) == 2 * len(seeds)
    by_seed = {seed: {} for seed in seeds}
    receipts, intervals = {}, []
    allocation = read(raw / 'allocation.json')
    for case in cases:
        name, env = case['name'], case['environment']
        mode = int(env[KEY])
        assert mode in (0, 1) and mode not in by_seed[case['seed']]
        by_seed[case['seed']][mode] = name
        assert {k: v for k, v in env.items() if k != KEY} == profile
        meta, summary = read(raw / name / 'metadata.json'), read(raw / name / 'summary.json')[0]
        assert meta['trick'] is None and meta['trick_argv'] == [] and meta['experiment_track'] == 'GENERIC'
        assert meta['build_provenance'] == build and meta['max_process_memory_bytes'] == 32000000000
        assert summary['trick_receipt_valid'] and summary['experiment_track_valid']
        resources = meta['cpu_resources']
        assert resources['effective_cpu_quota'] is None and resources['cpu_model'] == 'AMD EPYC 9354 32-Core Processor'
        binding = summary['cpu']
        assert len(binding) == 4 and binding == meta['cpu_binding'] and set(binding).issubset(allocation['selected_cpus'])
        core_of = {c: tuple(group) for group in resources['logical_cpus_by_physical_core'] for c in group}
        cores = {(resources['hostname'], core_of[c]) for c in binding}
        assert len(cores) == 4
        begin, end = (datetime.datetime.fromisoformat(meta[k]) for k in ['started_utc', 'finished_utc'])
        for other_begin, other_end, other_cores in intervals:
            assert not (begin < other_end and other_begin < end and cores.intersection(other_cores))
        intervals.append((begin, end, cores))
        log = (raw / name / 'WAREHOUSE.log').read_text().splitlines()
        score_receipts = [line for line in log if line.startswith('[cgar-temporal-score]')]
        assert score_receipts == (['[cgar-temporal-score] remaining_flow=1 paid_forward_extra=0'] if mode else [])
        assert not any(line.startswith('[CGAR_TRICK]') for line in log)
        receipts[name] = {'mode': mode, 'unique_expected_receipt': True, 'generic_track_verified': True}
        if summary['valid']:
            assert summary['makespan'] == summary['entry_compute_samples'] == horizon
            assert summary['entry_compute_max_seconds'] <= 1 and summary['peak_process_rss_bytes'] < 32000000000
            global_rows = [fields(line) for line in log if line.startswith('[cgar-temporal]')]
            regions = [fields(line) for line in log if line.startswith('[cgar-temporal-regions]')]
            expected_steps = list(range(200, horizon + 1, 200))
            assert [x['step'] for x in global_rows] == [x['step'] for x in regions] == expected_steps
            for x in global_rows:
                assert x['candidate_limit'] == 4000000 and x['workers'] == x['threads'] == 1
            for x in regions:
                assert x['regions'] == x['threads'] == 4 and x['rounds'] == 2 and x['temperature_ppm'] == 1000
                assert x['repairs'] == (x['kept'] + x['reverted']) * 25000 and x['score_after'] + 1e-6 >= x['score_before']

    helpers = support / 'experiments/construction-20260918'
    if horizon == 200:
        subprocess.run(['/usr/bin/python3', str(helpers / 'collect_cold.py'), '--input', str(raw), '--output', str(out)], check=True)
        summaries = read(out / 'run-summaries.json')
        all_valid = all(v[0]['valid'] for v in summaries.values())
        prefix_exact = False
        if all_valid:
            off, on = by_seed[0][0], by_seed[0][1]
            assert read(out / 'trajectory-fingerprints.json')[off] == 'a4bfe67f610e2451bb40e68da0e9fad0b75985d3f7701528334fee73f93cea8b'
            data = [read(raw / name / 'WAREHOUSE.json') for name in (off, on)]
            for x, y in zip(data[0]['actualPaths'], data[1]['actualPaths']):
                assert x.split(',')[:128] == y.split(',')[:128]
            for x, y in zip(data[0]['actualSchedule'], data[1]['actualSchedule']):
                before = lambda s: [v for v in s.split(',') if v and int(v.split(':')[0]) <= 128]
                assert before(x) == before(y)
            assert len(data[0]['actualPaths']) == len(data[1]['actualPaths']) == 10000
            assert len(data[0]['actualSchedule']) == len(data[1]['actualSchedule']) == 10000
            prefix_exact = True
        result = dict(all_valid=all_valid, eligible_for_full_comparison=all_valid, full_run=False,
                      exact_control=all_valid, all_128_prepublication_actions_and_schedules_exact=prefix_exact)
    else:
        subprocess.run(['/usr/bin/python3', str(support / 'experiments/sequences-20260918/analyze.py'),
                        '--input', str(raw), '--output', str(out), '--workers', '1'], check=True)
        sys.path.insert(0, str(helpers))
        verifier = importlib.import_module('verify_full')
        verifier.ROOT = ROOT
        result = verifier.verify(raw, out, a.commit, allow_failed=True)
        write(out / 'verification.json', result)
        reference = {r['seed']: r for r in read(support / 'reference.json')['rows'] if r['environment']['CGAR_TEMPORAL_REGIONS'] == '4'}
        controls = {r['seed']: r for r in result['rows'] if r['environment'][KEY] == '0'}
        for seed, row in controls.items():
            assert row['tasks'] == reference[seed]['tasks'] and row['trajectory_sha256'] == reference[seed]['trajectory_sha256']
        metrics = {row['case']: row for row in read(out / 'metrics.json')}
        pairs = []
        for row in result['rows']:
            if row['environment'][KEY] != '1' or row['seed'] not in controls:
                continue
            control = controls[row['seed']]
            pairs.append(dict(seed=row['seed'], tasks=row['tasks'], control_tasks=control['tasks'],
                         task_difference=row['tasks'] - control['tasks'], final1000_difference=row['final1000'] - control['final1000'],
                         age_p90_difference=row['outstanding_age_p90'] - control['outstanding_age_p90'],
                         candidate_per1000=metrics[row['case']]['completed_per_1000'],
                         control_per1000=metrics[control['case']]['completed_per_1000']))
        complete = not result['failures'] and len(pairs) == 2
        result.update(full_run=True, exact_control_seeds=sorted(controls), pairs=pairs,
                      mean_effect_percent=(statistics.mean(r['tasks'] for r in pairs) / statistics.mean(r['control_tasks'] for r in pairs) - 1) * 100 if complete else None,
                      all_tested_totals_improve=complete and all(r['task_difference'] > 0 for r in pairs), promoted=False)
    result.update(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(), source_commit=a.commit,
                  source_files_verified=len(sources), binary_sha256=build['binary_sha256'], receipts=receipts,
                  scope='GENERIC remaining-flow score only; short screens establish feasibility, not throughput. Failed runs have no partial quality score.')
    for name in ['remaining-analysis-request.json', 'remaining-analysis-submission.json']:
        shutil.copy2(raw / name, out / name)
    write(out / 'comparison.json', result)
    print('REMAINING_ANALYSIS_COMPLETE', json.dumps({k: result.get(k) for k in ['full_run', 'all_valid', 'pairs', 'mean_effect_percent', 'eligible_for_full_comparison']}))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
