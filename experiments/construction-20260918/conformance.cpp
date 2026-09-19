// Compare the new search kernel directly with frozen native NMS. Native NMS
// supplies candidate scores and priorities; geometry is computed independently.
#include <Objects/Environment/environment.hpp>
#include <Planner/PIBT/pibts.hpp>
#include <settings.hpp>
#include "temporal_geometry.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <numeric>
#include <random>

int main(int argc, char** argv) {
    nlohmann::json result;
    try {
        if (argc != 2) throw std::runtime_error("conformance OUTPUT");
        SharedEnvironment env; env.rows = 10; env.cols = 10; env.map.assign(100, 0);
        env.num_of_agents = 80; env.curr_task_schedule.resize(80); env.goal_locations.resize(80);
        env.curr_states.resize(80); env.file_storage_path = ".";
        init_environment(env);
        cgar::TemporalGeometry geometry;
        geometry.initialize(std::vector<char>(100, 1), 10, 10, []{});
        long long geometry_checks = 0;
        for (int cell = 0; cell < 100; ++cell) for (int ori = 0; ori < 4; ++ori) {
            const uint32_t node = get_graph().get_node(Position(cell + 1, ori));
            for (int op = 0; op < 129; ++op) {
                const auto& path = geometry.paths(cell, ori)[op];
                const auto& native = get_omap().get_nodes_path(node, op);
                if (path.valid != (native[0] > 0)) throw std::runtime_error("geometry validity differs");
                if (path.valid) {
                    for (int t = 0; t < 5; ++t) {
                        if (path.cells[t] != static_cast<int>(get_graph().get_pos(native[t]).get_pos()) - 1)
                            throw std::runtime_error("geometry positions differ");
                    }
                    if (path.depth != get_operation_depth(op)) throw std::runtime_error("geometry depth differs");
                    if (path.first_action != static_cast<int>(get_operations()[op][0])) throw std::runtime_error("operation order differs");
                }
                ++geometry_checks;
            }
        }
        std::mt19937 random(719);
        long long checked = 0, decisions = 0;
        int max_depth = 0;
        for (int fixture = 0; fixture < 96; ++fixture) {
            std::vector<int> cells(100); std::iota(cells.begin(), cells.end(), 0);
            std::shuffle(cells.begin(), cells.end(), random);
            env.task_pool.clear(); env.curr_timestep = fixture;
            for (int r = 0; r < 80; ++r) {
                env.curr_states[r] = State(cells[r], fixture, random() % 4);
                env.curr_task_schedule[r] = -1; env.goal_locations[r].clear();
                if ((r + fixture) % 11 == 0) continue;
                Task task; task.task_id = r; task.agent_assigned = r;
                int goal = random() % 100; if (goal == cells[r]) goal = (goal + 1) % 100;
                task.locations = {goal}; env.task_pool[r] = task;
                env.curr_task_schedule[r] = r; env.goal_locations[r] = {{goal, 0}};
            }
            update_environment(env);
            PIBTS native(get_robots_handler().get_robots(), get_now() + Milliseconds(30000));
            std::vector<std::vector<cgar::TemporalChoice>> choices(80);
            for (int r = 0; r < 80; ++r) {
                const auto& paths = geometry.paths(cells[r], env.curr_states[r].orientation);
                choices[r].push_back({&paths[0], native.probe_costs()[r][0], 0});
                for (uint32_t op : native.probe_choices()[r][5])
                    choices[r].push_back({&paths[op], native.probe_costs()[r][op], static_cast<int>(op)});
            }
            std::vector<int> order(native.probe_order().begin(), native.probe_order().end());
            const auto seed = uint64_t(fixture) * 2654435761ULL + 17;
            const std::vector<char> fixed(80, false);
            cgar::TemporalPibt port(100, choices, fixed, native.probe_power(), 32768, seed);
            port.construct(order, []{});
            const auto construction_exhausted = port.stats.budget_exhausted;
            if (PIBTS_STEPS != UINT32_MAX && PIBTS_STEPS) port.repair(PIBTS_STEPS, []{});
            native.solve(seed);
            if (!native.trace.construction_complete || construction_exhausted)
                throw std::runtime_error("fixture construction incomplete");
            const auto desires = native.get_desires();
            for (int r = 0; r < 80; ++r) {
                if (port.choice(r).operation != static_cast<int>(desires[r])) {
                    result["mismatch"] = {{"fixture", fixture}, {"robot", r}, {"native", desires[r]},
                                          {"port", port.choice(r).operation}, {"native_score", native.get_score()},
                                          {"port_score", port.score()}, {"steps", PIBTS_STEPS}};
                    throw std::runtime_error("selected operations differ");
                }
                ++checked;
            }
            max_depth = std::max(max_depth, port.stats.max_depth); ++decisions;
        }
        result["valid"] = true; result["geometry_checks"] = geometry_checks;
        result["decisions"] = decisions; result["robot_decisions"] = checked;
        result["max_displacement_depth"] = max_depth; result["search_steps"] = PIBTS_STEPS;
    } catch (const std::exception& error) {
        result["valid"] = false; result["error"] = error.what();
    }
    std::ofstream(argv[1]) << result.dump(2) << '\n';
    std::cout << result.dump(2) << '\n';
    return result.value("valid", false) ? 0 : 1;
}
