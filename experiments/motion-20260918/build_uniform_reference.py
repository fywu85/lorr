#!/usr/bin/env python3
"""Create a neutral NMS motion reference, preserving the archived implementation."""
import argparse
import hashlib
import json
from pathlib import Path
import shlex
import shutil
import subprocess
import sys
from snapshot_jobs import ROOT,HERE,sha,submit,write,cpu_resources
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',required=True,type=Path);p.add_argument('--base',required=True,type=Path);p.add_argument('--execute',action='store_true');a=p.parse_args();out=a.output.resolve();base=a.base.resolve()
if not a.execute:
 out.mkdir(parents=True,exist_ok=False)
 raise SystemExit(submit(out,[sys.executable,str(Path(__file__).resolve()),'--output',str(out),'--base',str(base),'--execute'],4,4,'motion_uniform_build'))
write(out/'build-allocation.json',cpu_resources());build=json.loads((base/'build.json').read_text())
b=(ROOT/'nms/build').resolve();flags={}
for line in (b/'CMakeFiles/lifelong.dir/flags.make').read_text().splitlines():
 if ' = ' in line:
  k,v=line.split(' = ',1);flags[k]=shlex.split(v)
link=shlex.split((b/'CMakeFiles/lifelong.dir/link.txt').read_text());obj=out/'uniform-environment.o'
compile=[link[0]]+flags['CXX_DEFINES']+flags['CXX_INCLUDES']+flags['CXX_FLAGS']+['-c',str(HERE/'nms_uniform_environment.cpp'),'-o',str(obj)]
subprocess.run(compile,cwd=str(b),check=True)
command=[]
for x in link:
 if x.endswith('/src/driver.cpp.o'):command.append(str(base/'nms-snapshot.o'))
 elif x.endswith('/Solution/Objects/Environment/environment.cpp.o'):command.append(str(obj))
 elif x.endswith('.o'):command.append(str(b/x))
 elif x=='lifelong':command.append(str(out/'nms-snapshot'))
 else:command.append(x)
subprocess.run(command,cwd=str(b),check=True)
shutil.copy2(base/'cgar-snapshot',out/'cgar-snapshot');shutil.copytree(base/'snapshots',out/'snapshots')
cases=json.loads((out/'snapshots/manifest.json').read_text())
for case in cases:case['snapshot']=str(out/'snapshots'/Path(case['snapshot']).name)
write(out/'snapshots/manifest.json',cases)
build['nms']['binary_sha256']=sha(out/'nms-snapshot');build['nms']['environment_override']={'source':str(HERE/'nms_uniform_environment.cpp'),'sha256':sha(HERE/'nms_uniform_environment.cpp'),'compile':compile,'link':command,'object_sha256':sha(obj)}
write(out/'build.json',build);write(out/'build-completion.json',{'valid':True})
