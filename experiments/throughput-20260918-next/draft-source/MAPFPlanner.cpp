#include <random>
#include <Entry.h>

#include <cstdlib>
#include <string>

#include "cgar.hpp"

// default planner includes (kept for the CGAR_PLANNER=default ablation)
#include "planner.h"
#include "const.h"

namespace {
bool use_default_planner() {
    const char* v = std::getenv("CGAR_PLANNER");
    return v != nullptr && std::string(v) == "default";
}

// Milliseconds left of this entry call, minus the start-kit's timing tolerance.
int remaining(SharedEnvironment* env, int limit) {
    const auto spent = std::chrono::duration_cast<milliseconds>(
                           std::chrono::steady_clock::now() - env->plan_start_time)
                           .count();
    return limit - static_cast<int>(spent) - DefaultPlanner::PLANNER_TIMELIMIT_TOLERANCE;
}
}  // namespace

void MAPFPlanner::initialize(int preprocess_time_limit)
{
    if (use_default_planner()) {
        DefaultPlanner::initialize(remaining(env, preprocess_time_limit), env);
        return;
    }
    cgar::Cgar::instance().initialize(env, remaining(env, preprocess_time_limit));
}

void MAPFPlanner::plan(int time_limit, vector<Action> & actions)
{
    if (use_default_planner()) {
        DefaultPlanner::plan(remaining(env, time_limit), actions, env);
        return;
    }
    cgar::Cgar::instance().plan(env, remaining(env, time_limit), actions);
}
