#pragma once
// Map-specific policies have a separate, explicit CLI activation path.
// Generic callers leave SharedEnvironment::trick_instance empty.
#include "../tricks/warehouse_lanes.hpp"
#include "../tricks/warehouse_native.hpp"
#include "../tricks/sortation_native.hpp"
#include "../tricks/random_native.hpp"
#include "../tricks/random_reference.hpp"
#include "../tricks/city_native.hpp"
#include "../tricks/game_native.hpp"
#include <stdexcept>
#include <string>
#include <vector>
#include <cstdint>
#include <cstdlib>

namespace cgar { namespace tricks {

inline bool random_instance(const std::string& name) { return name == "RANDOM-01" || name == "RANDOM-02" || name == "RANDOM-03" || name == "RANDOM-04" || name == "RANDOM-05"; }
inline bool dense_random_instance(const std::string& name) { return name == "RANDOM-04" || name == "RANDOM-05"; }

inline bool grid_instance(const std::string& name) { return name == "CITY-01" || name == "CITY-02" || name == "GAME"; }

inline void validate_name(const std::string& name) {
    if (name != "WAREHOUSE" && name != "SORTATION" && !random_instance(name) && !grid_instance(name))
        throw std::invalid_argument("unknown --trick instance: " + name + "; supported: WAREHOUSE, SORTATION, CITY-01, CITY-02, GAME, RANDOM-01, RANDOM-02, RANDOM-03, RANDOM-04, RANDOM-05");
}

struct Options { bool lanes = false, short_tasks = false, matching = false, remaining_flow = false, native_metric = false, native_bands = false; int known_horizon = 0; bool horizon_margin = false; int horizon_margin_percentile = 0; bool native_neutral_tail = false; bool match_horizon = false; int native_turn_cost = 1; int native_prewarm_threads = 0; bool random_uniform = false; bool rank_squared = false; int random_reference = 0; int game_active_limit = 0; bool game_tabu = false; bool horizon_manhattan = false; int lane_cost = 16; int random_task_cap = 0; };

// Environment settings select components only after explicit CLI activation.
// Even a zero-valued setting without --trick is rejected to prevent silent use.
inline Options options(const std::string& instance) {
    const char* lanes = std::getenv("CGAR_TRICK_LANES");
    const char* lane_cost = std::getenv("CGAR_TRICK_LANE_COST");
    const char* short_tasks = std::getenv("CGAR_TRICK_SHORT_TASKS");
    const char* task_cap = std::getenv("CGAR_TRICK_RANDOM_TASK_CAP");
    const char* matching = std::getenv("CGAR_TRICK_UNOPENED_MATCH");
    const char* remaining_flow = std::getenv("CGAR_TRICK_REMAINING_FLOW");
    const char* native_metric = std::getenv("CGAR_TRICK_NATIVE_METRIC");
    const char* native_bands = std::getenv("CGAR_TRICK_NATIVE_BANDS");
    const char* known_horizon = std::getenv("CGAR_TRICK_KNOWN_HORIZON");
    const char* horizon_manhattan = std::getenv("CGAR_TRICK_HORIZON_MANHATTAN");
    const char* horizon_margin = std::getenv("CGAR_TRICK_HORIZON_MARGIN");
    const char* margin_percentile = std::getenv("CGAR_TRICK_HORIZON_MARGIN_PERCENTILE");
    const char* neutral_tail = std::getenv("CGAR_TRICK_NATIVE_NEUTRAL_TAIL");
    const char* match_horizon = std::getenv("CGAR_TRICK_MATCH_HORIZON");
    const char* native_turn_cost = std::getenv("CGAR_TRICK_NATIVE_TURN_COST");
    const char* prewarm_threads = std::getenv("CGAR_TRICK_NATIVE_PREWARM_THREADS");
    const char* random_uniform = std::getenv("CGAR_TRICK_RANDOM_UNIFORM");
    const char* rank_squared = std::getenv("CGAR_TRICK_RANK_SQUARED");
    const char* reference = std::getenv("CGAR_TRICK_RANDOM_REFERENCE");
    const char* game_active = std::getenv("CGAR_TRICK_GAME_ACTIVE_LIMIT");
    const char* game_tabu = std::getenv("CGAR_TRICK_GAME_TABU");
    if (instance.empty()) {
        if (lanes || short_tasks || task_cap || matching || remaining_flow || native_metric || native_bands || known_horizon || horizon_margin || margin_percentile || neutral_tail || match_horizon || native_turn_cost || prewarm_threads || random_uniform || rank_squared || reference || game_active || game_tabu || horizon_manhattan || lane_cost)
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
    int opposing = 16;
    if (lane_cost) {
        const std::string value(lane_cost);
        if (value != "4" && value != "8" && value != "12" && value != "16")
            throw std::invalid_argument("lane cost must be 4, 8, 12 or 16");
        if (!grid_instance(instance) || !boolean(lanes, true) || boolean(native_metric, false))
            throw std::invalid_argument("lane cost requires explicit CITY/GAME adapted lanes");
        opposing = std::stoi(value);
    }
    int active_limit = 0;
    if ((game_active || game_tabu) && instance != "GAME")
        throw std::invalid_argument("GAME fleet selectors require --trick GAME");
    if (game_active) {
        if (!*game_active) throw std::invalid_argument("GAME active limit must be an integer in [0,6500]");
        for (const char* p = game_active; *p; ++p) {
            if (*p < '0' || *p > '9' || active_limit > (6500 - (*p - '0')) / 10)
                throw std::invalid_argument("GAME active limit must be an integer in [0,6500]");
            active_limit = active_limit * 10 + (*p - '0');
        }
    }
    const bool preserve_tabu = boolean(game_tabu, false);
    if (preserve_tabu && !active_limit)
        throw std::invalid_argument("GAME tabu selection requires a positive active limit");
    int reference_id = 0;
    if (reference) {
        const std::string value(reference);
        if (value != "0" && value != "1" && value != "2" && value != "3")
            throw std::invalid_argument("random reference must be 0(existing), 1(NMS arrows), 2(KK forward) or 3(PILOT flow)");
        reference_id = value[0] - '0';
        if (!random_instance(instance) || (reference_id && (!boolean(lanes, true) || !boolean(native_metric, false))))
            throw std::invalid_argument("random reference requires a RANDOM CLI gate and active references require native static lanes");
        if (reference_id == 2 && instance != "RANDOM-02" && instance != "RANDOM-03" && instance != "RANDOM-04")
            throw std::invalid_argument("KK forward reference is available only for RANDOM-02/03/04");
    }
    if (random_instance(instance) && !dense_random_instance(instance) && boolean(lanes, true) && !reference_id)
        throw std::invalid_argument("RANDOM-01/02/03 static guidance requires an explicit reference provider");
    const bool squared = boolean(rank_squared, false);
    if (rank_squared && instance != "GAME" && !dense_random_instance(instance))
        throw std::invalid_argument("squared rank selector requires --trick GAME, RANDOM-04 or RANDOM-05");
    const bool uniform = boolean(random_uniform, false);
    if (random_uniform && (!random_instance(instance) || !boolean(lanes, true) || !boolean(native_metric, false)))
        throw std::invalid_argument("random uniform control requires RANDOM instance, static lanes and native metric");
    if (random_instance(instance) && (boolean(native_bands, false) ||
        (boolean(lanes, true) && !boolean(native_metric, false))))
        throw std::invalid_argument("RANDOM field requires explicit native metric without bands");
    if (grid_instance(instance) && boolean(native_bands, false))
        throw std::invalid_argument("CITY/GAME native fields have no outer-band surcharge");
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
    const int max_native_turn = reference_id ? 64 : 16;
    int native_turn = reference_id ? 20 : 1;
    if (native_turn_cost) {
        native_turn = 0;
        if (!*native_turn_cost) throw std::invalid_argument("native turn cost must be an integer in [1," + std::to_string(max_native_turn) + "]");
        for (const char* p = native_turn_cost; *p; ++p) {
            if (*p < '0' || *p > '9' || native_turn > (max_native_turn - (*p - '0')) / 10)
                throw std::invalid_argument("native turn cost must be an integer in [1," + std::to_string(max_native_turn) + "]");
            native_turn = native_turn * 10 + (*p - '0');
        }
        if (native_turn < 1 || !boolean(native_metric, false))
            throw std::invalid_argument("native turn cost requires a native metric and value in [1," + std::to_string(max_native_turn) + "]");
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
    int random_task_cap = 0;
    if (task_cap) {
        if (!random_instance(instance) || !*task_cap)
            throw std::invalid_argument("random task cap requires --trick RANDOM-01..05 and an integer in [0,800]");
        for (const char* p = task_cap; *p; ++p) {
            if (*p < '0' || *p > '9' || random_task_cap > (800 - (*p - '0')) / 10)
                throw std::invalid_argument("random task cap must be an integer in [0,800]");
            random_task_cap = random_task_cap * 10 + (*p - '0');
        }
    }
    const bool neutral = boolean(neutral_tail, false);
    if (neutral && !boolean(native_metric, false))
        throw std::invalid_argument("native neutral tail requires the native metric");
    const bool manhattan = boolean(horizon_manhattan, false);
    if (manhattan && !horizon) throw std::invalid_argument("Manhattan horizon bound requires a positive configured horizon");
    const bool margin = boolean(horizon_margin, false);
    if (percentile && !margin) throw std::invalid_argument("positive horizon percentile requires horizon margin");
    if (margin && !horizon) throw std::invalid_argument("horizon margin requires a configured positive known horizon");
    const bool guard = boolean(match_horizon, false);
    if (guard && (!horizon || !boolean(matching, false)))
        throw std::invalid_argument("matching horizon guard requires configured horizon and unopened matching");
    return {boolean(lanes, true), boolean(short_tasks, false), boolean(matching, false), boolean(remaining_flow, false),
            boolean(native_metric, false), boolean(native_bands, false), horizon, margin, percentile, neutral, guard, native_turn, prewarm, uniform, squared, reference_id, active_limit, preserve_tabu, manhattan, opposing, random_task_cap};
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
    const uint8_t* native_costs = nullptr;
};

inline FieldAsset field_asset(const std::string& name, int reference = 0) {
    validate_name(name);
    if (reference < 0 || reference > 3 || (reference && !random_instance(name)))
        throw std::invalid_argument("reference field requires a RANDOM instance and provider0..3");
    if (random_instance(name)) {
        const uint8_t* costs = random_forward;
        const char* hash = random_native_nobands_field_sha256;
        uint64_t fingerprint = random_native_nobands_fnv1a64;
        if (reference == 1) {
            costs = random_nms_forward; hash = random_nms_native_nobands_field_sha256;
            fingerprint = random_nms_native_nobands_fnv1a64;
        } else if (reference == 2) {
            if (name == "RANDOM-02") { costs = random_kk02_forward; hash = random_kk02_native_nobands_field_sha256; fingerprint = random_kk02_native_nobands_fnv1a64; }
            else if (name == "RANDOM-03") { costs = random_kk03_forward; hash = random_kk03_native_nobands_field_sha256; fingerprint = random_kk03_native_nobands_fnv1a64; }
            else if (name == "RANDOM-04") { costs = random_kk04_forward; hash = random_kk04_native_nobands_field_sha256; fingerprint = random_kk04_native_nobands_fnv1a64; }
            else throw std::invalid_argument("KK field has no RANDOM-01/05 provider");
        }
        return {random_rows, random_cols, random_free, random_masks, nullptr,
                random_occupancy_sha256, nullptr, nullptr, hash, 0, fingerprint, costs};
    }
    if (name == "CITY-01" || name == "CITY-02")
        return {city_rows, city_cols, city_free, city_masks, nullptr,
                city_occupancy_sha256, city_field_sha256, nullptr, city_native_nobands_field_sha256,
                0, city_native_nobands_fnv1a64};
    if (name == "GAME")
        return {game_rows, game_cols, game_free, game_masks, nullptr,
                game_occupancy_sha256, game_field_sha256, nullptr, game_native_nobands_field_sha256,
                0, game_native_nobands_fnv1a64};
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

inline void validate_instance(const std::string& name, const std::vector<int>& map,
                              int rows, int cols, int robots) {
    validate_map(name, map, rows, cols);
    if ((name == "RANDOM-01" && robots != 100) || (name == "RANDOM-02" && robots != 200) ||
        (name == "RANDOM-03" && robots != 400) || (name == "RANDOM-04" && robots != 700) || (name == "RANDOM-05" && robots != 800) ||
        (name == "CITY-01" && robots != 1500) || (name == "CITY-02" && robots != 3000) ||
        (name == "GAME" && robots != 6500))
        throw std::invalid_argument("--trick " + name + " requires its exact MR24 team size");
}

inline void validate_native_options(const std::string& name, bool bands, bool uniform, int reference = 0) {
    field_asset(name, reference);
    if (random_instance(name) && !dense_random_instance(name) && !reference)
        throw std::invalid_argument("sparse RANDOM native guidance requires a reference provider");
    if (((random_instance(name) || grid_instance(name)) && bands) || (uniform && !random_instance(name)))
        throw std::invalid_argument("native field bands/uniform selector incompatible with named instance");
}

inline std::vector<uint8_t> forward_costs(const std::string& name, const std::vector<int>& map,
                                          int rows, int cols, int opposing = 16) {
    validate_map(name, map, rows, cols);
    const auto asset = field_asset(name);
    if ((opposing != 4 && opposing != 8 && opposing != 12 && opposing != 16) ||
        (opposing != 16 && !grid_instance(name)))
        throw std::invalid_argument("adapted lane price requires CITY/GAME and one of 4/8/12/16");
    if (random_instance(name)) throw std::invalid_argument("RANDOM field has no legacy lane-mask metric");
    std::vector<uint8_t> result(map.size() * 4, 4);
    for (size_t cell = 0; cell < map.size(); ++cell) {
        const char ch = asset.masks[cell];
        if (ch == 'x') continue;
        const int mask = ch <= '9' ? ch - '0' : ch - 'a' + 10;
        for (int d = 0; d < 4; ++d) if (mask & (1 << d)) result[cell * 4 + d] = opposing;
    }
    return result;
}

// Verify the actual installed vector, including normalized wall entries.
// The default name preserves the existing Warehouse verification API.
inline uint64_t validate_native_field(const std::vector<uint8_t>& costs, bool bands,
                                      const std::string& name = "WAREHOUSE", bool uniform = false, int reference = 0) {
    validate_native_options(name, bands, uniform, reference);
    const auto asset = field_asset(name, reference);
    if (costs.size() != size_t(asset.rows) * asset.cols * 4)
        throw std::invalid_argument("native " + name + " field dimension mismatch");
    uint64_t value = 14695981039346656037ULL;
    for (uint8_t cost : costs) value = (value ^ cost) * 1099511628211ULL;
    if (value != (uniform ? random_uniform_fnv1a64 : bands ? asset.native_bands_fnv1a64 : asset.native_nobands_fnv1a64))
        throw std::invalid_argument("native " + name + " installed field fingerprint mismatch");
    return value;
}

inline std::vector<uint8_t> native_forward_costs(const std::string& name, const std::vector<int>& map,
                                                 int rows, int cols, bool bands, bool uniform = false, int reference = 0) {
    validate_native_options(name, bands, uniform, reference);
    validate_map(name, map, rows, cols);
    const auto asset = field_asset(name, reference);
    std::vector<uint8_t> result(map.size() * 4, 20);
    if (uniform) return result;
    if (asset.native_costs) return std::vector<uint8_t>(asset.native_costs, asset.native_costs + result.size());
    auto hex = [](char ch) { return ch <= '9' ? ch - '0' : ch - 'a' + 10; };
    for (size_t cell = 0; cell < map.size(); ++cell) {
        if (asset.masks[cell] == 'x') continue;
        const int mask = hex(asset.masks[cell]);
        const int extra = bands ? (hex(asset.band_hex[cell / 4]) >> (cell % 4)) & 1 : 0;
        for (int d = 0; d < 4; ++d) result[cell * 4 + d] = (mask & (1 << d) ? 200 : 20) + extra;
    }
    return result;
}

inline const char* native_field_hash(bool bands, const std::string& name = "WAREHOUSE", bool uniform = false, int reference = 0) {
    validate_native_options(name, bands, uniform, reference);
    if (uniform) return random_uniform_field_sha256;
    const auto asset = field_asset(name, reference);
    return bands ? asset.native_bands_sha256 : asset.native_nobands_sha256;
}

inline const char* lane_field_hash(const std::string& name, int opposing = 16) {
    if (random_instance(name)) throw std::invalid_argument("RANDOM field has no legacy lane hash");
    const auto asset = field_asset(name);
    if (opposing == 16) return asset.lane_sha256;
    if (!grid_instance(name) || (opposing != 4 && opposing != 8 && opposing != 12))
        throw std::invalid_argument("adapted lane hash requires CITY/GAME and one of 4/8/12/16");
    if (name == "GAME") return opposing == 4 ? game_lane4_field_sha256 : opposing == 8 ? game_lane8_field_sha256 : game_lane12_field_sha256;
    return opposing == 4 ? city_lane4_field_sha256 : opposing == 8 ? city_lane8_field_sha256 : city_lane12_field_sha256;
}
inline const char* occupancy_hash(const std::string& name) { return field_asset(name).occupancy_sha256; }

}}  // namespace cgar::tricks
