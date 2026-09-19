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
