// Three-step operation policy inspired by EPIBT (Straple, MIT).
// The reservation and bounded-revisit structure adapts the public algorithm;
// integration with task chains and rollout search is specific to this backend.
// See licenses/EPIBT-MIT.txt and RESEARCH.md for attribution and provenance.
#include "engine.hpp"
#include <algorithm>
#include <numeric>
#include <stdexcept>

namespace r05 {
OperationModel::OperationModel(const Graph& g) {
    static_assert(FW==0 && CR==1 && CCR==2 && W==3,"operation encoding requires four standard actions");
    paths.resize(size_t(g.states)*count);
    groups.resize(g.states);
    for(int start=0;start<g.states;++start)for(int code=0;code<count;++code) {
        auto& path=paths[size_t(start)*count+code];int v=start/4,d=start%4;
        bool valid=true;
        for(int t=0;t<horizon;++t) {
            Action action=Action((code>>(2*t))&3);
            if(action==FW) {v=g.next[v][d];if(v<0){valid=false;break;}}
            else if(action==CR)d=(d+1)%4;
            else if(action==CCR)d=(d+3)%4;
            path[t]=v*4+d;
        }
        if(!valid){path[0]=-1;continue;}
        // A rotation followed by its inverse before any forward motion never
        // changes the footprint or task visits. Do not let the tie preference
        // for rotation over waiting introduce these wasteful prefixes.
        int rotation=0,turns=0;bool redundant=false;
        for(int t=0;t<horizon;++t) {
            Action action=Action((code>>(2*t))&3);
            if(action==FW){rotation=0;turns=0;}
            else if(action==CR || action==CCR) {
                int sign=action==CR?1:-1;
                if((rotation && rotation!=sign) || ++turns>=3)redundant=true;
                rotation=sign;
            }
        }
        if(redundant)continue;
        // Different final headings can share the same reservation footprint.
        // Choose the best heading for the current task before sorting footprints.
        bool found=false;
        for(auto& group:groups[start]) {
            const auto& other=paths[size_t(start)*count+group[0]];
            bool same=true;
            for(int t=0;t<horizon;++t)same=same && path[t]/4==other[t]/4;
            if(same){group.push_back(uint8_t(code));found=true;break;}
        }
        if(!found)groups[start].push_back({uint8_t(code)});
    }
}

void Engine::advance_operations(Frame& f,const std::vector<float>& offsets,
                                std::vector<Action>& actions,uint64_t& expansion_count) const {
    const auto& assigned=cfg.rollout_match?f.active_chains:assigned_;
    const auto& g=*graph;const auto& model=*operation_model_;const int n=int(f.loc.size());
    constexpr int H=OperationModel::horizon;
    std::vector<int> state(n),chosen=f.operations,order(n),visits(n,0);
    std::vector<unsigned char> stack(n,0);
    std::vector<float> priority(n);
    std::vector<std::array<int,H>> vertex(g.cells),edge(g.states);
    for(auto& x:vertex)x.fill(-1);
    for(auto& x:edge)x.fill(-1);
    struct Candidate {float cost;int bias,code;};
    std::vector<std::vector<Candidate>> candidates(n);
    auto less=[](const Candidate& a,const Candidate& b) {
        if(a.cost!=b.cost)return a.cost<b.cost;
        if(a.bias!=b.bias)return a.bias>b.bias;
        return a.code<b.code;
    };
    for(int a=0;a<n;++a) {
        state[a]=f.loc[a]*4+f.dir[a];
        const Chain* chain=assigned[a];
        bool active=chain && f.stage[a]<int(chain->goals.size());
        int age=cfg.rollout_age?f.age[a]:age_[a];
        if(cfg.age_cap>0)age=std::min(age,cfg.age_cap);
        priority[a]=age+offsets[a]-(active?0:100000);
        if(cfg.deadends && g.pocket[f.loc[a]] &&
           (!active || g.pocket[chain->goals[f.stage[a]]]!=g.pocket[f.loc[a]]))priority[a]+=1000000;
        for(const auto& group:model.groups[state[a]]) {
            // Optional EPIBT-style active search: an unchanged footprint is a
            // valid inherited fallback, but cannot end a repair before trying
            // a detour. This keeps waiting safe without ranking it above every
            // temporary increase in distance when the goalward path is blocked.
            const auto& footprint=model.path(state[a],group[0]);
            if(cfg.operation_moving && std::all_of(footprint.begin(),footprint.end(),
                    [&](int v){return v/4==f.loc[a];}))continue;
            Candidate best{1e30f,0,-1};
            for(int code:group) {
                const auto& path=model.path(state[a],code);
                int stage=f.stage[a],previous=state[a],bias=0;float travel=0;
                for(int t=0;t<H;++t) {
                    Action action=Action((code>>(2*t))&3);
                    travel+=action==FW?g.weight[previous/4][previous%4]:
                            action==W?cfg.wait_cost:g.weight[previous/4][4];
                    bias+=(H-t)*(action==FW?8:action==W?0:1);
                    if(chain && stage<int(chain->goals.size()) && path[t]/4==chain->goals[stage])++stage;
                    previous=path[t];
                }
                float remaining=chain?chain->cost(g,stage,path.back()/4,path.back()%4):
                                      cfg.idle_eviction*g.pocket_depth[path.back()/4];
                Candidate item{remaining+(active?cfg.operation_cost_weight:1)*travel,bias,code};
                if(best.code<0 || less(item,best))best=item;
            }
            candidates[a].push_back(best);
        }
        std::sort(candidates[a].begin(),candidates[a].end(),less);
    }
    // A caller removes an agent's old path before visiting it. Success installs
    // a replacement; failure leaves its original choice uninstalled for rollback.
    auto install=[&](int a,bool add) {
        const auto& path=model.path(state[a],chosen[a]);int previous=state[a]/4;
        if(path[0]<0)throw std::runtime_error("invalid inherited operation");
        for(int t=0;t<H;++t) {
            int v=path[t]/4;
            if(vertex[v][t]!=(add?-1:a))throw std::runtime_error("operation vertex reservation mismatch");
            vertex[v][t]=add?a:-1;
            if(v!=previous) {
                int id=previous*4+path[t]%4;
                if(edge[id][t]!=(add?-1:a))throw std::runtime_error("operation edge reservation mismatch");
                edge[id][t]=add?a:-1;
            }
            previous=v;
        }
    };
    auto blocker=[&](int a,int code) {
        const auto& path=model.path(state[a],code);int previous=state[a]/4,found=-1;
        for(int t=0;t<H;++t) {
            int v=path[t]/4,b=vertex[v][t];
            if(b>=0){if(found>=0 && found!=b)return -2;found=b;}
            if(v!=previous) {
                b=edge[v*4+(path[t]%4+2)%4][t];
                if(b>=0){if(found>=0 && found!=b)return -2;found=b;}
            }
            previous=v;
        }
        return found;
    };
    for(int a=0;a<n;++a)install(a,true);
    int expansions=0;
    auto visit=[&](auto&& self,int a,float inherited_priority)->bool {
        if(expansions>=cfg.expansion_limit)return false;
        ++expansions;++visits[a];stack[a]=1;
        int old=chosen[a];
        for(const auto& candidate:candidates[a]) {
            int b=blocker(a,candidate.code);
            if(b==-1) {
                chosen[a]=candidate.code;install(a,true);stack[a]=0;return true;
            }
            if(b<0 || stack[b] || visits[b]>=cfg.operation_revisits || priority[b]>=inherited_priority)continue;
            install(b,false);chosen[a]=candidate.code;install(a,true);
            if(self(self,b,inherited_priority)){stack[a]=0;return true;}
            install(a,false);install(b,true);chosen[a]=old;
        }
        chosen[a]=old;stack[a]=0;return false;
    };
    std::iota(order.begin(),order.end(),0);
    std::stable_sort(order.begin(),order.end(),[&](int a,int b){return priority[a]>priority[b];});
    for(int a:order)if(!visits[a]) {
        install(a,false);
        if(!visit(visit,a,priority[a]))install(a,true);
    }
    expansion_count+=expansions;
    actions.resize(n);
    for(int a=0;a<n;++a) {
        const auto& path=model.path(state[a],chosen[a]);
        actions[a]=Action(chosen[a]&3);
        f.loc[a]=path[0]/4;f.dir[a]=path[0]%4;
        bool arrived=assigned[a] && f.stage[a]<int(assigned[a]->goals.size()) &&
                     f.loc[a]==assigned[a]->goals[f.stage[a]];
        if(arrived)++f.stage[a];
        if(cfg.rollout_age)f.age[a]=arrived?0:f.age[a]+1;
        f.operations[a]=cfg.operation_inherit?(chosen[a]>>2)|(int(W)<<4):OperationModel::waiting;
        f.pending[a]=cfg.operation_inherit?path[1]/4:f.loc[a];
        if(cfg.reverse_penalty>0)
            f.reverse_turns+=(actions[a]==CR && f.last_actions[a]==CCR) ||
                             (actions[a]==CCR && f.last_actions[a]==CR);
    }
    if(cfg.reverse_penalty>0)f.last_actions=actions;
}
}
