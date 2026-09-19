#pragma once
// Generic temporal PIBT construction and fixed-count repair, adapted from the
// archived No Man's Sky PIBTS implementation. See LICENSE_NMS in this directory.
// Geometry and candidate scores are supplied independently of the search policy.
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <random>
#include <stdexcept>
#include <vector>

namespace cgar {
constexpr int kTemporalHorizon = 5;

struct TemporalPath {
    std::array<int, kTemporalHorizon> cells{};
    std::array<int, kTemporalHorizon> edges{};  // undirected edge, or -1 for a stationary action
    uint8_t orientation = 0, first_action = 3, depth = 3;
    bool valid = false;
};

struct TemporalChoice {
    const TemporalPath* path = nullptr;
    int64_t cost = 0;
    int operation = 0;
};

struct TemporalStats {
    long long roots = 0, accepted = 0, recursive_calls = 0, candidates = 0;
    long long budget_exhausted = 0, repairs = 0, repairs_accepted = 0;
    long long repair_batches_kept = 0, repair_batches_reverted = 0;
    int max_depth = 0;
};

class TemporalPibt {
public:
    TemporalPibt(int cells, const std::vector<std::vector<TemporalChoice>>& choices,
                 const std::vector<char>& fixed, const std::vector<double>& power,
                 int displacement_limit, uint64_t seed, const std::vector<int>* initial = nullptr,
                 const std::vector<std::vector<int>>* choice_regions = nullptr, int region = -1)
        : choices_(choices), fixed_(fixed), power_(power), limit_(displacement_limit), rng_(seed),
          selected_(initial ? *initial : std::vector<int>(choices.size(), 0)), visited_(choices.size(), 0),
          used_cells_(cells), used_edges_(2 * cells), choice_regions_(choice_regions), region_(region) {
        if (selected_.size() != choices.size()) throw std::logic_error("invalid temporal initial size");
        for (auto& x : used_cells_) x.fill(-1);
        for (auto& x : used_edges_) x.fill(-1);
        for (int r = 0; r < static_cast<int>(choices_.size()); ++r) {
            if (selected_[r] < 0 || selected_[r] >= static_cast<int>(choices_[r].size()) ||
                !choice(r).path->valid) throw std::logic_error("invalid temporal seed");
            if (blocker(r) != -1) throw std::logic_error("conflicting temporal seed");
            add(r);
        }
    }

    template<class Deadline>
    void construct(const std::vector<int>& order, Deadline check) {
        for (int r : order) {
            check();
            if (fixed_[r] || selected_[r] != 0) continue;
            ++stats.roots;
            if (attempt(r, false, check)) ++stats.accepted;
        }
    }

    template<class Deadline>
    void repair(int steps, Deadline check, long long candidate_limit = 0, const std::vector<int>* roots = nullptr,
                int temperature_ppm = 1000) {
        if (temperature_ppm < 0 || temperature_ppm > 1000000)
            throw std::invalid_argument("temporal repair temperature must be in [0,1000000] ppm");
        if (roots && roots->empty()) return;
        double best_score = score_;
        auto best = selected_;
        temperature_ = static_cast<double>(temperature_ppm) / 1000000.0;
        // A positive limit prescribes a deterministic amount of candidate work,
        // including construction. Check only between complete repair attempts:
        // the last attempt may overshoot, and every root has already finished.
        for (int k = 0; k < steps && (!candidate_limit || stats.candidates < candidate_limit); ++k) {
            check();
            if (k && k % 128 == 0) horizon_ = random_int(3, 5);
            const int draw = random_int(0, static_cast<int>(roots ? roots->size() : choices_.size()) - 1);
            const int r = roots ? (*roots)[draw] : draw;
            ++stats.repairs;
            if (!fixed_[r] && attempt(r, true, check)) ++stats.repairs_accepted;
            temperature_ *= 0.999;
        }
        // NMS compares the final search state to construction. Keep the same
        // rule; do not let wall-clock time choose a different stopping point.
        if (score_ <= best_score + 1e-6) {
            ++stats.repair_batches_reverted;
            for (int r = 0; r < static_cast<int>(choices_.size()); ++r) remove(r);
            selected_ = std::move(best); score_ = 0;
            for (int r = 0; r < static_cast<int>(choices_.size()); ++r) add(r);
        } else ++stats.repair_batches_kept;
    }

