#!/usr/bin/env python3
"""Archive compact warehouse evidence and verify frozen prototype patches."""
import argparse
import datetime
import difflib
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import tempfile

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
BASE = 'd6be5084f055b2941892e9845b0c20833612d07c'
BUILDS = ['v1', 'v2', 'v3', 'v4', 'v5', 'v6', 'v7', 'v7b', 'v8', 'v9', 'v10', 'v11', 'v12', 'v13', 'v14', 'v15', 'final']
MATRICES = {
    'warehouse-final-full': 'cgar-warehouse-final-full-20260918',
    'warehouse-full-v2': 'cgar-warehouse-sequences-full-v2-20260918',
    'warehouse-cache-full-v3': 'cgar-warehouse-cache-full-v3-20260918',
    'warehouse-commitment-full-v4': 'cgar-warehouse-commitment-full-v4-20260918',
    'warehouse-cache-confirmation-v4': 'cgar-warehouse-cache-confirmation-v4-20260918',
    'warehouse-memory-full-v4': 'cgar-warehouse-memory-full-v4-20260918',
    'warehouse-repair-full-v5': 'cgar-warehouse-repair-full-v5-20260918',
    'warehouse-temporal-full-v6': 'cgar-warehouse-temporal-full-v6-20260918',
    'warehouse-turn-full-v8': 'cgar-warehouse-turn-full-v8-20260918',
    'warehouse-carry-full-v9': 'cgar-warehouse-carry-full-v9-20260918',
    'warehouse-cache-policy-confirmation-v13': 'cgar-warehouse-cache-policy-confirmation-v13-20260918',
    'warehouse-cache-policy-full-v13': 'cgar-warehouse-cache-policy-full-v13-20260918',
    'warehouse-construction-movable-v15': 'cgar-warehouse-construction-movable-v15-20260918',
    'warehouse-construction-complete-v14': 'cgar-warehouse-construction-complete-v14-20260918',
    'warehouse-construction-full-v13': 'cgar-warehouse-construction-full-v13-20260918',
    'warehouse-prerotation-full-v12': 'cgar-warehouse-prerotation-full-v12-20260918',
    'warehouse-exploration-full-v11': 'cgar-warehouse-exploration-full-v11-20260918',
}


def read(path):
    return json.loads(path.read_text())


def write(path, value):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(value, indent=2) + '\n')


