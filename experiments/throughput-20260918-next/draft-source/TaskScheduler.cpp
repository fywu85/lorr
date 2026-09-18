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
    // Dispatch is far cheaper than routing, so leave most of the step to the planner.
    cgar::Cgar::instance().schedule(env, time_limit / 3 - DefaultPlanner::SCHEDULER_TIMELIMIT_TOLERANCE,
                                    proposed_schedule);
}
