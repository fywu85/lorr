// Diagnostic reference: replace NMS's environment initialization only.
// Every legal action costs one; no map type, lane pattern or fleet cap is used.
// NMS's archived PIBTS search, operation pool and 32-worker portfolio are retained.
#include <Objects/Environment/environment.hpp>
#include <settings.hpp>

void init_environment(SharedEnvironment& env) {
    get_map_type() = MapType::NONE;
    get_test_type() = TestType::NONE;  // 10,000 agent capacity for every instance
    get_map() = Map(env);
    get_gg() = GraphGuidance(env.rows, env.cols);
    for (uint32_t cell = 1; cell < get_map().get_size(); ++cell)
        for (uint32_t d = 0; d < 4; ++d)
            for (uint32_t a = 0; a < 4; ++a) get_gg().set(cell, d, a, 1);
    get_graph() = Graph(get_map(), get_gg());
    get_hm() = HeuristicMatrix(get_graph());
    get_dhm() = DynamicHeuristicMatrix(get_map(), get_graph());
    get_dhmr() = DHMR(get_graph());
    init_operations();
    get_omap() = OperationsMap(get_graph(), get_operations());
    get_robots_handler() = RobotsHandler(env.num_of_agents);
}

void update_environment(SharedEnvironment& env) {
    get_robots_handler().update(env);
}

std::vector<Action>& get_myplan() {
    static std::vector<Action> plan;
    return plan;
}
