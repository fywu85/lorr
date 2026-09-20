#!/usr/bin/env python3
"""Derive native Warehouse cost receipts from the verified unmodified NMS dump."""
import argparse
import datetime
import hashlib
import json
from pathlib import Path
import re

ROOT=Path(__file__).resolve().parents[4]
BASE=ROOT/'experiments/construction-20260918'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def fnv(values):
    value=14695981039346656037
    for byte in values:value=((value^byte)*1099511628211)&((1<<64)-1)
    return value

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--dump',type=Path,required=True)
    p.add_argument('--output',type=Path,required=True)
    p.add_argument('--report',type=Path,required=True)
    a=p.parse_args()
    reference=BASE/'tricks/nms_warehouse/asset-v2/generation.json'
    receipt=json.loads(reference.read_text())
    assert sha(a.dump)==receipt['native_dump_sha256']
    mapfile=ROOT/'mr24/warehouse.domain/maps/warehouse_large.map'
    assert sha(mapfile)==receipt['map_sha256']
    old=ROOT/'cgar/tricks/warehouse_lanes.hpp'
    assert sha(old)==receipt['generated_header_sha256']
    text=old.read_text().split('warehouse_masks[] =',1)[1]
    masks=''.join(re.findall(r'"([0-9a-fx]+)"',text));assert len(masks)==70000
    data=a.dump.read_text().splitlines();assert data[0]=='140 500' and len(data)==70001
    bits=[];bare=[];banded=[];free_edges=0
    for cell,line in enumerate(data[1:]):
        w=list(map(int,line.split()));assert len(w)==4
        if masks[cell]=='x':
            assert w==[0]*4
            bits.append(0);bare.extend([20]*4);banded.extend([20]*4)
            continue
        assert all(x in (20,21,200,201) for x in w)
        band=w[0]%20;assert all(x%20==band for x in w) and band in (0,1)
        bits.append(band)
        mask=int(masks[cell],16)
        for d,value in enumerate(w):
            price=200 if mask&(1<<d) else 20
            assert value==price+band
            bare.append(price);banded.append(value);free_edges+=1
    assert free_edges==38586*4
    packed=''.join(format(sum(bits[i+j]<<j for j in range(4)),'x') for i in range(0,len(bits),4))
    header=['#pragma once','#include <cstdint>',
            '// TRICK: native NMS Warehouse weights; generated from the verified unmodified dump.',
            '// Direction masks remain in warehouse_lanes.hpp; this asset records native +1 bands.',
            'namespace cgar { namespace tricks {']
    fields={}
    for name,values in [('nobands',bare),('bands',banded)]:
        h=hashlib.sha256(bytes(values)).hexdigest();fingerprint=fnv(values)
        fields[name]=dict(field_sha256=h,fnv1a64=fingerprint,forward_base=20,opposing=200,turn=1,band_add=int(name=='bands'))
        header+=['inline constexpr char warehouse_native_'+name+'_field_sha256[] = "'+h+'";',
                 'inline constexpr uint64_t warehouse_native_'+name+'_fnv1a64 = '+str(fingerprint)+'ULL;']
    header+=['inline constexpr char warehouse_native_band_hex[] =']
    header+=['    "'+packed[i:i+100]+'"' for i in range(0,len(packed),100)]
    header+=['    ;','static_assert(sizeof(warehouse_native_band_hex) == 17500 + 1, "native band asset size");','}}','']
    a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text('\n'.join(header))
    a.report.parent.mkdir(parents=True,exist_ok=True)
    a.report.write_text(json.dumps(dict(created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
        native_dump=str(a.dump),native_dump_sha256=sha(a.dump),verified_generation_receipt=str(reference.relative_to(ROOT)),
        generation_receipt_sha256=sha(reference),legacy_asset_sha256=sha(old),map_sha256=sha(mapfile),
        generated_header_sha256=sha(a.output),native_free_directed_values_checked=free_edges,
        bands_recovered_from_dump=True,band_free_cells=sum(bits),fields=fields,
        walls='Unused wall slots normalized from native0 to20; every free directed native weight is exact.'),indent=2)+'\n')
    print(json.dumps(fields))
if __name__=='__main__':main()
