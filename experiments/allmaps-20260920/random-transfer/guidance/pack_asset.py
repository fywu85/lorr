#!/usr/bin/env python3
"""Pack the checked integer adaptation; --check verifies the committed header."""
import argparse
import hashlib
import json
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = next(p for p in HERE.parents if (p/'tools/cpu_resources.py').is_file())

def render():
    raw = (HERE/'field-integer.json').read_bytes()
    field = json.loads(raw)
    proof = json.loads((HERE/'generation.json').read_text())
    assert hashlib.sha256(raw).hexdigest() == proof['adapted_json_sha256']
    weights, occupancy = field['forward'], field['occupancy']
    assert len(weights) == 4096 and len(occupancy) == 1024 and occupancy.count(0) == 819
    assert hashlib.sha256(bytes(weights)).hexdigest() == proof['adapted_forward_bytes_sha256']
    assert hashlib.sha256(bytes(occupancy)).hexdigest() == proof['occupancy_bytes_sha256']
    original = json.loads((HERE/'field-float.json').read_text())['cell_weights']
    assert all(w == (20 if occupancy[i//4] else int(original[(i//4)*5+i%4]*10+.5)) for i,w in enumerate(weights))
    masks = ''.join('x' if wall else '0' for wall in occupancy)
    def fnv(values):
        value = 14695981039346656037
        for cost in values: value = ((value ^ cost)*1099511628211) & ((1<<64)-1)
        return value
    lines = ['#pragma once', '#include <cstdint>',
             '// Map-tuned RANDOM field from standalone source 233f5bf, integer adaptation.',
             '// Explicit --trick RANDOM-04 / RANDOM-05 only. Generator: guidance/pack_asset.py.',
             '// Round half-up x10; maximum original-unit error 0.04999218. Turn cost: 6.',
             'namespace cgar { namespace tricks {',
             'inline constexpr int random_rows = 32, random_cols = 32, random_free = 819;']
    for name,value in [('occupancy',proof['occupancy_bytes_sha256']),('native_nobands_field',proof['adapted_forward_bytes_sha256']),('uniform_field',hashlib.sha256(bytes([20]*4096)).hexdigest())]:
        lines.append('inline constexpr char random_'+name+'_sha256[] = "'+value+'";')
    lines += ['inline constexpr uint64_t random_native_nobands_fnv1a64 = '+str(fnv(weights))+'ULL;',
              'inline constexpr uint64_t random_uniform_fnv1a64 = '+str(fnv([20]*4096))+'ULL;',
              'inline constexpr char random_masks[] =']
    lines += ['    "'+masks[i:i+64]+'"' for i in range(0,len(masks),64)]
    lines[-1] += ';'
    lines += ['inline constexpr uint8_t random_forward[] = {']
    lines += ['    '+','.join(map(str,weights[i:i+32]))+',' for i in range(0,len(weights),32)]
    lines += ['};','}}  // namespace cgar::tricks','']
    return '\n'.join(lines)

if __name__ == '__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--check',action='store_true');a=p.parse_args()
    target=ROOT/'cgar/tricks/random_native.hpp';text=render()
    if a.check: assert target.read_text()==text, 'generated RANDOM field differs'
    else: target.write_text(text)
    print('RANDOM_INTEGER_FIELD_PACK_VERIFIED 4096 values')
