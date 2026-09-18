#!/usr/bin/env python3
"""Run large trajectory analysis on reserved cores after a matrix finishes."""
import argparse
from pathlib import Path
import shlex
import subprocess
import sys
ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'tools'))
from benchmark_gridengine import write
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--input',type=Path,required=True);p.add_argument('--output',type=Path,required=True)
p.add_argument('--hold-job');args=p.parse_args();raw=args.input.resolve()
command=['/usr/bin/python3',str(Path(__file__).with_name('analyze.py')),'--input',str(raw),'--output',str(args.output.resolve())]
script=raw/'assignment-analysis.sh';script.write_text('#!/bin/bash\nset -eu\nexec '+' '.join(shlex.quote(x) for x in command)+'\n')
submit=['/opt/n1ge/bin/lx24-amd64/qsub','-terse','-w','e','-cwd','-q','debian.q','-pe','threaded','4','-binding','linear:4','-l','exclusive=true,h_rt=00:30:00,h_vmem=4G','-m','n','-N','lorr_analysis','-j','y','-o',str(raw/'assignment-analysis.log'),'-S','/bin/bash']
if args.hold_job:submit+=['-hold_jid',args.hold_job]
submit.append(str(script))
r=subprocess.run(submit,cwd=str(ROOT),text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
write(raw/'assignment-analysis-submission.json',{'command':submit,'returncode':r.returncode,'response':r.stdout});print(r.stdout,end='');raise SystemExit(r.returncode)
