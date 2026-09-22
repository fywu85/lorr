#!/usr/bin/env python3
"""Audit short runtime controls; their scores never qualify as full throughput."""
import argparse
import datetime
import json
from pathlib import Path
import re
import shutil
from action_audit import audit as replay
from audit_generalization import ROOT, source_check, sha
from audit_startup_price_prefix import prefix


def read(path):
    return json.loads(path.read_text())


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--batch', required=True)
    p.add_argument('--sources', required=True, type=Path)
    p.add_argument('--references', required=True, type=Path)
    args = p.parse_args()
    sources = {}
    for digest, item in read(args.sources).items():
        checked = source_check(item['build'], item['source'])
        assert checked['binary_sha256'] == digest
        sources[digest] = checked
    directory = ROOT / 'runs/random05' / args.batch
    archive = ROOT / 'random05/results' / args.batch
    references = read(args.references)
    rows = []; grouped = {}
    for job in read(directory / 'batch.json')['jobs']:
        out = Path(job['output']); spec = read(out / 'spec.json'); case = spec['cases'][0]
        summary = read(out / 'summary.json')[0]; allocation = read(out / 'allocation.json')['resources']
        work = out / case['name']; source = sources[case['binary_sha256']]
        assert case['steps'] == 20 and int(case['env']['R05_HORIZON']) in (800, 1000, 2000)
        assert case['limit_ms'] == 1000 and case['preprocess_ms'] == 30000
        if summary.get('failure_kind') == 'resource_preflight':
            assert not summary['valid'] and not summary['solver_started']
            assert sha(work / 'lifelong') == case['binary_sha256'] == source['binary_sha256']
            for filename, digest in case['input_hashes'].items(): assert sha(Path(filename)) == digest
            row = dict(name=case['name'], instance=case['trick'], seed=int(case['env']['R05_SEED']),
                       valid=False, solver_started=False, failure_kind=summary['failure_kind'],
                       failure=summary['failure'], source_commit=source['commit'],
                       binary_sha256=case['binary_sha256'], finished_utc=summary['finished_utc'])
            dest = archive / case['name']; dest.mkdir(parents=True, exist_ok=True)
            for filename in ('summary.json','spec.json','allocation.json','submission.json','completion.json'):
                shutil.copy2(str(out / filename), str(dest / filename))
            rows.append(row); print(row['name'], row['failure'], flush=True); continue
        assert case['cores'] == allocation['physical_cores_visible'] == 16
        assert case['smt'] == 2 and len(allocation['logical_cpu_affinity']) == 32
        assert allocation['cpu_model'] == 'AMD EPYC 9354 32-Core Processor'
        assert allocation['effective_cpu_quota'] is None
        assert len(read(work / 'case.json')['cpus']) == 32
        assert sha(work / 'lifelong') == case['binary_sha256'] == source['binary_sha256']
        for filename, digest in case['input_hashes'].items(): assert sha(Path(filename)) == digest
        assert summary == read(work / 'summary.json')
        assert summary['usage']['peak_rss_kib'] * 1024 <= 32000000000
        row = dict(name=case['name'], instance=case['trick'], seed=int(case['env']['R05_SEED']),
                   valid=summary['valid'], latency_seconds=summary.get('latency_seconds'),
                   source_commit=source['commit'], binary_sha256=case['binary_sha256'],
                   finished_utc=summary['finished_utc'], native_log_sha256=sha(work / 'native.log'))
        if summary['valid']:
            assert summary['exit'] == 0 and summary['latency_seconds']['max'] <= 1
            assert summary['result']['entryComputeSamples'] == 20
            result_path = work / 'result.json'; data = read(result_path)
            checked = replay(result_path, next(Path(f) for f in case['input_hashes'] if f.endswith('.map')))
            checked.pop('completed_loaded_work')
            assert checked['steps'] == 20 and checked['replay_valid']
            row['replay'] = checked; row['first_entry_seconds'] = data['entryComputeTimes'][0]
            pref = prefix(data, 20)
            key = (case['trick'], row['seed'])
            if key in grouped:
                other_name, other = grouped[key]
                equal = {k: pref[k] == other[k] for k in pref}
                assert all(equal.values()), (case['name'], equal)
                row['paired_prefix'] = dict(reference=other_name, steps=20, equal=equal)
            else: grouped[key] = (case['name'], pref)
            if case['name'] in references:
                reference = ROOT / references[case['name']]; expected = prefix(read(reference), 20)
                equal = {k: pref[k] == expected[k] for k in pref}
                assert all(equal.values()), (case['name'], equal)
                row['archived_prefix'] = dict(reference=references[case['name']], reference_sha256=sha(reference),
                                              steps=20, equal=equal)
        else:
            row['exit'] = summary['exit']
            row['timeout'] = re.findall(r'R05_TIMEOUT[^\n]*', (work / 'native.log').read_text())
        dest = archive / case['name']; dest.mkdir(parents=True, exist_ok=True)
        for filename in ('summary.json','spec.json','allocation.json','submission.json','completion.json'):
            shutil.copy2(str(out / filename), str(dest / filename))
        rows.append(row)
        print(row['name'], 'valid='+str(row['valid']), row['latency_seconds'], flush=True)
    for filename in ('batch.json','summary.json','completion.json'):
        shutil.copy2(str(directory / filename), str(archive / filename))
    output = dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                  diagnostic_only=True, steps=20, complete=True, rows=rows, sources_by_binary=sources,
                  caveat='Strict-deadline startup/prefix diagnostics. No short-run score is eligible as a throughput record; full horizons remain required.')
    (archive / 'audit.json').write_text(json.dumps(output, indent=2)+'\n')


if __name__ == '__main__':
    main()
