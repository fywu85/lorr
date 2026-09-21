#pragma once
// Reuse only a complete prior plan's unexecuted suffix. Changed/protected paths
// return to the ordinary seed, along with every suffix their new reservations hit.
#include "temporal_geometry.hpp"

namespace cgar {
struct TemporalWarmStats {
    bool history_valid = false;
    int retained = 0, initial_resets = 0, collision_resets = 0;
};

// Carry only the next occupied cell of a complete previous plan. This is a
// one-action motion commitment; the remaining four actions are replanned. It
// does not require a suffix match. CGAR's current protected moves take priority.
struct TemporalMovePromiseStats {
    bool history_valid = false;
    int retained_forward = 0, retained_wait = 0, initial_resets = 0, collision_resets = 0;
};

class TemporalMovePromises {
public:
    void clear() { tick_ = -2; expected_.clear(); headings_.clear(); goals_.clear(); tasks_.clear(); pending_.clear(); }
    void remember(int tick, const std::vector<int>& expected, const std::vector<int>& headings,
                  const std::vector<int>& goals, const std::vector<int>& tasks, const std::vector<int>& pending) {
        if (headings.size() != expected.size() || goals.size() != expected.size() ||
            tasks.size() != expected.size() || pending.size() != expected.size())
            throw std::logic_error("invalid temporal move-promise history dimensions");
        tick_ = tick; expected_ = expected; headings_ = headings; goals_ = goals; tasks_ = tasks; pending_ = pending;
    }

    template<class Allowed, class Deadline>
    std::vector<int> prepare(int tick, int cells, int mode, const std::vector<int>& locations,
            const std::vector<int>& headings, const std::vector<int>& goals, const std::vector<int>& tasks,
            const std::vector<int>& forward, const std::vector<int>& baseline, const std::vector<char>& fixed,
            Allowed allowed, TemporalMovePromiseStats& stats, Deadline check) const {
        const int n = int(locations.size());
        if (mode < 1 || mode > 2 || headings.size() != locations.size() || goals.size() != locations.size() ||
            tasks.size() != locations.size() || forward.size() != locations.size() ||
            baseline.size() != locations.size() || fixed.size() != locations.size())
            throw std::logic_error("invalid temporal move-promise input dimensions");
        std::vector<int> result(n, -1);
        check();
        if (tick != tick_ + 1 || expected_.size() != locations.size()) return result;
        for (int r = 0; r < n; ++r) {
            if (!(r & 63)) check();
            if (locations[r] != expected_[r] || headings[r] != headings_[r]) return result;
        }
        stats.history_valid = true;
        std::vector<int> occupant(cells, -1), promised_owner(cells, -1), owner(cells, -1), queue;
        std::vector<char> keep(n, false);
        for (int r = 0; r < n; ++r) {
            check();
            const int from = locations[r], to = pending_[r], base = baseline[r];
            if (from < 0 || from >= cells || to < 0 || to >= cells || base < 0 || base >= cells ||
                (to != from && to != forward[r]) || (base != from && base != forward[r]) ||
                (!fixed[r] && base != from) || occupant[from] >= 0 || promised_owner[to] >= 0 || owner[base] >= 0)
                throw std::logic_error("invalid temporal move-promise geometry");
            occupant[from] = r; promised_owner[to] = r; owner[base] = r;
            keep[r] = !fixed[r] && goals[r] == goals_[r] && tasks[r] == tasks_[r] &&
                      (mode == 2 || to != from) && allowed(r, to);
            if (!keep[r]) { queue.push_back(r); ++stats.initial_resets; }
        }
        for (int r = 0; r < n; ++r) {
            const int prior = occupant[pending_[r]], base = occupant[baseline[r]];
            if ((pending_[r] != locations[r] && prior >= 0 && pending_[prior] == locations[r]) ||
                (baseline[r] != locations[r] && base >= 0 && baseline[base] == locations[r]))
                throw std::logic_error("temporal move-promise input edge swap");
        }
        // Both the old one-action promises and the protected/current-wait seeds
        // are jointly legal. Replacing a promise with its seed can conflict with
        // another old promise, which is then reset too. Each enters once.
        for (size_t head = 0; head < queue.size(); ++head) {
            check(); const int r = queue[head], target = baseline[r];
            auto reset = [&](int other) {
                if (other >= 0 && keep[other]) {
                    keep[other] = false; queue.push_back(other); ++stats.collision_resets;
                }
            };
            reset(promised_owner[target]);
            const int other = occupant[target];
            if (target != locations[r] && other >= 0 && pending_[other] == locations[r]) reset(other);
        }
        std::fill(owner.begin(), owner.end(), -1);
        for (int r = 0; r < n; ++r) {
            const int target = keep[r] ? pending_[r] : baseline[r];
            if (owner[target] >= 0) throw std::logic_error("temporal move-promise closure vertex conflict");
            owner[target] = r;
            if (keep[r]) {
                result[r] = target;
                if (target == locations[r]) ++stats.retained_wait; else ++stats.retained_forward;
            }
        }
        for (int r = 0; r < n; ++r) {
            const int target = keep[r] ? pending_[r] : baseline[r], other = occupant[target];
            if (target != locations[r] && other >= 0 &&
                (keep[other] ? pending_[other] : baseline[other]) == locations[r])
                throw std::logic_error("temporal move-promise closure edge swap");
        }
        check(); return result;
    }
private:
    int tick_ = -2;
    std::vector<int> expected_, headings_, goals_, tasks_, pending_;
};

class TemporalWarmStart {
public:
    void clear() { timestep_ = -2; paths_.clear(); operations_.clear(); goals_.clear(); orientations_.clear(); }

