#!/usr/bin/env python3
"""Archive a frozen experimental build as a verified patch against its commit."""
import argparse
import difflib
import hashlib
import json
from pathlib import Path
import shutil
import subprocess

ROOT = Path(__file__).resolve().parents[2]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--input', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--refresh', action='store_true', help='Recreate an existing archive from the same frozen build')
    args = parser.parse_args()
    raw, out = args.input.resolve(), args.output.resolve()
    request = json.loads((raw / 'requested.json').read_text())
    changes = []
    for name, expected in sorted(request['sources'].items()):
        contents = (raw / 'sources' / name).read_bytes()
        assert hashlib.sha256(contents).hexdigest() == expected, name
        original = subprocess.run(['git', 'show', request['commit'] + ':' + name],
                                  cwd=ROOT, capture_output=True, check=False)
        if original.returncode:
            assert b'does not exist' in original.stderr or b'exists on disk, but not in' in original.stderr, original.stderr
        for line in difflib.unified_diff(original.stdout.decode().splitlines(True),
                       contents.decode().splitlines(True),
                       fromfile='a/' + name if not original.returncode else '/dev/null',
                       tofile='b/' + name):
            changes.append(line if line.endswith('\n') else line + '\n\\ No newline at end of file\n')
    out.mkdir(parents=True, exist_ok=args.refresh)
    for name in ['requested.json', 'build.json', 'allocation.json', 'submission.json']:
        if (raw / name).exists():
            shutil.copy2(raw / name, out / name)
    log = (raw / 'build.log').read_bytes()
    validation = {'completed': (raw / 'build.json').exists(),
                  'build_log_sha256': hashlib.sha256(log).hexdigest(),
                  'passed_checks': [line for line in log.decode(errors='replace').splitlines()
                                    if ' passed' in line],
                  'patch_base': request['commit']}
    (out / 'validation.json').write_text(json.dumps(validation, indent=2) + '\n')
    (out / 'source.patch').write_text(''.join(changes))
    print(out.relative_to(ROOT), len(changes), 'patch lines')


if __name__ == '__main__':
    main()
