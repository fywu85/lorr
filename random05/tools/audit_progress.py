#!/usr/bin/env python3
"""Check each recorded frontier against its frozen full run and source commit."""
import argparse
import datetime
import hashlib
import json
from pathlib import Path
import re
import subprocess

ROOT = Path(__file__).resolve().parents[2]
SOURCES = ('src/engine.cpp', 'src/engine.hpp', 'src/Entry.cpp', 'CMakeLists.txt')


def read(path):
    return json.loads(path.read_text())


def audit():
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
            assert reference['valid'] and reference['result']['makespan'] == 2000
            reference_case = next(c for c in specification['cases'] if c['name'] == reference['name'])
            references[reference['result']['numTaskFinished']] = (reference_case, read(directory / 'allocation.json'), name)
    report = []
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
        assert result['result']['makespan'] == 2000, (utc, 'partial run')
        for key in ('numPlannerErrors', 'numScheduleErrors', 'numEntryTimeouts'):
            assert result['result'][key] == 0, (utc, key)
        spec = read((ROOT / evidence).parent / 'spec.json')
        case = next(c for c in spec['cases'] if c['name'] == result['name'])
        allocation = read((ROOT / evidence).parent / 'allocation.json')
        cpu = allocation['resources']['cpu_model']
        workers = int(case['env']['R05_THREADS'])
        expected = '{} / {} / {}'.format(workers, case['cores'], cpu.split(' 32-Core')[0].replace('AMD ', ''))
        assert compute == expected, (utc, 'compute metadata mismatch')
        nms_tasks = int(nms_reference.split()[0].replace(',', ''))
        reference_case, reference_allocation, reference_name = references[nms_tasks]
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
        report.append(dict(utc=utc, source_commit=commit, tasks=int(tasks),
                           evidence=evidence, binary_sha256=digest, workers=workers,
                           physical_cores=case['cores'], cpu_model=cpu, nms_tasks=nms_tasks,
                           nms_evidence='random05/results/' + reference_name + '/summary.json',
                           matched_cpu_model_and_allocation=matched))
    assert report, 'no frontier rows found'
    return dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                checks=report, all_valid=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, default=ROOT / 'random05/results/progress-audit.json')
    args = parser.parse_args()
    report = audit()
    args.output.write_text(json.dumps(report, indent=2) + '\n')
    print('Verified {} frontier rows against full runs and source hashes.'.format(len(report['checks'])))


if __name__ == '__main__':
    main()