    static int shifted_operation(int operation) {
        static const auto shifted = [] {
            const auto& pool = TemporalGeometry::operations();
            std::vector<int> result(pool.size(), -1);
            for (size_t i = 0; i < pool.size(); ++i) {
                auto suffix = pool[i];
                for (int t = 0; t < 4; ++t) suffix[t] = suffix[t + 1];
                suffix[4] = 3;
                for (size_t j = 0; j < pool.size(); ++j) if (pool[j] == suffix) { result[i] = j; break; }
                if (result[i] < 0) throw std::logic_error("temporal operation pool is not suffix-closed");
            }
            return result;
        }();
        return shifted.at(operation);
    }

    void remember(int timestep, const TemporalPibt& plan, const std::vector<int>& goals,
                  const std::vector<int>& expected_orientations) {
        if (goals.size() != plan.selections().size() || goals.size() != expected_orientations.size())
            throw std::logic_error("invalid temporal warm history dimensions");
        timestep_ = timestep; goals_ = goals; orientations_ = expected_orientations;
        paths_.resize(goals.size()); operations_.resize(goals.size());
        for (size_t r = 0; r < goals.size(); ++r) {
            paths_[r] = *plan.choice(r).path;
            operations_[r] = plan.choice(r).operation;
        }
    }

    template<class Deadline>
    std::vector<int> selections(int timestep, int cells, const std::vector<int>& locations,
            const std::vector<int>& orientations, const std::vector<int>& goals,
            const std::vector<std::vector<TemporalChoice>>& choices, const std::vector<char>& fixed,
            TemporalWarmStats& stats, Deadline check, bool after_turn_only = false) const {
        const int count = static_cast<int>(choices.size());
        if (locations.size() != choices.size() || orientations.size() != choices.size() ||
            goals.size() != choices.size() || fixed.size() != choices.size())
            throw std::logic_error("invalid temporal warm input dimensions");
        if (timestep != timestep_ + 1 || paths_.size() != choices.size()) return {};
        for (int r = 0; r < count; ++r) {
            if ((r & 63) == 0) check();
            if (locations[r] != paths_[r].cells[0] || orientations[r] != orientations_[r]) return {};
        }
        stats.history_valid = true;
        std::vector<int> cell_owner(cells * kTemporalHorizon, -1), edge_owner(2 * cells * kTemporalHorizon, -1);
        std::vector<int> selected(count, 0), queue;
        std::vector<char> warm(count, false);
        for (int r = 0; r < count; ++r) {
            if ((r & 63) == 0) check();
            const int operation = shifted_operation(operations_[r]);
            if (!fixed[r] && goals[r] >= 0 && goals[r] == goals_[r] && operation != 0 &&
                (!after_turn_only || paths_[r].first_action == 1 || paths_[r].first_action == 2)) {
                for (int k = 1; k < static_cast<int>(choices[r].size()); ++k)
                    if (choices[r][k].operation == operation) { selected[r] = k; warm[r] = true; break; }
            }
            if (!warm[r]) { queue.push_back(r); ++stats.initial_resets; }
            for (int t = 0; t < kTemporalHorizon; ++t) {
                const int cell = paths_[r].cells[std::min(t + 1, kTemporalHorizon - 1)];
                const int edge = t + 1 < kTemporalHorizon ? paths_[r].edges[t + 1] : -1;
                if (cell < 0 || cell >= cells || edge < -1 || edge >= 2 * cells)
                    throw std::logic_error("invalid temporal warm history geometry");
                int& owner = cell_owner[cell * kTemporalHorizon + t];
                if (owner >= 0) throw std::logic_error("temporal warm history vertex conflict");
                owner = r;
                if (edge >= 0) {
                    int& edge_robot = edge_owner[edge * kTemporalHorizon + t];
                    if (edge_robot >= 0) throw std::logic_error("temporal warm history edge conflict");
                    edge_robot = r;
                }
                if (warm[r] && (choices[r][selected[r]].path->cells[t] != cell ||
                                choices[r][selected[r]].path->edges[t] != edge))
                    throw std::logic_error("shifted temporal operation changed its reservations");
            }
        }
        // Original suffixes are mutually compatible. Ordinary seeds are also
        // mutually compatible, including all supporting protected first moves.
        // Monotone closure removes every suffix conflicting with a seed, then
        // checks that robot's seed too. Each robot enters the queue at most once.
        for (size_t head = 0; head < queue.size(); ++head) {
            check();
            const auto& seed = *choices[queue[head]][0].path;
            auto reset = [&](int other) {
                if (other >= 0 && warm[other]) {
                    warm[other] = false; selected[other] = 0;
                    queue.push_back(other); ++stats.collision_resets;
                }
            };
            for (int t = 0; t < kTemporalHorizon; ++t) {
                reset(cell_owner[seed.cells[t] * kTemporalHorizon + t]);
                if (seed.edges[t] >= 0) reset(edge_owner[seed.edges[t] * kTemporalHorizon + t]);
            }
        }
        for (char kept : warm) stats.retained += kept;
        check();
        return selected;
    }

