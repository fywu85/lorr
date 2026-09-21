#pragma once
// Explicit GAME task-admission trick, inspired by Kitty Knight's fleet selector.
// This selects who may receive a new task. Every robot remains in CGAR's motion
// and recovery system; existing task ownership is preserved.
#include "../tricks/game_fleet_tabu.hpp"
#include <algorithm>
#include <cstdint>
#include <iterator>
#include <stdexcept>
#include <utility>
#include <vector>

namespace cgar {
struct GameFleetSelection {
    std::vector<char> excluded;
    int disabled = 0, tabu_kept = 0, held_kept = 0, eligible_pool = 0;
    uint64_t fingerprint = 14695981039346656037ULL;
};

inline bool game_tabu_location(int cell) {
    return std::binary_search(std::begin(tricks::game_tabu_cells),
                              std::end(tricks::game_tabu_cells), cell);
}

// Independent deterministic stream, so selecting a fleet never consumes CGAR's
// motion RNG. This is an adaptation, not KK's stateful std::shuffle sequence.
inline uint64_t game_fleet_priority(uint64_t seed, int robot) {
    uint64_t x = seed ^ (uint64_t(uint32_t(robot)) + 0x9e3779b97f4a7c15ULL);
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    return x ^ (x >> 31);
}

template<class Deadline>
GameFleetSelection select_game_fleet(const std::vector<int>& locations,
        const std::vector<int>& held, int nominal_active, bool preserve_tabu,
        uint64_t seed, Deadline check) {
    const int count = static_cast<int>(locations.size());
    if (held.size() != locations.size() || nominal_active < 1 || nominal_active > count)
        throw std::invalid_argument("invalid GAME fleet selection inputs");
    GameFleetSelection result; result.excluded.assign(count, false);
    std::vector<std::pair<uint64_t, int>> pool; pool.reserve(count);
    for (int robot = 0; robot < count; ++robot) {
        if (!(robot % 64)) check();
        if (locations[robot] < 0 || locations[robot] >= 481*530 || held[robot] < -1)
            throw std::invalid_argument("invalid GAME start or task ownership");
        const bool tabu = preserve_tabu && game_tabu_location(locations[robot]);
        const bool owned = held[robot] >= 0;
        result.tabu_kept += tabu; result.held_kept += owned;
        if (!tabu && !owned) pool.emplace_back(game_fleet_priority(seed, robot), robot);
    }
    result.eligible_pool = static_cast<int>(pool.size());
    std::sort(pool.begin(), pool.end()); check();
    result.disabled = std::min(count - nominal_active, result.eligible_pool);
    for (int rank = 0; rank < result.disabled; ++rank) result.excluded[pool[rank].second] = true;
    for (char excluded : result.excluded)
        result.fingerprint = (result.fingerprint ^ uint64_t(excluded != 0)) * 1099511628211ULL;
    check(); // Publish only the complete selection; timeout never returns a prefix.
    return result;
}
} // namespace cgar
