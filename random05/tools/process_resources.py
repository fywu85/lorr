"""Read-only resource sampling for descendants of one benchmark wrapper."""
import json
import subprocess
import time
from pathlib import Path


def monitored_run(command, cwd, env, output, log, interval=0.02):
    process = subprocess.Popen(command, cwd=cwd, env=env, stdout=log, stderr=subprocess.STDOUT)
    report = dict(interval_seconds=interval, samples=0, processes={}, recent=[])
    while True:
        pending=[process.pid];seen=set();snapshot=[]
        while pending:
            pid=pending.pop()
            if pid in seen:continue
            seen.add(pid);base=Path('/proc')/str(pid)
            try:
                pending.extend(int(v) for v in (base/'task'/str(pid)/'children').read_text().split())
                fields={}
                for line in (base/'status').read_text().splitlines():
                    key,_,value=line.partition(':')
                    if key in ('Name','Threads','VmSize','VmPeak','VmRSS','VmHWM'):
                        fields[key]=value.strip() if key=='Name' else int(value.split()[0])
                key=str(pid)
                if key not in report['processes']:
                    report['processes'][key]=dict(name=fields.get('Name'),limits=(base/'limits').read_text(),cgroup=(base/'cgroup').read_text(),peaks={})
                record=report['processes'][key]
                for field,value in fields.items():
                    if field!='Name':record['peaks'][field]=max(record['peaks'].get(field,0),value)
                snapshot.append(dict(pid=pid,**fields))
            except (FileNotFoundError,ProcessLookupError,PermissionError):
                pass
        report['samples']+=1
        report['recent'].append(dict(monotonic=time.monotonic(),processes=snapshot))
        report['recent']=report['recent'][-32:]
        code=process.poll()
        if code is not None:break
        time.sleep(interval)
    report['exit']=code
    Path(output).write_text(json.dumps(report,indent=2)+'\n')
    return code
