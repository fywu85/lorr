#!/usr/bin/env python3
"""Rebuild the saved pre-fix CGAR core with the earliest recorded entry wrappers."""
import concurrent.futures
import hashlib
import json
from pathlib import Path
import shlex
import subprocess
ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
OUT = ROOT / 'runs/cgar-draft-rebuild-20260918'
OUT.mkdir(parents=True, exist_ok=True)
flags = {}
for line in (ROOT / 'cgar/build/CMakeFiles/lifelong.dir/flags.make').read_text().splitlines():
    if line.startswith('CXX_') and ' = ' in line:
        key, value = line.split(' = ', 1); flags[key] = shlex.split(value)
link = shlex.split((ROOT / 'cgar/build/CMakeFiles/lifelong.dir/link.txt').read_text())
mapping = {'cgar_planner/cgar.cpp': HERE / 'draft-source/cgar.cpp'}
for name in ['Entry.cpp', 'MAPFPlanner.cpp', 'TaskScheduler.cpp']:
    mapping['src/' + name] = HERE / 'draft-source' / name

def compile_one(item):
    source, path = item; obj = OUT / (path.name + '.o')
    command = [link[0], '-I' + str(HERE / 'draft-source')] + flags['CXX_DEFINES'] + flags['CXX_INCLUDES'] + flags['CXX_FLAGS']
    subprocess.run(command + ['-c', str(path), '-o', str(obj)], cwd=str(ROOT), check=True)
    return source, obj

with concurrent.futures.ThreadPoolExecutor(max_workers=4) as pool:
    objects = dict(pool.map(compile_one, mapping.items()))
for i, argument in enumerate(link):
    for source, obj in objects.items():
        if argument == 'CMakeFiles/lifelong.dir/' + source + '.o':
            link[i] = str(obj)
link[link.index('-o') + 1] = str(OUT / 'lifelong')
subprocess.run(link, cwd=str(ROOT / 'cgar/build'), check=True)
sources = {('cgar/' + key): hashlib.sha256(path.read_bytes()).hexdigest() for key, path in mapping.items()}
sources['cgar/cgar_planner/cgar.hpp'] = hashlib.sha256((HERE / 'draft-source/cgar.hpp').read_bytes()).hexdigest()
manifest = {'source_commit': None, 'source_dirty': False, 'sources': sources,
            'binary_sha256': hashlib.sha256((OUT / 'lifelong').read_bytes()).hexdigest(),
            'reconstruction': 'Pre-fix cgar.cpp/hpp saved in /tmp/lorr-cgar-before-fix-20260917; wrappers from first recorded commit c2f0e5b. Original wrapper snapshot and executable were not retained.'}
(OUT / 'build.json').write_text(json.dumps(manifest, indent=2) + '\n')
print(json.dumps(manifest), flush=True)
