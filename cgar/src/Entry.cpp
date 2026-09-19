#include "Entry.h"
#include "Tasks.h"
#include "utils.h"
#include "heuristics.h"
#include "cgar.hpp"
#include <cstdio>
#include <cstdlib>

// The initialize function will be called by competition system at the preprocessing stage.
// Implement the initialize functions of the planner and scheduler to load or compute auxiliary data.
// Note that, this function runs untill preprocess_time_limit (in milliseconds) is reached.
// This is an offline step, after it completes then evaluation begins.
void Entry::initialize(int preprocess_time_limit)
{
    try {
        scheduler->initialize(preprocess_time_limit);
        planner->initialize(preprocess_time_limit);
        cgar::check_deadline(env->plan_start_time + std::chrono::milliseconds(preprocess_time_limit),
                             "entry_initialize_complete");
    } catch (const cgar::Timeout& error) {
        const double elapsed = std::chrono::duration<double, std::milli>(
            std::chrono::steady_clock::now() - env->plan_start_time).count();
        std::fprintf(stderr, "CGAR_TIMEOUT phase=initialize elapsed_ms=%.3f stage=%s\n", elapsed, error.what());
        std::fflush(nullptr);
        std::_Exit(124);
    } catch (const std::exception& error) {
        std::fprintf(stderr, "CGAR_INITIALIZATION_ERROR message=%s\n", error.what());
        std::fflush(nullptr);
        std::_Exit(125);
    } catch (...) {
        std::fprintf(stderr, "CGAR_INITIALIZATION_ERROR message=unknown_exception\n");
        std::fflush(nullptr);
        std::_Exit(125);
    }
}

//The compute function will be called by competition system on each timestep.
//It computes:
//  1. a schedule that specifies which agent complete which task.
//  2. a next action that specifies how each agent should move in the next timestep.
//NB: the parameter time_limit is specified in milliseconds.
void Entry::compute(int time_limit, std::vector<Action> & plan, std::vector<int> & proposed_schedule)
{
    try {
        scheduler->plan(time_limit, proposed_schedule);
        update_goal_locations(proposed_schedule);
        planner->plan(time_limit, plan);
        const auto completed = std::chrono::steady_clock::now();
        last_compute_seconds_ = std::chrono::duration<double>(completed - env->plan_start_time).count();
        if (completed >= env->plan_start_time + std::chrono::milliseconds(time_limit))
            throw cgar::Timeout("entry_complete");
    } catch (const cgar::Timeout& error) {
        const double elapsed = std::chrono::duration<double, std::milli>(
            std::chrono::steady_clock::now() - env->plan_start_time).count();
        std::fprintf(stderr, "CGAR_TIMEOUT timestep=%d elapsed_ms=%.3f stage=%s\n",
                     env->curr_timestep, elapsed, error.what());
        std::fflush(nullptr);
        std::_Exit(124);  // The benchmark records an explicit timeout, never a partial result.
    }

}

// Set the next goal locations for each agent based on the proposed schedule
void Entry::update_goal_locations(std::vector<int> & proposed_schedule)
{
    // record the proposed schedule so that we can tell the planner
    env->curr_task_schedule = proposed_schedule;

    // The first unfinished errand/location of each task is the next goal for the assigned agent.
    for (size_t i = 0; i < proposed_schedule.size(); i++)
    {
        env->goal_locations[i].clear();
        int t_id = proposed_schedule[i];
        if (t_id == -1)
            continue;

        int i_loc = env->task_pool[t_id].idx_next_loc;
        env->goal_locations[i].push_back({env->task_pool[t_id].locations.at(i_loc), env->task_pool[t_id].t_revealed});
    }
    return;
}