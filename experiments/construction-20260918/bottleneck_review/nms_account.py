"""Summarize verified NMS replay without qualifying its timed-out run as a score."""
import datetime
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
BASE = ROOT / 'experiments/construction-20260918'


def read(path):
    return json.loads(path.read_text())


def main():
    out = BASE / 'results/nms-task-pool-audit'
    d = read(out / 'nms_diagnostic.json')
    p = read(out / 'provenance.json')
    old = read(BASE / 'results/travel/provenance.json')
    assert p['inputs']['nms_diagnostic']['sha256'] == old['inputs']['nms_diagnostic']['sha256']
    for key in ['fixture_passed', 'loaded_action_fixture_passed', 'opposite_turn_fixture_passed',
                'phase_boundary_fixture_passed', 'initial_gap_fixture_passed']:
        assert p[key]
    assert d['team_size'] == 10000 and d['steps'] == 5000 and d['completed_tasks'] == 154981
    assert d['entry_timeouts'] == 1 and d['planner_errors'] == d['schedule_errors'] == 0
    assert d['assignments_after_pickup'] == 0
    a = d['completed_loaded_action_totals']; n = d['completed_tasks']
    amounts = {'completed_chain_lower_bound': d['completed_chain_shortest']['sum'],
               'completed_forward_detour': d['completed_loaded_forward_excess_over_shortest'],
               'completed_turns': a['cr'] + a['ccr'], 'completed_waits': a['wait'],
               'empty_including_unfinished': d['empty_robot_steps_including_reassignments'],
               'loaded_unfinished': d['loaded_robot_steps_including_unfinished_tasks'] - sum(a.values()),
               'idle': d['unassigned_robot_steps']}
    assert sum(amounts.values()) == 50000000 and min(amounts.values()) >= 0
    assert a['other'] == 0 and sum(a.values()) == d['completed_loaded_steps']['sum']
    assert amounts['completed_chain_lower_bound'] + amounts['completed_forward_detour'] == a['fw']
    assert sum(sum(row.values()) for row in d['full_phase_actions'].values()) == 50000000
    refs = read(BASE / 'results/current-bottleneck-audit-v50/accounting.json')
    per = {k: v / n for k, v in amounts.items()}
    cgar = refs['pooled_cgar']['per_completed_task']
    kk = refs['kittyknight']['per_completed_task']
    nms_root = ROOT / 'nms'
    inspected = ['CMakeLists.txt', 'src/Entry.cpp', 'src/MAPFPlanner.cpp', 'src/TaskScheduler.cpp',
                 'Solution/settings.hpp', 'Solution/Planner/eplanner.cpp', 'Solution/Planner/PIBT/pibts.cpp',
                 'Solution/Objects/Environment/environment.cpp', 'Solution/Objects/Environment/robot_handler.cpp',
                 'Solution/Objects/Environment/graph_guidance.cpp', 'Solution/Objects/Environment/heuristic_matrix.cpp',
                 'Solution/Objects/Environment/operations.cpp', 'Solution/Objects/Environment/info.cpp',
                 'Solution/Scheduler/scheduler.cpp', 'Solution/Scheduler/scheduler_solver.cpp']
    result = {'checked_utc': datetime.datetime.now(datetime.timezone.utc).isoformat(),
              'scope': 'Descriptive replay of a saved NMS run with one timeout and RSS above 32 GB; not an admissible performance score.',
              'tasks': n, 'robot_steps': 50000000, 'components': amounts, 'per_completed_task': per,
              'cgar_minus_nms_per_completed_task': {k: cgar[k] - per[k] for k in per},
              'kittyknight_per_completed_task': kk, 'pooled_cgar_per_completed_task': cgar,
              'historical_raw_hash_reproduced': True, 'all_five_action_fixtures_pass': True,
              'accounting_corrections': read(out / 'cohorts.json')['accounting_corrections'],
              'nms_source_root': str(nms_root.resolve()),
              'inspected_source_sha256': {name: hashlib.sha256((nms_root / name).read_bytes()).hexdigest() for name in inspected},
              'limits': ['Different task cohorts and resources; deltas are not causal recoverable savings.',
                         'The first timeout is idle time with its original non-motion markers retained.',
                         'The old NMS report remains unchanged; its initial idle/loaded residual error is explicitly corrected here.',
                         'Pool snapshots do not establish a bound between checkpoints or identify a rematching treatment effect.']}
    (out / 'accounting.json').write_text(json.dumps(result, indent=2) + '\n')
    for key in per:
        print('%s: CGAR %.6f KK %.6f NMS %.6f' % (key, cgar[key], kk[key], per[key]))


if __name__ == '__main__':
    main()
