#include "engine.hpp"
#include <algorithm>
#include <cstdio>
#include <cmath>
#include <exception>
#include <stdexcept>

namespace r05 {
namespace {
uint64_t mix(uint64_t x) {
    x+=0x9e3779b97f4a7c15ULL;x=(x^(x>>30))*0xbf58476d1ce4e5b9ULL;
    x=(x^(x>>27))*0x94d049bb133111ebULL;return x^(x>>31);
}
}

// Branch zero preserves the current priority vector. Its mixture probability
// can be fixed independently of how many randomized future samples are taken.
double weighted_static_future_score(const std::vector<double>& scores,double static_weight) {
    if(scores.size()<2 || !std::isfinite(static_weight) || static_weight<0 || static_weight>1)
        throw std::invalid_argument("invalid static/random future mixture");
    double random_sum=0;
    for(size_t b=1;b<scores.size();++b)random_sum+=scores[b];
    return static_weight*scores[0]+(1-static_weight)*random_sum/(scores.size()-1);
}

// Search reuses common continuation samples across many generations. Recheck
// fully evaluated finalists using independent common draws to test sensitivity
// to that sample. No finalist receives a partial evaluation or live RNG draws.
int Engine::rescore_candidates(const SharedEnvironment& env,const Frame& frame,
                               const std::vector<Rollout>& results,int best) {
    std::vector<int> ranking;
    for(int k=0;k<int(results.size());++k)if(results[k].fully_evaluated)ranking.push_back(k);
    std::sort(ranking.begin(),ranking.end(),[&](int a,int b) {
        if(results[a].score!=results[b].score)return results[a].score>results[b].score;
        return cfg.accept_equal?a>b:a<b;
    });
    std::vector<int> finalists{best};
    for(int k:ranking) {
        if(int(finalists.size())>=cfg.rescore_roots)break;
        bool duplicate=false;
        for(int old:finalists)if(results[k].actions==results[old].actions &&
                                results[k].first.pending==results[old].first.pending)duplicate=true;
        if(!duplicate)finalists.push_back(k);
    }
    const uint64_t key=(uint64_t(uint32_t(cfg.seed))<<32)|uint32_t(env.curr_timestep);
    std::mt19937 random(uint32_t(mix(key^0x243f6a8885a308d3ULL)));
    std::uniform_real_distribution<float> unit(0,1),noise(-cfg.noise,cfg.noise);
    std::vector<Continuation> futures(cfg.rescore_branches-1,Continuation(cfg.depth));
    for(size_t b=0;b<futures.size();++b) {
        const auto* elite=cfg.future_elite_blend>0 && !past_offsets_.empty()?
            &past_offsets_[b%past_offsets_.size()]:nullptr;
        for(int t=cfg.continuation_start;t<cfg.depth;++t)
            for(int a=0;a<env.num_of_agents;++a)if(unit(random)<cfg.future_mutation) {
                float value=noise(random);
                if(elite)value=cfg.future_elite_blend==1?(*elite)[a]:
                    (1-cfg.future_elite_blend)*value+cfg.future_elite_blend*(*elite)[a];
                futures[b][t].push_back({a,value});
            }
    }
    std::vector<Rollout> checked(finalists.size());
    std::vector<std::exception_ptr> errors(finalists.size());
    #pragma omp parallel for num_threads(cfg.threads) schedule(static)
    for(int j=0;j<int(finalists.size());++j) {
        try {
            const auto& old=results[finalists[j]];
            std::vector<double> branch_scores;
            checked[j]=evaluate(frame,old.offsets,futures,old.cycle_moves,
                                cfg.rescore_static_weight>=0?&branch_scores:nullptr);
            if(cfg.rescore_static_weight>=0)
                checked[j].score=weighted_static_future_score(branch_scores,cfg.rescore_static_weight);
            const auto& fresh=checked[j];
            if(fresh.actions!=old.actions || fresh.first.loc!=old.first.loc ||
               fresh.first.dir!=old.first.dir || fresh.first.pending!=old.first.pending ||
               fresh.first.stage!=old.first.stage || fresh.first.operations!=old.first.operations)
                throw std::runtime_error("independent rescoring changed the candidate decision");
        } catch(...) {errors[j]=std::current_exception();}
    }
    auto& stats=rescore_stats_;stats.roots=int(finalists.size());stats.branches=cfg.rescore_branches;
    std::vector<double> scores(finalists.size());
    int selected=0;
    for(int j=0;j<int(finalists.size());++j) {
        if(errors[j])std::rethrow_exception(errors[j]);
        stats.evaluations+=checked[j].evaluated_branches;
        scores[j]=cfg.rescore_blend==1?results[finalists[j]].score:
            (1-cfg.rescore_blend)*checked[j].score+cfg.rescore_blend*results[finalists[j]].score;
        if(scores[j]>scores[selected]+1e-7)selected=j;
    }
    if(stats.evaluations!=stats.roots*stats.branches)
        throw std::runtime_error("independent rescoring changed its complete work count");
    stats.selected_rank=int(std::find(ranking.begin(),ranking.end(),finalists[selected])-ranking.begin());
    if(!quiet_ && env.curr_timestep%100==0)std::fprintf(stderr,
        "R05_RESCORE t=%d roots=%d branches=%d evaluations=%d rank=%d score=%.3f old_score=%.3f blend=%.3f static_weight=%.3f\n",
        env.curr_timestep,stats.roots,stats.branches,stats.evaluations,stats.selected_rank,
        scores[selected],scores[0],cfg.rescore_blend,cfg.rescore_static_weight);
    // Preserve ordinary parent ranking, while retaining the chosen root first.
    // Mixing old/new sample scores in the search history would change two things.
    return finalists[selected];
}
}
