#pragma once
// Map-specific policies have a separate, explicit CLI activation path.
// Generic callers leave SharedEnvironment::trick_instance empty.
#include "../tricks/warehouse_lanes.hpp"
#include "../tricks/warehouse_native.hpp"
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

struct Options { bool lanes = false, short_tasks = false, matching = false, remaining_flow = false, native_metric = false, native_bands = false; int known_horizon = 0; };

// Environment settings select components only after explicit CLI activation.
// Even a zero-valued setting without --trick is rejected to prevent silent use.
inline Options options(const std::string& instance) {
    const char* lanes = std::getenv("CGAR_TRICK_LANES");
    const char* short_tasks = std::getenv("CGAR_TRICK_SHORT_TASKS");
    const char* matching = std::getenv("CGAR_TRICK_UNOPENED_MATCH");
    const char* remaining_flow = std::getenv("CGAR_TRICK_REMAINING_FLOW");
    const char* native_metric = std::getenv("CGAR_TRICK_NATIVE_METRIC");
    const char* native_bands = std::getenv("CGAR_TRICK_NATIVE_BANDS");
    const char* known_horizon = std::getenv("CGAR_TRICK_KNOWN_HORIZON");
    if (instance.empty()) {
        if (lanes || short_tasks || matching || remaining_flow || native_metric || native_bands || known_horizon)
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
    int horizon = 0;
    if (known_horizon) {
        if (!*known_horizon) throw std::invalid_argument("CGAR_TRICK_KNOWN_HORIZON must be an integer in [0,1000000]");
        for (const char* p = known_horizon; *p; ++p) {
            if (*p < '0' || *p > '9' || horizon > (1000000 - (*p - '0')) / 10)
                throw std::invalid_argument("CGAR_TRICK_KNOWN_HORIZON must be an integer in [0,1000000]");
            horizon = horizon * 10 + (*p - '0');
        }
    }
    return {boolean(lanes, true), boolean(short_tasks, false), boolean(matching, false), boolean(remaining_flow, false),
            boolean(native_metric, false), boolean(native_bands, false), horizon};
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

// Verify the actual installed vector, including normalized wall entries.
// This binds both generated headers, not merely the printed SHA256 constant.
inline uint64_t validate_native_field(const std::vector<uint8_t>& costs, bool bands) {
    if (costs.size() != size_t(warehouse_rows) * warehouse_cols * 4)
        throw std::invalid_argument("native Warehouse field dimension mismatch");
    uint64_t value = 14695981039346656037ULL;
    for (uint8_t cost : costs) value = (value ^ cost) * 1099511628211ULL;
    if (value != (bands ? warehouse_native_bands_fnv1a64 : warehouse_native_nobands_fnv1a64))
        throw std::invalid_argument("native Warehouse installed field fingerprint mismatch");
    return value;
}

inline std::vector<uint8_t> native_forward_costs(const std::string& name, const std::vector<int>& map,
                                                 int rows, int cols, bool bands) {
    validate_map(name, map, rows, cols);
    std::vector<uint8_t> result(map.size() * 4, 20);
    auto hex = [](char ch) { return ch <= '9' ? ch - '0' : ch - 'a' + 10; };
    for (size_t cell = 0; cell < map.size(); ++cell) {
        if (warehouse_masks[cell] == 'x') continue;
        const int mask = hex(warehouse_masks[cell]);
        const int extra = bands ? (hex(warehouse_native_band_hex[cell / 4]) >> (cell % 4)) & 1 : 0;
        for (int d = 0; d < 4; ++d) result[cell * 4 + d] = (mask & (1 << d) ? 200 : 20) + extra;
    }
    return result;
}

inline const char* native_field_hash(bool bands) {
    return bands ? warehouse_native_bands_field_sha256 : warehouse_native_nobands_field_sha256;
}

}}  // namespace cgar::tricks
