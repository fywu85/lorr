#include "cgar.hpp"

#include <cstdlib>
#include <iostream>
#include <vector>

SharedEnvironment make_env(int columns, int now) {
    SharedEnvironment e;
    e.num_of_agents = 1;
    e.rows = 2;
    e.cols = columns;
    e.map.assign(e.rows * e.cols, 0);
    e.curr_timestep = now;
    e.curr_states.emplace_back(0, now, 0);
    e.curr_task_schedule = {-1};
    e.goal_locations.resize(1);
    return e;
}

void add_task(SharedEnvironment& e, int id, int revealed, std::vector<int> stops) {
    Task task;
    task.task_id = id;
    task.t_revealed = revealed;
    task.locations = std::move(stops);
    e.task_pool.emplace(id, task);
}

int main() {
    setenv("CGAR_MAX_PAIRS", "2000000", 1);
    setenv("CGAR_SCHED_TABLES", "128", 1);
    setenv("CGAR_HRRN", "1", 1);
    setenv("CGAR_CERT", "1", 1);
    setenv("CGAR_TXN", "1", 1);
    {
        auto e = make_env(101, 0);
        for (int id = 0; id < 16; ++id) add_task(e, id, 0, {0, 100});
        add_task(e, 16, 0, {1, 1});
        cgar::Cgar planner;
        planner.initialize(&e, 1000);
        std::vector<int> proposed;
        planner.schedule(&e, 100, proposed);
        const int selected = proposed.at(0);
        const int cost = selected == 16 ? 1 : 100;
        std::cout << "CHAIN_CAP selected=" << selected << " selected_cost=" << cost
                  << " optimal_task=16 optimal_cost=1\n";
    }
    {
        auto e = make_env(31, 100);
        for (int id = 0; id < 16; ++id) add_task(e, id, 80, {20, 21});
        add_task(e, 16, 0, {20, 30});
        cgar::Cgar planner;
        planner.initialize(&e, 1000);
        std::vector<int> proposed;
        planner.schedule(&e, 100, proposed);
        const int selected = proposed.at(0);
        const int cost = selected == 16 ? 30 : 21;
        const double score = selected == 16 ? 1.0 + 100.0 / 30.0 : 1.0 + 20.0 / 21.0;
        std::cout << "BUCKET_HRRN selected=" << selected << " selected_cost=" << cost
                  << " selected_score=" << score
                  << " optimal_task=16 optimal_cost=30 optimal_score=" << 1.0 + 100.0 / 30.0 << '\n';
    }
}
