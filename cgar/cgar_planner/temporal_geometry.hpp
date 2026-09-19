#pragma once
// Generic five-action pool from No Man's Sky; see LICENSE_NMS.
#include "temporal_pibt.hpp"
#include <sstream>
#include <string>

namespace cgar {
class TemporalGeometry {
public:
    static const std::vector<std::array<uint8_t, 5>>& operations() {
        static const auto value = [] {
            const char* text =
                "WWWWW RRWWF RRWFW RWWWF CWWWF RRFWW RRWFF RRFRF RRFCF RWWFW CWWFW RRFWF WWWWF RWFWW RWWFF CWFWW CWWFF RWFRF RWFCF CWFRF CWFCF RRFFW WWWFW RFRWF RFCWF RFWWW RWFWF CFRWF CFCWF CFWWW CWFWF RRFFF WWFWW WWWFF RFRFW RFCFW RFWWF RWFFW CFRFW CFCFW CFWWF CWFFW WWFRF WWFCF WFRWF WFCWF WFWWW WWFWF RFRFF RFCFF RFWFW RWFFF CFRFF CFCFF CFWFW CWFFF FRWWF FCWWF FWWWW WFRFW WFCFW WFWWF WWFFW RFFWW RFWFF CFFWW CFWFF FRWFW FCWFW RFFRF RFFCF CFFRF CFFCF FWWWF WFRFF WFCFF WFWFW WWFFF RFFWF CFFWF FRFWW FRWFF FCFWW FCWFF FRFRF FRFCF FCFRF FCFCF FWWFW WFFWW WFWFF RFFFW CFFFW WFFRF WFFCF FRFWF FCFWF FWFWW FWWFF WFFWF FWFRF FWFCF RFFFF CFFFF FRFFW FCFFW FFRWF FFCWF FFWWW FWFWF WFFFW FRFFF FCFFF FFRFW FFCFW FFWWF FWFFW WFFFF FFRFF FFCFF FFWFW FWFFF FFFWW FFWFF FFFRF FFFCF FFFWF FFFFW FFFFF";
            std::istringstream input(text); std::string word;
            std::vector<std::array<uint8_t, 5>> pool;
            while (input >> word) {
                if (word.size() != 5) throw std::logic_error("invalid temporal operation");
                std::array<uint8_t, 5> actions{};
                for (int t = 0; t < 5; ++t) actions[t] = word[t] == 'F' ? 0 : word[t] == 'R' ? 1 : word[t] == 'C' ? 2 : 3;
                pool.push_back(actions);
            }
            if (pool.size() != 129) throw std::logic_error("invalid temporal pool size");
            return pool;
        }();
        return value;
    }

    // Orient an ordinary wait seed without changing occupancy. The legacy
    // rule prefers a turn on equal distances and can oscillate between tied
    // headings. Strict mode retains the current heading unless one turn
    // strictly lowers the remaining distance; equal improving turns keep the
    // existing clockwise tie-break. Values are in the oracle's own cost units.
    static uint8_t wait_action(int wait, int right, int left, bool strict_improvement) {
        const int best = std::min({right, left, wait});
        if (strict_improvement && best == wait) return 3;
        return best == right ? 1 : best == left ? 2 : 3;
    }

    // This local approximation knows only the first useful turn, not future
    // corners. Keep it in unit-action units; a weighted surcharge would charge
    // unseen corner turns without matching potential credit.
    template<class Spatial, class Neighbor>
    static int fallback_distance(int cell, int direction, int unreachable, Spatial spatial, Neighbor neighbor) {
        const int d = spatial(cell);
        if (d == 0 || d >= unreachable) return d;
        int turns = 2;
        for (int dir = 0; dir < 4; ++dir) {
            const int to = neighbor(cell, dir);
            if (to < 0) continue;
            if (spatial(to) < d) {
                const int delta = (dir - direction + 4) % 4;
                turns = std::min(turns, delta == 3 ? 1 : delta);
            }
        }
        return d + turns;
    }

    template<class EdgeCost>
    static int forward_surcharge(const TemporalPath& path, int start, int goal, EdgeCost edge_cost, int unit_cost = 1) {
        int extra = 0, from = start;
        for (int to : path.cells) {
            if (to != from) extra += edge_cost(from, to) - unit_cost;
            if (to == goal) break;
            from = to;
        }
        return extra;
    }

