#!/usr/bin/env python3
"""Summarize same-tick cumulative assignment counters from verified full runs."""
import argparse,datetime,hashlib,json,re
from pathlib import Path

def read(p):return json.loads(p.read_text())
def fields(line):return {k:int(v) for k,v in re.findall(r'(\w+)=(-?\d+)',line)}
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--comparison',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args()
 verified=read(a.comparison);assert verified['full_run'] and verified['all_valid_within_deadline_and_memory'] and not verified['failures']
 rows=[]
 for row in verified['rows']:
  raw=Path(row['raw_case']);path=raw/'WAREHOUSE.log';selected={}
  for line in path.read_text().splitlines():
   if line.startswith('[cgar] t='):key='agent'
   elif line.startswith('[cgar-scheduler] t='):key='scheduler'
   elif line.startswith('[cgar-unopened-match] t='):key='matching'
   else:continue
   f=fields(line)
   if f['t']==4800:assert key not in selected;selected[key]=f
  assert set(selected)=={'agent','scheduler','matching'} and selected['matching']['enabled']==int(row['environment']['CGAR_TRICK_UNOPENED_MATCH'])
  m=selected['matching'];assigned=selected['agent']['assigned'];assert assigned>0
  unit=int(row['environment']['CGAR_FLOW_COST_SCALE'])
  rows.append(dict(case=row['case'],seed=row['seed'],tasks=row['tasks'],short=int(row['environment']['CGAR_TRICK_SHORT_TASKS']),
   bands=int(row['environment']['CGAR_TRICK_NATIVE_BANDS']),pickup_weight=int(row['environment']['CGAR_PICKUP_WEIGHT']),
   cumulative_through_tick=4800,assignments_including_initial=assigned,moved=m['moved'],moved_per_assignment=m['moved']/assigned,
   accepted_cycles=m['accepted_cycles'],saving_guidance_units=m['saving'],saving_forward_base_units=m['saving']/unit,
   eligible_exposures=m['eligible'],budget_protected_exposures=m['budget_protected'],fair_protected_exposures=m['fair_protected'],
   budget_share_of_eligible_plus_budget=m['budget_protected']/max(1,m['budget_protected']+m['eligible']),
   resident_exposures=m['resident'],missing_table_exposures=m['missing'],matrix_entries=m['matrix_entries'],
   trajectory_sha256=row['trajectory_sha256'],log_sha256=hashlib.sha256(path.read_bytes()).hexdigest(),counters=selected))
 out=dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),comparison=str(a.comparison),comparison_sha256=hashlib.sha256(a.comparison.read_bytes()).hexdigest(),rows=rows,
  scope='Cumulative counters at t4800 only, through the scheduler preceding that plan. Ratios use assignments including initial10000. Exposures count repeated visits, not unique tasks. Guidance savings divided by forward base are not physical robot ticks or causal throughput. No inference that excluded holders have beneficial cycles. Full quality/validity and source binding come from the linked comparison.')
 a.output.write_text(json.dumps(out,indent=2)+'\n');print('ASSIGNMENT_COUNTER_AUDIT',len(rows),'cases')
if __name__=='__main__':main()
