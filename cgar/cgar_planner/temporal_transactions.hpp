#pragma once
// Bounded multi-owner repair of a complete temporal plan. Input reservations and
// selections are immutable; a result is published only after every move commits.
#include "temporal_pibt.hpp"
#include <memory>

namespace cgar {
struct TemporalTransactionOptions {
    int work = 0, roots = 256, per_root = 2048, max_agents = 8, max_owners = 2;
    int distance_scale = 50, unit_cost = 1;
};
struct TemporalTransactionStats {
    long long roots = 0, candidates = 0, accepted = 0, terminals = 0;
    long long rollbacks = 0, work_exhausted = 0, multi_owner_choices = 0;
    int max_agents = 0;
    double score_before = 0, score_after = 0;
};

class TemporalTransactionSearch {
    struct Change { int robot, selected; bool active, locked; };
public:
    TemporalTransactionSearch(int cells, const std::vector<std::vector<TemporalChoice>>& choices,
            const std::vector<char>& fixed, const std::vector<double>& power,
            const std::vector<int>& selected, const TemporalTransactionOptions& options)
        : choices_(choices), fixed_(fixed), power_(power), selected_(selected), options_(options),
          active_(selected.size(), false), locked_(selected.size(), false),
          cells_(cells), edges_(2 * cells) {
        if (options.work < 0 || options.work > 2000000 || options.roots < 1 || options.roots > 4096 ||
            options.per_root < 1 || options.per_root > 1000000 || options.max_agents < 1 || options.max_agents > 32 ||
            options.max_owners < 1 || options.max_owners > 2 || options.distance_scale < 1 || options.unit_cost < 1)
            throw std::invalid_argument("invalid temporal transaction limits");
        if (choices.size() != selected.size() || fixed.size() != selected.size() || power.size() != selected.size())
            throw std::invalid_argument("invalid temporal transaction input size");
        for (auto& row : cells_) row.fill(-1);
        for (auto& row : edges_) row.fill(-1);
        for (int r = 0; r < int(selected.size()); ++r) {
            if (selected[r] < 0 || selected[r] >= int(choices[r].size()) || !choices[r][selected[r]].path->valid)
                throw std::invalid_argument("invalid temporal transaction selection");
            apply(r, selected[r], true, false);
        }
        stats.score_before = stats.score_after = score_;
    }

