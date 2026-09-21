#pragma once
// Exact bounded pickup permutation: integer distance first, fewer moved tasks
// second. Identity must be feasible. No planner state, RNG or cache is touched.
#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace cgar {
inline constexpr int maximum_pickup_group = 256;
struct AssignmentPermutation {
    std::vector<int> column;
    long long before = 0, after = 0;
    int changed = 0;
};

template<class Check>
AssignmentPermutation minimum_pickup_permutation(const std::vector<int>& costs, int n,
                                                int unreachable, Check check) {
    check();
    if (n < 0 || n > maximum_pickup_group || costs.size() != size_t(n) * n || unreachable < 1)
        throw std::invalid_argument("invalid bounded pickup matrix");
    int maximum = 0;
    for (int value : costs) {
        if (value < 0 || value > unreachable) throw std::invalid_argument("invalid pickup matrix entry");
        if (value < unreachable) maximum = std::max(maximum, value);
    }
    AssignmentPermutation result; result.column.resize(n);
    for (int i = 0; i < n; ++i) {
        if (costs[i*n+i] >= unreachable) throw std::invalid_argument("pickup identity must be feasible");
        result.before += costs[i*n+i];
    }
    // A forbidden edge costs more than every feasible complete identity. All
    // arithmetic is int64 even when valid input costs approach INT_MAX.
    const long long forbidden = (static_cast<long long>(maximum) * (n + 1) + 1) * (n + 1) + 1;
    const long long infinity = 1LL << 60;
    std::vector<long long> u(n+1), v(n+1), minimum(n+1);
    std::vector<int> owner(n+1), previous(n+1);
    std::vector<char> used(n+1);
    for (int row = 1; row <= n; ++row) {
        check(); owner[0] = row; int column = 0;
        std::fill(minimum.begin(), minimum.end(), infinity);
        std::fill(used.begin(), used.end(), false);
        do {
            check(); used[column] = true;
            const int current_row = owner[column];
            long long delta = infinity; int next_column = -1;
            for (int j = 1; j <= n; ++j) if (!used[j]) {
                const int value = costs[(current_row-1)*n+j-1];
                const long long encoded = value == unreachable ? forbidden :
                    static_cast<long long>(value) * (n + 1) + (current_row != j);
                const long long reduced = encoded - u[current_row] - v[j];
                if (reduced < minimum[j]) { minimum[j] = reduced; previous[j] = column; }
                if (minimum[j] < delta) { delta = minimum[j]; next_column = j; }
            }
            if (next_column < 0 || delta == infinity) throw std::logic_error("pickup augmentation failed");
            for (int j = 0; j <= n; ++j) {
                if (used[j]) { u[owner[j]] += delta; v[j] -= delta; }
                else minimum[j] -= delta;
            }
            column = next_column;
        } while (owner[column]);
        do {
            const int last = previous[column]; owner[column] = owner[last]; column = last;
        } while (column);
    }
    for (int j = 1; j <= n; ++j) result.column[owner[j]-1] = j-1;
    for (int i = 0; i < n; ++i) {
        const int value = costs[i*n+result.column[i]];
        if (value >= unreachable) throw std::logic_error("pickup permutation selected a forbidden edge");
        result.after += value; result.changed += result.column[i] != i;
    }
    if (result.after > result.before) throw std::logic_error("pickup permutation lost feasible identity");
    check(); return result;
}

struct PickupPermutationCycle {
    std::vector<int> rows;
    long long before = 0, after = 0;
    bool accepted = false;
};

