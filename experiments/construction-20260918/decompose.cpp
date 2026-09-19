// Post-hoc travel decomposition. Reads completed outputs; never used by a planner.
#include "nlohmann/json.hpp"
#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
using json = nlohmann::ordered_json;

struct Assignment { int step, robot, cell; };
struct TaskInfo {
    int id, revealed, pickup = -1, finished = -1;
    std::vector<int> stops;
    std::vector<Assignment> assignments;
    long long chain = 0;
    int first_pickup_distance = -1, last_pickup_distance = -1;
};
struct Query { int target, source, task, kind; };

json distribution(std::vector<long long> values) {
    if (values.empty()) return {{"n", 0}, {"mean", nullptr}, {"p90", nullptr}, {"sum", 0}};
    std::sort(values.begin(), values.end());
    auto total = std::accumulate(values.begin(), values.end(), 0LL);
    return {{"n", values.size()}, {"mean", double(total) / values.size()},
            {"p90", values[(values.size() - 1) * 9 / 10]}, {"sum", total}};
}

int main(int argc, char** argv) {
    if (argc != 4) throw std::runtime_error("usage: decompose MAP RESULT OUTPUT");
    std::ifstream map_file(argv[1]);
    std::string word; int rows = 0, cols = 0;
    while (map_file >> word && word != "map") {
        if (word == "height") map_file >> rows;
        else if (word == "width") map_file >> cols;
    }
    if (!rows || !cols) throw std::runtime_error("invalid map header");
    std::vector<char> free(rows * cols);
    for (int r = 0; r < rows; ++r) {
        map_file >> word;
        if (int(word.size()) != cols) throw std::runtime_error("invalid map row");
        for (int c = 0; c < cols; ++c) free[r * cols + c] = word[c] != '@' && word[c] != 'T';  // same alphabet as the simulator's Grid
    }
    auto neighbor = [&](int u, int direction) {
        if (direction == 0) return u % cols + 1 < cols ? u + 1 : -1;
        if (direction == 1) return u / cols + 1 < rows ? u + cols : -1;
        if (direction == 2) return u % cols ? u - 1 : -1;
        return u / cols ? u - cols : -1;
    };
    json report;
    std::vector<TaskInfo> tasks;
    std::vector<Query> queries;
    {
        std::ifstream input(argv[2]); json data; input >> data;
        report = {{"input", argv[2]}, {"team_size", data.at("teamSize")},
                  {"steps", data.at("makespan")}, {"completed_tasks", data.at("numTaskFinished")},
                  {"planner_errors", data.at("numPlannerErrors")},
                  {"schedule_errors", data.at("numScheduleErrors")},
                  {"entry_timeouts", data.at("numEntryTimeouts")}};
        std::unordered_map<int, int> index;
        for (const auto& row : data.at("tasks")) {
            TaskInfo task; task.id = row.at(0); task.revealed = row.at(1);
            const auto& positions = row.at(2);
            for (size_t k = 0; k < positions.size(); k += 2)
                task.stops.push_back(positions[k].get<int>() * cols + positions[k + 1].get<int>());
            index.emplace(task.id, tasks.size()); tasks.push_back(std::move(task));
        }
        for (const auto& event : data.at("events")) {
            auto& task = tasks.at(index.at(event[2].get<int>()));
            int stop = event[3];
            if (stop == 1) task.pickup = event[0];
            if (stop == int(task.stops.size())) task.finished = event[0];
        }
        long long non_motion_actions = 0;
        long long idle_steps = 0, empty_forward = 0, empty_turns = 0, empty_waits = 0, empty_other = 0;
        for (size_t robot = 0; robot < data.at("actualSchedule").size(); ++robot) {
            const auto& start = data.at("start")[robot];
            int cell = start[0].get<int>() * cols + start[1].get<int>();
            int direction;
            if (start[2].is_number_integer()) direction = start[2];
            else {
                std::string heading = start[2];
                direction = heading == "E" ? 0 : heading == "S" ? 1 : heading == "W" ? 2 : heading == "N" ? 3 : -1;
            }
            if (direction < 0 || direction > 3) throw std::runtime_error("unknown heading");
            const auto& path = data.at("actualPaths")[robot].get_ref<const std::string&>();
            std::istringstream schedule(data.at("actualSchedule")[robot].get<std::string>());
            std::string entry; int applied = 0;
            std::vector<std::pair<int,int>> entries;
            while (std::getline(schedule, entry, ',')) if (!entry.empty()) {
                size_t colon = entry.find(':');
                entries.emplace_back(std::stoi(entry.substr(0, colon)), std::stoi(entry.substr(colon + 1)));
            }
            for (size_t slot = 0; slot < entries.size(); ++slot) {
                int step = entries[slot].first, task_id = entries[slot].second;
                int end = slot + 1 < entries.size() ? entries[slot + 1].first - 1 : data.at("makespan").get<int>();
                if (task_id < 0) idle_steps += std::max(0, end - std::max(1, step) + 1);
                else {
                    int pickup = tasks.at(index.at(task_id)).pickup;
                    if (pickup >= 0) end = std::min(end, pickup);
                    for (int at = std::max(1, step); at <= end; ++at) {
                        char action = path.at(2 * (at - 1));
                        if (action == 'F') ++empty_forward;
                        else if (action == 'R' || action == 'C') ++empty_turns;
                        else if (action == 'W') ++empty_waits;
                        else ++empty_other;
                    }
                }
                while (applied < step - 1) {
                    char action = path.at(2 * applied++);
                    if (action == 'F') cell = neighbor(cell, direction);
                    else if (action == 'R') direction = (direction + 1) % 4;
                    else if (action == 'C') direction = (direction + 3) % 4;
                    else if (action != 'W') ++non_motion_actions;
                    if (cell < 0 || !free[cell]) throw std::runtime_error("invalid replay position");
                }
                if (task_id >= 0) tasks.at(index.at(task_id)).assignments.push_back({step, int(robot), cell});
            }
        }
        report["non_motion_action_markers_before_last_assignment"] = non_motion_actions;
        report["unassigned_robot_steps"] = idle_steps;
        report["empty_forward_actions"] = empty_forward;
        report["empty_turn_actions"] = empty_turns;
        report["empty_wait_actions"] = empty_waits;
        report["empty_other_actions"] = empty_other;
        report["empty_robot_steps_including_reassignments"] = empty_forward + empty_turns + empty_waits + empty_other;
        report["loaded_robot_steps_including_unfinished_tasks"] = data.at("teamSize").get<long long>() * data.at("makespan").get<int>() - idle_steps - empty_forward - empty_turns - empty_waits - empty_other;
    }
    for (int i = 0; i < int(tasks.size()); ++i) {
        auto& task = tasks[i];
        for (size_t k = 1; k < task.stops.size(); ++k)
            queries.push_back({task.stops[k], task.stops[k - 1], i, 0});
        std::sort(task.assignments.begin(), task.assignments.end(), [](auto& a, auto& b) {
            return std::tie(a.step, a.robot) < std::tie(b.step, b.robot);
        });
        if (!task.assignments.empty()) {
            queries.push_back({task.stops[0], task.assignments.front().cell, i, 1});
            queries.push_back({task.stops[0], task.assignments.back().cell, i, 2});
        }
    }
    std::sort(queries.begin(), queries.end(), [](auto& a, auto& b) { return a.target < b.target; });
    std::vector<int> distance(free.size()), queue; queue.reserve(free.size());
    int target = -1, builds = 0;
    for (auto& query : queries) {
        if (query.target != target) {
            target = query.target; ++builds;
            if (target < 0 || target >= int(free.size()) || !free[target]) throw std::runtime_error("invalid target");
            std::fill(distance.begin(), distance.end(), -1); distance[target] = 0;
            queue.assign(1, target);
            for (size_t head = 0; head < queue.size(); ++head) {
                int u = queue[head];
                for (int d = 0; d < 4; ++d) {
                    int v = neighbor(u, d);
                    if (v >= 0 && free[v] && distance[v] < 0) {
                        distance[v] = distance[u] + 1; queue.push_back(v);
                    }
                }
            }
        }
        int d = distance[query.source];
        if (d < 0) throw std::runtime_error("unreachable diagnostic query");
        auto& task = tasks[query.task];
        if (query.kind == 0) task.chain += d;
        else if (query.kind == 1) task.first_pickup_distance = d;
        else task.last_pickup_distance = d;
    }
    std::vector<long long> revealed_chain, completed_chain, assigned_chain, first_pickup_distance, last_pickup_distance;
    std::vector<long long> first_pickup_delay, last_pickup_delay, loaded_steps;
    long long pickup_elapsed = 0, pickup_distance = 0, loaded_elapsed = 0, loaded_distance = 0;
    int changes = 0, assigned_unpicked = 0, assignments_after_pickup = 0;
    for (auto& task : tasks) {
        revealed_chain.push_back(task.chain);
        if (task.finished >= 0) {
            if (task.pickup < 0 || task.pickup > task.finished) throw std::runtime_error("missing or misordered pickup event");
            completed_chain.push_back(task.chain);
            loaded_steps.push_back(task.finished - task.pickup);
            loaded_elapsed += task.finished - task.pickup; loaded_distance += task.chain;
        }
        if (task.assignments.empty()) continue;
        assigned_chain.push_back(task.chain);
        first_pickup_distance.push_back(task.first_pickup_distance);
        last_pickup_distance.push_back(task.last_pickup_distance);
        changes += task.assignments.size() > 1;
        if (task.pickup < 0) { ++assigned_unpicked; continue; }
        first_pickup_delay.push_back(task.pickup - task.assignments.front().step + 1);
        last_pickup_delay.push_back(task.pickup - task.assignments.back().step + 1);
        pickup_elapsed += task.pickup - task.assignments.back().step + 1;
        pickup_distance += task.last_pickup_distance;
        for (auto& assignment : task.assignments) assignments_after_pickup += assignment.step > task.pickup;
    }
    if (completed_chain.size() != report["completed_tasks"].get<size_t>()) throw std::runtime_error("completion count mismatch");
    report["robot_steps_per_completed_task"] = double(report["team_size"].get<int>()) * report["steps"].get<int>() / report["completed_tasks"].get<int>();
    report["revealed_chain_shortest"] = distribution(revealed_chain);
    report["completed_chain_shortest"] = distribution(completed_chain);
    report["assigned_chain_shortest"] = distribution(assigned_chain);
    report["first_assignment_pickup_shortest"] = distribution(first_pickup_distance);
    report["last_assignment_pickup_shortest"] = distribution(last_pickup_distance);
    report["observed_first_assignment_pickup_delay"] = distribution(first_pickup_delay);
    report["observed_last_assignment_pickup_delay"] = distribution(last_pickup_delay);
    report["completed_loaded_steps"] = distribution(loaded_steps);
    report["observed_pickup_steps_per_shortest_cell"] = pickup_distance ? json(double(pickup_elapsed) / pickup_distance) : json(nullptr);
    report["completed_loaded_steps_per_shortest_cell"] = loaded_distance ? json(double(loaded_elapsed) / loaded_distance) : json(nullptr);
    report["tasks_with_multiple_assignments"] = changes;
    report["assignments_after_pickup"] = assignments_after_pickup;
    report["assigned_without_observed_pickup"] = assigned_unpicked;
    report["distance_queries"] = queries.size(); report["complete_bfs_tables"] = builds;
    report["distance_basis"] = "Exact unweighted shortest paths on the supplied free-cell graph; ignores turns and traffic.";
    std::ofstream(argv[3]) << report.dump(2) << '\n';
    std::cout << "DECOMPOSITION_COMPLETE " << argv[3] << '\n';
}
