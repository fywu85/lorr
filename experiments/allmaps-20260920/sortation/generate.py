#!/usr/bin/env python3
"""Generate explicit Sortation fields from the unchanged archived NMS constructor."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import shlex
import shutil
import subprocess
import sys

ROOT=next(p for p in Path(__file__).resolve().parents if (p/'tools/cpu_resources.py').is_file())
HERE=ROOT/'experiments/allmaps-20260920/sortation'


def sha(path):return hashlib.sha256(path.read_bytes()).hexdigest()
def read(path):return json.loads(path.read_text())
def write(path,value):path.write_text(json.dumps(value,indent=2)+'\n')


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--output',type=Path,required=True);p.add_argument('--execute',action='store_true')
    a=p.parse_args();out=a.output.resolve();src=ROOT/'nms/Solution'
    native=[src/('Objects/'+n) for n in ['Environment/graph_guidance.cpp','Environment/map.cpp','Environment/info.cpp','Basic/position.cpp']]
    map_path=ROOT/'mr24/warehouse.domain/maps/sortation_large.map'
    paths=native+list(src.rglob('*.hpp'))+list((ROOT/'nms/inc').rglob('*.h'))+[map_path]
    source_hashes={str(x.relative_to(ROOT)):sha(x) for x in paths}
    support=out/'support'
    if not a.execute:
        support.mkdir(parents=True,exist_ok=False)
        for name in ['generate.py','dump_native.cpp']:shutil.copy2(HERE/name,support/name)
        write(out/'request.json',dict(sources=source_hashes,support={n:sha(support/n) for n in ['generate.py','dump_native.cpp']}))
        command=['/usr/bin/python3',str(support/'generate.py'),'--execute','--output',str(out)]
        job=out/'job.sh';job.write_text('#!/bin/bash\nset -eu\nexec '+' '.join(map(shlex.quote,command))+'\n')
        submit=['qsub','-h','-terse','-w','e','-cwd','-q','debian.q','-pe','threaded','1','-binding','linear:1',
                '-l','exclusive=false,h_rt=00:10:00,h_vmem=4G','-m','n','-N','sortation_asset','-j','y','-o',str(out/'job.log'),'-S','/bin/bash',str(job)]
        r=subprocess.run(submit,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True)
        write(out/'submission.json',dict(command=submit,returncode=r.returncode,response=r.stdout));r.check_returncode()
        print(r.stdout,end='',flush=True);subprocess.run(['qrls',r.stdout.strip()],check=True);return
    request=read(out/'request.json');assert request['sources']==source_hashes
    for name,expected in request['support'].items():assert sha(support/name)==expected
    sys.path.insert(0,str(ROOT/'tools'));from cpu_resources import cpu_resources
    resources=cpu_resources();assert resources['effective_cpu_quota'] is None and resources['physical_cores_visible']==1
    os.sched_setaffinity(0,resources['representative_cpus']);write(out/'allocation.json',resources)
    compiler=ROOT/'env/bin/x86_64-conda-linux-gnu-c++'
    command=[str(compiler),'-std=c++17','-O2','-ffunction-sections','-fdata-sections','-Wl,--gc-sections',
             '-I'+str(src),'-I'+str(ROOT/'nms/inc'),'-isystem',str(ROOT/'env/include'),str(support/'dump_native.cpp')]
    command+=list(map(str,native))+['-o',str(out/'dump_native')]
    subprocess.run(command,check=True,cwd=ROOT)
    with (out/'native-forward.txt').open('w') as stream:
        subprocess.run([str(out/'dump_native'),str(map_path)],check=True,stdout=stream,cwd=out)
    data=(out/'native-forward.txt').read_text().splitlines();rows,cols=map(int,data[0].split())
    assert (rows,cols)==(140,500) and len(data)==rows*cols+1
    text_map=''.join(map_path.read_text().splitlines()[4:]);assert len(text_map)==rows*cols
    masks=[];occupancy=[];bands=[];adapted=[];native_bands=[];native_nobands=[]
    for cell,line in enumerate(data[1:]):
        values=list(map(int,line.split()));assert len(values)==4
        wall=text_map[cell] in '@T';occupancy.append(int(wall))
        if wall:
            assert values==[0]*4;masks.append('x');bands.append(0)
        else:
            assert all(v in (20,21,200,201) for v in values)
            assert len({v%2 for v in values})==1
            masks.append(format(sum(1<<d for d,v in enumerate(values) if v>=200),'x'));bands.append(values[0]%2)
        adapted.extend(4 if wall or v<200 else 16 for v in values)
        native_bands.extend([20]*4 if wall else values)
        native_nobands.extend([20]*4 if wall else [v-bands[-1] for v in values])
    # Independently check the declared source's row/column bands and both lane masks.
    for cell in range(rows*cols):
        if occupancy[cell]:continue
        r,c=divmod(cell,cols);expected=[20]*4
        expected[0 if ((0b1001>>(r%4))&1) else 2]=200
        expected[1 if ((0b1100>>(c%4))&1) else 3]=200
        extra=int(r<=8 or r>=130)
        assert native_nobands[4*cell:4*cell+4]==expected
        assert native_bands[4*cell:4*cell+4]==[v+extra for v in expected]
    def digest(values):return hashlib.sha256(bytes(values)).hexdigest()
    def fnv(values):
        value=14695981039346656037
        for x in values:value=((value^x)*1099511628211)&((1<<64)-1)
        return value
    bithex=''.join(format(sum(bands[i+j]<<j for j in range(min(4,len(bands)-i))),'x') for i in range(0,len(bands),4))
    header=['#pragma once','// Generated from unchanged NMS Sortation source; explicit --trick SORTATION only.',
            '// Generator: experiments/allmaps-20260920/sortation/generate.py.',
            'namespace cgar { namespace tricks {',
            'inline constexpr int sortation_rows = 140, sortation_cols = 500, sortation_free = '+str(occupancy.count(0))+';']
    hashes={'sortation_map_sha256':sha(map_path),'sortation_occupancy_sha256':digest(occupancy),
            'sortation_field_sha256':digest(adapted),'sortation_native_bands_field_sha256':digest(native_bands),
            'sortation_native_nobands_field_sha256':digest(native_nobands)}
    header+=['inline constexpr char '+name+'[] = "'+value+'";' for name,value in hashes.items()]
    header+=['inline constexpr uint64_t sortation_native_'+name+'_fnv1a64 = '+str(fnv(values))+'ULL;' for name,values in [('bands',native_bands),('nobands',native_nobands)]]
    for name,value in [('sortation_masks',''.join(masks)),('sortation_native_band_hex',bithex)]:
        header+=['inline constexpr char '+name+'[] =']+['    "'+value[k:k+100]+'"' for k in range(0,len(value),100)]+['    ;']
    header+=['static_assert(sizeof(sortation_masks) == 140 * 500 + 1, "sortation asset size");',
             'static_assert(sizeof(sortation_native_band_hex) == (140 * 500 + 3) / 4 + 1, "sortation band size");','}}','']
    # Include the type dependency explicitly, independent of include order.
    header.insert(1,'#include <cstdint>')
    (out/'sortation_native.hpp').write_text('\n'.join(header))
    write(out/'generation.json',dict(completed_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),track='TRICK',activation='--trick SORTATION',native_source_unchanged=True,compiler_command=command,native_binary_sha256=sha(out/'dump_native'),native_dump_sha256=sha(out/'native-forward.txt'),header_sha256=sha(out/'sortation_native.hpp'),rows=rows,cols=cols,free_cells=occupancy.count(0),field_hashes=hashes,verified_forward_values=rows*cols*4,verified_nonforward_values=rows*cols*4*3,independent_formula_check=True,direction_offsets=[1,cols,-1,-cols],wall_normalization='unused4 for adapted lanes, unused20 for native field; NMS stores0onwalls'))
    assert request['sources']=={str(x.relative_to(ROOT)):sha(x) for x in paths}
    print('SORTATION_FIELD_VERIFIED',occupancy.count(0),hashes,flush=True)


if __name__=='__main__':main()
