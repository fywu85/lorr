#pragma once
// Exact static guidance cost through an ordered, revealed task chain. Distances
// retain all four arrival headings. Services occur after actions, one per tick.
// This is a score oracle only; CGAR still owns reservations and protected paths.
#include <algorithm>
#include <array>
#include <atomic>
#include <cstdint>
#include <exception>
#include <limits>
#include <stdexcept>
#include <thread>
#include <vector>

namespace cgar {
class ChainPotential {
public:
    static constexpr int distance_inf = 1 << 29;
    static constexpr int64_t infinity = int64_t(1) << 50;
    struct Chain {
        std::vector<int> goals;
        std::vector<std::array<int64_t, 4>> tail;
    };
    size_t storage_bytes() const { return distances_.size() * sizeof(int); }
    int free_cells() const { return static_cast<int>(cells_.size()); }
    bool ready() const { return ready_; }

    template<class ForwardCost, class Check>
    void initialize(const std::vector<char>& free, const std::vector<char>& core,
                    const std::vector<int>& pocket, int rows, int cols,
                    ForwardCost forward_cost, int turn_cost, int wait_cost,
                    size_t max_bytes, int threads, Check check) {
        ready_ = false; distances_.clear();
        if (rows <= 0 || cols <= 0 || size_t(rows) * cols != free.size() ||
            free.size() > size_t(std::numeric_limits<int>::max()) ||
            core.size() != free.size() || pocket.size() != free.size() ||
            turn_cost < 1 || turn_cost > 255 || wait_cost < 1 || wait_cost > 255 ||
            !max_bytes || threads < 1 || threads > 32)
            throw std::invalid_argument("invalid chain potential graph, costs or limits");
        rows_ = rows; cols_ = cols; turn_cost_ = turn_cost; wait_cost_ = wait_cost;
        cells_.clear(); index_.assign(free.size(), -1);
        for (int u = 0; u < static_cast<int>(free.size()); ++u) if (free[u]) {
            index_[u] = static_cast<int>(cells_.size()); cells_.push_back(u);
        }
        const size_t n = cells_.size();
        // Four source headings and four destination headings per cell pair.
        if (n > size_t((distance_inf - 255) / (4 * 255)) ||
            (n && n > max_bytes / sizeof(int) / 16 / n))
            throw std::invalid_argument("complete chain heading tables exceed the configured memory limit");
        const int states = static_cast<int>(4 * n);
        std::vector<int> backward(states, -1), costs(states);
        int maximum = turn_cost;
        for (size_t u = 0; u < n; ++u) {
            check();
            for (int d = 0; d < 4; ++d) {
                const int predecessor = neighbor(cells_[u], (d + 2) % 4);
                if (predecessor >= 0 && index_[predecessor] >= 0)
                    backward[u * 4 + d] = index_[predecessor] * 4 + d;
                const int cost = forward_cost(cells_[u], d);
                if (cost < 1 || cost > 255) throw std::invalid_argument("invalid chain forward cost");
                costs[u * 4 + d] = cost; maximum = std::max(maximum, cost);
            }
        }
        check(); distances_.assign(n * n * 16, distance_inf); check();
        std::atomic<int> next{0};
        std::vector<std::exception_ptr> errors(threads);
        auto work = [&](int worker) {
            try {
                std::vector<int> dist(states);
                std::vector<std::vector<int>> buckets(maximum + 1);
                for (;;) {
                    const int goal = next.fetch_add(1);
                    if (goal >= static_cast<int>(n)) break;
                    check();
                    const int goal_pocket = pocket[cells_[goal]];
                    // One worker owns all four headings of a goal, avoiding
                    // false sharing in the interleaved lookup representation.
                    for (int heading = 0; heading < 4; ++heading) {
                        std::fill(dist.begin(), dist.end(), distance_inf);
                        for (auto& b : buckets) b.clear();
                        const int root = goal * 4 + heading;
                        dist[root] = 0; buckets[0].push_back(root);
                        size_t pending = 1, popped = 0;
                        int distance = 0, bucket = 0;
                        while (pending) {
                            if ((popped & 1023) == 0) check();
                            auto& current = buckets[bucket];
                            if (current.empty()) {
                                ++distance; if (++bucket > maximum) bucket = 0;
                                continue;
                            }
                            const int node = current.back(); current.pop_back(); --pending; ++popped;
                            if (dist[node] != distance) continue;
                            auto relax = [&](int from, int cost) {
                                if (from < 0 || distance + cost >= dist[from]) return;
                                dist[from] = distance + cost;
                                int slot = bucket + cost;
                                if (slot > maximum) slot -= maximum + 1;
                                buckets[slot].push_back(from); ++pending;
                            };
                            const int d = node % 4, cell = cells_[node / 4];
                            relax(node / 4 * 4 + (d + 1) % 4, turn_cost);
                            relax(node / 4 * 4 + (d + 3) % 4, turn_cost);
                            const int prev = backward[node];
                            if (prev >= 0) {
                                const int from = cells_[prev / 4];
                                // Enter only the core or the current goal's
                                // pocket. A robot already in another pocket may
                                // traverse it to exit, without admitting entry.
                                if (core[cell] || (goal_pocket >= 0 && pocket[cell] == goal_pocket) ||
                                    (pocket[from] >= 0 && pocket[cell] == pocket[from]))
                                    relax(prev, costs[prev]);
                            }
                        }
                        const size_t base = size_t(goal) * states * 4 + heading;
                        for (int state = 0; state < states; ++state)
                            distances_[base + size_t(state) * 4] = dist[state];
                    }
                }
            } catch (...) { errors[worker] = std::current_exception(); }
        };
        std::vector<std::thread> workers;
        try { for (int t = 1; t < threads; ++t) workers.emplace_back(work, t); }
        catch (...) { for (auto& t : workers) t.join(); throw; }
        work(0); for (auto& t : workers) t.join();
        for (const auto& error : errors) if (error) std::rethrow_exception(error);
        check(); ready_ = true;
    }

