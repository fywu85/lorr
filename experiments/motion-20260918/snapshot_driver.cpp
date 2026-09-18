// Identical one-goal-per-robot motion workloads; no scheduler is called.
#include "SharedEnv.h"
#include "ActionModel.h"
#ifdef SNAPSHOT_NMS
#include "MAPFPlanner.h"
#include <Objects/Environment/environment.hpp>
#else
#include "cgar.hpp"
#endif
#include <array>
#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>
using json = nlohmann::json;
using Clock = std::chrono::steady_clock;

int neighbor(int p, int d, int rows, int cols) {
    if (d == 0) return p % cols + 1 < cols ? p + 1 : -1;
    if (d == 1) return p / cols + 1 < rows ? p + cols : -1;
    if (d == 2) return p % cols > 0 ? p - 1 : -1;
    return p / cols > 0 ? p - cols : -1;
}

// Validate all actions before committing any state, including row wrapping,
// vertex collisions and edge swaps. Rotation follows the competition model.
std::vector<State> apply_checked(const SharedEnvironment& env, const std::vector<Action>& actions) {
    if (actions.size() != env.curr_states.size()) throw std::runtime_error("action_count");
    auto next = env.curr_states;
    std::vector<int> owner(env.map.size(), -1), current(env.map.size(), -1);
    for (size_t i = 0; i < next.size(); ++i) current[next[i].location] = i;
    for (size_t i = 0; i < next.size(); ++i) {
        auto& s = next[i];
        if (actions[i] == Action::FW) s.location = neighbor(s.location, s.orientation, env.rows, env.cols);
        else if (actions[i] == Action::CR) s.orientation = (s.orientation + 1) % 4;
        else if (actions[i] == Action::CCR) s.orientation = (s.orientation + 3) % 4;
        else if (actions[i] != Action::W) throw std::runtime_error("invalid_action");
        if (s.location < 0 || env.map.at(s.location) || owner[s.location] >= 0) throw std::runtime_error("vertex_or_obstacle_collision");
        owner[s.location] = i; ++s.timestep;
    }
    for (size_t i = 0; i < next.size(); ++i) {
        const int j = current[next[i].location];
        if (j >= 0 && j != static_cast<int>(i) && next[j].location == env.curr_states[i].location)
            throw std::runtime_error("edge_swap");
    }
    return next;
}

