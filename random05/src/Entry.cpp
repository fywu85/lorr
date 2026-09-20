#include "Entry.h"
#include <cstdio>
#include <cstdlib>
namespace {
void fail(const char* phase,const std::exception& e) {
    std::fprintf(stderr,"R05_ERROR phase=%s message=%s\n",phase,e.what());
    std::fflush(nullptr); std::_Exit(125);
}
void deadline(SharedEnvironment* env,int limit,const char* phase) {
    const double ms=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-env->plan_start_time).count();
    if(ms>=limit) {
        std::fprintf(stderr,"R05_TIMEOUT phase=%s timestep=%d elapsed_ms=%.3f\n",phase,env->curr_timestep,ms);
        std::fflush(nullptr); std::_Exit(124);
    }
}
}
void Entry::initialize(int limit) {
    try {
        engine_=std::make_unique<r05::Engine>(r05::Config::environment(*env));
        engine_->initialize(env); deadline(env,limit,"initialize");
    } catch(const std::exception& e) { fail("initialize",e); }
}
void Entry::compute(int limit,std::vector<Action>& plan,std::vector<int>& schedule) {
    try {
        engine_->compute(env,plan,schedule);
        // Keep the simulator's goal view consistent with the combined schedule.
        // Its solution-cost/makespan accounting reads this view after compute.
        env->curr_task_schedule=schedule;
        for(size_t a=0;a<schedule.size();++a) {
            env->goal_locations[a].clear();
            if(schedule[a]>=0) {
                const auto& task=env->task_pool.at(schedule[a]);
                env->goal_locations[a].push_back({task.locations.at(task.idx_next_loc),task.t_revealed});
            }
        }
        seconds_=std::chrono::duration<double>(std::chrono::steady_clock::now()-env->plan_start_time).count();
        deadline(env,limit,"compute");
    } catch(const std::exception& e) { fail("compute",e); }
}
