#pragma once
// Persistent priority proposals, inspired by the independent RANDOM-05 solver.
// These order CGAR's existing complete temporal searches; they do not replace
// its reservations, primary protection, scoring, recovery or fixed work limits.
#include <algorithm>
#include <cstdint>
#include <random>
#include <stdexcept>
#include <vector>

namespace cgar {
struct TemporalPriorityBatch {
    std::vector<std::vector<int>> orders, offsets;
    int changed_orders = 0;
    bool reused = false;
};

class TemporalPriorityPortfolio {
public:
    void configure(int amplitude, bool persistent, int mutation_percent, uint64_t seed) {
        if (amplitude < 0 || amplitude > 1000000 || mutation_percent < 0 || mutation_percent > 100)
            throw std::invalid_argument("invalid temporal priority portfolio configuration");
        amplitude_ = amplitude; persistent_ = persistent; mutation_percent_ = mutation_percent;
        seed_ = seed; previous_.clear(); previous_step_ = -2;
    }

    template<class Deadline>
    TemporalPriorityBatch prepare(const std::vector<int>& priorities, const std::vector<int>& order,
                                  const std::vector<int>& goals, int workers, int unit_cost,
                                  int step, Deadline check) const {
        if (workers < 1 || unit_cost < 1 || step < 0 || priorities.size() != order.size() || goals.size() != order.size())
            throw std::invalid_argument("invalid temporal priority inputs");
        std::vector<char> seen(order.size(), false);
        for (int robot : order) {
            if (robot < 0 || size_t(robot) >= order.size() || seen[robot])
                throw std::invalid_argument("temporal priority order is not a permutation");
            seen[robot] = true;
        }
        TemporalPriorityBatch result;
        result.reused = persistent_ && previous_step_ + 1 == step && previous_.size() == order.size();
        const auto parent = result.reused ? previous_ : std::vector<int>(order.size(), 0);
        std::mt19937_64 rng(mix(seed_ ^ (uint64_t(uint32_t(step)) << 32) ^ 0xa0761d6478bd642fULL));
        std::uniform_int_distribution<int> noise(-amplitude_, amplitude_), percent(0, 99);
        result.orders.resize(workers); result.offsets.resize(workers);
        for (int worker = 0; worker < workers; ++worker) {
            check();
            auto& offsets = result.offsets[worker]; offsets = parent;
            // Re-evaluate the incumbent as worker0. Every fourth alternative
            // restarts globally; others mutate a fixed proportion of its vector.
            if (worker && amplitude_) for (size_t robot = 0; robot < order.size(); ++robot) {
                if (!(robot % 64)) check();
                if (worker % 4 == 0 || percent(rng) < mutation_percent_) offsets[robot] = noise(rng);
            }
            for (size_t robot = 0; robot < order.size(); ++robot) if (goals[robot] < 0) offsets[robot] = 0;
            auto& candidate = result.orders[worker]; candidate = order;
            if (amplitude_) std::stable_sort(candidate.begin(), candidate.end(), [&](int a, int b) {
                if ((goals[a] < 0) != (goals[b] < 0)) return goals[a] >= 0;
                const int64_t left = int64_t(priorities[a]) + int64_t(offsets[a]) * unit_cost;
                const int64_t right = int64_t(priorities[b]) + int64_t(offsets[b]) * unit_cost;
                return left != right ? left < right : a < b;
            });
            result.changed_orders += candidate != order;
            check();
        }
        return result;
    }

    void remember(const TemporalPriorityBatch& batch, int selected, int step) {
        if (selected < 0 || size_t(selected) >= batch.offsets.size())
            throw std::invalid_argument("invalid temporal priority winner");
        if (persistent_) { previous_ = batch.offsets[selected]; previous_step_ = step; }
    }

    static uint64_t fingerprint(const std::vector<int>& offsets) {
        uint64_t hash = 14695981039346656037ULL;
        for (int value : offsets) {
            const uint32_t bits = static_cast<uint32_t>(value);
            for (int byte = 0; byte < 4; ++byte) hash = (hash ^ ((bits >> (8 * byte)) & 255)) * 1099511628211ULL;
        }
        return hash;
    }

private:
    static uint64_t mix(uint64_t value) {
        value += 0x9e3779b97f4a7c15ULL;
        value = (value ^ (value >> 30)) * 0xbf58476d1ce4e5b9ULL;
        value = (value ^ (value >> 27)) * 0x94d049bb133111ebULL;
        return value ^ (value >> 31);
    }
    int amplitude_ = 0, mutation_percent_ = 30, previous_step_ = -2;
    bool persistent_ = false;
    uint64_t seed_ = 0;
    std::vector<int> previous_;
};
}  // namespace cgar
