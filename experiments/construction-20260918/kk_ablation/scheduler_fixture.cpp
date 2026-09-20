// Real KittyKnight warehouse matching on a hand-counted nine-cell corridor.
// Compile once, then link against native and patched scheduler objects.
#include "scheduler.h"
#include "TaskManager.h"
#include <cstdlib>
#include <iostream>
#include <set>
#include <stdexcept>

static void require(bool value, const char* message) {
    if (!value) throw std::runtime_error(message);
}

int main(int argc, char** argv) {
    require(argc == 3, "expected rematching behavior and weighting arguments");
    const bool weighted = std::string(argv[2]) == "1";
    const bool rematch = std::string(argv[1]) == "1";
    SharedEnvironment env;
    env.rows = 3; env.cols = 11; env.num_of_agents = 3;
    env.map.assign(33, 1);
    for (int x = 1; x <= 9; ++x) env.map[11 + x] = 0;
    env.map_name = "diagnostic_corridor.map";
    env.file_storage_path = ".";
    env.curr_states = {State(12, 0, 0), State(20, 0, 2), State(16, 0, 0)};
    env.goal_locations.resize(3);
    auto planner = std::make_shared<MyPlanner::WPPLPlanner>(&env);
    planner->map_weights = std::make_shared<std::vector<float>>(165, 1.0f);
    if (weighted) for (int cell = 0; cell < 33; ++cell) {
        (*planner->map_weights)[cell * 5] = 2.0f;
        (*planner->map_weights)[cell * 5 + 2] = 3.0f;
        (*planner->map_weights)[cell * 5 + 4] = 2.0f;
    }
    planner->heuristics = std::make_shared<HeuristicTable>(&env, planner->map_weights, true);
    planner->heuristics->compute_weighted_heuristics();
    require(planner->heuristics->get(12, 0, 20) == (weighted ? 16 : 8), "forward heuristic");
    require(planner->heuristics->get(20, 2, 13) == (weighted ? 21 : 7), "reverse heuristic");
    nlohmann::json cfg = {{"seed", 0}, {"use_external_executor", false},
        {"planning_window", 10}, {"execution_window", 1}, {"disable_agent_goals", false}};
    planner->lacam2_solver = std::make_shared<LaCAM2::LaCAM2Solver>(
        planner->heuristics, &env, planner->map_weights, 3, false, 1000000, cfg);
    planner->lacam2_solver->initialize(env);
    MyPlanner::MyScheduler scheduler(planner);
    scheduler.endpoint_locs = {20, 13, 16};
    for (int i = 0; i < 3; ++i) scheduler.endpoints_loc_to_id_mapping[scheduler.endpoint_locs[i]] = i;
    auto setup = [&] {
        env.task_pool.clear();
        env.task_pool.emplace(10, Task(10, {20, 19}, 0));
        env.task_pool.emplace(11, Task(11, {13, 14}, 3));
        env.task_pool.emplace(12, Task(12, {16, 17}, 1));
        env.task_pool.at(10).agent_assigned = 0;
        env.task_pool.at(12).agent_assigned = 2;
        env.task_pool.at(12).idx_next_loc = 1;
        env.curr_task_schedule = {10, -1, 12};
        env.new_freeagents = {1}; env.new_tasks = {11};
        scheduler.skip_matching_if_no_free_agents = true;
    };
    nlohmann::json records = nlohmann::json::array();
    for (int scenario = 0; scenario < 4; ++scenario) {
        setup();
        if (scenario == 1) {
            env.task_pool.at(10).agent_assigned = -1;
            env.curr_task_schedule[0] = -1;
            env.new_freeagents = {0, 1};
        } else if (scenario >= 2) {
            env.task_pool.at(11).agent_assigned = 1;
            env.curr_task_schedule[1] = 11;
            env.new_freeagents.clear(); env.new_tasks.clear();
            if (scenario == 3) scheduler.skip_matching_if_no_free_agents = false;
        }
        const auto before = env.curr_task_schedule;
        const auto tasks = env.task_pool;
        std::vector<int> proposed;
        scheduler.schedule_plan_greedy_matching_warehouse(1000, proposed, &env);
        const bool swaps = scenario == 1 || (rematch && (scenario == 0 || scenario == 3));
        const std::vector<int> expected = swaps ? std::vector<int>{11, 10, 12} : std::vector<int>{10, 11, 12};
        require(proposed == expected, "hand-computed complete assignment differs");
        require(std::set<int>(proposed.begin(), proposed.end()).size() == 3, "duplicate task");
        require(proposed[2] == 12, "started task reassigned");
        require(env.curr_task_schedule == before, "proposal overwrote simulator schedule");
        for (const auto& entry : tasks) {
            const auto& after = env.task_pool.at(entry.first);
            require(after.locations == entry.second.locations && after.idx_next_loc == entry.second.idx_next_loc
                    && after.t_revealed == entry.second.t_revealed && after.t_completed == entry.second.t_completed,
                    "task progress or age mutated");
        }
        for (int robot = 0; robot < 3; ++robot)
            require(env.task_pool.at(proposed[robot]).agent_assigned == robot, "proposal owner metadata inconsistent");
        records.push_back({{"scenario", scenario}, {"schedule", proposed}});
    }
    // Exercise actual TaskManager commit, service, replenishment and environment
    // synchronization over two scheduler calls, including an unopened->open task.
    std::vector<std::list<int>> stream = {{20, 19}, {13, 14}, {16, 17}};
    TaskManager manager(stream, 3);
    Logger logger("", 5); manager.set_logger(&logger); manager.set_num_tasks_reveal(1.0f);
    manager.curr_timestep = 0; manager.reveal_tasks(0);
    std::vector<int> initial = {0, -1, 2};
    require(manager.set_task_assignment(initial), "initial simulator assignment invalid");
    auto states = env.curr_states;
    manager.curr_timestep = 1;
    manager.check_finished_tasks(states, 1); // starts task2 only
    manager.new_freeagents = {1}; manager.sync_shared_env(&env);
    scheduler.skip_matching_if_no_free_agents = true;
    std::vector<int> first;
    scheduler.schedule_plan_greedy_matching_warehouse(1000, first, &env);
    require(manager.set_task_assignment(first), "first simulator proposal rejected");
    manager.sync_shared_env(&env);
    const int holder = first[0] == 0 ? 0 : 1;
    require(first[holder] == 0, "held task missing");
    states[holder].location = 20;
    states[1 - holder].location = 12;
    states[2].location = 17;
    manager.curr_timestep = 2; env.curr_timestep = 2; env.curr_states = states;
    manager.check_finished_tasks(states, 2); // opens task0, completes task2
    manager.reveal_tasks(2); manager.sync_shared_env(&env);
    require(env.task_pool.at(0).idx_next_loc == 1 && env.task_pool.at(0).agent_assigned == holder,
            "task did not open with its committed owner");
    require(env.curr_task_schedule[2] == -1 && env.task_pool.at(3).agent_assigned == -1,
            "completed robot or replenished task not free");
    std::vector<int> second;
    scheduler.schedule_plan_greedy_matching_warehouse(1000, second, &env);
    require(second[holder] == 0, "opened task changed owner on next tick");
    require(manager.set_task_assignment(second), "second simulator proposal rejected");
    manager.sync_shared_env(&env);
    for (int robot = 0; robot < 3; ++robot) {
        require(second[robot] >= 0, "free robot left unassigned despite sufficient tasks");
        require(env.task_pool.at(second[robot]).agent_assigned == robot, "simulator owner resync failed");
    }
    require(manager.get_number_errors() == 0, "simulator reported schedule errors");
    records.push_back({{"scenario", "real_task_manager_lifecycle"}, {"first", first}, {"second", second}});
    std::cout << "KK_FIXTURE " << records.dump() << std::endl;
    return 0;
}
