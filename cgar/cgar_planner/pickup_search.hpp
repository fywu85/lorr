#pragma once
// Fixed-work forward shortest paths over (cell, heading). Only settled cell
// minima reach the caller; an exception fails the enclosing schedule/entry.
#include <algorithm>
#include <cstdint>
#include <functional>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace cgar {
struct PickupSearchStats {
    int pops = 0, states = 0, cells = 0;
    bool stopped = false, limited = false;
};

class OrientedPickupSearch {
public:
    template<class Neighbor, class Allowed, class ForwardCost, class Visit, class Check>
    PickupSearchStats run(int cells, int start, int heading, int max_pops, int turn_cost,
                          Neighbor neighbor, Allowed allowed, ForwardCost forward_cost,
                          Visit visit, Check check) {
        check();
        if (cells < 1 || cells > std::numeric_limits<int>::max() / 4 || start < 0 ||
            start >= cells || heading < 0 || heading > 3 || max_pops < 1 || max_pops > 65536 ||
            turn_cost < 1 || turn_cost > 16 || !allowed(start))
            throw std::invalid_argument("invalid oriented pickup search");
        if (cell_seen_.size() != size_t(cells)) {
            distance_.resize(size_t(cells) * 4);
            seen_.assign(size_t(cells) * 4, 0); cell_seen_.assign(cells, 0);
            generation_ = 0;
        }
        if (++generation_ == 0) {
            std::fill(seen_.begin(), seen_.end(), 0);
            std::fill(cell_seen_.begin(), cell_seen_.end(), 0); generation_ = 1;
        }
        heap_.clear();
        auto offer = [&](int state, int cost) {
            if (seen_[state] == generation_ && distance_[state] <= cost) return;
            seen_[state] = generation_; distance_[state] = cost;
            heap_.emplace_back(cost, state);
            std::push_heap(heap_.begin(), heap_.end(), std::greater<Item>());
        };
        offer(start * 4 + heading, 0);
        PickupSearchStats stats;
        while (!heap_.empty() && stats.pops < max_pops) {
            if ((stats.pops & 63) == 0) check();
            std::pop_heap(heap_.begin(), heap_.end(), std::greater<Item>());
            const auto item = heap_.back(); heap_.pop_back(); ++stats.pops;
            const int cost = item.first, state = item.second;
            if (distance_[state] != cost) continue;
            ++stats.states;
            const int cell = state / 4, direction = state % 4;
            if (cell_seen_[cell] != generation_) {
                cell_seen_[cell] = generation_; ++stats.cells;
                if (visit(cell, cost)) { stats.stopped = true; break; }
            }
            offer(cell * 4 + (direction + 1) % 4, cost + turn_cost);
            offer(cell * 4 + (direction + 3) % 4, cost + turn_cost);
            const int next = neighbor(cell, direction);
            if (next >= 0 && next < cells && allowed(next)) {
                const int edge = forward_cost(cell, direction);
                if (edge < 1 || edge > 16) throw std::invalid_argument("invalid pickup edge cost");
                offer(next * 4 + direction, cost + edge);
            }
        }
        stats.limited = !stats.stopped && !heap_.empty() && stats.pops == max_pops;
        check();
        return stats;
    }
private:
    using Item = std::pair<int, int>;
    std::vector<Item> heap_;
    std::vector<int> distance_;
    std::vector<uint32_t> seen_, cell_seen_;
    uint32_t generation_ = 0;
};
}  // namespace cgar