    template<class Check>
    void run(const std::vector<int>& order, int timestep, Check check) {
        check();
        if (!options_.work || order.empty()) return;
        const size_t start = (uint64_t(std::max(0, timestep)) * options_.roots) % order.size();
        for (size_t offset = 0; offset < order.size() && stats.roots < options_.roots && stats.candidates < options_.work; ++offset) {
            const int r = order[(start + offset) % order.size()];
            if (r < 0 || r >= int(selected_.size())) throw std::invalid_argument("invalid temporal transaction root");
            const auto& old = choices_[r][selected_[r]];
            const int from = choices_[r][0].path->cells[0];
            if (fixed_[r] || power_[r] <= 0 || old.path->cells[0] != from) continue;
            check(); ++stats.roots; root_work_ = 0; before_ = score_;
            const int old_selected = selected_[r];
            const int64_t old_physical = old.cost + int64_t(old.operation) * options_.unit_cost;
            change(r, old_selected, false, false);
            bool accepted = false;
            for (int k = 1; k < int(choices_[r].size()) && !exhausted(); ++k) {
                consume(check);
                const auto& candidate = choices_[r][k];
                if (!candidate.path->valid || candidate.path->cells[0] == from || candidate.cost >= old.cost) continue;
                const int64_t physical = candidate.cost + int64_t(candidate.operation) * options_.unit_cost;
                if (old_physical - physical < int64_t(options_.distance_scale) * options_.unit_cost) continue;
                if (place(r, k, {}, 1, check)) { accepted = true; break; }
            }
            if (accepted) {
                if (active_count_ != int(selected_.size()) || score_ <= before_ + 1e-6)
                    throw std::logic_error("incomplete or non-improving temporal transaction");
                // Locks apply only to one transaction; preserve the committed
                // plan while allowing later roots to use its movable robots.
                for (const auto& entry : trail_) locked_[entry.robot] = false;
                trail_.clear(); ++stats.accepted;
            } else {
                undo(0); score_ = before_; ++stats.rollbacks;
                stats.work_exhausted += exhausted();
            }
        }
        stats.score_after = score_;
        check();
    }
    const std::vector<int>& selections() const { return selected_; }
    double score() const { return score_; }
    TemporalTransactionStats stats;

private:
    bool exhausted() const { return stats.candidates >= options_.work || root_work_ >= options_.per_root; }
    template<class Check> void consume(Check& check) {
        if ((stats.candidates & 63) == 0) check();
        ++stats.candidates; ++root_work_;
    }
    void reserve(int r, bool insert) {
        const auto& choice = choices_[r][selected_[r]];
        const auto& path = *choice.path;
        for (int t = 0; t < kTemporalHorizon; ++t) {
            auto update = [&](int& owner) {
                if (owner != (insert ? -1 : r)) throw std::logic_error("temporal transaction reservation ownership violated");
                owner = insert ? r : -1;
            };
            update(cells_.at(path.cells[t])[t]);
            if (path.edges[t] >= 0) update(edges_.at(path.edges[t])[t]);
        }
        score_ += (insert ? 1 : -1) * double(choices_[r][0].cost - choice.cost) * power_[r];
    }
    void apply(int r, int selected, bool active, bool locked) {
        if (active_[r]) { reserve(r, false); --active_count_; }
        selected_[r] = selected; active_[r] = active; locked_[r] = locked;
        if (active) { reserve(r, true); ++active_count_; }
    }
    void change(int r, int selected, bool active, bool locked) {
        trail_.push_back({r, selected_[r], bool(active_[r]), bool(locked_[r])});
        apply(r, selected, active, locked);
    }
    void undo(size_t checkpoint) {
        while (trail_.size() > checkpoint) {
            const auto entry = trail_.back(); trail_.pop_back();
            apply(entry.robot, entry.selected, entry.active, entry.locked);
        }
    }
    template<class Check>
    bool place(int r, int k, std::vector<int> pending, int moved, Check& check) {
        std::array<int, 2 * kTemporalHorizon> owners{}; int count = 0;
        auto take = [&](int owner) {
            if (owner < 0) return;
            for (int i = 0; i < count; ++i) if (owners[i] == owner) return;
            owners[count++] = owner;
        };
        const auto& path = *choices_[r][k].path;
        for (int t = 0; t < kTemporalHorizon; ++t) {
            take(cells_.at(path.cells[t])[t]);
            if (path.edges[t] >= 0) take(edges_.at(path.edges[t])[t]);
        }
        if (count > options_.max_owners || moved + count > options_.max_agents) return false;
        for (int i = 0; i < count; ++i) if (fixed_[owners[i]] || locked_[owners[i]]) return false;
        const size_t checkpoint = trail_.size(); const double old_score = score_;
        for (int i = 0; i < count; ++i) {
            const int other = owners[i];
            change(other, selected_[other], false, false); pending.push_back(other);
        }
        stats.multi_owner_choices += count == 2;
        stats.max_agents = std::max(stats.max_agents, moved + count);
        change(r, k, true, true);
        bool accepted = false;
        if (pending.empty()) {
            ++stats.terminals;
            if (active_count_ != int(selected_.size())) throw std::logic_error("temporal transaction lost a displaced robot");
            accepted = score_ > before_ + 1e-6;
        } else if (!exhausted()) accepted = resolve(std::move(pending), moved + count, check);
        if (!accepted) { undo(checkpoint); score_ = old_score; }
        return accepted;
    }
    template<class Check>
    bool resolve(std::vector<int> pending, int moved, Check& check) {
        const int r = pending.back(); pending.pop_back();
        if (active_[r] || locked_[r]) throw std::logic_error("temporal transaction duplicate pending owner");
        // Native candidates are already sorted. The wait seed remains available
        // last; it can differ from the displaced robot's previously selected path.
        for (int offset = 1; offset <= int(choices_[r].size()) && !exhausted(); ++offset) {
            const int k = offset == int(choices_[r].size()) ? 0 : offset;
            consume(check);
            if (choices_[r][k].path->valid && place(r, k, pending, moved, check)) return true;
        }
        return false;
    }
    const std::vector<std::vector<TemporalChoice>>& choices_;
    const std::vector<char>& fixed_;
    const std::vector<double>& power_;
    std::vector<int> selected_;
    TemporalTransactionOptions options_;
    std::vector<char> active_, locked_;
    std::vector<std::array<int, kTemporalHorizon>> cells_, edges_;
    std::vector<Change> trail_;
    int active_count_ = 0, root_work_ = 0;
    double score_ = 0, before_ = 0;
};

template<class Check>
std::unique_ptr<TemporalPibt> repair_temporal_transactions(int cells,
        const std::vector<std::vector<TemporalChoice>>& choices, const std::vector<char>& fixed,
        const std::vector<double>& power, int displacement_limit, const TemporalPibt& baseline,
        const TemporalTransactionOptions& options, const std::vector<int>& order, int timestep,
        TemporalTransactionStats& stats, Check check) {
    if (!options.work) return nullptr;
    check();
    TemporalTransactionSearch trial(cells, choices, fixed, power, baseline.selections(), options);
    trial.run(order, timestep, check); stats = trial.stats;
    if (!stats.accepted) return nullptr;
    auto result = std::make_unique<TemporalPibt>(cells, choices, fixed, power, displacement_limit, 0, &trial.selections());
    if (result->score() <= baseline.score() + 1e-6) throw std::logic_error("transaction result failed complete-score improvement");
    for (int r = 0; r < int(fixed.size()); ++r) if (fixed[r] && result->selected(r) != baseline.selected(r))
        throw std::logic_error("transaction changed a protected robot");
    check(); return result;
}
}  // namespace cgar
