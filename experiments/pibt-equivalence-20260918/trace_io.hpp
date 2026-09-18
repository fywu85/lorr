#pragma once
#include <algorithm>
#include <chrono>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "nlohmann/json.hpp"

namespace traceio {
using Json = nlohmann::json;
using Clock = std::chrono::steady_clock;
struct Input {
    Json source;
    int rows, cols, steps, seed;
    bool rotation;
    std::vector<int> map, positions, orientations, goals;
    explicit Input(const std::string& path) {
        std::ifstream(path) >> source;
        rows = source.at("rows"); cols = source.at("cols");
        steps = source.value("steps", 256); seed = source.value("seed", 0);
        rotation = source.value("model", std::string("native")) == "rotation";
        map = source.at("map").get<std::vector<int>>();
        goals = source.at("goals").get<std::vector<int>>();
        for (const auto& state : source.at("states")) {
            positions.push_back(state.at(0)); orientations.push_back(state.at(1));
        }
        if (map.size() != static_cast<size_t>(rows * cols) || positions.size() != goals.size())
            throw std::runtime_error("invalid input dimensions");
        // Native PIBT has a fixed goal for every robot. Idle LoRR robots receive
        // their snapshot position as a fixed goal for this conformance probe.
        for (size_t i = 0; i < goals.size(); ++i) if (goals[i] < 0) goals[i] = positions[i];
    }
};
inline int neighbor(int p, int d, int rows, int cols) {
    if (d == 0) return p % cols + 1 < cols ? p + 1 : -1;
    if (d == 1) return p / cols + 1 < rows ? p + cols : -1;
    if (d == 2) return p % cols > 0 ? p - 1 : -1;
    return p / cols > 0 ? p - cols : -1;
}
inline void validate(const Input& in, const std::vector<int>& now, const std::vector<int>& next) {
    if (now.size() != next.size()) throw std::runtime_error("wrong agent count");
    std::vector<int> old(in.map.size(), -1), used(in.map.size(), -1);
    for (size_t i = 0; i < now.size(); ++i) old.at(now[i]) = i;
    for (size_t i = 0; i < next.size(); ++i) {
        const int v = next[i];
        if (v < 0 || v >= static_cast<int>(in.map.size()) || in.map[v] || used[v] >= 0)
            throw std::runtime_error("vertex or obstacle collision");
        bool adjacent = v == now[i];
        for (int d = 0; d < 4; ++d) adjacent |= neighbor(now[i], d, in.rows, in.cols) == v;
        if (!adjacent) throw std::runtime_error("nonadjacent destination");
        const int j = old[v];
        if (j >= 0 && j != static_cast<int>(i) && next[j] == now[i]) throw std::runtime_error("edge swap");
        used[v] = i;
    }
}
// Independent LoRR adapter for the upstream reference. Upstream proposes a legal
// joint spatial move. Turns stay in place; cancel all forward predecessors of
// those stationary robots to a fixed point. Codes follow LoRR: FW, CR, CCR, W.
inline std::vector<int> rotate(const Input& in, const std::vector<int>& now,
                               const std::vector<int>& ori, const std::vector<int>& targets) {
    std::vector<int> actions(now.size(), 3), occupant(in.map.size(), -1);
    for (size_t i = 0; i < now.size(); ++i) occupant[now[i]] = i;
    for (size_t i = 0; i < now.size(); ++i) {
        if (targets[i] == now[i]) continue;
        int direction = -1;
        for (int d = 0; d < 4; ++d) if (neighbor(now[i], d, in.rows, in.cols) == targets[i]) direction = d;
        if (direction < 0) throw std::runtime_error("invalid reference target");
        const int delta = (direction - ori[i] + 4) % 4;
        actions[i] = delta == 0 ? 0 : delta == 3 ? 2 : 1;
    }
    bool changed = true;
    while (changed) {
        changed = false;
        for (size_t i = 0; i < now.size(); ++i) if (actions[i] == 0) {
            const int other = occupant[targets[i]];
            if (other >= 0 && actions[other] != 0) { actions[i] = 3; changed = true; }
        }
    }
    return actions;
}
inline std::vector<int> apply(const Input& in, const std::vector<int>& positions,
                              std::vector<int>& orientations, const std::vector<int>& actions) {
    auto next = positions;
    for (size_t i = 0; i < next.size(); ++i) {
        if (actions[i] == 0) next[i] = neighbor(positions[i], orientations[i], in.rows, in.cols);
        else if (actions[i] == 1) orientations[i] = (orientations[i] + 1) % 4;
        else if (actions[i] == 2) orientations[i] = (orientations[i] + 3) % 4;
        else if (actions[i] != 3) throw std::runtime_error("invalid action");
    }
    validate(in, positions, next);
    return next;
}
inline void write(const std::string& path, const Json& data) { std::ofstream(path) << data.dump() << '\n'; }
}  // namespace traceio
