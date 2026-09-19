#pragma once
// Learn soft directional costs from executed movement. Freeze once by default,
// or publish complete cumulative fields at fixed observation-count intervals.
// No map identities, geometric lane rules, task forecasts, or wall-clock stops.
#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

namespace cgar {
class FlowGuidance {
public:
    void initialize(const std::vector<char>& free, int rows, int cols,
                    int warmup, int strength, int minimum_samples, int minimum_margin_percent = 0,
                    int refresh_interval = 0) {
        if (rows < 1 || cols < 1 || free.size() != size_t(rows) * cols ||
            warmup < 1 || warmup > 4096 || strength < 1 || strength > 8 ||
            minimum_samples < 1 || minimum_samples > 65536 ||
            minimum_margin_percent < 0 || minimum_margin_percent > 100 ||
            refresh_interval < 0 || refresh_interval > 4096)
            throw std::invalid_argument("invalid learned flow configuration");
        free_ = free; rows_ = rows; cols_ = cols; warmup_ = warmup;
        strength_ = strength; minimum_samples_ = minimum_samples; minimum_margin_percent_ = minimum_margin_percent;
        counts_.assign(free.size() * 4, 0); costs_.clear(); previous_.clear();
        last_step_ = -1; samples_ = 0; frozen_ = false;
        moves_ = 0; penalized_edges_ = 0; refresh_interval_ = refresh_interval;
        publications_ = last_publication_samples_ = 0;
    }

    // Returns true only at a prescribed publication point after all edge costs
    // are complete. Refreshing retains cumulative observed counts; duplicate or
    // skipped observations never advance work or invent intervening movement.
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
        if (samples_ < warmup_ || (publications_ && samples_ - last_publication_samples_ < refresh_interval_)) return false;
        std::vector<uint8_t> next_costs(free_.size() * 4, 1);
        int next_penalized = 0;
        for (int u = 0; u < int(free_.size()); ++u) if (free_[u])
            for (int dir = 0; dir < 4; ++dir) {
                const int v = neighbor(u, dir);
                if (v < 0 || !free_[v]) continue;
                const uint64_t forward = counts_[size_t(u) * 4 + dir];
                const uint64_t reverse = counts_[size_t(v) * 4 + (dir + 2) % 4];
                const uint64_t total = forward + reverse;
                if (total < uint64_t(minimum_samples_) || reverse <= forward) continue;
                if ((reverse - forward) * 100 <= uint64_t(minimum_margin_percent_) * total) continue;
                // Penalize going against observed dominant flow. Equal or
                // unobserved traffic stays neutral; every edge stays usable.
                const int extra = int((strength_ * (reverse - forward) + total - 1) / total);
                next_costs[size_t(u) * 4 + dir] += extra; ++next_penalized;
            }
        costs_ = std::move(next_costs); penalized_edges_ = next_penalized;
        last_publication_samples_ = samples_; ++publications_;
        frozen_ = refresh_interval_ == 0;
        if (frozen_) { std::vector<uint64_t>().swap(counts_); std::vector<int>().swap(previous_); }
        return true;
    }

    bool frozen() const { return frozen_; }
    const std::vector<uint8_t>& costs() const { return costs_; }
    int samples() const { return samples_; }
    uint64_t moves() const { return moves_; }
    int penalized_edges() const { return penalized_edges_; }
    int minimum_margin_percent() const { return minimum_margin_percent_; }
    int refresh_interval() const { return refresh_interval_; }
    int publications() const { return publications_; }

private:
    int neighbor(int u, int dir) const {
        if (dir == 0) return u % cols_ + 1 < cols_ ? u + 1 : -1;
        if (dir == 1) return u / cols_ + 1 < rows_ ? u + cols_ : -1;
        if (dir == 2) return u % cols_ > 0 ? u - 1 : -1;
        return u / cols_ > 0 ? u - cols_ : -1;
    }
    int rows_ = 0, cols_ = 0, warmup_ = 0, strength_ = 0, minimum_samples_ = 0;
    int last_step_ = -1, samples_ = 0, penalized_edges_ = 0, minimum_margin_percent_ = 0;
    int refresh_interval_ = 0, publications_ = 0, last_publication_samples_ = 0;
    bool frozen_ = false;
    uint64_t moves_ = 0;
    std::vector<char> free_;
    std::vector<uint64_t> counts_;
    std::vector<uint8_t> costs_;
    std::vector<int> previous_;
};
}  // namespace cgar