    // Keep the ordinary seed at index zero as the unchanged score baseline.
    // Promised robots start at a nonzero compatible suffix. Every TemporalPibt
    // searchable alternative (indices >= 1) has the same first action; successful searches
    // and their rollbacks therefore preserve it. Filtering preserves cost order
    // and operation IDs. The adapter also checks the final emitted action.
    // Branch transactions can search index zero and must remain disabled.
    template<class Deadline>
    static std::vector<int> constrain_first_actions(std::vector<std::vector<TemporalChoice>>& choices,
            std::vector<int>& selected, const std::vector<char>& fixed, Deadline check) {
        if (fixed.size() != choices.size() || (!selected.empty() && selected.size() != choices.size()))
            throw std::logic_error("invalid temporal promise dimensions");
        std::vector<int> promised(choices.size(), -1);
        if (selected.empty()) { check(); return promised; }
        for (size_t r = 0; r < choices.size(); ++r) {
            check();
            const int old = selected[r];
            if (old < 0 || old >= static_cast<int>(choices[r].size()) || (fixed[r] && old))
                throw std::logic_error("invalid temporal promise selection");
            if (!old) continue;
            const int action = choices[r][old].path->first_action;
            std::vector<TemporalChoice> allowed;
            allowed.reserve(choices[r].size()); allowed.push_back(choices[r][0]);
            int remapped = -1;
            for (int k = 1; k < static_cast<int>(choices[r].size()); ++k) {
                if (choices[r][k].path->first_action != action) continue;
                if (k == old) remapped = static_cast<int>(allowed.size());
                allowed.push_back(choices[r][k]);
            }
            if (remapped <= 0) throw std::logic_error("temporal promise lost its complete suffix");
            choices[r] = std::move(allowed); selected[r] = remapped; promised[r] = action;
        }
        check();
        return promised;
    }

private:
    int timestep_ = -2;
    std::vector<TemporalPath> paths_;
    std::vector<int> operations_, goals_, orientations_;
};
}  // namespace cgar