int main(int argc, char** argv) {
    if (argc != 5) { std::cerr << "snapshot_driver INPUT OUTPUT STEPS LIMIT_MS\n"; return 2; }
    json result;
    try {
        json input; std::ifstream(argv[1]) >> input;
        const int steps = std::stoi(argv[3]), limit = std::stoi(argv[4]);
        auto* env = new SharedEnvironment;
        env->rows = input.at("rows"); env->cols = input.at("cols"); env->map_name = input.at("map_name");
        env->map = input.at("map").get<std::vector<int>>(); env->file_storage_path = ".";
        auto goals = input.at("goals").get<std::vector<int>>(); env->num_of_agents = goals.size();
        env->goal_locations.resize(goals.size()); env->curr_task_schedule.assign(goals.size(), -1);
        std::vector<int> arrival(goals.size(), -1);
        for (size_t i = 0; i < goals.size(); ++i) {
            const auto& s = input.at("states").at(i);
            env->curr_states.emplace_back(s.at(0).get<int>(), 0, s.at(1).get<int>());
            if (goals[i] < 0) continue;
            if (goals[i] == env->curr_states[i].location) { arrival[i] = 0; continue; }
            Task task; task.task_id = i; task.t_revealed = 0; task.locations = {goals[i]}; task.agent_assigned = i;
            env->task_pool.emplace(i, task); env->curr_task_schedule[i] = i; env->goal_locations[i] = {{goals[i], 0}};
        }
        result["input"] = argv[1]; result["steps_requested"] = steps; result["time_limit_ms"] = limit;
        result["source_timestep"] = input.at("source_timestep"); result["instance"] = input.at("instance");
        result["initial_goals"] = env->task_pool.size(); result["initially_at_goal"] = std::count(arrival.begin(), arrival.end(), 0);
        const auto initial = env->curr_states;
        env->plan_start_time = Clock::now();
#ifdef SNAPSHOT_NMS
        MAPFPlanner planner(env); planner.initialize(1800000);
        result["backend"] = "nms_supplied";
#else
        cgar::Cgar planner; planner.initialize(env, 30000);
        result["backend"] = "cgar";
#endif
        result["preprocess_seconds"] = std::chrono::duration<double>(Clock::now() - env->plan_start_time).count();
        std::array<long long, 4> actions_all{}, actions_pending{};
        std::vector<double> times;
        std::vector<int> reached, reference_disabled;
        json checkpoints = json::array();
        std::vector<std::vector<State>> checkpoint_states{initial};
        std::vector<std::vector<int>> checkpoint_arrivals{arrival};
        std::vector<int> checkpoint_steps{0};
        for (int t = 0; t < steps; ++t) {
            env->curr_timestep = t; env->plan_start_time = Clock::now(); std::vector<Action> actions;
#ifdef SNAPSHOT_NMS
            planner.plan(limit, actions);
            int disabled = 0;
            for (size_t i = 0; i < goals.size(); ++i)
                disabled += env->curr_task_schedule[i] >= 0 && get_robots_handler().get_robot(i).is_disable();
            reference_disabled.push_back(disabled);
#else
            planner.plan(env, env->plan_start_time + std::chrono::milliseconds(limit), actions);
#endif
            const double seconds = std::chrono::duration<double>(Clock::now() - env->plan_start_time).count();
            times.push_back(seconds);
            // Retain late reference decisions explicitly, never label them valid.
            if (seconds * 1000 > limit) result["late_decisions"].push_back({{"step", t}, {"seconds", seconds}});
            auto next = apply_checked(*env, actions);
            for (size_t i = 0; i < goals.size(); ++i) {
                ++actions_all[actions[i]];
                if (env->curr_task_schedule[i] >= 0) ++actions_pending[actions[i]];
                if (goals[i] >= 0 && arrival[i] < 0 && next[i].location == goals[i]) {
                    arrival[i] = t + 1; env->task_pool.erase(i); env->curr_task_schedule[i] = -1; env->goal_locations[i].clear();
                }
            }
            env->curr_states = std::move(next);
            reached.push_back(std::count_if(arrival.begin(), arrival.end(), [](int x) { return x > 0; }));
            if ((t + 1) % 25 == 0 || t + 1 == steps) {
                checkpoint_states.push_back(env->curr_states); checkpoint_arrivals.push_back(arrival); checkpoint_steps.push_back(t + 1);
            }
        }
        // Exact unweighted spatial distances, calculated outside timed decisions.
        // Arrived robots contribute zero even if later pushed off their goals.
        std::vector<long long> distances(checkpoint_states.size(), 0);
        std::map<int, std::vector<int>> by_goal;
        for (size_t i = 0; i < goals.size(); ++i) if (goals[i] >= 0) by_goal[goals[i]].push_back(i);
        for (const auto& item : by_goal) {
            std::vector<int> d(env->map.size(), -1), queue{item.first}; d[item.first] = 0;
            for (size_t head = 0; head < queue.size(); ++head) for (int dir = 0; dir < 4; ++dir) {
                const int v = neighbor(queue[head], dir, env->rows, env->cols);
                if (v >= 0 && !env->map[v] && d[v] < 0) { d[v] = d[queue[head]] + 1; queue.push_back(v); }
            }
            for (size_t k = 0; k < checkpoint_states.size(); ++k) for (int i : item.second) {
                if (checkpoint_arrivals[k][i] >= 0) continue;
                const int h = d[checkpoint_states[k][i].location];
                if (h < 0) throw std::runtime_error("unreachable_goal");
                distances[k] += h;
            }
        }
        for (size_t k = 0; k < checkpoint_states.size(); ++k)
            checkpoints.push_back({{"step", checkpoint_steps[k]}, {"remaining_spatial_distance", distances[k]}});
        result["checkpoints"] = checkpoints; result["reached_curve"] = reached;
        result["arrival_steps"] = arrival; result["actions_all"] = actions_all; result["actions_pending"] = actions_pending;
        result["reference_disabled_pending"] = reference_disabled;
        result["decision_seconds"] = times; result["steps_completed"] = steps;
        result["valid"] = !result.contains("late_decisions");
#ifndef SNAPSHOT_NMS
        result["parked"] = planner.parked_count();
        delete env;
#endif
        std::ofstream(argv[2]) << result.dump(2) << '\n';
        return result.at("valid").get<bool>() ? 0 : 1;
    } catch (const std::exception& error) {
        result["valid"] = false; result["error"] = error.what();
        std::ofstream(argv[2]) << result.dump(2) << '\n'; std::cerr << error.what() << '\n'; return 1;
    }
}
