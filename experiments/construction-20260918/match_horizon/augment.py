"""Add departure queries and cutoff accounting to the independent replay.

The production planner is never linked or changed. Exact spatial bounds and a
prospective p90 model here are an independent diagnostic, not its cached quotes.
"""
from pathlib import Path


def augment(source, report_source):
    replacements = [
        ('#include <numeric>', '#include <numeric>\n#include <map>'),
        ('    std::vector<Assignment> assignments;',
         '    std::vector<Assignment> assignments;\n    std::vector<Assignment> departures;\n    int departure_pickup_distance = -1;'),
        ('                if (task_id >= 0) tasks.at(index.at(task_id)).assignments.push_back({step, int(robot), cell});',
         '''                if (slot && entries[slot - 1].second >= 0 && entries[slot - 1].second != task_id) {
                    auto& previous = tasks.at(index.at(entries[slot - 1].second));
                    if (previous.finished < 0 || previous.finished >= step)
                        previous.departures.push_back({step, int(robot), cell});
                }
                if (task_id >= 0) tasks.at(index.at(task_id)).assignments.push_back({step, int(robot), cell});'''),
        ('    std::sort(queries.begin(), queries.end(), [](auto& a, auto& b) { return a.target < b.target; });',
         '''    for (int i = 0; i < int(tasks.size()); ++i) {
        auto& task = tasks[i];
        if (task.assignments.size() > 2) throw std::runtime_error("audit requires at most one observed retarget");
        if (task.assignments.size() != 2) {
            if (!task.departures.empty()) throw std::runtime_error("unmatched observed departure");
            continue;
        }
        if (task.departures.size() != 1 || task.departures[0].step != task.assignments[1].step ||
            task.departures[0].robot != task.assignments[0].robot ||
            task.assignments[0].robot == task.assignments[1].robot ||
            (task.pickup >= 0 && task.assignments[1].step > task.pickup))
            throw std::runtime_error("retarget is not an unopened simultaneous handoff");
        queries.push_back({task.stops[0], task.departures[0].cell, i, 3});
    }
    std::sort(queries.begin(), queries.end(), [](auto& a, auto& b) { return a.target < b.target; });'''),
        ('        else task.last_pickup_distance = d;',
         '        else if (query.kind == 2) task.last_pickup_distance = d;\n        else task.departure_pickup_distance = d;'),
        ('    std::ofstream(argv[3]) << report.dump(2)',
         report_source + '\n    std::ofstream(argv[3]) << report.dump(2)'),
    ]
    for old, new in replacements:
        if source.count(old) != 1:
            raise ValueError('Independent replay hook changed: ' + old[:100])
        source = source.replace(old, new)
    return source
