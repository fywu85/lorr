#pragma once
// Obstacle-free physical lower bound. Ignore turns, other robots and certificates;
// each distinct-cell leg needs at least its Manhattan travel, and a repeated
// service location still needs one simulator tick. No map or task-stream lookup.
#include <algorithm>
#include <cstdlib>
#include <stdexcept>
#include <vector>

namespace cgar {
template<class Check>
long long manhattan_service_bound(int from, const std::vector<int>& stops,
                                 int next_stop, int columns, Check check) {
    if (from < 0 || columns < 1 || next_stop < 0 || size_t(next_stop) > stops.size())
        throw std::invalid_argument("invalid geometric task bound inputs");
    long long bound = 0;
    for (size_t k = next_stop; k < stops.size(); ++k) {
        check();
        const int to = stops[k];
        if (to < 0) throw std::invalid_argument("negative task stop in geometric bound");
        const long long distance = std::abs(from / columns - to / columns) +
                                   static_cast<long long>(std::abs(from % columns - to % columns));
        bound += std::max(1LL, distance);
        from = to;
    }
    return bound;
}
}  // namespace cgar