    template<class Check>
    Chain make_chain(const std::vector<int>& goals, size_t next, Check check) const {
        if (!ready_ || next > goals.size()) throw std::invalid_argument("chain potential is unavailable or stage is invalid");
        Chain result; result.goals.assign(goals.begin() + next, goals.end());
        result.tail.resize(result.goals.size(), {0, 0, 0, 0});
        for (int goal : result.goals)
            if (goal < 0 || goal >= static_cast<int>(index_.size()) || index_[goal] < 0)
                throw std::invalid_argument("chain errand is not a free cell");
        for (int stage = static_cast<int>(result.goals.size()) - 2; stage >= 0; --stage) {
            check();
            for (int h = 0; h < 4; ++h)
                result.tail[stage][h] = value(result, stage + 1, result.goals[stage], h);
        }
        return result;
    }

    int64_t value(const Chain& chain, size_t stage, int cell, int heading) const {
        if (!ready_) throw std::logic_error("incomplete chain potential lookup");
        if (stage >= chain.goals.size()) return 0;
        if (cell < 0 || cell >= static_cast<int>(index_.size()) || index_[cell] < 0 || heading < 0 || heading > 3)
            return infinity;
        const auto& tail = chain.tail.at(stage);
        const int goal = chain.goals[stage];
        if (goal == cell) {
            // Merely starting on an errand is not service. A wait or either
            // quarter-turn services it after one action. Leaving and returning
            // cannot beat these on a positive-cost grid with on-cell turns.
            return std::min({plus(wait_cost_, tail[heading]),
                             plus(turn_cost_, tail[(heading + 1) % 4]),
                             plus(turn_cost_, tail[(heading + 3) % 4])});
        }
        const size_t base = size_t(index_[goal]) * cells_.size() * 16 + size_t(index_[cell] * 4 + heading) * 4;
        int64_t best = infinity;
        for (int h = 0; h < 4; ++h) {
            const int distance = distances_[base + h];
            if (distance < distance_inf) best = std::min(best, plus(distance, tail[h]));
        }
        return best;
    }

    template<class Path>
    static size_t advance(const Chain& chain, const Path& path) {
        size_t stage = 0;
        for (int cell : path.cells)
            if (stage < chain.goals.size() && cell == chain.goals[stage]) ++stage;
        return stage;
    }

private:
    static int64_t plus(int64_t a, int64_t b) { return a >= infinity || b >= infinity || a > infinity - b ? infinity : a + b; }
    int neighbor(int cell, int d) const {
        if (d == 0) return cell % cols_ + 1 < cols_ ? cell + 1 : -1;
        if (d == 1) return cell / cols_ + 1 < rows_ ? cell + cols_ : -1;
        if (d == 2) return cell % cols_ > 0 ? cell - 1 : -1;
        return cell / cols_ > 0 ? cell - cols_ : -1;
    }
    bool ready_ = false;
    int rows_ = 0, cols_ = 0, turn_cost_ = 1, wait_cost_ = 1;
    std::vector<int> index_, cells_, distances_;
};
} // namespace cgar
