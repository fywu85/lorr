#!/usr/bin/env python3
"""Analyze a completed matrix on four reserved CPU cores."""
import argparse
import json
from pathlib import Path
import shlex
import subprocess
import sys
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--input',required=True,type=Path);p.add_argument('--output',required=True,type=Path);p.add_argument('--hold-job');p.add_argument('--control',default='baseline');a=p.parse_args()
raw=a.input.resolve();script=raw/'motion-analysis.sh'
cmd=[sys.executable,str(HERE/'analyze.py'),'--input',str(raw),'--output',str(a.output.resolve()),'--control',a.control]
script.write_text('#!/bin/bash\nset -eu\nexec '+' '.join(shlex.quote(x) for x in cmd)+'\n')
submit=['/opt/n1ge/bin/lx24-amd64/qsub','-terse','-w','e','-cwd','-q','debian.q','-pe','threaded','4','-binding','linear:4','-l','exclusive=true,h_rt=00:30:00,h_vmem=4G','-m','n','-N','motion_analysis','-j','y','-o',str(raw/'motion-analysis.log'),'-S','/bin/bash']
if a.hold_job:submit+=['-hold_jid',a.hold_job]
run=subprocess.run(submit+[str(script)],cwd=str(ROOT),text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
(raw/'motion-analysis-submission.json').write_text(json.dumps({'command':submit+[str(script)],'response':run.stdout,'exit':run.returncode},indent=2)+'\n');print(run.stdout,end='');raise SystemExit(run.returncode)
