#!/usr/bin/env python3
"""Verify unchanged chain-pricing shadow trajectories and summarize resident coverage."""
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
import subprocess
import sys

ROOT = next(p for p in Path(__file__).resolve().parents if (p / 'tools/cpu_resources.py').is_file())
BASE = ROOT / 'experiments/construction-20260918'
KEY = 'CGAR_CHAIN_FLOW_PRICING'


def read(p):
    return json.loads(p.read_text())


def write(p, value):
    p.write_text(json.dumps(value, indent=2) + '\n')


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
    support = raw / 'chain-analysis-support'
    if not a.execute:
        support.mkdir(exist_ok=False)
        own = support / 'analyze_shadow.py'
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
        write(raw / 'chain-analysis-request.json', dict(root=str(ROOT), commit=a.commit,
              files={str(f): digest(f) for f in files}, raw=str(raw), output=str(out)))
        command = ['/usr/bin/python3', str(own), '--execute', '--raw', str(raw), '--output', str(out), '--commit', a.commit]
        job = raw / 'chain-analysis.sh'
        job.write_text('#!/bin/bash\nset -eu\nexec ' + ' '.join(map(shlex.quote, command)) + '\n')
        # Numeric analysis has no benchmark timing claim and can share a host.
        submit = ['qsub', '-h', '-terse', '-w', 'n', '-cwd', '-q', 'debian.q', '-pe', 'threaded', '1',
                  '-binding', 'linear:1', '-l', 'exclusive=false,h_rt=00:40:00,h_vmem=8G', '-m', 'n',
                  '-N', 'chain_shadow_analysis', '-j', 'y', '-o', str(raw / 'chain-analysis.log'), '-S', '/bin/bash']
        if a.hold_job:
            submit += ['-hold_jid', a.hold_job]
        submit.append(str(job))
        result = subprocess.run(submit, cwd=ROOT, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        receipt = dict(command=submit, returncode=result.returncode, response=result.stdout)
        write(raw / 'chain-analysis-submission.json', receipt)
        write(raw / 'motion-analysis-submission.json', receipt)
        print(result.stdout, end='')
        if result.returncode:
            return result.returncode
        assert re.fullmatch(r'\d+\s*', result.stdout), result.stdout
        subprocess.run(['qrls', result.stdout.strip()], check=True)
        return 0

    request = read(raw / 'chain-analysis-request.json')
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
    assert spec['instances'] == ['WAREHOUSE'] and spec['time_limit_ms'] in (1000, 5000) and spec['cpus_per_instance'] == 4
    if spec['time_limit_ms'] != 1000:
        assert spec['benchmark_mode'] == 'relaxed_development'
    sources = dict(build['sources'], **build['test_sources'])
    for name, sha in sources.items():
        assert hashlib.sha256(subprocess.check_output(['git', 'show', a.commit + ':' + name], cwd=ROOT)).hexdigest() == sha, name
    assert digest(raw / 'lifelong') == build['binary_sha256']
    horizon = (spec['horizons'] or {'WAREHOUSE': 5000})['WAREHOUSE']
    assert horizon in (200, 5000)
    cases = spec['cases']
    assert sorted((c['seed'], int(c['environment'][KEY])) for c in cases) == ([(0, 0), (0, 4)] if horizon == 200 else [(0, 4)])
    profile = next(iter(read(support / 'reference-profile.json').values()))
    allocation, intervals, coverage = read(raw / 'allocation.json'), [], {}
    for case in cases:
        name, env = case['name'], case['environment']
        mode = int(env[KEY])
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
        assert end > begin
        for other_begin, other_end, other_cores in intervals:
            assert not (begin < other_end and other_begin < end and cores.intersection(other_cores))
        intervals.append((begin, end, cores))
        log = (raw / name / 'WAREHOUSE.log').read_text().splitlines()
        receipt = [line for line in log if line.startswith('[cgar-chain-pricing]')]
        assert receipt == (['[cgar-chain-pricing] mode=4 shadow=1 resident_only=1 extra_tables=0'] if mode == 4 else [])
        assert not any(line.startswith(('[CGAR_TRICK]', '[cgar-temporal-score]')) for line in log)
        if summary['valid']:
            assert summary['makespan'] == summary['entry_compute_samples'] == horizon
            assert summary['entry_compute_max_seconds'] <= spec['time_limit_ms'] / 1000 and summary['peak_process_rss_bytes'] < 32000000000
            assert all(summary[k] == 0 for k in ['planner_errors', 'schedule_errors', 'timeouts', 'internal_timeouts', 'exit'])
            globals_ = [fields(line) for line in log if line.startswith('[cgar-temporal]')]
            regions = [fields(line) for line in log if line.startswith('[cgar-temporal-regions]')]
            assert [r['step'] for r in globals_] == [r['step'] for r in regions] == list(range(200, horizon + 1, 200))
            assert all(r['candidate_limit'] == 4000000 and r['workers'] == r['threads'] == 1 for r in globals_)
            for r in regions:
                assert r['regions'] == r['threads'] == 4 and r['rounds'] == 2 and r['temperature_ppm'] == 1000
                assert r['repairs'] == (r['kept'] + r['reverted']) * 25000 and r['score_after'] + 1e-6 >= r['score_before']
        snapshots = [fields(line) for line in log if line.startswith('[cgar-chain-snapshot]')]
        reports = [fields(line) for line in log if line.startswith('[cgar-chain-price]')]
        shadows = [fields(line) for line in log if line.startswith('[cgar-chain-shadow]')]
        if mode == 0:
            assert not snapshots and not reports and not shadows
            continue
        publications = {r['publications']: r['step'] for r in (fields(line) for line in log if line.startswith('[cgar-flow]'))}
        buckets = {}
        for r in snapshots:
            assert r['mode'] == 4 and r['publication'] in publications
            assert sum(r[k] for k in ['covered', 'missing', 'outside', 'unreachable', 'invalid']) == r['tasks']
            assert sum(r[k] for k in ['covered_table', 'covered_approx', 'uncovered_table', 'uncovered_approx']) == r['tasks']
            assert r['covered_table'] + r['covered_approx'] == r['covered']
            assert 0 <= r['covered_nonzero'] <= min(r['covered'], r['nonzero']) <= r['tasks']
            assert r['ratio_eligible'] == r['covered_nonzero'] - r['covered_clipped']
            assert (r['ratio_denominator'] == 0) == (r['ratio_eligible'] == 0)
            assert r['ratio_numerator'] >= 0 and r['ratio_denominator'] >= 0
            elapsed = r['t'] - publications[r['publication']]
            assert elapsed >= 1
            bucket = '1' if elapsed == 1 else '2-16' if elapsed <= 16 else '17-64' if elapsed <= 64 else '65-256' if elapsed <= 256 else '>256'
            b = buckets.setdefault(bucket, dict(calls=0, observations=0, nonzero=0, covered_nonzero=0, covered_table=0,
                                                covered_approx=0, uncovered_table=0, uncovered_approx=0, clipped_native=0, covered_clipped=0, ratio_eligible=0, ratios=[]))
            b['calls'] += 1
            b['observations'] += r['tasks']
            for key in ['nonzero', 'covered_nonzero', 'covered_table', 'covered_approx', 'uncovered_table', 'uncovered_approx', 'clipped_native', 'covered_clipped', 'ratio_eligible']:
                b[key] += r[key]
            if r['ratio_denominator']:
                b['ratios'].append(r['ratio_numerator'] / r['ratio_denominator'])
        category_keys = ['same', 'mode2_only', 'mode3_only', 'both_same', 'both_different']
        if summary['valid']:
            assert [(r['t'], r['publication']) for r in shadows] == [(r['t'], r['publication']) for r in snapshots]
        for r in shadows:
            elapsed = r['t'] - publications[r['publication']]
            bucket = '1' if elapsed == 1 else '2-16' if elapsed <= 16 else '17-64' if elapsed <= 64 else '65-256' if elapsed <= 256 else '>256'
            b = buckets[bucket]
            for key in ['small'] + category_keys:
                assert r[key] >= 0
                b[key] = b.get(key, 0) + r[key]
        for b in buckets.values():
            b['shadow_queries'] = sum(b.get(k, 0) for k in category_keys)
            ratios = b.pop('ratios')
            b['nonzero_coverage'] = b['covered_nonzero'] / b['nonzero'] if b['nonzero'] else None
            b['ratio_min'] = min(ratios) if ratios else None
            b['ratio_max'] = max(ratios) if ratios else None
        for r in reports:
            assert r['mode'] == 4 and r['changed'] == r['assigned_imputed'] == 0
            assert 0 <= r['assigned_covered'] <= r['active_assignments']
            assert all(0 <= r[k] <= r['shadow_queries'] for k in ['shadow_changed2', 'shadow_changed3', 'shadow_specific'])
            previous = [s for s in snapshots if s['t'] <= r['t']]
            assert r['calls'] == len(previous) and r['observations'] == sum(s['tasks'] for s in previous)
            for key in ['covered', 'missing', 'outside', 'unreachable', 'invalid']:
                assert r[key] == sum(s[key] for s in previous)
            previous_shadows = [v for v in shadows if v['t'] <= r['t']]
            total = {k: sum(v[k] for v in previous_shadows) for k in category_keys}
            assert r['shadow_queries'] == sum(total.values())
            assert r['shadow_changed2'] == total['mode2_only'] + total['both_same'] + total['both_different']
            assert r['shadow_changed3'] == total['mode3_only'] + total['both_same'] + total['both_different']
            assert r['shadow_specific'] == total['mode2_only'] + total['mode3_only'] + total['both_different']
        coverage[name] = dict(per_call_shadow_reports=len(shadows), last_shadow_timestep=shadows[-1]['t'] if shadows else None, publications=publications, buckets=buckets, last_cumulative_report=reports[-1] if reports else None,
                              snapshots=len(snapshots), scope='Observation-weighted coverage; shortlist rank changes cover every emitted per-call report and are conditional on retained native candidates; assignment counters stop at the last cumulative report timestamp. No counterfactual throughput claim.')

    helpers = support / 'experiments/construction-20260918'
    if horizon == 200:
        subprocess.run(['/usr/bin/python3', str(helpers / 'collect_cold.py'), '--input', str(raw), '--output', str(out)], check=True)
        summaries = read(out / 'run-summaries.json')
        all_valid = all(v[0]['valid'] for v in summaries.values())
        fingerprints = read(out / 'trajectory-fingerprints.json')
        for name, rows in summaries.items():
            if rows[0]['valid']:
                assert fingerprints[name] == 'a4bfe67f610e2451bb40e68da0e9fad0b75985d3f7701528334fee73f93cea8b'
        result = dict(all_valid=all_valid, full_run=False, eligible_for_full_shadow=all_valid,
                      all_valid_trajectories_exact_to_reference=True)
    else:
        subprocess.run(['/usr/bin/python3', str(support / 'experiments/sequences-20260918/analyze.py'),
                        '--input', str(raw), '--output', str(out), '--workers', '1'], check=True)
        sys.path.insert(0, str(helpers))
        verifier = importlib.import_module('verify_full')
        verifier.ROOT = ROOT
        result = verifier.verify(raw, out, a.commit, allow_failed=True, decision_limit_ms=spec['time_limit_ms'])
        reference = next(r for r in read(support / 'reference.json')['rows'] if r['seed'] == 0 and r['environment']['CGAR_TEMPORAL_REGIONS'] == '4')
        for row in result['rows']:
            assert row['tasks'] == reference['tasks'] and row['trajectory_sha256'] == reference['trajectory_sha256']
        result.update(full_run=True, all_valid=not result['failures'], all_valid_trajectories_exact_to_reference=True)
    result.update(decision_limit_ms=spec['time_limit_ms'], benchmark_mode=spec.get('benchmark_mode', 'competition_budget'),
                  competition_budget_confirmed=spec['time_limit_ms'] == 1000 and spec.get('exclusive_host', True) and result.get('all_valid', result.get('all_valid_within_deadline_and_memory', False)))
    result.update(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(), source_commit=a.commit,
                  source_files_verified=len(sources), binary_sha256=build['binary_sha256'], promoted=False,
                  scope='GENERIC unchanged-policy shadow. No throughput improvement or policy promotion; explicit failures have no partial quality score.')
    for name in ['chain-analysis-request.json', 'chain-analysis-submission.json']:
        shutil.copy2(raw / name, out / name)
    write(out / 'coverage.json', coverage)
    write(out / 'verification.json', result)
    print('CHAIN_SHADOW_ANALYSIS_COMPLETE', json.dumps({k: result.get(k) for k in ['all_valid', 'full_run', 'eligible_for_full_shadow']}))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
