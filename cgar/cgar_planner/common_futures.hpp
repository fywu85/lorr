#pragma once
// Fixed common-continuation comparison of complete CGAR temporal proposals.
// Only a root's already validated first action is ever executed. The longer
// paths forecast known task chains; no future task assignment is invented.
#include "rolling_window.hpp"
#include <thread>

namespace cgar {
struct FutureOptions {
    int roots = 0, horizon = 15, branches = 4, threads = 4, noise = 50, regional_roots = 0, crowd_cost = 0;
};
struct FutureStats {
    bool completed = false;
    int selected = 0, evaluations = 0, batches = 0, changed_first = 0;
    int64_t incumbent_cost = 0, selected_cost = 0;
    int64_t incumbent_base = 0, selected_base = 0, incumbent_pairs = 0, selected_pairs = 0;
    uint64_t orders_fingerprint = 0;
    std::vector<int64_t> costs, base_costs, pairs;
};

// Count unordered robot pairs within Chebyshev distance two at the forecast
// endpoint. All robots count, including fixed/goal-less occupants. Walls do not
// alter this geometric statistic. O(grid cells + 12 * robots), no pairwise scan.
template<class Check>
int64_t future_terminal_pairs(int rows, int cols, const std::vector<WindowPath>& paths, Check check) {
    if (rows < 1 || cols < 1 || int64_t(rows) * cols > std::numeric_limits<int>::max())
        throw std::invalid_argument("invalid common-future crowd geometry");
    check(); std::vector<uint8_t> occupied(size_t(rows) * cols, 0);
    for (size_t r = 0; r < paths.size(); ++r) {
        if (!(r % 256)) check();
        if (paths[r].empty() || paths[r].back() < 0)
            throw std::invalid_argument("missing common-future terminal pose");
        const int cell = paths[r].back() / 4;
        if (size_t(cell) >= occupied.size() || occupied[cell])
            throw std::invalid_argument("invalid common-future terminal occupancy");
        occupied[cell] = 1;
    }
    int64_t pairs = 0;
    for (size_t r = 0; r < paths.size(); ++r) {
        if (!(r % 256)) check();
        const int cell = paths[r].back() / 4, row = cell / cols, col = cell % cols;
        for (int dr = 0; dr <= 2 && dr < rows - row; ++dr)
            for (int dc = -2; dc <= 2; ++dc) {
                if ((!dr && dc <= 0) || dc < -col || dc >= cols - col) continue;
                pairs += occupied[(row + dr) * cols + col + dc];
            }
    }
    check(); return pairs;
}

template<class Check>
FutureStats select_common_futures(const std::vector<WindowProblem>& roots,
                                  const TemporalGeometry& geometry,
                                  const std::vector<int>& order, int displacement_limit,
                                  const FutureOptions& options, int unit_cost,
                                  uint64_t seed, Check check) {
    if (roots.empty() || roots.size() != size_t(options.roots) || options.roots > 32 ||
        options.horizon < 10 || options.horizon > 30 || options.horizon % 5 ||
        options.branches < 1 || options.branches > 16 || options.threads < 1 ||
        options.threads > 32 || options.regional_roots < 0 || options.regional_roots > 1 || options.noise < 0 || options.noise > 1000000 ||
        options.crowd_cost < 0 || options.crowd_cost > 255 || unit_cost < 1)
        throw std::invalid_argument("invalid common-future work declaration");
    const int robots = roots[0].seed.size();
    if (order.size() != size_t(robots)) throw std::invalid_argument("common-future order size mismatch");
    std::vector<char> seen(robots, false);
    for (int r : order) {
        if (r < 0 || r >= robots || seen[r]) throw std::invalid_argument("common-future order is not a permutation");
        seen[r] = true;
    }
    for (const auto& root : roots) {
        check(); root.validate(root.seed, check);
        if (root.horizon != options.horizon || root.seed.size() != size_t(robots) ||
            root.fixed != roots[0].fixed || root.tasks != roots[0].tasks || root.oracle != roots[0].oracle ||
            root.rows != roots[0].rows || root.cols != roots[0].cols || root.free != roots[0].free ||
            root.forward != roots[0].forward || root.allowed != roots[0].allowed || root.entry_allowed != roots[0].entry_allowed ||
            root.locked_prefix != roots[0].locked_prefix || root.first_cells != roots[0].first_cells ||
            root.turn_cost != roots[0].turn_cost || root.wait_cost != roots[0].wait_cost)
            throw std::invalid_argument("common-future roots describe different problems");
        for (int r = 0; r < robots; ++r)
            if (root.seed[r][0] != roots[0].seed[r][0] || root.chains[r].goals != roots[0].chains[r].goals ||
                (root.fixed[r] && root.seed[r] != roots[0].seed[r]))
                throw std::invalid_argument("common-future root changed a start, chain or protection");
    }
    // Each branch has a common priority perturbation across every root and all
    // its projected chunks. Randomness does not depend on thread scheduling.
    std::mt19937_64 rng(seed);
    std::uniform_int_distribution<int> perturb(-options.noise, options.noise);
    std::vector<std::vector<int>> orders(options.branches, order);
    std::vector<uint64_t> branch_seeds(options.branches);
    FutureStats result; result.orders_fingerprint = 14695981039346656037ULL;
    for (int b = 0; b < options.branches; ++b) {
        check(); std::vector<int64_t> priority(robots, ChainPotential::infinity);
        for (int r = 0; r < robots; ++r) {
            const auto& root = roots[0];
            const int noise = b ? perturb(rng) : 0;
            if (!root.chains[r].goals.empty() && !root.fixed[r])
                priority[r] = root.oracle->value(root.chains[r], 0, root.seed[r][0] / 4, root.seed[r][0] % 4) + int64_t(noise) * unit_cost;
        }
        if (b) std::stable_sort(orders[b].begin(), orders[b].end(), [&](int a, int c) {
            return std::tie(priority[a], a) < std::tie(priority[c], c);
        });
        branch_seeds[b] = rng();
        for (int r : orders[b]) for (int byte = 0; byte < 4; ++byte)
            result.orders_fingerprint = (result.orders_fingerprint ^ ((uint32_t(r) >> (8 * byte)) & 255)) * 1099511628211ULL;
    }
    const int evaluations = options.roots * options.branches;
    std::vector<int64_t> costs(evaluations, -1), base_costs(evaluations, -1), pairs(evaluations, 0);
    std::vector<int> batches(evaluations, 0);
    std::vector<std::exception_ptr> errors(evaluations);
    std::atomic<int> next{0};
    auto work = [&] {
        for (;;) {
            const int id = next.fetch_add(1); if (id >= evaluations) return;
            try {
                const int r = id / options.branches, b = id % options.branches;
                auto forecast = roots[r];
                batches[id] = extend_window_seed(forecast, geometry, orders[b], displacement_limit, branch_seeds[b], check);
                // This is only a forecast: root actions and all protected paths
                // are immutable even if the continuation selects another route.
                for (int agent = 0; agent < robots; ++agent)
                    for (int t = 0; t <= 5; ++t)
                        if (forecast.seed[agent][t] != roots[r].seed[agent][t])
                            throw std::logic_error("common continuation changed its committed root prefix");
                check(); base_costs[id] = forecast.score(forecast.seed);
                if (options.crowd_cost) pairs[id] = future_terminal_pairs(forecast.rows, forecast.cols, forecast.seed, check);
                const int64_t limit = std::numeric_limits<int64_t>::max() / options.branches;
                if (base_costs[id] < 0 || (options.crowd_cost && pairs[id] > limit / options.crowd_cost))
                    throw std::overflow_error("common-future aggregate cost overflow");
                const int64_t extra = pairs[id] * options.crowd_cost;
                if (base_costs[id] > limit - extra)
                    throw std::overflow_error("common-future aggregate cost overflow");
                costs[id] = base_costs[id] + extra;
            } catch (...) { errors[id] = std::current_exception(); }
        }
    };
    std::vector<std::thread> threads;
    try { for (int t = 1; t < std::min(options.threads, evaluations); ++t) threads.emplace_back(work); }
    catch (...) { for (auto& t : threads) t.join(); throw; }
    work(); for (auto& t : threads) t.join();
    // No best-so-far fallback: an error in any declared branch fails the entry.
    for (const auto& error : errors) if (error) std::rethrow_exception(error);
    result.costs.assign(options.roots, 0);
    result.base_costs.assign(options.roots, 0); result.pairs.assign(options.roots, 0);
    for (int id = 0; id < evaluations; ++id) {
        if (costs[id] < 0 || batches[id] != options.horizon / 5 - 1)
            throw std::logic_error("incomplete common-future evaluation");
        result.costs[id / options.branches] += costs[id]; result.batches += batches[id];
        result.base_costs[id / options.branches] += base_costs[id]; result.pairs[id / options.branches] += pairs[id];
    }
    for (int r = 1; r < options.roots; ++r)
        if (result.costs[r] < result.costs[result.selected]) result.selected = r;
    result.incumbent_cost = result.costs[0]; result.selected_cost = result.costs[result.selected];
    result.incumbent_base = result.base_costs[0]; result.selected_base = result.base_costs[result.selected];
    result.incumbent_pairs = result.pairs[0]; result.selected_pairs = result.pairs[result.selected];
    for (int r = 0; r < robots; ++r)
        result.changed_first += roots[result.selected].seed[r][1] != roots[0].seed[r][1];
    check(); result.evaluations = evaluations; result.completed = true; return result;
}
} // namespace cgar
