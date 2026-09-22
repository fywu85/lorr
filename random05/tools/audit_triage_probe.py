#!/usr/bin/env python3
"""Check offline triage masks against every native decision in exact full controls."""
import argparse
from collections import Counter, defaultdict
import datetime
import hashlib
import json
from pathlib import Path
import re
import shutil

ROOT = Path(__file__).resolve().parents[2]


def read(path):
    return json.loads(path.read_text())


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def write(path, value):
    path.write_text(json.dumps(value, indent=2) + '\n')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--batch', required=True)
    parser.add_argument('--probes', required=True)
    args = parser.parse_args()
    archive = ROOT / 'random05/results' / args.batch
    audit = read(archive / 'audit.json')
    exact = read(archive / 'control-equivalence.json')
    assert audit['complete'] and all(row['valid'] for row in audit['rows'])
    references = {row['candidate']: row for row in exact['checks']}
    checks = []
    for row in audit['rows']:
        assert row['replay']['replay_valid']
        candidate = Path(row['replay']['result'])
        pair = references[str(candidate.relative_to(ROOT))]
        assert all(pair['equal'].values())
        work = ROOT / 'runs/random05' / args.probes / row['name']
        spec = read(work / 'probe-spec.json')
        done = read(work / 'completion.json')
        assert done['valid'] and done['exit'] == 0
        assert sha(work / 'diagnostic.json') == done['diagnostic_sha256']
        for name, digest in spec['local_hashes'].items():
            assert sha(work / name) == digest, name
        for name, digest in spec['input_hashes'].items():
            assert sha(Path(name)) == digest, name
        assert sha(work / 'trace.json') == pair['reference_sha256']
        build = Path(spec['binary']).parent.parent
        frozen = read(build / 'spec.json')
        assert sha(build / 'source/tools/triage_probe.cpp') == frozen['source_hashes']['tools/triage_probe.cpp']
        assert sha(build / 'source/src/engine.cpp') == frozen['source_hashes']['src/engine.cpp']
        record = read(work / 'diagnostic.json')
        assert record['tasks'] == row['tasks'] and record['steps'] == row['replay']['steps']
        log = candidate.parent / 'native.log'
        native = re.findall(r'R05_TRIAGE t=(\d+) suppressed=(\d+) active_mask=(\d+)', log.read_text())
        assert len(native) == record['steps'], 'missing/duplicate native diagnostics'
        reconstructed = record.pop('suppression_masks')
        assert len(reconstructed) == record['steps']
        for step, (actual, expected) in enumerate(zip(native, reconstructed)):
            assert actual == (str(step), str(expected['suppressed']), expected['active_mask']), (step, actual, expected)
            assert expected['step'] == step
        phases = defaultdict(Counter)
        for key, count in record['counts'].items():
            words = key.split('_')
            if words[-1] in ('F', 'R', 'C', 'W'):
                phases[words[0]][words[-1]] += count
        assert sum(sum(value.values()) for value in phases.values()) == row['replay']['robot_steps']
        for phase in ('loaded', 'unopened', 'idle'):
            assert phases[phase] == Counter(row['replay']['phases'][phase]), phase
        assert sum(b.get('completed', 0) for b in record['bins'].values()) == row['tasks']
        assert record['orders']['finished'] == row['tasks']
        target = archive / 'diagnostics' / row['name']
        target.mkdir(parents=True, exist_ok=True)
        write(target / 'summary.json', record)
        for name in ('case.json', 'probe-spec.json', 'allocation.json', 'submission.json', 'completion.json'):
            shutil.copy2(str(work / name), str(target / name))
        checks.append(dict(name=row['name'], tasks=row['tasks'], steps=record['steps'],
                           masks_checked=len(native), all_masks_exact=True, action_accounting_exact=True,
                           result_sha256=row['replay']['result_sha256'], original_result_sha256=pair['reference_sha256'],
                           native_log_sha256=sha(log), diagnostic_sha256=done['diagnostic_sha256'],
                           probe_source_sha256=frozen['source_hashes']['tools/triage_probe.cpp'],
                           probe_binary_sha256=spec['local_hashes']['probe']))
        print(row['name'], row['tasks'], len(native), 'native suppression masks and phase accounting exact', flush=True)
    write(archive / 'triage-verification.json', dict(
        checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(), checks=checks,
        caveat='Observational classification only. A fitting collision-free lower bound does not establish recoverable throughput.'))


if __name__ == '__main__':
    main()
