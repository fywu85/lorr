#pragma once
#include <array>
#include <vector>

namespace cgar {

struct PibtCandidates {
    std::array<int, 5> cells{};
    int size = 0;
};

// Shared destination-assignment recursion. Policy supplies admissible ordered
// candidates; reservations retain failed branches exactly as in ordinary PIBT.
// Both CGAR's normal policy and its reference compatibility policy use this code.
template<class Candidates, class Reserve>
bool assign_pibt(int robot, int parent, const std::vector<int>& locations,
                 const std::vector<int>& occupants, const std::vector<int>& reservations,
                 std::vector<int>& next, Candidates& candidates, Reserve& reserve) {
    const auto choices = candidates(robot);
    for (int k = 0; k < choices.size; ++k) {
        const int cell = choices.cells[k];
        if (reservations[cell] != -1 && reservations[cell] != robot) continue;
        if (parent >= 0 && cell == locations[parent]) continue;
        reserve(cell, robot);
        next[robot] = cell;
        if (cell == locations[robot]) return true;
        const int other = occupants[cell];
        if (other >= 0 && next[other] == -1 &&
            !assign_pibt(other, robot, locations, occupants, reservations, next, candidates, reserve)) continue;
        return true;
    }
    next[robot] = locations[robot];
    reserve(locations[robot], robot);
    return false;
}

}  // namespace cgar
