// Exercise KK's actual configuration loader before heavy preprocessing.
#include "planner.h"
#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main(int argc, char** argv) {
    if (argc != 4) throw std::runtime_error("expected supplied, uniform, malformed configs");
    SharedEnvironment env;
    env.num_of_agents = 10000; env.rows = env.cols = 1; env.map = {0};
    env.map_name = "warehouse_large.map";
    nlohmann::json loaded[2];
    for (int i = 0; i < 2; ++i) {
        setenv("CONFIG_PATH", argv[i + 1], 1);
        unsetenv("MAP_WEIGHT_PATH");
        MyPlanner::WPPLPlanner planner(&env);
        planner.load_configs();
        loaded[i] = planner.config;
        const auto weights = planner.config.at("map_weights_path").get<std::string>();
        if ((i == 1) != weights.empty()) throw std::runtime_error("wrong decoded weight choice");
    }
    // Details is the native diagnostic string containing the original config.
    for (auto& c : loaded) { c.erase("map_weights_path"); c.erase("details"); }
    if (loaded[0] != loaded[1]) throw std::runtime_error("another planner option changed");
    bool rejected = false;
    try {
        setenv("CONFIG_PATH", argv[3], 1);
        MyPlanner::WPPLPlanner planner(&env); planner.load_configs();
    } catch (const nlohmann::json::type_error&) { rejected = true; }
    if (!rejected) throw std::runtime_error("malformed scalar configuration unexpectedly accepted");
    std::cout << "KK_CONFIG_FIXTURE {\"native_loader_passed\":true,\"only_weight_path_differs\":true,"
                 "\"old_malformed_shape_rejected\":true}" << std::endl;
    return 0;
}