def preserve_builds():
    reproductions = []
    for version in BUILDS:
        source = ROOT / ('runs/cgar-warehouse-final-build-20260918' if version == 'final' else
                         'runs/cgar-sequences-build-' + version + '-20260918')
        target = HERE / 'build-provenance' / version
        target.mkdir(parents=True, exist_ok=True)
        for name in ['requested.json', 'build.json', 'allocation.json', 'submission.json']:
            if (source / name).exists():
                shutil.copy2(source / name, target / name)
        log = (source / 'build.log').read_text()
        passed = [line for line in log.splitlines() if ' passed' in line or 'All CGAR regression checks passed' in line]
        write(target / 'validation.json', {'completed': (source / 'build.json').exists(),
              'build_log_sha256': hashlib.sha256(log.encode()).hexdigest(), 'passed_checks': passed,
              'failure_tail': [] if (source / 'build.json').exists() else log.splitlines()[-12:]})
        requested = read(source / 'requested.json')
        patch = []
        originals = {}
        for name, digest in requested['sources'].items():
            frozen = (source / 'sources' / name).read_bytes()
            assert hashlib.sha256(frozen).hexdigest() == digest, (version, name)
            result = subprocess.run(['git', 'show', BASE + ':' + name], cwd=ROOT, capture_output=True)
            original = result.stdout if result.returncode == 0 else None
            originals[name] = original
            if original == frozen:
                continue
            patch.append('diff --git a/' + name + ' b/' + name + '\n')
            if original is None:
                patch.append('new file mode 100644\n')
            patch.extend(difflib.unified_diff((original or b'').decode().splitlines(True),
                         frozen.decode().splitlines(True), fromfile='a/' + name if original is not None else '/dev/null',
                         tofile='b/' + name))
        path = HERE / 'prototypes' / (version + '.patch')
        path.parent.mkdir(exist_ok=True)
        path.write_text(''.join(patch))
        with tempfile.TemporaryDirectory(prefix='lorr-sequence-reproduce-') as tmp:
            tmp = Path(tmp)
            for name, contents in originals.items():
                if contents is not None:
                    dest = tmp / name; dest.parent.mkdir(parents=True, exist_ok=True); dest.write_bytes(contents)
            subprocess.run(['git', 'apply', '--check', str(path)], cwd=tmp, check=True, capture_output=True)
            subprocess.run(['git', 'apply', str(path)], cwd=tmp, check=True, capture_output=True)
            for name, digest in requested['sources'].items():
                assert hashlib.sha256((tmp / name).read_bytes()).hexdigest() == digest, (version, name)
        reproductions.append({'version': version, 'base_commit': BASE, 'patch_sha256': hashlib.sha256(path.read_bytes()).hexdigest(),
                              'sources_reproduced': len(requested['sources']), 'regression_build_completed': (source / 'build.json').exists()})
    write(HERE / 'results/prototype-reproduction.json', reproductions)


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--allow-incomplete', action='store_true')
    a = p.parse_args()
    preserve_builds()
    ledger = []
    for label, name in MATRICES.items():
        raw = ROOT / 'runs' / name; spec = read(raw / 'spec.json')
        dest = HERE / 'results' / label
        dest.mkdir(parents=True, exist_ok=True)
        assert spec['instances'] == ['WAREHOUSE'] and spec['horizons'] is None
        for filename in ['completion.json', 'submission.json', 'motion-analysis-submission.json', 'interruption.json', 'scheduler-accounting.txt']:
            if (raw / filename).exists(): shutil.copy2(raw / filename, dest / filename)
        if (raw / 'completed-view/motion-analysis-submission.json').exists():
            shutil.copy2(raw / 'completed-view/motion-analysis-submission.json', dest / 'completed-case-analysis-submission.json')
        if (raw / 'interruption.json').exists():
            shutil.copy2(raw / 'spec.json', dest / 'original-spec.json')
        interruptions = read(raw / 'interruption.json')['incomplete_cases'] if (raw / 'interruption.json').exists() else []
        status = {'label': label, 'raw_directory': name, 'cases': len(spec['cases']),
                  'full_horizon': 5000, 'decision_limit_ms': spec['time_limit_ms'],
                  'analysis_complete': (dest / 'metrics.json').exists(), 'interrupted_cases': interruptions, 'completed': 0, 'valid': 0, 'failures': [],
                  'peak_process_bytes': 0, 'within_32_decimal_GB': True}
        for case in spec['cases']:
            path = raw / case['name'] / 'summary.json'
            if not path.exists(): continue
            status['completed'] += 1
            result, = read(path)
            status['valid'] += bool(result['valid'])
            if not result['valid']:
                log = raw / case['name'] / 'WAREHOUSE.log'
                evidence = [line for line in log.read_text().splitlines() if 'CGAR_TIMEOUT' in line or 'planner timeout' in line] if log.exists() else []
                status['failures'].append({'case': case['name'], 'outcome': result['outcome'], 'diagnostics': evidence})
            peak = result['process_resources']['peak_rss_kib'] * 1024
            status['peak_process_bytes'] = max(status['peak_process_bytes'], peak)
            status['within_32_decimal_GB'] &= peak <= 32_000_000_000
        if not a.allow_incomplete:
            assert status['completed'] + len(interruptions) == status['cases'] and status['analysis_complete'], status
        ledger.append(status)
    cancelled = ROOT / 'runs/cgar-sequences-full-v1-20260918'
    completed = []
    for path in sorted(cancelled.glob('*/summary.json')):
        for row in read(path):
            if row['instance'] == 'WAREHOUSE':
                completed.append({'case': path.parent.name, 'summary': row})
    write(HERE / 'results/completed-before-scope-change.json', completed)
    write(HERE / 'results/study.json', {'updated_utc': datetime.datetime.now(datetime.timezone.utc).isoformat(),
          'scope': 'WAREHOUSE; generic policies only', 'iteration_horizon': 5000,
          'memory_limit_bytes': 32_000_000_000, 'matrices': ledger})
    print(json.dumps(ledger, indent=2))


if __name__ == '__main__':
    main()
