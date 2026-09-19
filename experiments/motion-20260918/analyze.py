#!/usr/bin/env python3
"""Add disjoint action/wait diagnostics to the established full-horizon analysis."""
import importlib.util
import json
from pathlib import Path
HERE=Path(__file__).resolve().parent
spec=importlib.util.spec_from_file_location('assignment_analysis',HERE.parent/'assignment-20260918/analyze.py')
assignment=importlib.util.module_from_spec(spec);spec.loader.exec_module(assignment)
base=assignment.get_metrics

def movement(log):
    phases={};orientation={}
    for line in log.read_text().splitlines():
        if line.startswith('[cgar-movement] '):
            row={k:int(v) for k,v in (field.split('=',1) for field in line.split()[1:])};phases[row['phase']]=row
        elif line.startswith('[cgar-orientation] '):
            orientation={k:int(v) for k,v in (field.split('=',1) for field in line.split()[1:])}
    if phases:
        assert set(phases)=={0,1,2}
        for row in phases.values():
            assert row['wait']==row['planned_wait']+row['blocked_forward']+row['safety_cancel']
            assert row['fw']==row['closer']+row['farther']+row['equal']+row['unknown']
    return phases,orientation

def get_metrics(item, require_complete=True):
    row=base(item);phases,orientation=movement(item[1].with_suffix('.log'))
    complete=bool(phases) and all(x['steps']==row['steps'] for x in phases.values())
    assert complete or not require_complete, 'incomplete final movement diagnostics: '+str(item[1])
    row['movement_diagnostics']={'complete':complete,'required_steps':row['steps'],
                                 'observed_steps':sorted({x['steps'] for x in phases.values()})}
    if not complete:
        # Retain independently extracted full trajectory metrics. Optional
        # partial counters are explicitly separated and never called full-run.
        row['partial_movement_phases']=phases;row['partial_orientation']=orientation
        return row
    row['movement_phases']=phases;row['orientation']=orientation
    # The external action records independently check the diagnostic phase counts.
    before=phases[1]
    assert row['empty_forward_actions']==before['fw']
    assert row['empty_turn_actions']==before['cr']+before['ccr']
    assert row['empty_wait_actions']==before['wait']
    return row

if __name__=='__main__':
    assignment.analyze_matrix.get_metrics=get_metrics;assignment.analyze_matrix.main()
