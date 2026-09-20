#pragma once
// Map-specific policies have a separate, explicit CLI activation path.
// Generic callers leave SharedEnvironment::trick_instance empty.
#include "../tricks/warehouse_lanes.hpp"
#include <stdexcept>
#include <string>
#include <vector>
#include <cstdint>

namespace cgar { namespace tricks {

inline void validate_name(const std::string& name) {
    if (name != "WAREHOUSE")
        throw std::invalid_argument("unknown --trick instance: " + name + "; supported: WAREHOUSE");
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
