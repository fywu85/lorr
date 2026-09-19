#pragma once
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <vector>
#include <sched.h>

inline uint32_t nms_probe_uint(const char* name, uint32_t fallback) {
    const char* value = std::getenv(name);
    if (!value) return fallback;
    const std::string text(value);
    size_t consumed = 0;
    const auto parsed = std::stoull(text, &consumed);
    if (text.empty() || text[0] == '-' || consumed != text.size() || parsed > UINT32_MAX)
        throw std::runtime_error(std::string("invalid probe setting: ") + name);
    return static_cast<uint32_t>(parsed);
}

inline double nms_probe_seconds() {
    return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

struct NmsConstructionTrace {
    uint32_t worker = 0, order_visited = 0, robots = 0, roots = 0, roots_accepted = 0;
    uint64_t recursive_calls = 0, candidates = 0;
    uint32_t max_depth = 0, search_steps = 0, helpers = 0;
    bool clock_cut = false, construction_complete = false;
    double construction_score = 0, final_score = 0, construction_seconds = 0, solve_seconds = 0;
    int start_cpu = -1, end_cpu = -1;
    std::vector<int> affinity;
};

inline std::vector<NmsConstructionTrace>& nms_probe_traces() {
    static std::vector<NmsConstructionTrace> traces;
    return traces;
}

inline double& nms_probe_prepare_seconds() {
    static double seconds = 0;
    return seconds;
}
