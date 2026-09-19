#pragma once
// Reuse only a complete prior plan's unexecuted suffix. Changed/protected paths
// return to the ordinary seed, along with every suffix their new reservations hit.
#include "temporal_geometry.hpp"

namespace cgar {
struct TemporalWarmStats {
    bool history_valid = false;
    int retained = 0, initial_resets = 0, collision_resets = 0;
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
            TemporalWarmStats& stats, Deadline check) const {
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
            if (!fixed[r] && goals[r] >= 0 && goals[r] == goals_[r] && operation != 0) {
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

private:
    int timestep_ = -2;
    std::vector<TemporalPath> paths_;
    std::vector<int> operations_, goals_, orientations_;
};
}  // namespace cgar
