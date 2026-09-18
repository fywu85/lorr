#!/usr/bin/env python3
"""Retain compact reference evidence, including every failure and resource allocation."""
import json
from pathlib import Path
import shutil

ROOT = Path(__file__).resolve().parents[2]
OUT = Path(__file__).resolve().parent / 'results'


def write(path, data):
    path.write_text(json.dumps(data, indent=2) + '\n')


def main():
    raw = ROOT / 'runs/leader-references-20260918'
    spec = json.loads((raw / 'spec.json').read_text())
    summaries = [json.loads((raw / c['name'] / 'summary.json').read_text()) for c in spec['cases']]
    assert len(summaries) == 20
    out = OUT / 'leaders'; out.mkdir(parents=True, exist_ok=True)
    for name in ['spec.json', 'submission.json']:
        shutil.copyfile(raw / name, out / name)
    write(out / 'run-summaries.json', summaries)
    write(out / 'allocations.json', {c['name']: json.loads((raw / c['name'] / 'allocation.json').read_text()) for c in spec['cases']})
    timeout_lines = {}
    for c in spec['cases']:
        lines = []
        with (raw / c['name'] / 'native.log').open() as stream:
            for line in stream:
                if 'timeout' in line.lower() or 'time out' in line.lower():
                    lines.append(line.rstrip())
        if lines:
            timeout_lines[c['name']] = lines
    write(out / 'timeout-diagnostics.json', timeout_lines)
    cgar_root = ROOT / 'runs/cgar-next-scheduler-screen-20260918'
    control = json.loads((cgar_root / 'baseline-s0-r0/summary.json').read_text())
    cgar = {r['instance']: r for r in control}
    rows = []
    for name in json.loads((ROOT / 'mr24/simulation_time.json').read_text()):
        row = {'instance': name, 'steps': cgar[name]['makespan'], 'cgar_seed0': cgar[name]['after']}
        for team in ['kk', 'nms']:
            d = next(r for r in summaries if r['team'] == team and r['instance'] == name)
            assert d['steps'] == row['steps']
            row[team] = d['result']['numTaskFinished']
            row[team + '_valid'] = d['valid']
            row[team + '_timeouts'] = d['result']['numEntryTimeouts']
        rows.append(row)
    write(out / 'comparison.json', rows)

    raw = ROOT / 'runs/cgar-draft-controlled-20260918'
    out = OUT / 'draft'; out.mkdir(parents=True, exist_ok=True)
    draft = json.loads((raw / 'draft-s0-r0/summary.json').read_text())
    assert len(draft) == 10
    for source, dest in [('spec.json','spec.json'),('allocation.json','allocation.json'),
                         ('draft-s0-r0/summary.json','run-summaries.json'),('draft-s0-r0/metadata.json','metadata.json')]:
        shutil.copyfile(raw / source, out / dest)
    for source, dest in [('build.json','build.json'),('build.log','build-validation.txt')]:
        p = ROOT / 'runs/cgar-draft-rebuild-20260918' / source
        if p.exists(): shutil.copyfile(p,out / dest)
    baseline = json.loads((ROOT / 'runs/cgar-next-scheduler-full-20260918/baseline-s0-r0/summary.json').read_text())
    rows = []
    for d in draft:
        b = next(r for r in baseline if r['instance'] == d['instance'])
        rows.append({'instance':d['instance'],'steps':d['makespan'],'draft':d['after'],'baseline':b['after'],
                     'effect_percent':100*(b['after']/d['after']-1) if d['valid'] and b['valid'] else None})
    write(out / 'comparison.json',rows)


if __name__ == '__main__':
    main()
