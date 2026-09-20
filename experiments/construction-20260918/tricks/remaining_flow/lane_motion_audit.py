#!/usr/bin/env python3
"""Replay verified Warehouse runs for task-cohort and static-lane motion accounting."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import re
import shlex
import shutil
import subprocess
import sys

ROOT = next(p for p in Path(__file__).resolve().parents if (p / 'tools/cpu_resources.py').is_file())
BASE = ROOT / 'experiments/construction-20260918'

def read(p): return json.loads(p.read_text())
def write(p,d): p.write_text(json.dumps(d,indent=2)+'\n')
def digest(p):
    h=hashlib.sha256()
    with p.open('rb') as f:
        for block in iter(lambda:f.read(4*1024*1024),b''): h.update(block)
    return h.hexdigest()

def augment(source):
    def replace(old,new):
        nonlocal source
        assert source.count(old)==1,old
        source=source.replace(old,new)
    replace('if (argc != 4) throw std::runtime_error("usage: decompose MAP RESULT OUTPUT");',
            'if (argc != 4 && argc != 5) throw std::runtime_error("usage: decompose MAP RESULT OUTPUT [LANE_FIELD]");')
    replace('    json report;','''    std::vector<int> lane_costs;
    if(argc==5){
        json field;std::ifstream(argv[4])>>field;
        if(field.at("rows")!=rows||field.at("cols")!=cols)throw std::runtime_error("lane field dimensions");
        const auto supplied=field.at("free").get<std::vector<int>>();
        if(supplied.size()!=free.size())throw std::runtime_error("lane field occupancy size");
        for(size_t i=0;i<free.size();++i)if(supplied[i]!=int(free[i]))throw std::runtime_error("lane field occupancy");
        lane_costs=field.at("forward_costs").get<std::vector<int>>();
        if(lane_costs.size()!=free.size()*4)throw std::runtime_error("lane field size");
        for(int value:lane_costs)if(value!=4&&value!=16)throw std::runtime_error("unexpected lane cost");
    }
    std::array<std::array<long long,5>,3> lane_forward{},lane_against{};
    long long completed_loaded_against=0;
    json report;''')
    replace('            int previous_step = -2, previous_task = -2, previous_phase = -1, previous_kind = -1;', '''            std::vector<char> against(data.at("makespan").get<int>(),false);
            if(!lane_costs.empty()){
                int at_cell=cell,at_heading=direction;
                for(size_t t=0;t<against.size();++t){
                    const char action=path.at(2*t);
                    if(action=='F'){
                        against[t]=lane_costs.at(at_cell*4+at_heading)>4;
                        at_cell=neighbor(at_cell,at_heading);
                    }else if(action=='R')at_heading=(at_heading+1)%4;
                    else if(action=='C')at_heading=(at_heading+3)%4;
                    if(at_cell<0||!free.at(at_cell))throw std::runtime_error("static lane replay invalid position");
                }
            }
            int previous_step = -2, previous_task = -2, previous_phase = -1, previous_kind = -1;''')
    replace('                    ++phase_actions[phase][kind];','''                    ++phase_actions[phase][kind];
                    if(!lane_costs.empty()&&kind==0){
                        const int window=(at-1)/std::max(1,(data.at("makespan").get<int>()+4)/5);
                        ++lane_forward[phase][window];lane_against[phase][window]+=against.at(at-1);
                        if(completed_loaded)completed_loaded_against+=against.at(at-1);
                    }''')
    replace('    std::ofstream(argv[3]) << report.dump(2)', '''    report["static_lane_accounting_enabled"]=!lane_costs.empty();
    if(!lane_costs.empty()){
        for(int phase=0;phase<3;++phase){
            const std::string name=phase==0?"idle":phase==1?"empty":"loaded";
            const auto fw=std::accumulate(lane_forward[phase].begin(),lane_forward[phase].end(),0LL);
            const auto against=std::accumulate(lane_against[phase].begin(),lane_against[phase].end(),0LL);
            if(fw!=report.at("full_phase_actions").at(name).at("fw").get<long long>()||against>fw)
                throw std::runtime_error("static lane action accounting differs");
            report["static_lane_motion"][name]={{"forward",fw},{"against_lane",against},
                {"forward_windows",lane_forward[phase]},{"against_lane_windows",lane_against[phase]}};
        }
        report["completed_loaded_against_lane"]=completed_loaded_against;
    }
    std::ofstream(argv[3]) << report.dump(2)''')
    return source

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--verification',type=Path,required=True)
    p.add_argument('--output',type=Path,required=True)
    p.add_argument('--archive',type=Path,required=True)
    p.add_argument('--execute',action='store_true')
    a=p.parse_args();raw=a.output.resolve();out=a.archive.resolve()
    if not a.execute:
        verification=read(a.verification)
        assert verification['all_valid_within_deadline_and_memory'] and not verification['failures']
        assert all(r['environment'].get('CGAR_TRICK_LANES')=='1' for r in verification['rows'])
        raw.mkdir(exist_ok=False,parents=True)
        files=[]
        for source,name in [(Path(__file__),'lane_motion_audit.py'),(a.verification,'verification.json'),
                            (BASE/'decompose.py','decompose.py'),(ROOT/'cgar/tricks/warehouse_lanes.hpp','warehouse_lanes.hpp'),
                            (ROOT/'cgar/inc/nlohmann/json.hpp','json.hpp'),(ROOT/'mr24/warehouse.domain/maps/warehouse_large.map','warehouse.map')]:
            dest=raw/name;shutil.copy2(source,dest);files.append(dest)
        cpp=raw/'decompose.cpp';cpp.write_text(augment((BASE/'decompose.cpp').read_text()));files.append(cpp)
        header=(raw/'warehouse_lanes.hpp').read_text();suffix=header.split('warehouse_masks[] =',1)[1]
        masks=''.join(re.findall(r'"([0-9a-fx]+)"',suffix));assert len(masks)==140*500
        costs=[4]*(len(masks)*4)
        for cell,ch in enumerate(masks):
            if ch!='x':
                for d in range(4):
                    if int(ch,16)&(1<<d):costs[cell*4+d]=16
        field_sha=re.search(r'warehouse_field_sha256\[\] = "([0-9a-f]+)"',header).group(1)
        assert hashlib.sha256(bytes(costs)).hexdigest()==field_sha
        field=raw/'lane-field.json';write(field,dict(rows=140,cols=500,free=[int(x!='x') for x in masks],forward_costs=costs));files.append(field)
        write(raw/'request.json',dict(created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
              root=str(ROOT),archive=str(out),files={str(f):digest(f) for f in files},field_sha256=field_sha))
        command=['/usr/bin/python3',str(raw/'lane_motion_audit.py'),'--verification',str(raw/'verification.json'),
                 '--output',str(raw),'--archive',str(out),'--execute']
        job=raw/'job.sh';job.write_text('#!/bin/bash\nset -eu\nexec '+' '.join(map(shlex.quote,command))+'\n')
        submit=['qsub','-h','-terse','-w','n','-cwd','-q','debian.q','-pe','threaded','1','-binding','linear:1',
                '-l','exclusive=false,h_rt=01:00:00,h_vmem=8G','-m','n','-N','lane_motion_audit','-j','y','-o',str(raw/'job.log'),'-S','/bin/bash',str(job)]
        r=subprocess.run(submit,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True)
        write(raw/'submission.json',dict(command=submit,returncode=r.returncode,response=r.stdout));print(r.stdout,end='')
        if r.returncode:return r.returncode
        assert re.fullmatch(r'\d+\s*',r.stdout)
        subprocess.run(['qrls',r.stdout.strip()],check=True);return 0
    request=read(raw/'request.json');assert Path(request['root'])==ROOT and Path(request['archive'])==out
    for name,sha in request['files'].items():assert digest(Path(name))==sha,name
    # The existing wrapper validates five independent task/action boundary fixtures.
    # Its compiler include must still describe the frozen nlohmann header.
    assert digest(ROOT/'cgar/inc/nlohmann/json.hpp')==digest(raw/'json.hpp')
    sys.path.insert(0,str(ROOT/'tools'));from cpu_resources import cpu_resources
    resources=cpu_resources();assert resources['effective_cpu_quota'] is None
    cpu=resources['representative_cpus'][0];os.sched_setaffinity(0,{cpu})
    write(raw/'allocation.json',dict(resources=resources,selected_cpu=cpu,job_id=os.environ['JOB_ID']))
    replay=raw/'replay';write(raw/'fixture-cases.json',{'extra_loaded_fixture':str(replay/'loaded-fixture.json')})
    subprocess.run(['/usr/bin/python3',str(raw/'decompose.py'),'--output',str(replay),'--cases',str(raw/'fixture-cases.json'),'--map',str(replay/'fixture.map')],check=True)
    # Hand count one empty contraflow step and one loaded contraflow step.
    fixture_cost=[4]*40;fixture_cost[0]=16;fixture_cost[2*4+1]=16
    write(raw/'fixture-field.json',dict(rows=2,cols=5,free=[1]*10,forward_costs=fixture_cost))
    binary=replay/'decompose'
    subprocess.run([str(binary),str(replay/'fixture.map'),str(replay/'loaded-fixture.json'),str(raw/'lane-fixture-result.json'),str(raw/'fixture-field.json')],check=True)
    f=read(raw/'lane-fixture-result.json')
    assert f['static_lane_motion']['empty']['forward']==2 and f['static_lane_motion']['empty']['against_lane']==1
    assert f['static_lane_motion']['loaded']['forward']==1 and f['static_lane_motion']['loaded']['against_lane']==1
    assert f['completed_loaded_against_lane']==1 and f['static_lane_motion']['idle']['forward']==0
    assert f['static_lane_motion']['empty']['against_lane_windows']==[1,0,0,0,0]
    assert f['static_lane_motion']['loaded']['against_lane_windows']==[0,0,0,0,1]
    # A field mismatch must be rejected before accounting.
    wrong=read(raw/'fixture-field.json');wrong['free'][0]=0;write(raw/'bad-field.json',wrong)
    r=subprocess.run([str(binary),str(replay/'fixture.map'),str(replay/'loaded-fixture.json'),str(raw/'must-not-exist.json'),str(raw/'bad-field.json')],stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True)
    assert r.returncode!=0 and 'lane field occupancy' in r.stdout and not (raw/'must-not-exist.json').exists()
    out.mkdir(parents=True,exist_ok=False);reports={};inputs={}
    for row in read(raw/'verification.json')['rows']:
        name=row['case'];trajectory=Path(row['raw_case'])/'WAREHOUSE.json';sha=digest(trajectory)
        result=raw/(name+'.json')
        subprocess.run([str(binary),str(raw/'warehouse.map'),str(trajectory),str(result),str(raw/'lane-field.json')],check=True)
        d=read(result);assert d['completed_tasks']==row['tasks'] and d['team_size']==10000 and d['steps']==5000
        assert d['empty_robot_steps_including_reassignments']==row['empty_robot_steps'] and d['assignments_after_pickup']==0
        assert sum(sum(phase.values()) for phase in d['full_phase_actions'].values())==50000000
        assert not any(d[k] for k in ['planner_errors','schedule_errors','entry_timeouts'])
        assert digest(trajectory)==sha
        inputs[name]=dict(path=str(trajectory),sha256=sha,trajectory_sha256=row['trajectory_sha256'])
        reports[name]=d;shutil.copy2(result,out/result.name)
        print('LANE_MOTION_COMPLETE',name,d['completed_tasks'],flush=True)
    for name in ['verification.json','request.json','submission.json','allocation.json','lane-fixture-result.json']:
        shutil.copy2(raw/name,out/name)
    write(out/'accounting.json',dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),inputs=inputs,
          source_files=request['files'],binary_sha256=digest(binary),field_sha256=request['field_sha256'],
          base_fixtures_verified=True,independent_lane_phase_fixture=True,field_mismatch_rejected=True,
          limits='Read-only completed-cohort and static-lane movement accounting; no causal attribution or new benchmark score.'))
    print('LANE_MOTION_AUDIT_COMPLETE',flush=True);return 0

if __name__=='__main__':raise SystemExit(main())