template<class Check>
std::vector<PickupPermutationCycle> pickup_permutation_cycles(const std::vector<int>& costs,
        const AssignmentPermutation& permutation, int unit_cost, Check check, int unit_limit = 16) {
    const int n = permutation.column.size();
    if (n > maximum_pickup_group || costs.size() != size_t(n)*n || unit_limit < 1 || unit_limit > 255 || unit_cost < 1 || unit_cost > unit_limit)
        throw std::invalid_argument("invalid pickup cycle input");
    std::vector<char> used(n), targets(n);
    for (int column : permutation.column) {
        if (column < 0 || column >= n || targets[column]) throw std::invalid_argument("pickup mapping is not a permutation");
        targets[column] = true;
    }
    std::vector<PickupPermutationCycle> result;
    for (int root = 0; root < n; ++root) if (!used[root]) {
        check(); PickupPermutationCycle cycle;
        for (int i = root; !used[i]; i = permutation.column[i]) {
            used[i] = true; cycle.rows.push_back(i);
            cycle.before += costs[i*n+i]; cycle.after += costs[i*n+permutation.column[i]];
        }
        if (cycle.rows.size() < 2) continue;
        const long long saving = cycle.before - cycle.after;
        cycle.accepted = saving >= 4LL * unit_cost && saving * 100 >= cycle.before * 10;
        result.push_back(std::move(cycle));
    }
    check(); return result;
}

// Optional complete-cycle veto. Column j is the TASK currently held by robot j;
// its new holder i must be compared with holder j, not with i's previous task.
// Only already-accepted cycles are inspected. Every rejected cycle stays at its
// original identity assignment, so independently accepted cycles remain valid.
struct PickupTierGuardStats {
    long long cycles = 0, rows = 0, rejected = 0, worse_rows = 0;
};

template<class Tier, class Check>
PickupTierGuardStats guard_pickup_cycle_tiers(std::vector<PickupPermutationCycle>& cycles,
        const AssignmentPermutation& permutation, Tier tier, Check check) {
    PickupTierGuardStats result;
    for (auto& cycle : cycles) if (cycle.accepted) {
        check(); ++result.cycles;
        bool worse = false;
        for (int row : cycle.rows) {
            check();
            const int column = permutation.column.at(row);
            const int before = tier(column, column), after = tier(row, column);
            if (before < 0 || before > 2 || after < 0 || after > 2)
                throw std::invalid_argument("invalid pickup cycle feasibility tier");
            ++result.rows;
            if (after > before) { worse = true; ++result.worse_rows; }
        }
        if (worse) { cycle.accepted = false; ++result.rejected; }
    }
    check(); return result;
}

// Read-only diagnostic: compare the same selected task set, optionally forbidding
// a new holder from worsening that task's original completion-feasibility tier.
// The task is the column: its reference is the column's diagonal, not the row's.
struct FreshPickupPermutationAudit {
    std::vector<PickupPermutationCycle> unrestricted, guarded;
    long long horizon_excluded_pairs = 0;
};

template<class Check>
FreshPickupPermutationAudit audit_fresh_pickup_permutation(const std::vector<int>& costs,
        const std::vector<int>& tiers, int n, int unreachable, int unit_cost, int unit_limit, Check check) {
    const auto ordinary = minimum_pickup_permutation(costs, n, unreachable, check);
    FreshPickupPermutationAudit result;
    result.unrestricted = pickup_permutation_cycles(costs, ordinary, unit_cost, check, unit_limit);
    if (!tiers.empty() && tiers.size() != costs.size())
        throw std::invalid_argument("invalid fresh pickup tier matrix");
    auto guarded = costs;
    for (int tier : tiers) if (tier < 0 || tier > 2)
        throw std::invalid_argument("invalid fresh pickup feasibility tier");
    if (!tiers.empty()) for (int row = 0; row < n; ++row) for (int column = 0; column < n; ++column) {
        check();
        const int index = row * n + column;
        if (guarded[index] < unreachable && tiers[index] > tiers[column * n + column]) {
            guarded[index] = unreachable;
            ++result.horizon_excluded_pairs;
        }
    }
    result.guarded = result.horizon_excluded_pairs ?
        pickup_permutation_cycles(guarded, minimum_pickup_permutation(guarded, n, unreachable, check),
                                  unit_cost, check, unit_limit) : result.unrestricted;
    check(); return result;
}
}  // namespace cgar
