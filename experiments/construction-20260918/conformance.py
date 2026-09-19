#!/usr/bin/env python3
"""Build the kernel comparison against frozen NMS objects on reserved cores."""
import argparse
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
from jobs import ROOT, HERE, sha, write, submit, cpu_resources


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--output', required=True, type=Path)
    p.add_argument('--base', required=True, type=Path)
    p.add_argument('--execute', action='store_true')
    a = p.parse_args(); out = a.output.resolve(); base = a.base.resolve()
    if not a.execute:
        out.mkdir(parents=True, exist_ok=False)
        for name in ['temporal_geometry.hpp', 'temporal_pibt.hpp']:
            shutil.copy2(ROOT / 'cgar/cgar_planner' / name, out / name)
        shutil.copy2(HERE / 'conformance.cpp', out / 'conformance.cpp')
        return submit(out, [sys.executable, str(Path(__file__).resolve()), '--execute', '--output', str(out), '--base', str(base)], 'temporal_compare', 4, 4, 4)
    write(out / 'allocation.json', cpu_resources())
    original = json.loads((base / 'build.json').read_text())
    overlay = out / 'include/Planner/PIBT'; overlay.mkdir(parents=True)
    header = (base / 'sources/nms/Solution/Planner/PIBT/pibts.hpp').read_text()
    needle = '    uint32_t step = 0;'
    assert header.count(needle) == 1
    header = header.replace(needle, '''    const auto& probe_costs() const { return smart_dist_dp; }
    const auto& probe_choices() const { return robot_desires; }
    const auto& probe_order() const { return order; }
    const auto& probe_power() const { return robot_power; }
''' + needle)
    (overlay / 'pibts.hpp').write_text(header)
    commands = []; replacements = {}
    for original_command in original['nms_compile']:
        source = original_command[-3]
        if not source.endswith(('Planner/PIBT/pibts.cpp', 'Planner/eplanner.cpp')): continue
        obj = out / (Path(source).stem + '.o')
        command = original_command[:1] + ['-I' + str(out / 'include')] + original_command[1:-1] + [str(obj)]
        subprocess.run(command, check=True); commands.append(command); replacements[original_command[-1]] = str(obj)
    template = original['nms_compile'][-1]
    command = template[:1] + ['-I' + str(out / 'include'), '-I' + str(out)] + template[1:-4] + ['-c', str(out / 'conformance.cpp'), '-o', str(out / 'driver.o')]
    subprocess.run(command, check=True); commands.append(command)
    link = [replacements.get(x, str(out / 'driver.o') if x == str(base / 'nms-snapshot.o') else str(out / 'compare') if x == str(base / 'nms-snapshot') else x) for x in original['nms_link']]
    subprocess.run(link, check=True)
    results = []
    for steps in [0, 256]:
        env = dict(os.environ); env.update(NMS_HELPERS='1', NMS_PREPROCESS_THREADS='1', NMS_STEPS=str(steps))
        run = subprocess.run([str(out / 'compare'), str(out / f'result-{steps}.json')], cwd=out, env=env)
        results.append({'steps': steps, 'exit': run.returncode})
    write(out / 'build.json', {'compile': commands, 'link': link, 'binary_sha256': sha(out / 'compare'),
          'sources': {p.name: sha(p) for p in out.glob('*.hpp')}, 'driver_sha256': sha(out / 'conformance.cpp'),
          'native_header_sha256': sha(overlay / 'pibts.hpp'), 'results': results})
    return int(any(x['exit'] for x in results))


if __name__ == '__main__':
    raise SystemExit(main())
