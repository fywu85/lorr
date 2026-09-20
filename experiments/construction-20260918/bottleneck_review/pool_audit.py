"""Replay task-pool cohorts on GRID; these observations are never planner inputs."""
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

ROOT = Path(__file__).resolve().parents[3]
BASE = ROOT / 'experiments/construction-20260918'
sys.path.insert(0, str(ROOT / 'tools'))
from cpu_resources import cpu_resources


def digest(path):
    h = hashlib.sha256()
    with Path(path).open('rb') as stream:
        for data in iter(lambda: stream.read(4 * 1024 * 1024), b''):
            h.update(data)
    return h.hexdigest()


def write(path, data):
    Path(path).write_text(json.dumps(data, indent=2) + '\n')


def augmented_source(source):
    def replace(old, new):
        nonlocal source
        assert source.count(old) == 1, old
        source = source.replace(old, new)
    replace('struct Assignment { int step, robot, cell; };',
            'struct Assignment { int step, robot, cell, end; };')
    replace('tasks.at(index.at(task_id)).assignments.push_back({step, int(robot), cell});',
            'tasks.at(index.at(task_id)).assignments.push_back({step, int(robot), cell,\n'
            '                    slot + 1 < entries.size() ? entries[slot + 1].first - 1 : data.at("makespan").get<int>()});')
    marker = '    std::ofstream(argv[3]) << report.dump(2)'
    replace(marker, r'''
    // End-of-tick snapshots: just-revealed tasks are present, finished tasks gone,
    // and assignments applied on this tick persist until their recorded replacement.
    // This is descriptive replay, with no hidden future use in a planner.
    report["pool_snapshots"] = json::array();
    const int horizon = report.at("steps");
    const int stride = std::max(1, horizon / 5);
    for (int at = 0; ; at = std::min(horizon, at + stride)) {
        std::array<std::vector<long long>, 8> chains, ages;
        const std::array<std::string, 8> names{
            "outstanding", "unassigned", "held_unopened", "started",
            "never_assigned", "unassigned_previously_assigned", "unpicked_age_at_least_1000", "unpicked_age_at_least_4000"};
        for (const auto& task : tasks) {
            if (task.revealed > at || (task.finished >= 0 && task.finished <= at)) continue;
            int owners = 0; bool ever = false;
            for (const auto& assignment : task.assignments) {
                ever |= assignment.step <= at;
                owners += assignment.step <= at && at <= assignment.end;
            }
            if (owners > 1) throw std::runtime_error("task has overlapping assignment intervals");
            const bool opened = task.pickup >= 0 && task.pickup <= at;
            if (opened && !owners) throw std::runtime_error("started outstanding task lost its owner");
            const long long age = at - task.revealed;
            auto add = [&](int group) { chains[group].push_back(task.chain); ages[group].push_back(age); };
            add(0);
            if (opened) add(3);
            else if (owners) add(2);
            else add(1);
            if (!ever) add(4);
            if (!owners && ever) add(5);
            if (!opened && age >= 1000) add(6);
            if (!opened && age >= 4000) add(7);
        }
        if (chains[0].size() != chains[1].size() + chains[2].size() + chains[3].size() ||
            chains[1].size() != chains[4].size() + chains[5].size())
            throw std::runtime_error("task-pool partitions do not conserve task count");
        json snapshot = {{"step", at}};
        for (size_t k = 0; k < names.size(); ++k)
            snapshot[names[k]] = {{"chain", distribution(chains[k])}, {"age", distribution(ages[k])}};
        report["pool_snapshots"].push_back(snapshot);
        if (at == horizon) break;
    }
    report["pool_limits"] = {"End-of-tick snapshots; task reveals on that tick are included.",
        "Chain distance ignores turns and traffic; task progress does not shorten this original whole chain.",
        "Long unfinished chains alone do not prove deliberate stranding; horizon censoring and task-prefix differences remain.",
        "Pool age includes service time for started tasks; unpicked age is reported separately."};
''' + marker)
    return source


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--execute', action='store_true')
    parser.add_argument('--references', type=Path, help='Optional JSON mapping case names to earlier accounting reports')
    parser.add_argument('--result-name', default='task-pool-audit-v50')
    args = parser.parse_args(); raw = args.output.resolve()
    if Path(args.result_name).name != args.result_name or args.result_name in ('', '.', '..'):
        parser.error('result-name must be one directory name')
    if not args.execute:
        assert raw.parent == ROOT / 'runs'
        raw.mkdir(exist_ok=False); (raw / 'support').mkdir()
        shutil.copy2(Path(__file__), raw / 'support/pool_audit.py')
        shutil.copy2(BASE / 'decompose.py', raw / 'decompose.py')
        (raw / 'decompose.cpp').write_text(augmented_source((BASE / 'decompose.cpp').read_text()))
        reports = {'kittyknight': BASE / 'results/loaded-motion-audit-v1/kittyknight.json'}
        reports.update({'cgar_seed%d' % seed: BASE / ('results/current-bottleneck-audit-v50/seed%d/seed%d.json' % (seed, seed)) for seed in range(6)})
        if args.references:
            reports = {name: Path(path).resolve() for name, path in json.loads(args.references.read_text()).items()}
            assert reports and all(Path(name).name == name and name not in ('', '.', '..') for name in reports)
        cases = {name: json.loads(path.read_text())['input'] for name, path in reports.items()}
        # Independent replays already archived the hashes of these large inputs.
        expected = {}
        for name, report in reports.items():
            provenance = json.loads((report.parent / 'provenance.json').read_text())
            matches = [r for r in provenance['inputs'].values() if Path(r['path']).resolve() == Path(cases[name]).resolve()]
            assert len(matches) == 1, name
            expected[name] = matches[0]['sha256']
        write(raw / 'cases.json', cases)
        files = [raw / 'support/pool_audit.py', raw / 'decompose.py', raw / 'decompose.cpp', raw / 'cases.json']
        write(raw / 'spec.json', {'files': {str(p): digest(p) for p in files},
            'base_source_sha256': digest(BASE / 'decompose.cpp'), 'expected_input_hashes': expected,
            'result_name': args.result_name,
            'reference_reports': {k: str(v) for k, v in reports.items()},
            'reference_report_hashes': {k: digest(v) for k, v in reports.items()}})
        command = ['/usr/bin/python3', str(raw / 'support/pool_audit.py'), '--output', str(raw), '--execute']
        script = raw / 'job.sh'
        script.write_text('#!/bin/bash\nset -eu\nexec ' + ' '.join(shlex.quote(x) for x in command) + '\n')
        submit = ['/opt/n1ge/bin/lx24-amd64/qsub', '-terse', '-w', 'e', '-cwd', '-q', 'debian.q',
            '-pe', 'threaded', '1', '-binding', 'linear:1', '-l', 'exclusive=true,h_rt=01:00:00,h_vmem=8G',
            '-m', 'n', '-N', 'task_pool_audit', '-j', 'y', '-o', str(raw / 'job.log'), '-S', '/bin/bash', str(script)]
        proc = subprocess.run(submit, cwd=str(ROOT), stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        write(raw / 'submission.json', {'command': submit, 'returncode': proc.returncode, 'response': proc.stdout})
        print(proc.stdout, end=''); return proc.returncode
    spec = json.loads((raw / 'spec.json').read_text())
    for path, sha in spec['files'].items():
        assert digest(path) == sha
    cpu = cpu_resources()
    assert cpu['physical_cores_visible'] >= 1 and cpu['effective_cpu_quota'] is None
    os.sched_setaffinity(0, cpu['representative_cpus'][:1])
    replay = raw / 'replay'
    subprocess.run(['/usr/bin/python3', str(raw / 'decompose.py'), '--output', str(replay),
                    '--cases', str(raw / 'cases.json')], check=True)
    # This fixture hand-counts ownership at pickup, completion, reassignment and
    # revelation boundaries, including a newly revealed task at the final tick.
    fixture = {'teamSize': 2, 'makespan': 5, 'numTaskFinished': 1,
        'numPlannerErrors': 0, 'numScheduleErrors': 0, 'numEntryTimeouts': 0,
        'start': [[0, 0, 'E'], [0, 4, 'W']],
        'actualPaths': ['F,F,F,W,W', 'W,W,W,W,F'],
        'actualSchedule': ['0:-1,2:0,4:-1', '1:0,2:-1,4:2'],
        'tasks': [[0, 0, [0, 2, 0, 3]], [1, 0, [0, 0, 0, 4]],
                  [2, 3, [0, 4, 0, 0]], [3, 5, [0, 1, 0, 2]]],
        'events': [[2, 0, 0, 1], [3, 0, 0, 2], [4, 1, 2, 1]]}
    write(raw / 'pool-fixture.json', fixture)
    subprocess.run([str(replay / 'decompose'), str(replay / 'fixture.map'), str(raw / 'pool-fixture.json'),
                    str(raw / 'pool-fixture-result.json')], check=True)
    observed = json.loads((raw / 'pool-fixture-result.json').read_text())['pool_snapshots']
    expected = [(2, 2, 0, 0), (2, 1, 1, 0), (2, 1, 0, 1), (2, 2, 0, 0), (2, 1, 0, 1), (3, 2, 0, 1)]
    for row, counts in zip(observed, expected):
        assert tuple(row[k]['chain']['n'] for k in ['outstanding', 'unassigned', 'held_unopened', 'started']) == counts
    assert len(observed) == 6
    assert observed[-1]['unassigned']['chain'] == {'n': 2, 'mean': 2.5, 'p90': 1, 'sum': 5}
    assert observed[-1]['unassigned']['age'] == {'n': 2, 'mean': 2.5, 'p90': 0, 'sum': 5}
    provenance = json.loads((replay / 'provenance.json').read_text())
    for name, sha in spec['expected_input_hashes'].items():
        assert provenance['inputs'][name]['sha256'] == sha, name
    out = BASE / 'results' / spec.get('result_name', 'task-pool-audit-v50'); out.mkdir(exist_ok=False)
    rows = {}; accounting_corrections = {}
    for name, path in spec['reference_reports'].items():
        assert digest(path) == spec['reference_report_hashes'][name]
        old = json.loads(Path(path).read_text()); new = json.loads((replay / (name + '.json')).read_text())
        changes = {k: {'old': v, 'new': new[k]} for k, v in old.items() if new[k] != v}
        if changes:
            # Earlier replay inferred loaded time as the residual and omitted the
            # initial unassigned interval before the first accepted schedule.
            implicit = new['implicit_initial_idle_steps']
            assert implicit > 0 and new['entry_timeouts'] > 0, (name, changes)
            assert set(changes) == {'unassigned_robot_steps', 'loaded_robot_steps_including_unfinished_tasks'}, (name, changes)
            assert new['unassigned_robot_steps'] == old['unassigned_robot_steps'] + implicit
            assert new['loaded_robot_steps_including_unfinished_tasks'] == old['loaded_robot_steps_including_unfinished_tasks'] - implicit
            accounting_corrections[name] = {'implicit_initial_idle_steps': implicit, 'changed_fields': changes}
        assert all(s['outstanding']['chain']['n'] == 15000 for s in new['pool_snapshots'])
        shutil.copy2(replay / (name + '.json'), out / (name + '.json'))
        rows[name] = {'pool_snapshots': new['pool_snapshots'], 'limits': new['pool_limits'],
                      'completed_chain_shortest': new['completed_chain_shortest'], 'completed_tasks': new['completed_tasks']}
    write(out / 'cohorts.json', {'completed_utc': datetime.datetime.now(datetime.timezone.utc).isoformat(),
        'all_original_accounting_identical': not accounting_corrections, 'accounting_corrections': accounting_corrections,
        'pool_fixture_passed': True, 'cases': rows,
        'scope': 'Descriptive revealed-task replay; no causal savings or throughput gain established.'})
    for path, sha in spec['files'].items():
        assert digest(path) == sha
    for source in [raw / 'spec.json', raw / 'submission.json', raw / 'pool-fixture.json', raw / 'pool-fixture-result.json', replay / 'provenance.json']:
        shutil.copy2(source, out / source.name)
    for source in replay.glob('*fixture*.json'):
        shutil.copy2(source, out / source.name)
    print(json.dumps({name: row['pool_snapshots'][-1] for name, row in rows.items()}, indent=2))


if __name__ == '__main__':
    raise SystemExit(main())
