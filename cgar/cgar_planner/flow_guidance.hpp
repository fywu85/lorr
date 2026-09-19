#pragma once
// Learn soft directional costs from executed movement, then freeze them once.
// No map identities, geometric lane rules, task forecasts, or wall-clock stops.
#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace cgar {
class FlowGuidance {
public:
    void initialize(const std::vector<char>& free, int rows, int cols,
                    int warmup, int strength, int minimum_samples) {
        if (rows < 1 || cols < 1 || free.size() != size_t(rows) * cols ||
            warmup < 1 || warmup > 4096 || strength < 1 || strength > 8 ||
            minimum_samples < 1 || minimum_samples > 65536)
            throw std::invalid_argument("invalid learned flow configuration");
        free_ = free; rows_ = rows; cols_ = cols; warmup_ = warmup;
        strength_ = strength; minimum_samples_ = minimum_samples;
        counts_.assign(free.size() * 4, 0); costs_.clear(); previous_.clear();
        last_step_ = -1; samples_ = 0; frozen_ = false;
        moves_ = 0; penalized_edges_ = 0;
    }

    // Returns true only when the complete frozen cost field is first ready.
    bool observe(int timestep, const std::vector<int>& locations) {
        if (frozen_ || timestep == last_step_) return false;
        if (timestep < last_step_) throw std::logic_error("flow observation time reversed");
        if (!previous_.empty() && previous_.size() != locations.size())
            throw std::logic_error("flow observation fleet changed");
        for (int u : locations) if (u < 0 || u >= int(free_.size()) || !free_[u])
            throw std::logic_error("invalid flow observation cell");
        if (last_step_ >= 0 && timestep == last_step_ + 1) {
            for (size_t r = 0; r < locations.size(); ++r) {
                const int u = previous_[r], v = locations[r];
                if (u == v) continue;
                for (int dir = 0; dir < 4; ++dir) if (neighbor(u, dir) == v) {
                    ++counts_[size_t(u) * 4 + dir]; ++moves_; break;
                }
            }
            ++samples_;
        }
        // A skipped observation never invents intermediate movement.
        previous_ = locations; last_step_ = timestep;
        if (samples_ < warmup_) return false;
        costs_.assign(free_.size() * 4, 1);
        for (int u = 0; u < int(free_.size()); ++u) if (free_[u])
            for (int dir = 0; dir < 4; ++dir) {
                const int v = neighbor(u, dir);
                if (v < 0 || !free_[v]) continue;
                const uint64_t forward = counts_[size_t(u) * 4 + dir];
                const uint64_t reverse = counts_[size_t(v) * 4 + (dir + 2) % 4];
                const uint64_t total = forward + reverse;
                if (total < uint64_t(minimum_samples_) || reverse <= forward) continue;
                // Penalize going against observed dominant flow. Equal or
                // unobserved traffic stays neutral; every edge stays usable.
                const int extra = int((strength_ * (reverse - forward) + total - 1) / total);
                costs_[size_t(u) * 4 + dir] += extra; ++penalized_edges_;
            }
        frozen_ = true;
        std::vector<uint64_t>().swap(counts_); std::vector<int>().swap(previous_);
        return true;
    }

    bool frozen() const { return frozen_; }
    const std::vector<uint8_t>& costs() const { return costs_; }
    int samples() const { return samples_; }
    uint64_t moves() const { return moves_; }
    int penalized_edges() const { return penalized_edges_; }

private:
    int neighbor(int u, int dir) const {
        if (dir == 0) return u % cols_ + 1 < cols_ ? u + 1 : -1;
        if (dir == 1) return u / cols_ + 1 < rows_ ? u + cols_ : -1;
        if (dir == 2) return u % cols_ > 0 ? u - 1 : -1;
        return u / cols_ > 0 ? u - cols_ : -1;
    }
    int rows_ = 0, cols_ = 0, warmup_ = 0, strength_ = 0, minimum_samples_ = 0;
    int last_step_ = -1, samples_ = 0, penalized_edges_ = 0;
    bool frozen_ = false;
    uint64_t moves_ = 0;
    std::vector<char> free_;
    std::vector<uint64_t> counts_;
    std::vector<uint8_t> costs_;
    std::vector<int> previous_;
};
}  // namespace cgar
