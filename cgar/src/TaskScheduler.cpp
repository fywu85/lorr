#include "TaskScheduler.h"

#include <cstdlib>
#include <string>

#include "cgar.hpp"

#include "scheduler.h"
#include "const.h"

namespace {
bool use_default_scheduler() {
    const char* v = std::getenv("CGAR_SCHEDULER");
    return v != nullptr && std::string(v) == "default";
}
}  // namespace

void TaskScheduler::initialize(int preprocess_time_limit)
{
    if (use_default_scheduler()) {
        DefaultPlanner::schedule_initialize(preprocess_time_limit / 2 - DefaultPlanner::SCHEDULER_TIMELIMIT_TOLERANCE, env);
        return;
    }
    // Shares the certificate and distance tables with the planner; whichever entry
    // point runs first builds them.
    cgar::Cgar::instance().initialize(env, preprocess_time_limit);
}

void TaskScheduler::plan(int time_limit, std::vector<int> & proposed_schedule)
{
    if (use_default_scheduler()) {
        DefaultPlanner::schedule_plan(time_limit / 2 - DefaultPlanner::SCHEDULER_TIMELIMIT_TOLERANCE, proposed_schedule, env);
        return;
    }
    // Scheduler and planner share the entry's real deadline. Do not shorten
    // computation or substitute a different policy to satisfy an internal slice.
    cgar::Cgar::instance().schedule(env, env->plan_start_time + std::chrono::milliseconds(time_limit),
                                    proposed_schedule);
}
