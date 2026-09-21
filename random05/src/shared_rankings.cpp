// Immutable task/pose rankings shared across workers and real steps. No clocks,
// priority order or collision-dependent quantities enter this optional cache.
#include "engine.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <exception>
#include <limits>
#include <unordered_set>

namespace r05 {
namespace {
int turn_distance(int a,int b) {int d=std::abs(a-b);return std::min(d,4-d);}
void build_rankings(const Graph& g,const Config& cfg,const Chain& chain,bool orders_only) {
    const size_t entries=size_t(chain.goals.size())*g.cells*8;
    std::vector<PreparedRanking> table(orders_only?0:entries);
    std::vector<PreparedOrder> orders(orders_only?entries:0);
    for(int stage=0;stage<int(chain.goals.size());++stage) {
        const float* row=chain.cached_row(g,stage);
        auto cost=[&](int v,int d){return row?row[v*4+d]:chain.cost(g,stage,v,d);};
        for(int p=0;p<g.cells;++p)for(int dir=0;dir<4;++dir)for(int moving=0;moving<2;++moving) {
            const size_t index=(size_t(stage)*g.cells+p)*8+dir*2+moving;
            PreparedRanking temporary;
            auto& entry=orders_only?temporary:table[index];
            auto allowed=[&](int d){return moving?d==dir:turn_distance(d,dir)<=1;};
            int best_dir=dir;float best=cost(p,best_dir);
            if(!moving)for(int q:{(dir+1)%4,(dir+3)%4}) {
                float value=cost(p,q)+0.05f*g.weight[p][4];
                if(value<best-1e-5f){best=value;best_dir=q;}
            }
            entry.idle_heading=uint8_t(best_dir);entry.base_cost=cost(p,best_dir);
            std::array<MoveCandidate,5> candidates;int count=0;
            for(int d=0;d<4;++d) {
                int v=g.next[p][d];if(v<0 || (!cfg.intent_rotation && !allowed(d)))continue;
                candidates[count++]={v,d,cost(v,d)+g.forward_weight(chain.goals[stage],p,d)};
            }
            float wait_value=entry.base_cost;
            if(cfg.prospective_wait)for(int d=0;d<4;++d)if(!moving || turn_distance(d,dir)<=1)
                wait_value=std::min(wait_value,cost(p,d));
            candidates[count++]={p,best_dir,wait_value+cfg.wait_cost+0.001f};
            for(int k=1;k<count;++k) {
                MoveCandidate value=candidates[k];int j=k;
                while(j>0 && value.score<candidates[j-1].score){candidates[j]=candidates[j-1];--j;}
                candidates[j]=value;
            }
            entry.count=uint8_t(count);entry.save(candidates,p);
            for(int k=0;k<count;++k)
                if(candidates[k].v==p || allowed(candidates[k].d))entry.kinematic_mask|=uint8_t(1u<<k);
            if(orders_only)orders[index].save(entry);
        }
    }
    chain.rankings=std::move(table);chain.order_rankings=std::move(orders);
}
}

void Engine::prepare_shared_rankings(int timestep) {
    // Shadow planners share Chain objects. Keep this prototype off in nested
    // forecasts and virtual matching, where independently built tables would
    // otherwise need a different ownership protocol. Dynamic push costs bypass.
    if(!cfg.shared_rankings_mb || cfg.push_price>0 || cfg.rollout_match || cfg.replan_roots || cfg.operation_depth)return;
    const auto& g=*graph;
    // Biased routing needs actual scores. Keep its full exact entries even if
    // the order-only optimization is requested for a shared preset.
    const bool orders_only=cfg.shared_orders && cfg.move_bias==0;
    const bool changed=cfg.wait_cost!=shared_wait_cost_ || cfg.intent_rotation!=shared_intent_rotation_ ||
                       cfg.prospective_wait!=shared_prospective_wait_ || orders_only!=shared_orders_only_;
    auto clear_tables=[&]() {
        for(const auto& item:chains_) {
            std::vector<PreparedRanking>().swap(item.second->rankings);
            std::vector<PreparedOrder>().swap(item.second->order_rankings);
        }
    };
    if(changed)clear_tables();
    shared_wait_cost_=cfg.wait_cost;shared_intent_rotation_=cfg.intent_rotation;shared_prospective_wait_=cfg.prospective_wait;
    shared_orders_only_=orders_only;
    const size_t limit=size_t(cfg.shared_rankings_mb)*1024*1024;
    size_t used=0;
    for(const auto& item:chains_)
        used+=item.second->rankings.size()*sizeof(PreparedRanking)+item.second->order_rankings.size()*sizeof(PreparedOrder);
    if(used>limit){clear_tables();used=0;}
    std::vector<const Chain*> pending;std::unordered_set<const Chain*> seen;
    // Stable agent order decides which tables fit, never elapsed time. A task
    // that does not fit still uses the exact existing worker cache/calculation.
    for(const Chain* chain:assigned_)if(chain && chain->rankings.empty() && chain->order_rankings.empty() && seen.insert(chain).second) {
        const size_t bytes=chain->goals.size()*size_t(g.cells)*8*(orders_only?sizeof(PreparedOrder):sizeof(PreparedRanking));
        if(bytes<=limit-used){used+=bytes;pending.push_back(chain);}
    }
    std::vector<std::exception_ptr> errors(pending.size());
    #pragma omp parallel for num_threads(cfg.threads) schedule(static)
    for(size_t i=0;i<pending.size();++i) {
        try{build_rankings(g,cfg,*pending[i],orders_only);}catch(...){errors[i]=std::current_exception();}
    }
    for(const auto& error:errors)if(error)std::rethrow_exception(error);
    if(!quiet_ && (timestep<5 || timestep%100==0))
        std::fprintf(stderr,"R05_SHARED_RANKINGS t=%d new_tables=%zu stored_bytes=%zu limit_bytes=%zu\n",
                     timestep,pending.size(),used,limit);
}
}
