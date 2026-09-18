#!/usr/bin/env python3
"""Prepare an explicit analysis view of completed cases in an interrupted matrix."""
import argparse
import hashlib
import json
from pathlib import Path
import shutil

p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--input', type=Path, required=True)
a = p.parse_args()
root = a.input.resolve()
assert (root / 'interruption.json').exists(), 'Record the scheduler termination before making a partial analysis view'
view = root / 'completed-view'
view.mkdir(exist_ok=False)
spec_bytes = (root / 'spec.json').read_bytes()
spec = json.loads(spec_bytes)
original = spec['cases']
spec['cases'] = [c for c in original if (root / c['name'] / 'summary.json').exists()]
spec['incomplete_cases'] = [c for c in original if c not in spec['cases']]
spec['original_case_count'] = len(original)
spec['original_spec_sha256'] = hashlib.sha256(spec_bytes).hexdigest()
spec['analysis_scope'] = 'Only completed full runs; interrupted cases have no accepted score'
(view / 'spec.json').write_text(json.dumps(spec, indent=2) + '\n')
for c in spec['cases']:
    (view / c['name']).symlink_to(root / c['name'], target_is_directory=True)
shutil.copy2(root / 'allocation.json', view / 'allocation.json')
print(view)
