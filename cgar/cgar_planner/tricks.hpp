#pragma once
// Map-specific policies have a separate, explicit CLI activation path.
// Generic callers leave SharedEnvironment::trick_instance empty.
#include "../tricks/warehouse_lanes.hpp"
#include "../tricks/warehouse_native.hpp"
#include "../tricks/sortation_native.hpp"
#include <stdexcept>
#include <string>
#include <vector>
#include <cstdint>
#include <cstdlib>

namespace cgar { namespace tricks {

inline void validate_name(const std::string& name) {
    if (name != "WAREHOUSE" && name != "SORTATION")
        throw std::invalid_argument("unknown --trick instance: " + name + "; supported: WAREHOUSE, SORTATION");
}

struct Options { bool lanes = false, short_tasks = false, matching = false, remaining_flow = false, native_metric = false, native_bands = false; int known_horizon = 0; bool horizon_margin = false; int horizon_margin_percentile = 0; bool native_neutral_tail = false; bool match_horizon = false; int native_turn_cost = 1; int native_prewarm_threads = 0; };

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
    const char* horizon_margin = std::getenv("CGAR_TRICK_HORIZON_MARGIN");
    const char* margin_percentile = std::getenv("CGAR_TRICK_HORIZON_MARGIN_PERCENTILE");
    const char* neutral_tail = std::getenv("CGAR_TRICK_NATIVE_NEUTRAL_TAIL");
    const char* match_horizon = std::getenv("CGAR_TRICK_MATCH_HORIZON");
    const char* native_turn_cost = std::getenv("CGAR_TRICK_NATIVE_TURN_COST");
    const char* prewarm_threads = std::getenv("CGAR_TRICK_NATIVE_PREWARM_THREADS");
    if (instance.empty()) {
        if (lanes || short_tasks || matching || remaining_flow || native_metric || native_bands || known_horizon || horizon_margin || margin_percentile || neutral_tail || match_horizon || native_turn_cost || prewarm_threads)
            throw std::invalid_argument("CGAR_TRICK component settings require --trick <instance>");
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
    int percentile = 0;
    if (margin_percentile) {
        if (!*margin_percentile) throw std::invalid_argument("CGAR_TRICK_HORIZON_MARGIN_PERCENTILE must be an integer in [0,100]");
        for (const char* p = margin_percentile; *p; ++p) {
            if (*p < '0' || *p > '9' || percentile > (100 - (*p - '0')) / 10)
                throw std::invalid_argument("CGAR_TRICK_HORIZON_MARGIN_PERCENTILE must be an integer in [0,100]");
            percentile = percentile * 10 + (*p - '0');
        }
    }
    int native_turn = 1;
    if (native_turn_cost) {
        native_turn = 0;
        if (!*native_turn_cost) throw std::invalid_argument("native turn cost must be an integer in [1,16]");
        for (const char* p = native_turn_cost; *p; ++p) {
            if (*p < '0' || *p > '9' || native_turn > (16 - (*p - '0')) / 10)
                throw std::invalid_argument("native turn cost must be an integer in [1,16]");
            native_turn = native_turn * 10 + (*p - '0');
        }
        if (native_turn < 1 || !boolean(native_metric, false))
            throw std::invalid_argument("native turn cost requires a native metric and value in [1,16]");
    }
    int prewarm = 0;
    if (prewarm_threads) {
        if (!*prewarm_threads) throw std::invalid_argument("native prewarm threads must be an integer in [0,32]");
        for (const char* p = prewarm_threads; *p; ++p) {
            if (*p < '0' || *p > '9' || prewarm > (32 - (*p - '0')) / 10)
                throw std::invalid_argument("native prewarm threads must be an integer in [0,32]");
            prewarm = prewarm * 10 + (*p - '0');
        }
        if (!boolean(native_metric, false))
            throw std::invalid_argument("native prewarm selector requires a native metric");
    }
    const bool neutral = boolean(neutral_tail, false);
    if (neutral && !boolean(native_metric, false))
        throw std::invalid_argument("native neutral tail requires the native metric");
    const bool margin = boolean(horizon_margin, false);
    if (percentile && !margin) throw std::invalid_argument("positive horizon percentile requires horizon margin");
    if (margin && !horizon) throw std::invalid_argument("horizon margin requires a configured positive known horizon");
    const bool guard = boolean(match_horizon, false);
    if (guard && (!horizon || !boolean(matching, false)))
        throw std::invalid_argument("matching horizon guard requires configured horizon and unopened matching");
    return {boolean(lanes, true), boolean(short_tasks, false), boolean(matching, false), boolean(remaining_flow, false),
            boolean(native_metric, false), boolean(native_bands, false), horizon, margin, percentile, neutral, guard, native_turn, prewarm};
}

// Select only after an explicit CLI name. No filename or size based dispatch:
// Warehouse and Sortation share dimensions but have different occupancy.
struct FieldAsset {
    int rows, cols, free_cells;
    const char* masks;
    const char* band_hex;
    const char* occupancy_sha256;
    const char* lane_sha256;
    const char* native_bands_sha256;
    const char* native_nobands_sha256;
    uint64_t native_bands_fnv1a64, native_nobands_fnv1a64;
};

inline FieldAsset field_asset(const std::string& name) {
    validate_name(name);
    if (name == "SORTATION")
        return {sortation_rows, sortation_cols, sortation_free, sortation_masks,
                sortation_native_band_hex, sortation_occupancy_sha256, sortation_field_sha256,
                sortation_native_bands_field_sha256, sortation_native_nobands_field_sha256,
                sortation_native_bands_fnv1a64, sortation_native_nobands_fnv1a64};
    return {warehouse_rows, warehouse_cols, warehouse_free, warehouse_masks,
            warehouse_native_band_hex, warehouse_occupancy_sha256, warehouse_field_sha256,
            warehouse_native_bands_field_sha256, warehouse_native_nobands_field_sha256,
            warehouse_native_bands_fnv1a64, warehouse_native_nobands_fnv1a64};
}

inline void validate_map(const std::string& name, const std::vector<int>& map, int rows, int cols) {
    const auto asset = field_asset(name);
    if (rows != asset.rows || cols != asset.cols || map.size() != size_t(rows) * cols)
        throw std::invalid_argument("--trick " + name + " requires its exact MR24 layout (" +
                                    std::to_string(asset.rows) + "x" + std::to_string(asset.cols) + ")");
    int free = 0;
    for (size_t cell = 0; cell < map.size(); ++cell) {
        if (bool(map[cell]) != (asset.masks[cell] == 'x'))
            throw std::invalid_argument("--trick " + name + " layout mismatch at cell " + std::to_string(cell));
        free += !map[cell];
    }
    if (free != asset.free_cells) throw std::invalid_argument("--trick " + name + " free-cell mismatch");
}

inline std::vector<uint8_t> forward_costs(const std::string& name, const std::vector<int>& map,
                                          int rows, int cols) {
    validate_map(name, map, rows, cols);
    const auto asset = field_asset(name);
    std::vector<uint8_t> result(map.size() * 4, 4);
    for (size_t cell = 0; cell < map.size(); ++cell) {
        const char ch = asset.masks[cell];
        if (ch == 'x') continue;
        const int mask = ch <= '9' ? ch - '0' : ch - 'a' + 10;
        for (int d = 0; d < 4; ++d) if (mask & (1 << d)) result[cell * 4 + d] = 16;
    }
    return result;
}

// Verify the actual installed vector, including normalized wall entries.
// The default name preserves the existing Warehouse verification API.
inline uint64_t validate_native_field(const std::vector<uint8_t>& costs, bool bands,
                                      const std::string& name = "WAREHOUSE") {
    const auto asset = field_asset(name);
    if (costs.size() != size_t(asset.rows) * asset.cols * 4)
        throw std::invalid_argument("native " + name + " field dimension mismatch");
    uint64_t value = 14695981039346656037ULL;
    for (uint8_t cost : costs) value = (value ^ cost) * 1099511628211ULL;
    if (value != (bands ? asset.native_bands_fnv1a64 : asset.native_nobands_fnv1a64))
        throw std::invalid_argument("native " + name + " installed field fingerprint mismatch");
    return value;
}

inline std::vector<uint8_t> native_forward_costs(const std::string& name, const std::vector<int>& map,
                                                 int rows, int cols, bool bands) {
    validate_map(name, map, rows, cols);
    const auto asset = field_asset(name);
    std::vector<uint8_t> result(map.size() * 4, 20);
    auto hex = [](char ch) { return ch <= '9' ? ch - '0' : ch - 'a' + 10; };
    for (size_t cell = 0; cell < map.size(); ++cell) {
        if (asset.masks[cell] == 'x') continue;
        const int mask = hex(asset.masks[cell]);
        const int extra = bands ? (hex(asset.band_hex[cell / 4]) >> (cell % 4)) & 1 : 0;
        for (int d = 0; d < 4; ++d) result[cell * 4 + d] = (mask & (1 << d) ? 200 : 20) + extra;
    }
    return result;
}

inline const char* native_field_hash(bool bands, const std::string& name = "WAREHOUSE") {
    const auto asset = field_asset(name);
    return bands ? asset.native_bands_sha256 : asset.native_nobands_sha256;
}

inline const char* lane_field_hash(const std::string& name) { return field_asset(name).lane_sha256; }
inline const char* occupancy_hash(const std::string& name) { return field_asset(name).occupancy_sha256; }

}}  // namespace cgar::tricks
