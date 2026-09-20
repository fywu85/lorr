#pragma once
// Map-specific policies have a separate, explicit CLI activation path.
// Generic callers leave SharedEnvironment::trick_instance empty.
#include "../tricks/warehouse_lanes.hpp"
#include <stdexcept>
#include <string>
#include <vector>
#include <cstdint>
#include <cstdlib>

namespace cgar { namespace tricks {

inline void validate_name(const std::string& name) {
    if (name != "WAREHOUSE")
        throw std::invalid_argument("unknown --trick instance: " + name + "; supported: WAREHOUSE");
}

struct Options { bool lanes = false, short_tasks = false, matching = false; };

// Environment settings select components only after explicit CLI activation.
// Even a zero-valued setting without --trick is rejected to prevent silent use.
inline Options options(const std::string& instance) {
    const char* lanes = std::getenv("CGAR_TRICK_LANES");
    const char* short_tasks = std::getenv("CGAR_TRICK_SHORT_TASKS");
    const char* matching = std::getenv("CGAR_TRICK_UNOPENED_MATCH");
    if (instance.empty()) {
        if (lanes || short_tasks || matching)
            throw std::invalid_argument("CGAR_TRICK component settings require --trick WAREHOUSE");
        return {};
    }
    validate_name(instance);
    auto boolean = [](const char* value, bool fallback) {
        if (!value) return fallback;
        if (std::string(value) == "0") return false;
        if (std::string(value) == "1") return true;
        throw std::invalid_argument("CGAR_TRICK component settings must be 0 or 1");
    };
    return {boolean(lanes, true), boolean(short_tasks, false), boolean(matching, false)};
}

inline void validate_map(const std::string& name, const std::vector<int>& map, int rows, int cols) {
    validate_name(name);
    if (rows != warehouse_rows || cols != warehouse_cols || map.size() != size_t(rows) * cols)
        throw std::invalid_argument("--trick WAREHOUSE requires the exact MR24 warehouse layout (140x500)");
    int free = 0;
    for (size_t cell = 0; cell < map.size(); ++cell) {
        if (bool(map[cell]) != (warehouse_masks[cell] == 'x'))
            throw std::invalid_argument("--trick WAREHOUSE layout mismatch at cell " + std::to_string(cell));
        free += !map[cell];
    }
    if (free != warehouse_free) throw std::invalid_argument("--trick WAREHOUSE free-cell mismatch");
}

inline std::vector<uint8_t> forward_costs(const std::string& name, const std::vector<int>& map,
                                          int rows, int cols) {
    validate_map(name, map, rows, cols);
    std::vector<uint8_t> result(map.size() * 4, 4);
    for (size_t cell = 0; cell < map.size(); ++cell) {
        const char ch = warehouse_masks[cell];
        if (ch == 'x') continue;
        const int mask = ch <= '9' ? ch - '0' : ch - 'a' + 10;
        for (int d = 0; d < 4; ++d) if (mask & (1 << d)) result[cell * 4 + d] = 16;
    }
    return result;
}

}}  // namespace cgar::tricks
