// Independent post-hoc physical replay. Future task outcomes label observations;
// they are never exposed to a planner or interpreted as a causal intervention.
#include "nlohmann/json.hpp"
#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
using json = nlohmann::ordered_json;
struct Task { int id, revealed, progress = 0, finished = -1; std::vector<int> goals; };
struct Event { int robot, task, stop; };
struct Counts {
    long long waits = 0, at_goal = 0, obstacle_ahead = 0, free_ahead = 0, free_closer_manhattan = 0;
    long long occupied_ahead = 0, vacated_ahead = 0, incoming_blocker = 0;
    long long completed_waiter = 0, completed_waiter_unfinished_blocker = 0;
    long long completed_waiter_completed_blocker = 0, unfinished_waiter_completed_blocker = 0;
    long long idle_blocker = 0, blocker_waiting = 0, blocker_rotating = 0, blocker_moving = 0;
    std::array<long long, 3> phases{};
    json value() const {
        return {{"wait_actions", waits}, {"at_current_goal", at_goal}, {"obstacle_ahead", obstacle_ahead},
            {"free_ahead_after_other_actions", free_ahead}, {"free_and_manhattan_closer", free_closer_manhattan},
            {"occupied_ahead_after_other_actions", occupied_ahead}, {"ahead_vacated_by_other_action", vacated_ahead},
            {"incoming_blocker", incoming_blocker}, {"completed_task_waiter", completed_waiter},
            {"completed_waiter_unfinished_blocker", completed_waiter_unfinished_blocker},
            {"completed_waiter_completed_blocker", completed_waiter_completed_blocker},
            {"unfinished_waiter_completed_blocker", unfinished_waiter_completed_blocker},
            {"idle_blocker", idle_blocker}, {"blocker_waiting", blocker_waiting},
            {"blocker_rotating", blocker_rotating}, {"blocker_moving", blocker_moving},
            {"wait_phases_idle_empty_loaded", phases}};
    }
};
int main(int argc, char** argv) {
    if (argc != 4) throw std::runtime_error("usage: audit MAP TRAJECTORY OUTPUT");
    std::ifstream mf(argv[1]); std::string word; int rows = 0, cols = 0;
    while (mf >> word && word != "map") {
        if (word == "height") mf >> rows;
        else if (word == "width") mf >> cols;
    }
    if (rows <= 0 || cols <= 0) throw std::runtime_error("invalid map");
    std::vector<char> floor(rows * cols);
    for (int r = 0; r < rows; ++r) {
        mf >> word; if (int(word.size()) != cols) throw std::runtime_error("short map row");
        for (int c = 0; c < cols; ++c) floor[r * cols + c] = word[c] != '@' && word[c] != 'T';
    }
    auto neighbor = [&](int u, int h) {
        if ((h == 0 && u % cols + 1 == cols) || (h == 1 && u / cols + 1 == rows) ||
            (h == 2 && u % cols == 0) || (h == 3 && u / cols == 0)) return -1;
        return u + (h == 0 ? 1 : h == 1 ? cols : h == 2 ? -1 : -cols);
    };
    std::ifstream input(argv[2]); json d; input >> d;
    const int n = d.at("teamSize"), horizon = d.at("makespan");
    if (n <= 0 || horizon <= 0) throw std::runtime_error("invalid trajectory size");
    for (const char* name : {"numPlannerErrors", "numScheduleErrors", "numEntryTimeouts"})
        if (d.at(name).get<int>()) throw std::runtime_error("failed trajectory");
    if (d.at("start").size() != size_t(n) || d.at("actualPaths").size() != size_t(n) ||
        d.at("actualSchedule").size() != size_t(n)) throw std::runtime_error("incomplete robot arrays");
    std::unordered_map<int, int> task_index; std::vector<Task> tasks;
    for (const auto& row : d.at("tasks")) {
        Task t; t.id = row.at(0); t.revealed = row.at(1);
        const auto& p = row.at(2); if (p.empty() || p.size() % 2) throw std::runtime_error("invalid task");
        for (size_t k = 0; k < p.size(); k += 2) {
            int u = p[k].get<int>() * cols + p[k + 1].get<int>();
            if (u < 0 || u >= int(floor.size()) || !floor[u]) throw std::runtime_error("bad goal");
            t.goals.push_back(u);
        }
        if (!task_index.emplace(t.id, tasks.size()).second) throw std::runtime_error("duplicate task");
        tasks.push_back(std::move(t));
    }
    std::vector<std::vector<Event>> events(horizon + 1);
    int recorded_completed = 0;
    for (const auto& e : d.at("events")) {
        const int time = e.at(0), r = e.at(1), task = task_index.at(e.at(2).get<int>()), stop = e.at(3);
        if (time < 1 || time > horizon || r < 0 || r >= n || stop < 1 || stop > int(tasks[task].goals.size()))
            throw std::runtime_error("invalid event");
        events[time].push_back({r, task, stop});
        if (stop == int(tasks[task].goals.size())) {
            if (tasks[task].finished >= 0) throw std::runtime_error("duplicate completion");
            tasks[task].finished = time; ++recorded_completed;
        }
    }
    if (recorded_completed != d.at("numTaskFinished").get<int>()) throw std::runtime_error("completion mismatch");
    std::vector<int> cells(n), heading(n), next_cells(n), next_heading(n), held(n, -1), slot(n, 0);
    std::vector<int> before_owner(floor.size(), -1), after_owner(floor.size(), -1), assigned_stamp(tasks.size(), 0);
    std::vector<std::vector<std::pair<int, int>>> schedules(n);
    std::vector<const std::string*> paths(n);
    for (int r = 0; r < n; ++r) {
        const auto& start = d.at("start")[r]; cells[r] = start[0].get<int>() * cols + start[1].get<int>();
        if (start[2].is_number_integer()) heading[r] = start[2];
        else { const std::string h = start[2]; heading[r] = h == "E" ? 0 : h == "S" ? 1 : h == "W" ? 2 : h == "N" ? 3 : -1; }
        if (heading[r] < 0 || heading[r] > 3 || cells[r] < 0 || cells[r] >= int(floor.size()) || !floor[cells[r]] || before_owner[cells[r]] >= 0)
            throw std::runtime_error("invalid initial state");
        before_owner[cells[r]] = r;
        paths[r] = &d.at("actualPaths")[r].get_ref<const std::string&>();
        if (paths[r]->size() != size_t(2 * horizon - 1)) throw std::runtime_error("path length mismatch");
        std::istringstream ss(d.at("actualSchedule")[r].get<std::string>()); std::string entry;
        while (std::getline(ss, entry, ',')) if (!entry.empty()) {
            const size_t colon = entry.find(':'); const int t = std::stoi(entry.substr(0, colon)), id = std::stoi(entry.substr(colon + 1));
            if (t < 0 || t > horizon || (!schedules[r].empty() && t <= schedules[r].back().first)) throw std::runtime_error("invalid schedule order");
            schedules[r].push_back({t, id < 0 ? -1 : task_index.at(id)});
        }
    }
    std::vector<int> thresholds{1, std::max(1, horizon - 999), std::max(1, horizon - 399), std::max(1, horizon - 99)};
    std::sort(thresholds.begin(), thresholds.end()); thresholds.erase(std::unique(thresholds.begin(), thresholds.end()), thresholds.end());
    std::vector<Counts> counters(thresholds.size()); std::array<long long, 3> phase_actions{}; long long serviced = 0;
    for (int t = 1; t <= horizon; ++t) {
        std::fill(after_owner.begin(), after_owner.end(), -1);
        for (int r = 0; r < n; ++r) {
            while (slot[r] < int(schedules[r].size()) && schedules[r][slot[r]].first <= t) held[r] = schedules[r][slot[r]++].second;
            if (held[r] >= 0) {
                auto& task = tasks[held[r]];
                if (task.revealed >= t || task.progress >= int(task.goals.size()) || assigned_stamp[held[r]] == t) throw std::runtime_error("invalid held task");
                assigned_stamp[held[r]] = t;
            }
            next_cells[r] = cells[r]; next_heading[r] = heading[r]; const char a = (*paths[r])[2 * (t - 1)];
            if (a == 'F') next_cells[r] = neighbor(cells[r], heading[r]);
            else if (a == 'R') next_heading[r] = (heading[r] + 1) % 4;
            else if (a == 'C') next_heading[r] = (heading[r] + 3) % 4;
            else if (a != 'W') throw std::runtime_error("unknown physical action");
            const int u = next_cells[r];
            if (u < 0 || u >= int(floor.size()) || !floor[u] || after_owner[u] >= 0) throw std::runtime_error("vertex or obstacle collision");
            after_owner[u] = r;
        }
        for (int r = 0; r < n; ++r) {
            const int other = before_owner[next_cells[r]];
            if (other >= 0 && other != r && next_cells[other] == cells[r]) throw std::runtime_error("edge collision");
            const int id = held[r], phase = id < 0 ? 0 : tasks[id].progress ? 2 : 1; ++phase_actions[phase];
            if ((*paths[r])[2 * (t - 1)] != 'W') continue;
            const int goal = id < 0 ? -1 : tasks[id].goals[tasks[id].progress];
            const int front = neighbor(cells[r], heading[r]);
            const bool obstacle = front < 0 || !floor[front], at_goal = cells[r] == goal;
            const int blocker = obstacle ? -1 : after_owner[front];
            const bool done = id >= 0 && tasks[id].finished >= 0;
            for (size_t k = 0; k < thresholds.size(); ++k) if (t >= thresholds[k]) {
                auto& c = counters[k]; ++c.waits; ++c.phases[phase]; c.completed_waiter += done;
                if (at_goal) { ++c.at_goal; continue; }
                if (obstacle) { ++c.obstacle_ahead; continue; }
                if (blocker < 0) {
                    ++c.free_ahead; c.vacated_ahead += before_owner[front] >= 0;
                    if (goal >= 0) {
                        auto md = [&](int u) { return std::abs(u / cols - goal / cols) + std::abs(u % cols - goal % cols); };
                        c.free_closer_manhattan += md(front) < md(cells[r]);
                    }
                } else {
                    ++c.occupied_ahead; c.incoming_blocker += before_owner[front] != blocker;
                    const int bid = held[blocker]; const bool bdone = bid >= 0 && tasks[bid].finished >= 0;
                    c.idle_blocker += bid < 0;
                    c.completed_waiter_unfinished_blocker += done && bid >= 0 && !bdone;
                    c.completed_waiter_completed_blocker += done && bdone;
                    c.unfinished_waiter_completed_blocker += id >= 0 && !done && bdone;
                    const char ba = (*paths[blocker])[2 * (t - 1)];
                    c.blocker_waiting += ba == 'W'; c.blocker_rotating += ba == 'R' || ba == 'C'; c.blocker_moving += ba == 'F';
                }
            }
        }
        std::vector<int> observed(n, -1);
        for (const auto& e : events[t]) {
            if (observed[e.robot] >= 0 || held[e.robot] != e.task || tasks[e.task].progress + 1 != e.stop ||
                next_cells[e.robot] != tasks[e.task].goals[e.stop - 1]) throw std::runtime_error("invalid service event");
            observed[e.robot] = e.task;
        }
        for (int r = 0; r < n; ++r) {
            const int id = held[r]; const bool at_goal = id >= 0 && next_cells[r] == tasks[id].goals[tasks[id].progress];
            if (at_goal != (observed[r] >= 0)) throw std::runtime_error("service event missing or spurious");
            if (at_goal) { ++tasks[id].progress; ++serviced; }
        }
        cells.swap(next_cells); heading.swap(next_heading); before_owner.swap(after_owner);
    }
    json cohorts;
    for (size_t k = 0; k < thresholds.size(); ++k) {
        const auto& c = counters[k];
        if (c.waits != c.at_goal + c.obstacle_ahead + c.free_ahead + c.occupied_ahead ||
            c.occupied_ahead != c.blocker_waiting + c.blocker_rotating + c.blocker_moving)
            throw std::runtime_error("wait categories do not partition observations");
        cohorts[std::to_string(thresholds[k])] = c.value();
    }
    json result = {{"steps", horizon}, {"robots", n}, {"tasks_completed", recorded_completed},
        {"physical_actions_checked", 1LL * horizon * n}, {"service_events_checked", serviced},
        {"phase_steps_idle_empty_loaded", phase_actions}, {"cohorts", cohorts},
        {"limits", "Physical one-step forward availability under all other observed actions, not five-step-plan feasibility or a required direction. At-goal waits are separated; rotations could also serve that goal. Manhattan closeness is not weighted-route preference. Finished/unfinished labels use eventual observed outcomes for post-hoc diagnosis only, never as planner inputs. Counts are not recoverable delays, causal effects, counterfactual task completions or a throughput upper bound."}};
    std::ofstream(argv[3]) << result.dump(2) << '\n'; std::cout << "PHYSICAL_WAIT_AUDIT_COMPLETE " << argv[3] << '\n';
}