    // All candidates consume five unit-time slots. Charge only the extra cost
    // of real or terminal-wait rotations, so a costly turn cannot manufacture
    // heuristic progress. Goal completion retains the native terminal reward.
    // unit_cost scales every physical slot and the operation tie term together;
    // changing units alone must multiply the whole score by that same factor.
    template<class Distance>
    static int64_t cost(const TemporalPath& path, int op, int goal, int turn_cost, Distance distance, int distance_scale = 50, int unit_cost = 1) {
        if (goal < 0) return int64_t(op) * unit_cost;
        const auto& actions = operations()[op];
        const int extra = turn_cost - unit_cost;
        int d = distance(path.cells[4], path.orientation);
        if (actions[4] == 3) {
            d = std::min({d, distance(path.cells[4], (path.orientation + 1) % 4) + extra,
                         distance(path.cells[4], (path.orientation + 3) % 4) + extra});
            if (actions[3] == 3)
                d = std::min(d, distance(path.cells[4], (path.orientation + 2) % 4) + 2 * extra);
        }
        int turns = 0, turns_to_goal = -1;
        for (int t = 0; t < 5; ++t) {
            const int action = op == 0 && t == 0 ? path.first_action : actions[t];
            turns += action == 1 || action == 2;
            if (path.cells[t] == goal) {
                if (turns_to_goal < 0) turns_to_goal = turns;
                d = -t * unit_cost;
            }
        }
        return int64_t(d + extra * (turns_to_goal < 0 ? turns : turns_to_goal)) * distance_scale - int64_t(op) * unit_cost;
    }

    template<class Deadline>
    void initialize(const std::vector<char>& free, int rows, int cols, Deadline check) {
        rows_ = rows; cols_ = cols; free_ = free; index_.assign(free.size() * 4, -1);
        int count = 0;
        for (size_t cell = 0; cell < free.size(); ++cell) if (free[cell])
            for (int d = 0; d < 4; ++d) index_[cell * 4 + d] = count++;
        paths_.resize(count);
        const auto& pool = operations();
        for (int cell = 0; cell < static_cast<int>(free.size()); ++cell) if (free[cell]) {
            check();
            for (int d = 0; d < 4; ++d) {
                auto& result = paths_[index_[cell * 4 + d]];
                for (int op = 0; op < 129; ++op) result[op] = make_path(cell, d, pool[op]);
            }
        }
    }

    const std::array<TemporalPath, 129>& paths(int cell, int orientation) const {
        return paths_.at(index_.at(cell * 4 + orientation));
    }

    TemporalPath seed(int cell, int orientation, int first_action) const {
        std::array<uint8_t, 5> actions{}; actions.fill(3); actions[0] = first_action;
        return make_path(cell, orientation, actions);
    }

private:
    int neighbor(int cell, int direction) const {
        if (direction == 0) return cell % cols_ + 1 < cols_ ? cell + 1 : -1;
        if (direction == 1) return cell / cols_ + 1 < rows_ ? cell + cols_ : -1;
        if (direction == 2) return cell % cols_ > 0 ? cell - 1 : -1;
        return cell / cols_ > 0 ? cell - cols_ : -1;
    }
    TemporalPath make_path(int cell, int orientation, const std::array<uint8_t, 5>& actions) const {
        TemporalPath path; path.first_action = actions[0]; path.edges.fill(-1); path.valid = true;
        int depth = 5;
        while (depth > 0 && actions[depth - 1] == 3) --depth;
        path.depth = std::max(3, depth);
        for (int t = 0; t < 5; ++t) {
            if (actions[t] == 0) {
                const int next = neighbor(cell, orientation);
                if (next < 0 || !free_[next]) { path.valid = false; return path; }
                path.edges[t] = 2 * std::min(cell, next) + (orientation % 2);
                cell = next;
            } else if (actions[t] == 1) orientation = (orientation + 1) % 4;
            else if (actions[t] == 2) orientation = (orientation + 3) % 4;
            path.cells[t] = cell;
        }
        path.orientation = orientation;
        return path;
    }
    int rows_ = 0, cols_ = 0;
    std::vector<char> free_;
    std::vector<int> index_;
    std::vector<std::array<TemporalPath, 129>> paths_;
};
}  // namespace cgar
