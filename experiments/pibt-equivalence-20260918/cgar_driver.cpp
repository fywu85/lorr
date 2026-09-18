#include "cgar.hpp"
#include "trace_io.hpp"
#include <cstdlib>
#include <iostream>

int main(int argc, char** argv) {
    if (argc != 3) return 2;
    traceio::Json result;
    try {
        traceio::Input in(argv[1]);
        setenv("CGAR_SEED", std::to_string(in.seed).c_str(), 1);
        setenv("CGAR_PIBT_REFERENCE", "1", 1);
        SharedEnvironment env;
        env.rows = in.rows; env.cols = in.cols; env.map = in.map; env.num_of_agents = in.goals.size();
        env.goal_locations.resize(in.goals.size()); env.curr_task_schedule.resize(in.goals.size());
        for (size_t i = 0; i < in.goals.size(); ++i) {
            env.curr_states.emplace_back(in.positions[i], 0, in.orientations[i]);
            env.goal_locations[i] = {{in.goals[i], 0}}; env.curr_task_schedule[i] = i;
            Task task; task.task_id = i; task.locations = {in.goals[i]}; env.task_pool.emplace(i, task);
        }
        cgar::Cgar planner;
        auto started = traceio::Clock::now(); planner.initialize(&env, 30000);
        result["preprocess_seconds"] = std::chrono::duration<double>(traceio::Clock::now() - started).count();
        result["positions"] = {in.positions}; result["orientations"] = {in.orientations};
        auto positions = in.positions; auto orientations = in.orientations;
        for (int step = 0; step < in.steps; ++step) {
            env.curr_timestep = step;
            std::vector<Action> proposed;
            const auto begin = traceio::Clock::now();
            planner.plan(&env, begin + std::chrono::seconds(30), proposed);
            result["decision_seconds"].push_back(std::chrono::duration<double>(traceio::Clock::now() - begin).count());
            const auto targets = planner.proposed_cells();
            traceio::validate(in, positions, targets); result["targets"].push_back(targets);
            std::vector<int> actions;
            for (auto a : proposed) actions.push_back(static_cast<int>(a));
            auto next = in.rotation ? traceio::apply(in, positions, orientations, actions) : targets;
            if (!in.rotation) traceio::validate(in, positions, next);
            else result["actions"].push_back(actions);
            for (size_t i = 0; i < next.size(); ++i) env.curr_states[i] = State(next[i], step + 1, orientations[i]);
            positions = std::move(next);
            result["positions"].push_back(positions); result["orientations"].push_back(orientations);
            if (positions == in.goals) break;
        }
        result["valid"] = true; result["solved"] = positions == in.goals;
        result["steps"] = result["positions"].size() - 1;
        traceio::write(argv[2], result); return 0;
    } catch (const std::exception& e) {
        result["valid"] = false; result["error"] = e.what(); traceio::write(argv[2], result);
        std::cerr << e.what() << '\n'; return 1;
    }
}
