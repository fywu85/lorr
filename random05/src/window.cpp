// Fixed-work rolling-window LNS: time-space A* replans small groups against
// complete reservations. Independent implementation of the standard windowed
// MAPF/LNS pattern; no participant code or map-specific geometry is embedded.
#include "engine.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <exception>
#include <limits>
#include <numeric>
#include <stdexcept>

namespace r05 {
namespace {
using Paths=std::vector<std::vector<int>>;
struct Cost { double total=0, remaining=0, integral=0; };
bool better(const Cost& a,const Cost& b) {
    return a.total<b.total-1e-5 || (std::abs(a.total-b.total)<=1e-5 &&
        (a.remaining<b.remaining-1e-5 || (std::abs(a.remaining-b.remaining)<=1e-5 &&
         a.integral<b.integral-1e-5)));
}
float action_cost(const Graph& g,const Config& cfg,int from,int to,int goal) {
    if(goal<0 && from==to)return 0;
    // Use the same current-waypoint edge prices as the exact chained heuristic,
    // including its optional local guidance taper. Completed chains have no goal.
    if(from/4!=to/4)return g.forward_weight(goal,from/4,from%4);
    return from==to?cfg.wait_cost:g.weight[from/4][4];
}
int arrived(const Chain* chain,int stage,int state) {
    // The simulator consumes at most one waypoint per physical action,
    // including repeated goals and waits on an already occupied goal cell.
    return stage+(chain && stage<int(chain->goals.size()) && chain->goals[stage]==state/4);
}
Cost path_cost(const Graph& g,const Config& cfg,const Chain* chain,int stage,const std::vector<int>& path) {
    Cost value;
    for(size_t t=1;t<path.size();++t) {
        value.total+=action_cost(g,cfg,path[t-1],path[t],chain && stage<int(chain->goals.size())?chain->goals[stage]:-1);
        stage=arrived(chain,stage,path[t]);
        // A tertiary preference for progress earlier in the complete window.
        // It never trades a worse primary path cost or terminal potential for
        // apparent early progress, and does not alter the bounded A* search.
        if(cfg.window_progress_tie && chain)
            value.integral+=chain->cost(g,stage,path[t]/4,path[t]%4);
    }
    if(chain)value.remaining=chain->cost(g,stage,path.back()/4,path.back()%4);
    value.total+=value.remaining;return value;
}
struct Reservations {
    const Graph& g;int horizon;
    std::vector<int> owners;
    explicit Reservations(const Graph& graph,int h):g(graph),horizon(h),owners(size_t(h+1)*g.cells,-1){}
    int owner(int t,int cell) const {return owners[size_t(t)*g.cells+cell];}
    void set(int agent,const std::vector<int>& path,bool insert) {
        for(int t=0;t<=horizon;++t) {
            auto& slot=owners[size_t(t)*g.cells+path[t]/4];
            if(insert) {
                if(slot>=0 && slot!=agent)throw std::runtime_error("window reservation collision");
                slot=agent;
            } else {
                if(slot!=agent)throw std::runtime_error("window reservation ownership mismatch");
                slot=-1;
            }
        }
    }
    bool allowed(int t,int from,int to) const {
        if(owner(t+1,to)>=0)return false;
        // Same owner crossing the reverse edge forbids a head-on swap.
        int other=owner(t,to);
        return from==to || other<0 || owner(t+1,from)!=other;
    }
};
struct Node {float f,h,g;int id,time,state,stage;};
struct Greater {
    bool operator()(const Node& a,const Node& b) const {
        if(a.f!=b.f)return a.f>b.f;
        if(a.h!=b.h)return a.h>b.h;
        if(a.time!=b.time)return a.time<b.time;
        return a.id>b.id;
    }
};
struct Search {
    std::vector<float> costs;
    std::vector<int> parent;
    std::vector<uint32_t> seen;
    std::vector<Node> heap;
    std::vector<const float*> heuristic_rows;
    uint32_t epoch=0;
    uint64_t expanded=0;
    // A four-way heap uses the same complete ordering as the binary reference.
    // Fewer levels trade a short contiguous sibling scan for dependent loads.
    void insert(Node node,bool four_way) {
        if(!four_way) {
            heap.push_back(node);std::push_heap(heap.begin(),heap.end(),Greater{});return;
        }
        size_t hole=heap.size();heap.push_back(node);
        while(hole>0) {
            const size_t parent=(hole-1)/4;
            if(!Greater{}(heap[parent],node))break;
            heap[hole]=heap[parent];hole=parent;
        }
        heap[hole]=node;
    }
    Node remove(bool four_way) {
        if(!four_way) {
            std::pop_heap(heap.begin(),heap.end(),Greater{});
            Node node=heap.back();heap.pop_back();return node;
        }
        const Node node=heap.front(),tail=heap.back();heap.pop_back();
        if(!heap.empty()) {
            size_t hole=0;
            while(hole*4+1<heap.size()) {
                const size_t first=hole*4+1,end=std::min(first+4,heap.size());
                size_t best=first;
                for(size_t child=first+1;child<end;++child)
                    if(Greater{}(heap[best],heap[child]))best=child;
                if(!Greater{}(tail,heap[best]))break;
                heap[hole]=heap[best];hole=best;
            }
            heap[hole]=tail;
        }
        return node;
    }
    bool solve(const Graph& g,const Config& cfg,const Reservations& reserve,
               const Chain* chain,int stage,int state,std::vector<int>& path) {
        const int stages=chain?int(chain->goals.size())+1:1,horizon=reserve.horizon;
        const size_t size=size_t(horizon+1)*stages*g.states;
        if(size>size_t(std::numeric_limits<int>::max()))throw std::runtime_error("window state index overflow");
        if(costs.size()<size){costs.resize(size);parent.resize(size);seen.resize(size,0);}
        if(++epoch==0){std::fill(seen.begin(),seen.end(),0);++epoch;}
        heap.clear();
        if(!chain)stage=0;
        heuristic_rows.assign(stages,nullptr);
        if(chain)for(int k=0;k<stages-1;++k)heuristic_rows[k]=chain->cached_row(g,k);
        auto estimate=[&](int k,int s) {
            if(!chain || k==stages-1)return 0.f;
            return heuristic_rows[k]?heuristic_rows[k][s]:chain->cost(g,k,s/4,s%4);
        };
        auto push=[&](int time,int k,int s,float cost,int previous) {
            int id=(time*stages+k)*g.states+s;
            if(seen[id]==epoch && costs[id]<=cost)return;
            seen[id]=epoch;costs[id]=cost;parent[id]=previous;
            float h=estimate(k,s);insert({cost+h,h,cost,id,time,s,k},cfg.window_heap4);
        };
        push(0,stage,state,0,-1);int count=0;
        while(!heap.empty() && count<cfg.window_expansions) {
            const Node node=remove(cfg.window_heap4);
            if(costs[node.id]!=node.g)continue;
            ++count;++expanded;
            const int s=node.state,k=node.stage,t=node.time;
            if(t==horizon) {
                path.resize(horizon+1);int id=node.id;
                for(int j=horizon;j>=0;--j){path[j]=id%g.states;id=parent[id];}
                return true;
            }
            const int cell=s/4,dir=s%4,forward=g.next[cell][dir];
            const std::array<int,4> options={forward<0?-1:forward*4+dir,cell*4+(dir+1)%4,cell*4+(dir+3)%4,s};
            for(int next:options)if(next>=0 && reserve.allowed(t,cell,next/4)) {
                float cost=action_cost(g,cfg,s,next,chain && k<int(chain->goals.size())?chain->goals[k]:-1);
                push(t+1,arrived(chain,k,next),next,node.g+cost,node.id);
            }
        }
        // A failed bounded repair preserves the entire previously legal plan.
        return false;
    }
};
struct Island { Paths paths;Cost cost;uint64_t expansions=0;int accepted=0,skipped_sorts=0; };
}

void Engine::window_plan(const Frame& initial,const SharedEnvironment& env,std::vector<Action>& plan) {
    const auto& g=*graph;const int n=int(initial.loc.size()),h=cfg.window;
    // Initial calls have a weak or absent retained plan. Their declared fixed
    // budget can reserve headroom without any elapsed-time early return.
    const int iterations=env.curr_timestep<cfg.window_initial_steps && cfg.window_first_iterations>0
        ?cfg.window_first_iterations:cfg.window_iterations;
    auto agent_weight=[&](int a) {return score_weights_.empty()?1.0:score_weights_[a];};
    auto agent_cost=[&](int a,const std::vector<int>& path) {
        auto cost=path_cost(g,cfg,assigned_[a],initial.stage[a],path);
        const double weight=agent_weight(a);cost.total*=weight;cost.remaining*=weight;cost.integral*=weight;
        return cost;
    };
    auto total_cost=[&](const Paths& paths) {
        Cost out;
        for(int a=0;a<n;++a) {
            auto c=agent_cost(a,paths[a]);
            out.total+=c.total;out.remaining+=c.remaining;out.integral+=c.integral;
        }
        return out;
    };
    auto validate=[&](const Paths& paths) {
        std::vector<int> from(n),to(n);
        if(int(paths.size())!=n)throw std::runtime_error("window team size mismatch");
        for(int a=0;a<n;++a) {
            if(int(paths[a].size())!=h+1 || paths[a][0]!=initial.loc[a]*4+initial.dir[a])
                throw std::runtime_error("window start mismatch");
        }
        for(int t=1;t<=h;++t) {
            for(int a=0;a<n;++a) {
                int u=paths[a][t-1],v=paths[a][t];from[a]=u/4;to[a]=v/4;
                bool legal=(u/4==v/4 && (v%4-u%4+4)%4!=2) ||
                    (u%4==v%4 && g.next[u/4][u%4]==v/4);
                if(!legal)throw std::runtime_error("window action is kinematically illegal");
            }
            certify(g,from,to);
        }
    };
    // Shift a short prefix from the last plan and regenerate the tail with the
    // collision-free pipeline. Also compare a completely fresh pipeline seed.
    // Neither seed locks the prefix against LNS revisions.
    auto seed=[&](bool retain,const std::vector<float>& offsets) {
        Paths paths(n,std::vector<int>(h+1));Frame frame=initial;
        int keep=retain?std::min(cfg.window_keep,h-1):0;
        for(int a=0;a<n;++a) {
            paths[a][0]=initial.loc[a]*4+initial.dir[a];
            for(int t=1;t<=keep;++t)paths[a][t]=window_paths_[a][t+1];
        }
        for(int t=1;t<=keep;++t)for(int a=0;a<n;++a) {
            frame.loc[a]=paths[a][t]/4;frame.dir[a]=paths[a][t]%4;
            frame.stage[a]=arrived(assigned_[a],frame.stage[a],paths[a][t]);
        }
        frame.pending=frame.loc;
        std::vector<Action> actions;uint64_t expansions=0;
        for(int t=keep+1;t<=h;++t) {
            advance(frame,offsets,actions,expansions,true);
            for(int a=0;a<n;++a)paths[a][t]=frame.loc[a]*4+frame.dir[a];
        }
        validate(paths);return paths;
    };
    auto seed_merit=[&](const Paths& paths) {
        // Rank initialization by task progress throughout the window. Charging
        // travelled distance here can favor stationary plans in crowded states;
        // LNS still uses its ordinary complete-path objective after seeding.
        double value=0;
        for(int a=0;a<n;++a)if(assigned_[a]) {
            int stage=initial.stage[a];
            for(int t=1;t<=h;++t) {
                stage=arrived(assigned_[a],stage,paths[a][t]);
                value+=agent_weight(a)*assigned_[a]->cost(g,stage,paths[a][t]/4,paths[a][t]%4);
            }
        }
        return value;
    };
    Paths base=seed(false,best_offsets_);Cost base_cost=total_cost(base);
    double base_merit=cfg.window_starts>1?seed_merit(base):0;
    auto selected_offsets=best_offsets_;
    if(cfg.window_starts>1) {
        std::vector<Paths> starts(cfg.window_starts-1);
        std::vector<Cost> costs(cfg.window_starts-1);
        std::vector<double> merits(cfg.window_starts-1);
        std::vector<std::vector<float>> offsets(cfg.window_starts-1);
        std::vector<std::exception_ptr> failures(cfg.window_starts-1);
        #pragma omp parallel for num_threads(cfg.threads) schedule(static)
        for(int k=0;k<cfg.window_starts-1;++k) {
            try {
                std::seed_seq keys{uint32_t(cfg.seed),uint32_t(env.curr_timestep),uint32_t(k),uint32_t(0x53454544)};
                std::mt19937 random(keys);std::uniform_real_distribution<float> noise(-cfg.noise,cfg.noise);
                offsets[k].resize(n);for(float& value:offsets[k])value=noise(random);
                starts[k]=seed(false,offsets[k]);costs[k]=total_cost(starts[k]);merits[k]=seed_merit(starts[k]);
            } catch(...) {failures[k]=std::current_exception();}
        }
        for(int k=0;k<cfg.window_starts-1;++k) {
            if(failures[k])std::rethrow_exception(failures[k]);
            if(merits[k]<base_merit || (merits[k]==base_merit && better(costs[k],base_cost))) {
                base=std::move(starts[k]);base_cost=costs[k];base_merit=merits[k];selected_offsets=std::move(offsets[k]);
            }
        }
    }
    if(!window_paths_.empty()) {
        for(int a=0;a<n;++a)if(window_paths_[a].size()!=size_t(h+1) ||
              window_paths_[a][1]!=initial.loc[a]*4+initial.dir[a])
            throw std::runtime_error("persistent window disagrees with simulator");
        Paths shifted=seed(true,best_offsets_);Cost cost=total_cost(shifted);
        const double merit=cfg.window_starts>1?seed_merit(shifted):0;
        if((cfg.window_starts==1 && better(cost,base_cost)) ||
           (cfg.window_starts>1 && (merit<base_merit || (merit==base_merit && better(cost,base_cost))))) {
            base=std::move(shifted);base_cost=cost;selected_offsets=best_offsets_;
        }
    }
    // Unconstrained task-chain routes identify actual reservation blockers.
    // They depend only on visible tasks, current states and the ordinary cost
    // field. The spatial policy remains available as the exact old control.
    Paths guides;
    if(cfg.window_blockers) {
        guides.assign(n,std::vector<int>(h+1));
        for(int a=0;a<n;++a) {
            int stage=initial.stage[a],state=initial.loc[a]*4+initial.dir[a];
            guides[a][0]=state;const Chain* chain=assigned_[a];
            for(int t=1;t<=h;++t) {
                int cell=state/4,dir=state%4,forward=g.next[cell][dir],selected=state;
                float best=std::numeric_limits<float>::infinity(),best_h=best;
                const std::array<int,4> options={forward<0?-1:forward*4+dir,cell*4+(dir+1)%4,cell*4+(dir+3)%4,state};
                for(int next:options)if(next>=0) {
                    int k=arrived(chain,stage,next);
                    float remaining=chain?chain->cost(g,k,next/4,next%4):0;
                    float cost=remaining+action_cost(g,cfg,state,next,chain && stage<int(chain->goals.size())?chain->goals[stage]:-1);
                    if(cost<best || (cost==best && remaining<best_h)) {
                        best=cost;best_h=remaining;selected=next;
                    }
                }
                state=selected;stage=arrived(chain,stage,state);guides[a][t]=state;
            }
        }
    }
    std::vector<Island> islands(cfg.window_islands);
    int best=0;uint64_t expanded=0;int accepted=0,skipped_sorts=0;
    // Optional fixed rounds share the best complete plan between islands.
    // Total repair attempts per island remain window_iterations, independent
    // of wall time and worker scheduling. One round is the original control.
    for(int round=0;round<cfg.window_rounds;++round) {
    std::vector<std::exception_ptr> errors(cfg.window_islands);
    #pragma omp parallel for num_threads(cfg.threads) schedule(static)
    for(int index=0;index<cfg.window_islands;++index) {
        try {
            auto& island=islands[index];island.paths=base;island.cost=base_cost;island.accepted=0;island.skipped_sorts=0;
            Paths incumbent_paths;
            Cost current_cost=base_cost,incumbent_cost=base_cost;
            if(cfg.window_temperature>0)incumbent_paths=island.paths;
            std::seed_seq seeds{uint32_t(cfg.seed),uint32_t(env.curr_timestep),uint32_t(index),
                               uint32_t(0x57494e44)+uint32_t(round)*uint32_t(0x9e3779b9)};
            std::mt19937 random(seeds);
            Reservations reserve(g,h);
            Search fresh_search;
            thread_local Search reused_search;
            Search& search=cfg.window_reuse?reused_search:fresh_search;
            search.expanded=0;
            for(int a=0;a<n;++a)reserve.set(a,island.paths[a],true);
            std::vector<Cost> costs(n);
            for(int a=0;a<n;++a)costs[a]=agent_cost(a,island.paths[a]);
            std::vector<int> group(n);std::vector<float> keys(n);
            const int count=std::min(cfg.window_neighborhood,n);
            std::vector<int> linked;linked.reserve(count);
            std::vector<uint32_t> marked(n,0);
            for(int iteration=0;iteration<iterations/cfg.window_rounds;++iteration) {
                int pivot=int(random()%n),time=int(random()%(h+1));
                // Earliest-first blockers can repeat the same small repair
                // group. Rotate the conflict scan using its existing sampled
                // time; mode2 mixes early-first and rotated search islands.
                const int blocker_start=(cfg.window_blocker_rotation==1 ||
                    (cfg.window_blocker_rotation==2 && index%2))?time%h:0;
                // Half the neighborhoods emphasize delayed routes; the rest
                // explore uniformly. Neighbors follow current planned positions.
                if(iteration%2)for(int k=0;k<3;++k) {
                    int a=int(random()%n);
                    auto delay=[&](int b){return costs[b].total-agent_weight(b)*(assigned_[b]?
                        assigned_[b]->cost(g,initial.stage[b],initial.loc[b],initial.dir[b]):0);};
                    if(delay(a)>delay(pivot))pivot=a;
                }
                auto spatial_group=[&]() {
                for(int a=0;a<n;++a) {
                    group[a]=a;
                    keys[a]=float(g.hop(island.paths[pivot][time]/4,island.paths[a][time]/4))+
                            float(random()%1024)/512;
                }
                keys[pivot]=-1;
                std::partial_sort(group.begin(),group.begin()+count,group.end(),[&](int a,int b){
                    return keys[a]!=keys[b]?keys[a]<keys[b]:a<b;
                });
                };
                if(cfg.window_fast_groups && cfg.window_blockers) {
                    linked.clear();linked.push_back(pivot);
                    const uint32_t epoch=uint32_t(iteration)+1;marked[pivot]=epoch;
                    auto add=[&](int a) {
                        if(a>=0 && marked[a]!=epoch && int(linked.size())<count) {
                            marked[a]=epoch;linked.push_back(a);
                        }
                    };
                    for(size_t k=0;k<linked.size() && int(linked.size())<count;++k) {
                        const auto& route=guides[linked[k]];
                        for(int offset=0;offset<h && int(linked.size())<count;++offset) {
                            const int t=1+(blocker_start+offset)%h;
                            add(reserve.owner(t,route[t]/4));
                            const int crossing=reserve.owner(t-1,route[t]/4);
                            if(crossing>=0 && reserve.owner(t,route[t-1]/4)==crossing)add(crossing);
                        }
                    }
                    if(int(linked.size())<count) {
                        spatial_group();for(int a:group)add(a);
                    } else {
                        // The original spatial ranking is overwritten entirely.
                        // Consume exactly its draws so every later choice agrees.
                        random.discard(n);++island.skipped_sorts;
                    }
                    std::copy(linked.begin(),linked.end(),group.begin());
                } else {
                    spatial_group();
                if(cfg.window_blockers) {
                    std::vector<int> linked{pivot};std::vector<unsigned char> marked(n,0);marked[pivot]=1;
                    auto add=[&](int a) {
                        if(a>=0 && !marked[a] && int(linked.size())<count){marked[a]=1;linked.push_back(a);}
                    };
                    for(size_t k=0;k<linked.size() && int(linked.size())<count;++k) {
                        const auto& route=guides[linked[k]];
                        for(int offset=0;offset<h && int(linked.size())<count;++offset) {
                            const int t=1+(blocker_start+offset)%h;
                            add(reserve.owner(t,route[t]/4));
                            const int crossing=reserve.owner(t-1,route[t]/4);
                            if(crossing>=0 && reserve.owner(t,route[t-1]/4)==crossing)add(crossing);
                        }
                    }
                    for(int a:group)add(a);
                    std::copy(linked.begin(),linked.end(),group.begin());
                }
                }
                std::shuffle(group.begin(),group.begin()+count,random);
                std::vector<std::vector<int>> old(count),replacement(count);
                Cost previous,proposed;
                for(int k=0;k<count;++k) {
                    int a=group[k];old[k]=island.paths[a];reserve.set(a,old[k],false);
                    previous.total+=costs[a].total;previous.remaining+=costs[a].remaining;previous.integral+=costs[a].integral;
                }
                int planned=0;
                for(;planned<count;++planned) {
                    int a=group[planned];
                    if(!search.solve(g,cfg,reserve,assigned_[a],initial.stage[a],initial.loc[a]*4+initial.dir[a],replacement[planned]))break;
                    reserve.set(a,replacement[planned],true);
                    auto cost=agent_cost(a,replacement[planned]);
                    proposed.total+=cost.total;proposed.remaining+=cost.remaining;proposed.integral+=cost.integral;
                }
                const bool equal=std::abs(proposed.total-previous.total)<=1e-8 &&
                                 std::abs(proposed.remaining-previous.remaining)<=1e-8 &&
                                 std::abs(proposed.integral-previous.integral)<=1e-8;
                bool accept=planned==count && (better(proposed,previous) ||
                    (cfg.window_equal && equal && replacement!=old));
                if(!accept && planned==count && cfg.window_temperature>0 && replacement!=old) {
                    // Explore complete legal repairs above the incumbent, then
                    // return the best complete plan visited, never this walk's
                    // possibly worse endpoint. Cooling uses iteration count.
                    const double fraction=1-double(iteration)/std::max(1,iterations/cfg.window_rounds);
                    const double temperature=cfg.window_temperature*fraction;
                    const double increase=std::max(0.0,proposed.total-previous.total);
                    accept=std::generate_canonical<double,32>(random)<std::exp(-increase/temperature);
                }
                if(accept) {
                    ++island.accepted;
                    for(int k=0;k<count;++k) {
                        int a=group[k];island.paths[a]=std::move(replacement[k]);
                        costs[a]=agent_cost(a,island.paths[a]);
                    }
                    if(cfg.window_temperature>0) {
                        current_cost.total+=proposed.total-previous.total;
                        current_cost.remaining+=proposed.remaining-previous.remaining;
                        current_cost.integral+=proposed.integral-previous.integral;
                        if(better(current_cost,incumbent_cost)) {
                            // Recompute before retaining a new best so any
                            // accumulated delta rounding cannot win selection.
                            current_cost=total_cost(island.paths);
                            if(better(current_cost,incumbent_cost)) {
                                incumbent_cost=current_cost;incumbent_paths=island.paths;
                            }
                        }
                    }
                } else {
                    for(int k=0;k<planned;++k)reserve.set(group[k],replacement[k],false);
                    for(int k=0;k<count;++k)reserve.set(group[k],old[k],true);
                }
            }
            if(cfg.window_temperature>0)island.paths=std::move(incumbent_paths);
            island.cost=total_cost(island.paths);island.expansions=search.expanded;
        } catch(...) {errors[index]=std::current_exception();}
    }
    best=0;
    for(int k=0;k<cfg.window_islands;++k) {
        if(errors[k])std::rethrow_exception(errors[k]);
        expanded+=islands[k].expansions;accepted+=islands[k].accepted;skipped_sorts+=islands[k].skipped_sorts;
        if(better(islands[k].cost,islands[best].cost))best=k;
    }
    if(cfg.window_merge) {
        Paths merged=islands[best].paths;
        const Cost before=islands[best].cost;
        std::vector<Cost> current(n);
        for(int a=0;a<n;++a)current[a]=agent_cost(a,merged[a]);
        for(int index=0;index<cfg.window_islands;++index)if(index!=best) {
            const auto& alternative=islands[index].paths;
            const auto groups=window_conflict_components(g,merged,alternative);
            for(const auto& group:groups) {
                Cost old_cost,new_cost;std::vector<Cost> replacement;replacement.reserve(group.size());
                for(int a:group) {
                    old_cost.total+=current[a].total;old_cost.remaining+=current[a].remaining;old_cost.integral+=current[a].integral;
                    replacement.push_back(agent_cost(a,alternative[a]));
                    new_cost.total+=replacement.back().total;new_cost.remaining+=replacement.back().remaining;new_cost.integral+=replacement.back().integral;
                }
                // Never trade increased path cost for a secondary improvement:
                // tiny accepted increases could otherwise accumulate by group.
                if(new_cost.total<=old_cost.total && better(new_cost,old_cost)) {
                    for(size_t k=0;k<group.size();++k) {
                        const int a=group[k];merged[a]=alternative[a];current[a]=replacement[k];
                    }
                }
            }
        }
        const Cost after=total_cost(merged);validate(merged);
        if(better(before,after))throw std::runtime_error("window component merge worsened its incumbent");
        if(better(after,before)){islands[best].paths=std::move(merged);islands[best].cost=after;}
    }
    if(better(base_cost,islands[best].cost))throw std::runtime_error("window sharing round worsened its complete seed");
    if(round+1<cfg.window_rounds) {
        base=islands[best].paths;base_cost=islands[best].cost;
    }
    }
    auto& chosen=islands[best].paths;validate(chosen);
    if(better(base_cost,islands[best].cost))throw std::runtime_error("window LNS worsened the complete seed plan");
    plan.resize(n);predicted_loc_.resize(n);predicted_dir_.resize(n);
    for(int a=0;a<n;++a) {
        int from=chosen[a][0],to=chosen[a][1],delta=(to%4-from%4+4)%4;
        plan[a]=from/4!=to/4?FW:delta==1?CR:delta==3?CCR:W;
        predicted_loc_[a]=to/4;predicted_dir_[a]=to%4;
    }
    pending_=predicted_loc_;window_paths_=std::move(chosen);best_offsets_=std::move(selected_offsets);
    if(!quiet_ && (env.curr_timestep<5 || env.curr_timestep%100==0))
        std::fprintf(stderr,"R05_WINDOW t=%d horizon=%d islands=%d iterations=%d rounds=%d accepted=%d skipped_sorts=%d expansions=%llu cost=%.3f base=%.3f\n",
            env.curr_timestep,h,cfg.window_islands,iterations,cfg.window_rounds,accepted,skipped_sorts,
            (unsigned long long)expanded,islands[best].cost.total,base_cost.total);
}
}