    const TemporalChoice& choice(int r) const { return choices_[r][selected_[r]]; }
    int selected(int r) const { return selected_[r]; }
    const std::vector<int>& selections() const { return selected_; }
    double score() const { return score_; }
    TemporalStats stats;

private:
    double random_real() { return static_cast<double>(std::uniform_int_distribution<uint64_t>{}(rng_)) / UINT64_MAX; }
    int random_int(int a, int b) { return static_cast<int>(std::uniform_int_distribution<int64_t>(a, b)(rng_)); }
    int blocker(int r) const {
        int answer = -1;
        auto take = [&](int other) {
            if (other >= 0) {
                if (answer >= 0 && answer != other) return false;
                answer = other;
            }
            return true;
        };
        const auto& path = *choice(r).path;
        for (int t = 0; t < kTemporalHorizon; ++t)
            if (!take(used_cells_[path.cells[t]][t])) return -2;
        for (int t = 0; t < kTemporalHorizon; ++t)
            if (path.edges[t] >= 0 && !take(used_edges_[path.edges[t]][t])) return -2;
        return answer;
    }
    void reserve(int r, bool insert) {
        const auto& path = *choice(r).path;
        for (int t = 0; t < kTemporalHorizon; ++t) {
            used_cells_[path.cells[t]][t] = insert ? r : -1;
            if (path.edges[t] >= 0) used_edges_[path.edges[t]][t] = insert ? r : -1;
        }
        const auto gain = choices_[r][0].cost - choice(r).cost;
        score_ += (insert ? 1 : -1) * static_cast<double>(gain) * power_[r];
    }
    void add(int r) { reserve(r, true); }
    void remove(int r) { reserve(r, false); }
    bool consider() {
        if (old_score_ - 1e-6 <= score_) return true;
        const double draw = random_real();  // construction also consumes this draw
        return temperature_ > 0 && old_score_ > 0 &&
               draw < std::exp(-((old_score_ - score_) / old_score_) / temperature_);
    }
    template<class Deadline>
    int build(int r, int depth, int& counter, bool repairing, Deadline& check) {
        ++stats.recursive_calls;
        stats.max_depth = std::max(stats.max_depth, depth);
        if ((counter & 15) == 0) check();
        if (counter > (repairing ? std::min(limit_, 1000) : limit_)) { ++stats.budget_exhausted; return 2; }
        visited_[r] = generation_;
        const int old = selected_[r];
        // Choices are sorted by the native terminal score, excluding the wait
        // seed at index zero. A failed ordinary wait remains displaceable.
        for (int k = 1; k < static_cast<int>(choices_[r].size()); ++k) {
            if (choices_[r][k].path->depth > horizon_ ||
                (choice_regions_ && (*choice_regions_)[r][k] != region_)) continue;
            ++stats.candidates;
            selected_[r] = k;
            const int other = blocker(r);
            if (other == -1) {
                add(r);
                if (consider()) return 1;
                remove(r); selected_[r] = old; return 2;
            }
            if (other < 0 || fixed_[other]) continue;
            if (repairing) {
                if (visited_[other] == generation_ || random_real() < 0.2) continue;
            } else {
                if (counter > 3000 && depth >= 6) continue;
                if (selected_[other] != 0 && random_real() < 0.8) continue;
            }
            remove(other); add(r);
            const int result = build(other, depth + 1, ++counter, repairing, check);
            if (result == 1) return 1;
            remove(r); add(other);
            if (result == 2) { selected_[r] = old; return 2; }
        }
        selected_[r] = old; visited_[r] = 0; return 0;
    }
    template<class Deadline>
    bool attempt(int r, bool repairing, Deadline& check) {
        ++generation_; old_score_ = score_; remove(r);
        int counter = 0;
        const int result = build(r, 0, counter, repairing, check);
        if (result != 1) add(r);
        return result == 1;
    }
    const std::vector<std::vector<TemporalChoice>>& choices_;
    const std::vector<char>& fixed_;
    const std::vector<double>& power_;
    int limit_, horizon_ = 3;
    std::mt19937_64 rng_;
    std::vector<int> selected_;
    std::vector<uint64_t> visited_;
    std::vector<std::array<int, kTemporalHorizon>> used_cells_, used_edges_;
    const std::vector<std::vector<int>>* choice_regions_ = nullptr;
    int region_ = -1;
    uint64_t generation_ = 0;
    double score_ = 0, old_score_ = 0, temperature_ = 0;
};

}  // namespace cgar
