#include "engine.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <exception>
#include <numeric>
#include <stdexcept>
#include <unordered_set>

namespace r05 {
namespace {
uint64_t mix(uint64_t x) {
    x+=0x9e3779b97f4a7c15ULL;x=(x^(x>>30))*0xbf58476d1ce4e5b9ULL;
    x=(x^(x>>27))*0x94d049bb133111ebULL;return x^(x>>31);
}
struct Forecast {double score=0;int completed=0,decisions=0,pool_after=0;};
}

// Share only immutable tables. Runtime vectors, RNG, assignment pointers and
// ranking caches belong to each shadow; no trial may mutate the live solver.
void Engine::copy_replan_state(Engine& target,const Config& config) const {
    ++target.ranking_epoch_;
    target.cfg=config;target.quiet_=true;target.graph=graph;target.rng_=rng_;
    target.policy_profile_active_=false;target.replan_stats_=ReplanStats{};
    target.total_forward_=total_forward_;target.total_agent_steps_=total_agent_steps_;
    target.active_forward_=active_forward_;target.active_agent_steps_=active_agent_steps_;
    target.triaged_=triaged_;target.chains_=chains_;target.assigned_=assigned_;
    target.score_chains_.clear();target.score_assigned_.clear();
    target.future_tasks_.clear();target.future_plain_.clear();target.future_lengths_.clear();target.score_weights_.clear();target.progress_normalization_=1;
    target.age_=age_;target.previous_task_=previous_task_;target.previous_stage_=previous_stage_;
    target.pending_=pending_;target.best_offsets_=best_offsets_;target.past_offsets_=past_offsets_;
    target.last_actions_=last_actions_;target.predicted_loc_=predicted_loc_;target.predicted_dir_=predicted_dir_;
    if(config.candidate_cache && config.push_price==0) {
        target.candidate_rankings_.resize(config.threads);
        for(auto& ranking:target.candidate_rankings_)
            ranking.resize(size_t(age_.size())*config.cache_slots);
    }
}

int Engine::rank_replanned(const SharedEnvironment& env,const std::vector<int>& schedule,
                          const std::vector<Rollout>& results,int best) {
    if(cfg.operation_depth || cfg.plain_score || cfg.reverse_penalty || cfg.progress_discount!=1 || cfg.score_rank_power)
        throw std::invalid_argument("unsupported closed-loop forecast score or motion model");
    const auto& g=*graph;const int n=env.num_of_agents;
    std::vector<int> ranking;
    for(int k=0;k<int(results.size());++k)if(results[k].fully_evaluated)ranking.push_back(k);
    std::sort(ranking.begin(),ranking.end(),[&](int a,int b) {
        if(results[a].score!=results[b].score)return results[a].score>results[b].score;
        return cfg.accept_equal?a>b:a<b;
    });
    std::vector<int> roots{best};int target_rank=0;
    for(int rank=0;rank<int(ranking.size()) && int(roots.size())<cfg.replan_roots;++rank) {
        if(rank<target_rank)continue;
        const auto& candidate=results[ranking[rank]];bool duplicate=false;
        for(int old:roots)if(candidate.actions==results[old].actions &&
                            candidate.first.pending==results[old].first.pending)duplicate=true;
        if(duplicate)continue;
        roots.push_back(ranking[rank]);target_rank=target_rank?target_rank*2:2;
    }
    // Preserve the selected-vector-first history policy used after real search.
    std::vector<std::vector<std::vector<float>>> histories(roots.size());
    for(int r=0;r<int(roots.size());++r)if(cfg.persist_elites>1) {
        auto& history=histories[r];history.push_back(results[roots[r]].offsets);
        for(int k:ranking) {
            if(int(history.size())>=cfg.persist_elites)break;
            if(std::find(history.begin(),history.end(),results[k].offsets)==history.end())
                history.push_back(results[k].offsets);
        }
    }
    const int count=int(roots.size())*cfg.replan_futures;
    while(int(replan_engines_.size())<count)replan_engines_.push_back(std::make_unique<Engine>(Config{}));
    Config inner=cfg;inner.replan_roots=0;inner.component_trials=0;inner.first_futures=0;
    // Shadows share Chain ownership. Their current-task updates must not race
    // while lazily populating shared task tables; retain private ranking caches.
    inner.shared_rankings_mb=0;
    inner.futures=cfg.replan_k;inner.continuations=cfg.replan_continuations;
    inner.generations=1;inner.elites=1;inner.persist_elites=1;inner.screen_branches=0;
    if(cfg.replan_policy) {
        const int inner_roots=inner.futures/inner.continuations;
        inner.generations=std::min(cfg.generations,inner_roots);
        inner.elites=std::min(cfg.elites,std::max(1,inner_roots/inner.generations));
        inner.persist_elites=std::min(cfg.persist_elites,inner_roots);
    }
    inner.continuation_start=std::min(cfg.continuation_start,cfg.depth-1);
    if(inner.continuation_start<1)throw std::invalid_argument("forecast needs inner depth at least2");
    inner.local_trials=0;inner.branch_diagnostics=0;inner.snapshot_interval=0;
    inner.profile=false;inner.policy_profile=false;inner.threads=cfg.replan_threads;inner.random_by_step=true;
    if(inner.threads<1 || inner.threads>cfg.threads)
        throw std::invalid_argument("forecast worker allocation exceeds declared total");
    // Whole futures and their candidate evaluations share one fixed allocation.
    // Each shadow still owns a separate ranking cache for each inner worker.
    const int outer_workers=std::min(count,cfg.threads/inner.threads);
    const uint64_t key=(uint64_t(uint32_t(cfg.seed))<<32)|uint32_t(env.curr_timestep);
    std::vector<Forecast> forecasts(count);std::vector<std::exception_ptr> errors(count);
    #pragma omp parallel for num_threads(outer_workers) schedule(static)
    for(int trial=0;trial<count;++trial) {
        try {
            const int r=trial/cfg.replan_futures,future=trial%cfg.replan_futures;
            auto config=inner;config.seed=int(uint32_t(mix(key^0x6a09e667f3bcc909ULL^uint64_t(future)))&0x7fffffffU);
            auto& shadow=*replan_engines_[trial];copy_replan_state(shadow,config);
            SharedEnvironment state=env;const auto& root=results[roots[r]];
            shadow.pending_=root.first.pending;shadow.best_offsets_=root.offsets;
            shadow.predicted_loc_=root.first.loc;shadow.predicted_dir_=root.first.dir;
            if(cfg.persist_elites>1)shadow.past_offsets_=histories[r];
            shadow.record_travel(root.actions);
            auto& forecast=forecasts[trial];std::vector<Action> actions=root.actions;
            std::vector<int> assignment=schedule,from(n),to(n);
            for(int step=0;step<cfg.replan_steps;++step) {
                if(step) {
                    shadow.compute(&state,actions,assignment);++forecast.decisions;
                }
                if(int(actions.size())!=n || int(assignment.size())!=n)
                    throw std::runtime_error("short forecast decision");
                std::unordered_set<int> selected;
                for(int a=0;a<n;++a) {
                    const int old=state.curr_task_schedule[a],id=assignment[a];
                    if(old>=0 && state.task_pool.at(old).idx_next_loc>0 && old!=id)
                        throw std::runtime_error("forecast reassigned an opened task");
                    if(id>=0 && (!state.task_pool.count(id) || !selected.insert(id).second))
                        throw std::runtime_error("forecast invalid assignment");
                    auto& robot=state.curr_states[a];from[a]=g.from_grid.at(robot.location);to[a]=from[a];
                    if(actions[a]==FW)to[a]=g.next.at(from[a]).at(robot.orientation);
                    else if(actions[a]!=CR && actions[a]!=CCR && actions[a]!=W)
                        throw std::runtime_error("forecast invalid action");
                }
                certify(g,from,to);
                state.curr_task_schedule=assignment;state.new_tasks.clear();state.new_freeagents.clear();
                for(auto& item:state.task_pool)item.second.agent_assigned=-1;
                ++state.curr_timestep;
                for(int a=0;a<n;++a) {
                    const int id=assignment[a];auto& robot=state.curr_states[a];
                    const Chain* chain=shadow.assigned_[a];
                    const int stage=id<0?0:state.task_pool.at(id).idx_next_loc;
                    const double before=chain?chain->cost(g,stage,from[a],robot.orientation):0;
                    if(actions[a]==CR)robot.orientation=(robot.orientation+1)%4;
                    if(actions[a]==CCR)robot.orientation=(robot.orientation+3)%4;
                    robot.location=g.to_grid.at(to[a]);robot.timestep=state.curr_timestep;
                    if(step==0 && (to[a]!=root.first.loc[a] || robot.orientation!=root.first.dir[a]))
                        throw std::runtime_error("forecast changed the saved first action");
                    if(id<0)continue;
                    auto& task=state.task_pool.at(id);task.agent_assigned=a;
                    if(task.locations.at(task.idx_next_loc)==robot.location)++task.idx_next_loc;
                    const double after=chain?chain->cost(g,task.idx_next_loc,to[a],robot.orientation):0;
                    forecast.score+=(before-after)/2;
                    if(task.idx_next_loc==int(task.locations.size())) {
                        ++forecast.completed;
                        if(chain)forecast.score+=cfg.completion_bonus;
                        state.task_pool.erase(id);state.curr_task_schedule[a]=-1;state.new_freeagents.push_back(a);
                    }
                }
                // Deliberately no replacement task source: only the live visible
                // pool exists here. Reassignment cost is rebased on the next call.
                if(state.task_pool.size()+size_t(forecast.completed)!=env.task_pool.size())
                    throw std::runtime_error("forecast invented or lost an unrevealed task");
            }
            std::vector<int> final_locations(n);
            for(int a=0;a<n;++a)final_locations[a]=g.from_grid.at(state.curr_states[a].location);
            forecast.score-=cfg.dispersion*g.nearby_pairs(final_locations);
            forecast.pool_after=int(state.task_pool.size());
        } catch(...) {errors[trial]=std::current_exception();}
    }
    auto& stats=replan_stats_;stats.roots=int(roots.size());stats.futures=cfg.replan_futures;
    stats.steps=cfg.replan_steps;stats.pool_before=stats.min_pool_after=int(env.task_pool.size());
    std::vector<double> scores(roots.size(),0);
    for(int trial=0;trial<count;++trial) {
        if(errors[trial])std::rethrow_exception(errors[trial]);
        const auto& forecast=forecasts[trial];scores[trial/cfg.replan_futures]+=forecast.score/cfg.replan_futures;
        stats.decisions+=forecast.decisions;stats.min_pool_after=std::min(stats.min_pool_after,forecast.pool_after);
        stats.max_completed=std::max(stats.max_completed,forecast.completed);
    }
    if(stats.decisions!=count*(cfg.replan_steps-1))
        throw std::runtime_error("forecast did not complete its fixed work budget");
    stats.branch_evaluations=uint64_t(stats.decisions)*cfg.replan_k;
    int selected=0;
    for(int r=1;r<int(roots.size());++r)if(scores[r]>scores[selected]+1e-7)selected=r;
    stats.selected_rank=int(std::find(ranking.begin(),ranking.end(),roots[selected])-ranking.begin());
    if(!quiet_ && env.curr_timestep%100==0)std::fprintf(stderr,
        "R05_REPLAN t=%d roots=%d futures=%d steps=%d inner_K=%d decisions=%d branches=%llu rank=%d score=%.3f old_score=%.3f pool=%d min_pool=%d outer_workers=%d inner_workers=%d policy=%d\n",
        env.curr_timestep,stats.roots,stats.futures,stats.steps,cfg.replan_k,stats.decisions,
        (unsigned long long)stats.branch_evaluations,stats.selected_rank,scores[selected],scores[0],
        stats.pool_before,stats.min_pool_after,outer_workers,inner.threads,int(cfg.replan_policy));
    return roots[selected];
}
}
