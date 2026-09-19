#pragma once
// Complete positive-integer shortest paths from one observed (cell, heading).
// Reusable worker scratch; a result becomes usable only after the caller joins
// every field builder. No partial field is a successful result.
#include <array>
#include <limits>
#include <stdexcept>
#include <vector>

namespace cgar {
struct FullPickupField {
    std::vector<int> distance;
    long long pops = 0, states = 0;
};

class CompletePickupSearch {
public:
    template<class Neighbor, class Allowed, class ForwardCost, class Check>
    void run(int cells, int start, int heading, int turn_cost, int infinity,
             Neighbor neighbor, Allowed allowed, ForwardCost forward_cost,
             Check check, FullPickupField& output) {
        check();
        if (cells < 1 || infinity <= 16 || cells > (infinity - 16) / 64 ||
            cells > std::numeric_limits<int>::max() / 4 || start < 0 || start >= cells ||
            heading < 0 || heading > 3 || turn_cost < 1 || turn_cost > 16 || !allowed(start))
            throw std::invalid_argument("invalid complete pickup search");
        distance_.assign(size_t(cells) * 4, infinity);
        settled_.assign(size_t(cells) * 4, 0);
        for (auto& bucket : buckets_) bucket.clear();
        output.distance.assign(cells, infinity); output.pops = output.states = 0;
        int pending = 0;
        auto offer = [&](int state, int cost) {
            if (cost >= distance_[state]) return;
            distance_[state] = cost;
            buckets_[cost % 17].push_back(state); ++pending;
        };
        offer(start * 4 + heading, 0);
        int current = 0;
        while (pending) {
            if ((output.pops & 1023) == 0) check();
            while (buckets_[current % 17].empty()) ++current;
            auto& bucket = buckets_[current % 17];
            const int state = bucket.back(); bucket.pop_back(); --pending; ++output.pops;
            if (settled_[state] || distance_[state] != current) continue;
            settled_[state] = 1; ++output.states;
            const int cell = state / 4, direction = state % 4;
            if (output.distance[cell] == infinity) output.distance[cell] = current;
            offer(cell * 4 + (direction + 1) % 4, current + turn_cost);
            offer(cell * 4 + (direction + 3) % 4, current + turn_cost);
            const int next = neighbor(cell, direction);
            if (next >= 0 && next < cells && allowed(next)) {
                const int edge = forward_cost(cell, direction);
                if (edge < 1 || edge > 16) throw std::invalid_argument("invalid complete pickup edge cost");
                offer(next * 4 + direction, current + edge);
            }
        }
        check();
    }
private:
    std::vector<int> distance_;
    std::vector<char> settled_;
    std::array<std::vector<int>, 17> buckets_;
};
} // namespace cgar
