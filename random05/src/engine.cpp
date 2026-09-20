#include "engine.hpp"
#include <algorithm>
#include <cmath>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <limits>
#include <numeric>
#include <queue>
#include <stdexcept>
#include <unordered_set>
#include <omp.h>

namespace r05 {
namespace {
constexpr float INF=1e20f;
int integer(const char* key,int value) { const char* v=std::getenv(key);return v?std::stoi(v):value; }
float real(const char* key,float value) { const char* v=std::getenv(key);return v?std::stof(v):value; }
int turn(int a,int b) { const int d=(a-b+4)%4;return std::min(d,4-d); }
constexpr int ranking_cache_slots=64;
struct PolicyScratch {
    std::vector<int> p,moving,owner,chosen,reserve,idle_heading,forced_heading,candidate_count,order,prepared,intent;
    std::vector<float> base_cost,priorities;
    std::vector<const Chain*> active_chain;
    std::vector<const float*> cost_table;
    std::vector<std::array<MoveCandidate,5>> candidates;
    std::vector<uint64_t> priority_keys, radix_buffer;
    std::vector<CachedRanking*> ranking_slots;
    std::vector<unsigned char> ranking_hits;
};
}
Config Config::environment(const SharedEnvironment& env) {
    Config c;
    c.futures=integer("R05_K",c.futures);c.depth=integer("R05_DEPTH",c.depth);
    c.generations=integer("R05_GENERATIONS",1);
    c.continuations=integer("R05_CONTINUATIONS",1);
    c.continuation_start=integer("R05_CONTINUATION_START",1);
    c.future_mutation=real("R05_FUTURE_MUTATION",0.3);
    c.continuation_risk=real("R05_CONTINUATION_RISK",0);
    if(!std::isfinite(c.continuation_risk))
        throw std::invalid_argument("continuation coefficient must be finite");
    c.share_prefix=integer("R05_SHARE_PREFIX",0);
    c.packed_order=integer("R05_PACKED_ORDER",0);c.fast_dispersion=integer("R05_FAST_DISPERSION",0);
    c.scratch_reuse=integer("R05_SCRATCH_REUSE",0);c.profile=integer("R05_PROFILE",0);
    c.goal_cache=integer("R05_GOAL_CACHE",0);
    c.policy_profile=integer("R05_POLICY_PROFILE",0);
    c.radix_order=integer("R05_RADIX_ORDER",0);
    c.candidate_cache=integer("R05_CANDIDATE_CACHE",0);
    if(c.continuations<1 || c.futures<1 || c.futures%c.continuations ||
       c.generations>c.futures/c.continuations || c.continuation_start<1 ||
       (c.continuations>1 && c.continuation_start>=c.depth) ||
       !std::isfinite(c.future_mutation) || c.future_mutation<0 || c.future_mutation>1)
        throw std::invalid_argument("continuations must divide K and preserve at least the first decision");
    c.threads=integer("R05_THREADS",c.threads);c.seed=integer("R05_SEED",c.seed);
    c.noise=real("R05_NOISE",c.noise);c.mutation=real("R05_MUTATION",c.mutation);
    c.mutation_radius=integer("R05_MUTATION_RADIUS",0);
    if(c.mutation_radius<0)throw std::invalid_argument("mutation radius must be nonnegative");
    c.dispersion=real("R05_DISPERSION",c.dispersion);c.push_price=real("R05_PUSH",c.push_price);
    c.loop_threshold=real("R05_LOOP_THRESHOLD",c.loop_threshold);
    c.length_weight=real("R05_LENGTH_WEIGHT",c.length_weight);c.keep_bonus=real("R05_KEEP_BONUS",c.keep_bonus);
    c.initial_length_weight=real("R05_INITIAL_LENGTH_WEIGHT",-1);
    c.initial_length_steps=integer("R05_INITIAL_LENGTH_STEPS",250);
    if((c.initial_length_weight<0 && c.initial_length_weight!=-1) || c.initial_length_steps<0)
        throw std::invalid_argument("invalid initial task-length preference");
    if(c.initial_length_weight>=0 && env.trick_instance!="RANDOM-05")
        throw std::invalid_argument("initial task-length preference requires --trick RANDOM-05");
    c.turn_cost=real("R05_TURN_COST",c.turn_cost);c.wait_cost=real("R05_WAIT_COST",c.wait_cost);
    c.matching=integer("R05_MATCH",1);c.loops=integer("R05_LOOPS",1);c.deadends=integer("R05_DEADENDS",1);
    c.progress_discount=real("R05_PROGRESS_DISCOUNT",1);c.flow_turn_load=real("R05_FLOW_TURN_LOAD",0);
    c.plain_score=real("R05_PLAIN_SCORE",0);
    c.reverse_penalty=real("R05_REVERSE_PENALTY",0);
    c.completion_bonus=real("R05_COMPLETE_BONUS",0);
    if(c.completion_bonus<0)throw std::invalid_argument("completion bonus must be nonnegative");
    if(c.reverse_penalty<0)throw std::invalid_argument("reverse-turn penalty must be nonnegative");
    if(c.plain_score<0 || c.plain_score>1)
        throw std::invalid_argument("plain score blend must be in [0,1]");
    if(c.progress_discount<=0 || c.progress_discount>1 || c.flow_turn_load<0)
        throw std::invalid_argument("discount must be in (0,1] and turn-load multiplier nonnegative");
    c.cycle_portfolio=integer("R05_CYCLE_PORTFOLIO",0);
    c.early_fill=integer("R05_EARLY_FILL",0);c.early_fill_gain=real("R05_EARLY_FILL_GAIN",0);
    if(c.early_fill_gain<0)throw std::invalid_argument("early-fill threshold must be nonnegative");
    c.operation_depth=integer("R05_OPERATIONS",0);
    c.operation_revisits=integer("R05_OPERATION_REVISITS",4);
    c.operation_inherit=integer("R05_OPERATION_INHERIT",1);
    c.operation_moving=integer("R05_OPERATION_MOVING",0);
    c.operation_protect=integer("R05_OPERATION_PROTECT",0);
    c.operation_finish_move=integer("R05_OPERATION_FINISH_MOVE",0);
    c.operation_cost_weight=real("R05_OPERATION_COST",0);
    if((c.operation_depth!=0 && c.operation_depth!=3) || c.operation_revisits<1 || c.operation_cost_weight<0)
        throw std::invalid_argument("operations must be off or length3, with positive revisit limit and nonnegative cost weight");
    c.idle_eviction=real("R05_IDLE_EVICTION",0);
    c.pre_cycles=integer("R05_PRE_CYCLES",0);c.pre_cycle_gain=real("R05_PRE_CYCLE_GAIN",0);
    c.random_by_step=integer("R05_RANDOM_BY_STEP",0);c.age_cap=integer("R05_AGE_CAP",0);
    if(c.age_cap>0 && env.trick_instance!="RANDOM-05")
        throw std::invalid_argument("capped priority aging requires --trick RANDOM-05");
    c.chain_matching=integer("R05_SCHED_CHAIN",0);c.hungarian_limit=integer("R05_HUNGARIAN",0);c.prospective_wait=integer("R05_PROSPECTIVE_WAIT",0);
    c.local_trials=integer("R05_LOCAL",0);c.horizon=integer("R05_HORIZON",0);
    c.triage_scale=real("R05_TRIAGE_SCALE",c.triage_scale);c.accept_equal=integer("R05_EQUAL",0);
    if(c.horizon>0 && env.trick_instance!="RANDOM-05")
        throw std::invalid_argument("known-horizon triage requires --trick RANDOM-05");
    c.intent_rotation=integer("R05_INTENT_ROTATION",1);
    c.intent_mode=integer("R05_INTENT_MODE",0);
    if(c.intent_mode<0 || c.intent_mode>2 || (c.intent_mode && !c.intent_rotation))
        throw std::invalid_argument("intent mode must be 0..2 and requires intent rotation");
    c.flow_seed=integer("R05_FLOW_SEED",c.flow_seed);c.flow_iterations=integer("R05_FLOW_ITERS",c.flow_iterations);
    c.rollout_age=integer("R05_ROLLOUT_AGE",0);c.rollout_match=integer("R05_ROLLOUT_MATCH",0);c.cost_cache=integer("R05_COST_CACHE",0);
    c.pocket_components=integer("R05_POCKET_COMPONENTS",0);
    c.flow_turn=real("R05_FLOW_TURN",0);
    c.flow_power=real("R05_FLOW_POWER",1);c.flow_alpha=real("R05_FLOW_ALPHA",1);
    c.flow_confidence_power=real("R05_FLOW_CONFIDENCE_POWER",0);
    if(!std::isfinite(c.flow_confidence_power) || c.flow_confidence_power<0)
        throw std::invalid_argument("flow confidence exponent must be finite and nonnegative");
    c.flow_betweenness=real("R05_FLOW_BETWEENNESS",0);
    c.flow_average=integer("R05_FLOW_AVERAGE",0);c.flow_normalize=integer("R05_FLOW_NORMALIZE",0);c.loop_extent=integer("R05_LOOP_EXTENT",2);
    c.predict_matching=integer("R05_SCHED_PREDICT",0);
    c.flow_penalty=real("R05_FLOW_PENALTY",c.flow_penalty);c.guided_matching=integer("R05_SCHED_GUIDE",0);
    c.flow_output_penalty=real("R05_FLOW_OUTPUT_PENALTY",-1);
    c.flow_normalize_ref=real("R05_FLOW_NORMALIZE_REF",-1);
    if(c.flow_normalize_ref<0 && c.flow_normalize_ref!=-1)
        throw std::invalid_argument("flow normalization reference must be nonnegative or -1");
    if(c.flow_output_penalty<0 && c.flow_output_penalty!=-1)
        throw std::invalid_argument("flow output penalty must be nonnegative or -1 for the assignment penalty");
    if(const char* v=std::getenv("R05_GUIDANCE")) c.guidance=v;
    if(const char* v=std::getenv("R05_WEIGHTS")) c.weights=v;
    c.flow_flips=integer("R05_FLOW_FLIPS",0);c.flow_flip_seed=integer("R05_FLOW_FLIP_SEED",1);
    if(c.flow_flips<0 || (c.flow_flips && c.guidance!="flow"))
        throw std::invalid_argument("field flips require flow guidance and a nonnegative count");
    if(c.guidance!="none" && env.trick_instance!="RANDOM-05")
        throw std::invalid_argument("guidance experiments require --trick RANDOM-05");
    if(c.futures<1 || c.generations<1 || c.generations>c.futures || c.depth<1 || c.threads<1 || c.depth>64 || c.turn_cost<=0 ||
       c.wait_cost<=0 || c.mutation<0 || c.mutation>1)
        throw std::invalid_argument("invalid R05 configuration");
    return c;
}

static void order_priorities(const std::vector<float>& priorities,bool packed,bool radix,
                             std::vector<int>& order,std::vector<uint64_t>& keys,
                             std::vector<uint64_t>& buffer) {
    order.resize(priorities.size());
    std::iota(order.begin(),order.end(),0);
    if((!packed && !radix) || !std::all_of(priorities.begin(),priorities.end(),[](float p){return std::isfinite(p);})) {
        std::stable_sort(order.begin(),order.end(),[&](int a,int b){return priorities[a]>priorities[b];});
        return;
    }
    static_assert(sizeof(float)==sizeof(uint32_t) && std::numeric_limits<float>::is_iec559,
                  "packed priorities require IEEE binary32");
    // Sort the float value and original agent ID together. This avoids an
    // indirect float lookup per comparison and preserves stable tie ordering.
    // Normalize signed zero because the original float comparator equates them.
    keys.resize(priorities.size());
    for(size_t a=0;a<priorities.size();++a) {
        float p=priorities[a]==0?0.0f:priorities[a];uint32_t bits;
        std::memcpy(&bits,&p,sizeof(bits));
        uint32_t ascending=(bits&0x80000000u)?~bits:(bits^0x80000000u);
        keys[a]=(uint64_t(~ascending)<<32)|uint32_t(a);
    }
    if(radix) {
        // Input is already in ascending agent-ID order. Four stable byte
        // passes on the high (float-priority) word therefore produce exactly
        // the packed 64-bit order, including equal priorities and signed zero.
        std::array<std::array<uint32_t,256>,4> counts{};
        for(uint64_t key:keys)for(int pass=0;pass<4;++pass)
            ++counts[pass][(key>>(32+pass*8))&255];
        buffer.resize(keys.size());
        for(int pass=0;pass<4;++pass) {
            uint32_t sum=0;int occupied=0;
            for(auto& count:counts[pass]) {
                occupied+=count>0;uint32_t next=sum+count;count=sum;sum=next;
            }
            if(occupied<=1)continue;
            for(uint64_t key:keys)buffer[counts[pass][(key>>(32+pass*8))&255]++]=key;
            keys.swap(buffer);
        }
    } else std::sort(keys.begin(),keys.end());
    for(size_t k=0;k<keys.size();++k)order[k]=int(uint32_t(keys[k]));
}
std::vector<int> priority_order(const std::vector<float>& priorities,bool packed,bool radix) {
    std::vector<int> order;std::vector<uint64_t> keys,buffer;
    order_priorities(priorities,packed,radix,order,keys,buffer);
    return order;
}

int Graph::nearby_pairs(const std::vector<int>& locations) const {
    std::vector<unsigned char> occupied(cells,0);
    for(int v:locations)occupied[v]=1;
    int pairs=0;
    if(locations.size()<=size_t(cells/2)) {
        for(int v:locations)for(int u:nearby[v])if(u>v)pairs+=occupied[u];
    } else {
        // Count all free-cell pairs, remove edges incident to a hole, then add
        // back hole-hole pairs that the degree subtraction removed twice.
        pairs=all_nearby_pairs;
        for(int v=0;v<cells;++v)if(!occupied[v]) {
            pairs-=int(nearby[v].size());
            for(int u:nearby[v])if(u>v && !occupied[u])++pairs;
        }
    }
    return pairs;
}

Graph::Graph(const SharedEnvironment& env,const Config& cfg) {
    rows=env.rows;cols=env.cols;from_grid.assign(env.map.size(),-1);
    for(int i=0;i<int(env.map.size());++i) if(!env.map[i]) {
        from_grid[i]=int(to_grid.size());to_grid.push_back(i);
    }
    cells=int(to_grid.size());states=4*cells;
    if(cells>4096) throw std::invalid_argument("initial exact-table implementation limited to 4096 free cells");
    next.resize(cells);weight.resize(cells);degree.assign(cells,0);
    nearby.resize(cells);
    for(int v=0;v<cells;++v) {
        const int p=to_grid[v],x=p%cols,y=p/cols;
        for(int yy=std::max(0,y-2);yy<=std::min(rows-1,y+2);++yy)
            for(int xx=std::max(0,x-2);xx<=std::min(cols-1,x+2);++xx) {
                int u=from_grid[yy*cols+xx];if(u<0 || u==v)continue;
                nearby[v].push_back(u);all_nearby_pairs+=u>v;
            }
    }
    for(int v=0;v<cells;++v) {
        int p=to_grid[v],x=p%cols,y=p/cols;
        const int dx[]={1,0,-1,0},dy[]={0,1,0,-1};
        for(int d=0;d<4;++d) {
            int xx=x+dx[d],yy=y+dy[d];
            int u=(xx<0 || xx>=cols || yy<0 || yy>=rows)?-1:from_grid[yy*cols+xx];
            next[v][d]=u;degree[v]+=u>=0;
            weight[v][d]=2;
            if(cfg.guidance=="lanes") {
                bool preferred=d%2==0 ? d==((y%2)?2:0) : d==((x%2)?1:3);
                weight[v][d]=preferred?2:6;
            }
        }
        weight[v][4]=cfg.turn_cost;
    }
    if(cfg.guidance=="file") {
        std::ifstream f(cfg.weights);
        if(!f) throw std::invalid_argument("cannot open guidance weights");
        nlohmann::json j;f>>j;
        if(j.is_object()) j=j.at("weights");
        if(j.size()!=env.map.size()*5) throw std::invalid_argument("guidance array must have five weights per grid cell");
        double sum=0;int count=0;
        for(int v=0;v<cells;++v)for(int d=0;d<4;++d)if(next[v][d]>=0) {
            sum+=j.at(size_t(to_grid[v])*5+d).get<float>();++count;
        }
        float scale=2*count/sum;
        for(int v=0;v<cells;++v)for(int d=0;d<5;++d) {
            float w=j.at(size_t(to_grid[v])*5+d).get<float>();
            if(!std::isfinite(w)||w<=0)throw std::invalid_argument("guidance costs must be finite and positive");
            weight[v][d]=std::max(0.01f,w*scale);
        }
    } else if(cfg.guidance!="none" && cfg.guidance!="lanes" && cfg.guidance!="flow")
        throw std::invalid_argument("R05_GUIDANCE must be none, lanes, flow or file");
    if(cfg.guidance=="flow") {
        // Traffic assignment with uniform all-pairs demand. Opposing traffic is
        // priced more strongly than co-flow, then the resulting dominant edge
        // directions are rounded into soft one-way preferences.
        std::mt19937 random(cfg.flow_seed);
        std::uniform_real_distribution<float> jitter(0,0.2f);
        std::vector<std::array<float,4>> price(cells),bias(cells);
        std::vector<std::array<double,4>> flow(cells),average(cells);
        for(int v=0;v<cells;++v)for(int d=0;d<4;++d)price[v][d]=bias[v][d]=1+jitter(random);
        for(int iteration=0;iteration<cfg.flow_iterations;++iteration) {
            std::vector<std::vector<std::array<double,4>>> partial(
                cfg.threads,std::vector<std::array<double,4>>(cells));
            #pragma omp parallel for num_threads(cfg.threads) schedule(static)
            for(int source=0;source<cells;++source) {
                auto& accumulator=partial[omp_get_thread_num()];
                if(cfg.flow_turn>0) {
                    // Route demand in orientation space so field construction
                    // prices the turns required by the actual robot kinematics.
                    std::vector<float> d(states,INF);
                    std::vector<int> parent(states,-1),parent_dir(states,-1),order;
                    std::vector<double> demand(states,0);
                    using Item=std::pair<float,int>;
                    std::priority_queue<Item,std::vector<Item>,std::greater<Item>> q;
                    for(int o=0;o<4;++o){d[source*4+o]=0;q.emplace(0,source*4+o);}
                    while(!q.empty()) {
                        auto [cost,state]=q.top();q.pop();if(cost!=d[state])continue;
                        order.push_back(state);int v=state/4,o=state%4;
                        auto relax=[&](int dest,float w,int dir) {
                            float value=cost+w;
                            if(value<d[dest]){d[dest]=value;parent[dest]=state;parent_dir[dest]=dir;q.emplace(value,dest);}
                        };
                        int u=next[v][o];if(u>=0)relax(u*4+o,price[v][o],o);
                        relax(v*4+(o+1)%4,cfg.flow_turn,4);relax(v*4+(o+3)%4,cfg.flow_turn,4);
                    }
                    for(int v=0;v<cells;++v) {
                        int end=v*4;for(int o=1;o<4;++o)if(d[v*4+o]<d[end])end=v*4+o;
                        demand[end]=1;
                    }
                    for(auto it=order.rbegin();it!=order.rend();++it) {
                        int v=*it,u=parent[v];if(u<0)continue;
                        if(parent_dir[v]<4)accumulator[u/4][parent_dir[v]]+=demand[v];
                        demand[u]+=demand[v];
                    }
                } else {
                std::vector<float> d(cells,INF);
                std::vector<int> parent(cells,-1),parent_dir(cells,-1),order;
                std::vector<double> demand(cells,1);
                using Item=std::pair<float,int>;
                std::priority_queue<Item,std::vector<Item>,std::greater<Item>> q;
                d[source]=0;q.emplace(0,source);
                while(!q.empty()) {
                    auto [cost,v]=q.top();q.pop();if(cost!=d[v])continue;
                    order.push_back(v);
                    for(int dir=0;dir<4;++dir) {
                        int u=next[v][dir];if(u<0)continue;
                        float candidate=cost+price[v][dir];
                        if(candidate<d[u]) {d[u]=candidate;parent[u]=v;parent_dir[u]=dir;q.emplace(candidate,u);}
                    }
                }
                for(auto it=order.rbegin();it!=order.rend();++it) {
                    int v=*it,u=parent[v];if(u<0)continue;
                    accumulator[u][parent_dir[v]]+=demand[v];demand[u]+=demand[v];
                }
                }
            }
            double sum=0;int edges=0;
            for(int v=0;v<cells;++v)for(int d=0;d<4;++d) {
                flow[v][d]=0;
                for(int t=0;t<cfg.threads;++t)flow[v][d]+=partial[t][v][d];
                if(cfg.flow_average) {
                    average[v][d]+=(flow[v][d]-average[v][d])/(iteration+1);
                    flow[v][d]=average[v][d];
                }
                if(next[v][d]>=0){sum+=flow[v][d];++edges;}
            }
            const double mean=std::max(1.0,sum/edges);
            for(int v=0;v<cells;++v)for(int d=0;d<4;++d) {
                int u=next[v][d];if(u<0)continue;
                float target=bias[v][d]+float((flow[v][d]+cfg.flow_penalty*flow[u][(d+2)%4])/mean);
                if(cfg.flow_power!=1 || cfg.flow_alpha!=1)
                    target=bias[v][d]+cfg.flow_alpha*std::pow(float(flow[v][d]/mean),cfg.flow_power)
                         +cfg.flow_penalty*std::pow(float(flow[u][(d+2)%4]/mean),cfg.flow_power);
                price[v][d]=0.65f*price[v][d]+0.35f*target;
            }
        }
        std::vector<double> load(cells,0);double total_load=0;
        for(int v=0;v<cells;++v)for(int d=0;d<4;++d)if(next[v][d]>=0)
            load[v]+=flow[v][d]+flow[next[v][d]][(d+2)%4];
        for(double x:load)total_load+=x;
        const double mean_load=std::max(1.0,total_load/cells);
        if(cfg.flow_turn_load>0)for(int v=0;v<cells;++v)
            weight[v][4]*=float((1+cfg.flow_turn_load*load[v]/mean_load)/(1+cfg.flow_turn_load));
        // Separate layout construction from its final directional contrast.
        // This allows controlled cost-field comparisons on the same streets.
        const float output_penalty=cfg.flow_output_penalty<0?cfg.flow_penalty:cfg.flow_output_penalty;
        double weight_sum=0;int weight_count=0;
        for(int v=0;v<cells;++v)for(int d=0;d<4;++d) {
            int u=next[v][d];if(u<0)continue;
            // Near-balanced aggregate demand provides weak evidence for a
            // preferred direction. Optionally soften only those opposing-edge
            // penalties while preserving the constructed direction ordering.
            const double forward=flow[v][d],reverse=flow[u][(d+2)%4];
            const float confidence=cfg.flow_confidence_power>0
                ?std::pow(float(std::abs(forward-reverse)/std::max(1.0,forward+reverse)),cfg.flow_confidence_power):1;
            weight[v][d]=2*(forward>=reverse?1:1+output_penalty*confidence);
            const float load_factor=1+cfg.flow_betweenness*float((load[v]+load[u])/(2*mean_load));
            weight[v][d]*=load_factor;
            if(cfg.flow_normalize_ref>=0) {
                // Hold the preferred-direction scale fixed while changing the
                // price of opposing traffic. Otherwise global normalization
                // also changes the relative prices of turns, waits and matching.
                float reference=2*(forward>=reverse?1:1+cfg.flow_normalize_ref*confidence);
                reference*=load_factor;weight_sum+=reference;
            } else weight_sum+=weight[v][d];
            ++weight_count;
        }
        if(cfg.flow_normalize) {
            float scale=float(2*weight_count/weight_sum);
            for(auto& w:weight)for(int d=0;d<4;++d)w[d]*=scale;
        }
    }
    if(cfg.flow_flips) {
        // Mutate the existing field without changing its cost scale or removing
        // physical edges. Equal-cost pairs have no direction to reverse.
        std::vector<std::pair<int,int>> edges;
        for(int v=0;v<cells;++v)for(int d=0;d<4;++d) {
            int u=next[v][d];
            if(u>v && weight[v][d]!=weight[u][(d+2)%4])edges.emplace_back(v,d);
        }
        if(cfg.flow_flips>int(edges.size()))throw std::invalid_argument("too many distinct guidance flips");
        std::mt19937 random(cfg.flow_flip_seed);std::shuffle(edges.begin(),edges.end(),random);
        for(int k=0;k<cfg.flow_flips;++k) {
            auto [v,d]=edges[k];int u=next[v][d];
            std::swap(weight[v][d],weight[u][(d+2)%4]);
        }
    }
    if(cfg.loop_extent<2 || cfg.loop_extent>8)throw std::invalid_argument("cycle extent must be 2..8");
    if(cfg.loops)for(int height=2;height<=cfg.loop_extent;++height)
    for(int width=2;width<=cfg.loop_extent;++width)
    for(int y=0;y+height<=rows;++y)for(int x=0;x+width<=cols;++x) {
        std::vector<int> ring;
        for(int xx=x;xx<x+width;++xx)ring.push_back(from_grid[y*cols+xx]);
        for(int yy=y+1;yy<y+height;++yy)ring.push_back(from_grid[yy*cols+x+width-1]);
        for(int xx=x+width-2;xx>=x;--xx)ring.push_back(from_grid[(y+height-1)*cols+xx]);
        for(int yy=y+height-2;yy>y;--yy)ring.push_back(from_grid[yy*cols+x]);
        if(std::all_of(ring.begin(),ring.end(),[](int v){return v>=0;}))cycles.push_back(std::move(ring));
    }
    // Peel tree pockets. Agents exiting these get precedence over agents entering.
    pocket.assign(cells,0);
    auto deg=degree;std::queue<int> leaves;
    for(int v=0;v<cells;++v)if(deg[v]<=1)leaves.push(v);
    while(!leaves.empty()) {
        int v=leaves.front();leaves.pop();pocket[v]=1;
        for(int u:next[v])if(u>=0 && !pocket[u] && --deg[u]==1)leaves.push(u);
    }
    if(cfg.pocket_components) {
        auto peeled=pocket;std::fill(pocket.begin(),pocket.end(),0);int component=0;
        for(int seed=0;seed<cells;++seed)if(peeled[seed] && !pocket[seed]) {
            ++component;std::vector<int> q={seed};pocket[seed]=component;
            for(size_t k=0;k<q.size();++k)for(int u:next[q[k]])
                if(u>=0 && peeled[u] && !pocket[u]){pocket[u]=component;q.push_back(u);}
        }
    }
    // A goal-less robot in a peeled tree pocket must prefer leaving it over
    // waiting. Otherwise its exit priority reserves the doorway indefinitely.
    pocket_depth.assign(cells,0);
    std::vector<int> exits;
    for(int v=0;v<cells;++v)if(!pocket[v])exits.push_back(v);
    for(size_t k=0;k<exits.size();++k)for(int u:next[exits[k]])
        if(u>=0 && pocket[u] && pocket_depth[u]==0) {
            pocket_depth[u]=pocket_depth[exits[k]]+1;exits.push_back(u);
        }
    hops.assign(size_t(cells)*cells,65535);
    #pragma omp parallel for num_threads(cfg.threads) schedule(static)
    for(int target=0;target<cells;++target) {
        auto* h=hops.data()+size_t(target)*cells;
        std::vector<int> q(cells);int read=0,end=1;q[0]=target;h[target]=0;
        while(read<end) {
            int v=q[read++];
            for(int u:next[v])if(u>=0 && h[u]==65535) { h[u]=h[v]+1;q[end++]=u; }
        }
    }
    distance.assign(size_t(states)*states,INF);
    #pragma omp parallel for num_threads(cfg.threads) schedule(dynamic,8)
    for(int target=0;target<states;++target) {
        float* d=distance.data()+size_t(target)*states;
        using Item=std::pair<float,int>;
        std::priority_queue<Item,std::vector<Item>,std::greater<Item>> q;
        d[target]=0;q.emplace(0,target);
        while(!q.empty()) {
            auto [cost,state]=q.top();q.pop();if(cost!=d[state])continue;
            const int v=state/4,o=state%4;
            auto relax=[&](int pred,float w) {
                const float value=cost+w;
                if(value<d[pred]) { d[pred]=value;q.emplace(value,pred); }
            };
            relax(v*4+(o+1)%4,weight[v][4]);relax(v*4+(o+3)%4,weight[v][4]);
            int u=next[v][(o+2)%4];
            if(u>=0)relax(u*4+o,weight[u][o]);
        }
    }
    if(cfg.goal_cache) {
        // This table depends only on the map and its costs, so preprocessing
        // may build it before any tasks or starting states are revealed.
        any_heading_distance.resize(size_t(cells)*states);
        #pragma omp parallel for num_threads(cfg.threads) schedule(static)
        for(int target=0;target<cells;++target)for(int source=0;source<states;++source) {
            float best=INF;
            for(int d=0;d<4;++d)best=std::min(best,dist(target*4+d,source));
            any_heading_distance[size_t(target)*states+source]=best;
        }
    }
}
float Graph::approach(int target,int source) const {
    if(!any_heading_distance.empty())return any_heading_distance[size_t(target)*states+source];
    float best=INF;
    for(int d=0;d<4;++d)best=std::min(best,dist(target*4+d,source));
    return best;
}
int Graph::direction(int a,int b) const {
    for(int d=0;d<4;++d)if(next[a][d]==b)return d;
    return -1;
}
Chain::Chain(const Graph& g,const Task& task,bool cache) {
    for(int p:task.locations) {
        if(p<0 || p>=int(g.from_grid.size()) || g.from_grid[p]<0)throw std::invalid_argument("task on blocked cell");
        goals.push_back(g.from_grid[p]);
    }
    tail.resize(goals.size());
    for(int k=int(goals.size())-2;k>=0;--k)
        for(int o=0;o<4;++o) {
            float best=INF;
            for(int q=0;q<4;++q)best=std::min(best,g.dist(goals[k+1]*4+q,goals[k]*4+o)+tail[k+1][q]);
            tail[k][o]=best;
        }
    if(cache) {
        values.resize(goals.size());
        for(int k=0;k<int(goals.size());++k) {
            // The final errand has no remaining tail. Borrow the immutable
            // map-level row instead of rebuilding a copy for every task.
            if(k+1==int(goals.size()) && !g.any_heading_distance.empty())continue;
            values[k].resize(g.states);
            for(int state=0;state<g.states;++state) {
                float best=INF;
                for(int q=0;q<4;++q)best=std::min(best,g.dist(goals[k]*4+q,state)+tail[k][q]);
                values[k][state]=best;
            }
        }
    }
}
const float* Chain::cached_row(const Graph& g,int stage) const {
    if(!values.empty() && !values[stage].empty())return values[stage].data();
    if(stage+1==int(goals.size()) && !g.any_heading_distance.empty())
        return g.any_heading_distance.data()+size_t(goals[stage])*g.states;
    return nullptr;
}
float Chain::cost(const Graph& g,int stage,int cell,int direction) const {
    if(stage>=int(goals.size()))return 0;
    if(const float* row=cached_row(g,stage))return row[cell*4+direction];
    float best=INF;
    for(int q=0;q<4;++q)best=std::min(best,g.dist(goals[stage]*4+q,cell*4+direction)+tail[stage][q]);
    return best;
}
void Engine::initialize(SharedEnvironment* env) {
    rng_.seed(cfg.seed);graph=std::make_unique<Graph>(*env,cfg);
    if(cfg.plain_score>0) {
        // The policy can prefer traffic lanes while evaluation measures actual
        // unit-cost forward/turn actions, including every remaining task stop.
        Config metric=cfg;metric.guidance="none";metric.turn_cost=2;metric.loops=false;metric.flow_flips=0;
        score_graph_=std::make_unique<Graph>(*env,metric);
    }
    const int n=env->num_of_agents;
    if(cfg.candidate_cache && cfg.push_price==0) {
        // Bounded per-worker storage is independent of map area/task history.
        // Allocation uses no task/start information and belongs to preprocessing.
        candidate_rankings_.resize(cfg.threads);
        #pragma omp parallel for num_threads(cfg.threads) schedule(static)
        for(int worker=0;worker<cfg.threads;++worker)
            candidate_rankings_[worker].resize(size_t(n)*ranking_cache_slots);
    }
    if(cfg.operation_depth) {operation_model_=std::make_unique<OperationModel>(*graph);operations_.assign(n,OperationModel::waiting);}
    age_.assign(n,0);previous_task_.assign(n,-1);previous_stage_.assign(n,0);
    last_actions_.assign(n,W);
    best_offsets_.resize(n);
    for(float& x:best_offsets_)x=std::uniform_real_distribution<float>(0,1)(rng_);
    std::fprintf(stderr,"R05_INIT agents=%d cells=%d K=%d depth=%d threads=%d guidance=%s seed=%d table_mb=%.1f\n",
                 n,graph->cells,cfg.futures,cfg.depth,cfg.threads,cfg.guidance.c_str(),cfg.seed,
                 graph->distance.size()*sizeof(float)/1e6);
}
void Engine::match(SharedEnvironment* env,std::vector<int>& schedule) {
    std::chrono::steady_clock::time_point match_start;
    if(cfg.profile)match_start=std::chrono::steady_clock::now();
    const auto& g=*graph;const int n=env->num_of_agents;
    schedule=env->curr_task_schedule;
    const float length_weight=cfg.initial_length_weight>=0 && env->curr_timestep<cfg.initial_length_steps
        ?cfg.initial_length_weight:cfg.length_weight;
    std::vector<int> agents, tasks;std::unordered_set<int> locked;
    for(int i=0;i<n;++i) {
        int id=schedule[i];
        bool started=id>=0 && env->task_pool.at(id).idx_next_loc>0;
        if(started || (!cfg.matching && id>=0)) { locked.insert(id);continue; }
        agents.push_back(i);schedule[i]=-1;
    }
    for(const auto& kv:env->task_pool)if(!locked.count(kv.first))tasks.push_back(kv.first);
    std::sort(tasks.begin(),tasks.end());
    std::vector<float> length(tasks.size(),0);
    for(size_t j=0;j<tasks.size();++j) {
        const auto& stops=env->task_pool.at(tasks[j]).locations;
        for(size_t k=1;k<stops.size();++k)length[j]+=g.hop(g.from_grid[stops[k]],g.from_grid[stops[k-1]]);
    }
    std::vector<std::array<float,4>> continuation;
    if(cfg.chain_matching) {
        continuation.resize(tasks.size());
        for(int j=0;j<int(tasks.size());++j) {
            Chain chain(g,env->task_pool.at(tasks[j]));
            continuation[j]=chain.tail[0];
        }
    }
    struct Pair { float cost;int agent,task; };
    const bool exact=cfg.hungarian_limit>0 && int(agents.size())<=cfg.hungarian_limit && tasks.size()>=agents.size();
    std::vector<float> matrix(exact?agents.size()*tasks.size():0);
    std::vector<Pair> pairs;if(!exact)pairs.reserve(agents.size()*tasks.size());
    for(int row=0;row<int(agents.size());++row) {
        int a=agents[row];
        int p=g.from_grid[env->curr_states[a].location];
        if(cfg.predict_matching && !pending_.empty())p=pending_[a];
        for(int j=0;j<int(tasks.size());++j) {
            int t=tasks[j];const auto& task=env->task_pool.at(t);
            float cost=g.hop(g.from_grid[task.locations[task.idx_next_loc]],p)+length_weight*length[j];
            if(cfg.guided_matching) {
                const int goal=g.from_grid[task.locations[task.idx_next_loc]];
                cost=g.approach(goal,p*4+env->curr_states[a].orientation)/2+length_weight*length[j];
            }
            if(cfg.chain_matching) {
                const int goal=g.from_grid[task.locations[0]];
                cost=INF;
                for(int d=0;d<4;++d)cost=std::min(cost,
                    (g.dist(goal*4+d,p*4+env->curr_states[a].orientation)
                     +length_weight*continuation[j][d])/2);
            }
            if(t==env->curr_task_schedule[a])cost-=cfg.keep_bonus;
            if(exact)matrix[size_t(row)*tasks.size()+j]=cost;
            else pairs.push_back({cost,a,j});
        }
    }
    std::chrono::steady_clock::time_point matrix_done;
    if(cfg.profile)matrix_done=std::chrono::steady_clock::now();
    auto report_match=[&]() {
        if(cfg.profile && (env->curr_timestep<5 || env->curr_timestep%100==0)) {
            const auto now=std::chrono::steady_clock::now();
            std::fprintf(stderr,"R05_MATCH_PROFILE t=%d agents=%zu tasks=%zu matrix_ms=%.3f solve_ms=%.3f\n",
                env->curr_timestep,agents.size(),tasks.size(),
                std::chrono::duration<double,std::milli>(matrix_done-match_start).count(),
                std::chrono::duration<double,std::milli>(now-matrix_done).count());
        }
    };
    if(exact) {
        const int nr=int(agents.size()),nc=int(tasks.size());
        std::vector<double> u(nr+1),v(nc+1);
        std::vector<int> owner(nc+1),previous(nc+1);
        for(int row=1;row<=nr;++row) {
            owner[0]=row;int column=0;
            std::vector<double> distance(nc+1,1e30);std::vector<bool> visited(nc+1,false);
            do {
                visited[column]=true;int active=owner[column],next_column=0;double delta=1e30;
                for(int j=1;j<=nc;++j)if(!visited[j]) {
                    double reduced=matrix[size_t(active-1)*nc+j-1]-u[active]-v[j];
                    if(reduced<distance[j]){distance[j]=reduced;previous[j]=column;}
                    if(distance[j]<delta){delta=distance[j];next_column=j;}
                }
                for(int j=0;j<=nc;++j) {
                    if(visited[j]){u[owner[j]]+=delta;v[j]-=delta;}
                    else distance[j]-=delta;
                }
                column=next_column;
            } while(owner[column]);
            do {int prev=previous[column];owner[column]=owner[prev];column=prev;}while(column);
        }
        for(int j=1;j<=nc;++j)if(owner[j])schedule[agents[owner[j]-1]]=tasks[j-1];
        report_match();return;
    }
    std::sort(pairs.begin(),pairs.end(),[](const Pair& a,const Pair& b) {
        if(a.cost!=b.cost)return a.cost<b.cost;
        return std::tie(a.agent,a.task)<std::tie(b.agent,b.task);
    });
    std::vector<bool> used(tasks.size(),false);
    for(const auto& p:pairs)if(schedule[p.agent]<0 && !used[p.task]) {
        schedule[p.agent]=tasks[p.task];used[p.task]=true;
    }
    report_match();
}
void Engine::certify(const Graph& g,const std::vector<int>& from,const std::vector<int>& to) {
    if(from.size()!=to.size())throw std::runtime_error("invalid move count");
    std::vector<int> owner(g.cells,-1),dest(g.cells,-1);
    for(int i=0;i<int(from.size());++i) {
        if(from[i]<0 || from[i]>=g.cells || to[i]<0 || to[i]>=g.cells)throw std::runtime_error("out-of-map move");
        if(owner[from[i]]>=0)throw std::runtime_error("duplicate source");
        owner[from[i]]=i;
        if(dest[to[i]]>=0)throw std::runtime_error("vertex conflict");
        dest[to[i]]=i;
        if(from[i]!=to[i] && g.direction(from[i],to[i])<0)throw std::runtime_error("nonadjacent move");
    }
    for(int i=0;i<int(from.size());++i)if(to[i]!=from[i]) {
        int j=owner[to[i]];
        if(j>=0 && to[j]==from[i])throw std::runtime_error("edge swap");
    }
}

void Engine::fill_ready_moves(const Frame& f,const std::vector<float>& offsets,std::vector<int>& to) const {
    const auto& g=*graph;const int n=int(f.loc.size());
    const auto& assigned=cfg.rollout_match?f.active_chains:assigned_;
    std::vector<int> owner(g.cells,-1),next(n,-1),order(n),stamp(n,-1),index(n);
    std::vector<unsigned char> eligible(n,0);
    std::vector<float> gain(n),priority(n);
    for(int a=0;a<n;++a)owner[to[a]]=a;
    for(int a=0;a<n;++a) {
        const Chain* chain=assigned[a];
        const bool active=chain && f.stage[a]<int(chain->goals.size());
        auto value=[&](int v,int d) {
            return active?chain->cost(g,f.stage[a],v,d):cfg.idle_eviction*g.pocket_depth[v];
        };
        int age=cfg.rollout_age?f.age[a]:age_[a];if(cfg.age_cap>0)age=std::min(age,cfg.age_cap);
        priority[a]=age+offsets[a]-(active?0:100000);
        if(cfg.deadends && g.pocket[f.loc[a]] &&
           (!active || g.pocket[chain->goals[f.stage[a]]]!=g.pocket[f.loc[a]]))priority[a]+=1000000;
        // Preserve every already promised forward move. An idle robot can only
        // add a forward move in its current heading; no turn-and-move shortcut.
        if(to[a]!=f.loc[a])continue;
        int v=g.next[f.loc[a]][f.dir[a]];if(v<0)continue;
        next[a]=v;eligible[a]=1;
        float before=value(f.loc[a],f.dir[a]);
        for(int d:{(f.dir[a]+1)%4,(f.dir[a]+3)%4})before=std::min(before,value(f.loc[a],d));
        gain[a]=before-value(v,f.dir[a]);
    }
    std::iota(order.begin(),order.end(),0);
    std::stable_sort(order.begin(),order.end(),[&](int a,int b){return priority[a]>priority[b];});
    std::vector<int> chain;chain.reserve(n);
    for(int root:order)if(eligible[root]) {
        chain.clear();int a=root,begin=0;bool possible=false;
        while(eligible[a]) {
            if(stamp[a]==root) {
                begin=index[a];possible=int(chain.size())-begin>=3;break;
            }
            stamp[a]=root;index[a]=int(chain.size());chain.push_back(a);
            int b=owner[next[a]];
            if(b<0){possible=true;break;}
            a=b;
        }
        if(!possible)continue;
        float total=0;for(int k=begin;k<int(chain.size());++k)total+=gain[chain[k]];
        if(total<=cfg.early_fill_gain+1e-5f)continue;
        // A cycle cannot accept its incoming tail; a chain must end at an
        // unreserved cell after the existing promises. Two-agent swaps fail
        // the minimum cycle-length check above.
        for(int k=begin;k<int(chain.size());++k)owner[f.loc[chain[k]]]=-1;
        for(int k=begin;k<int(chain.size());++k) {
            int b=chain[k];to[b]=next[b];owner[to[b]]=b;eligible[b]=0;
        }
    }
}

void Engine::advance(Frame& f,const std::vector<float>& offsets,std::vector<Action>& actions,
                     uint64_t& expansion_count,bool cycle_moves) const {
    PolicyTiming* timing=policy_profile_active_?&policy_timings_[omp_get_thread_num()]:nullptr;
    const bool sampled=timing && ++timing->calls%64==0;
    std::chrono::steady_clock::time_point measured;
    if(sampled){++timing->samples;measured=std::chrono::steady_clock::now();}
    auto mark_policy=[&](int phase) {
        if(!sampled)return;
        const auto now=std::chrono::steady_clock::now();
        timing->nanoseconds[phase]+=std::chrono::duration_cast<std::chrono::nanoseconds>(now-measured).count();
        measured=now;
    };
    const auto& assigned=cfg.rollout_match?f.active_chains:assigned_;
    const int cycle_mode=cycle_moves?cfg.pre_cycles:0;
    const auto& g=*graph;const int n=int(f.loc.size());
    // pending is the already promised forward/wait move. Plan the following
    // movement on its exact resulting occupancy, while current idle robots turn.
    PolicyScratch fresh;
    thread_local PolicyScratch reused;
    auto& scratch=cfg.scratch_reuse?reused:fresh;
    auto& p=scratch.p;p=f.pending;
    auto& moving=scratch.moving;moving.resize(n);
    auto& owner=scratch.owner;owner.assign(g.cells,-1);
    auto& chosen=scratch.chosen;chosen.resize(n);
    auto& reserve=scratch.reserve;reserve.resize(g.cells);
    if(cfg.early_fill)fill_ready_moves(f,offsets,p);
    for(int i=0;i<n;++i) {
        moving[i]=p[i]!=f.loc[i];owner[p[i]]=i;
        if(moving[i] && g.next[f.loc[i]][f.dir[i]]!=p[i])
            throw std::runtime_error("pending move not aligned with heading");
        bool arrived=assigned[i] && f.stage[i]<int(assigned[i]->goals.size()) &&
                     p[i]==assigned[i]->goals[f.stage[i]];
        if(arrived)++f.stage[i];
        if(cfg.rollout_age)f.age[i]=arrived?0:f.age[i]+1;
    }
    // Stage is fixed throughout this policy step. Resolve the active chain
    // and its cached row once, rather than for every candidate lookup.
    auto& active_chain=scratch.active_chain;active_chain.assign(n,nullptr);
    auto& cost_table=scratch.cost_table;cost_table.assign(n,nullptr);
    for(int a=0;a<n;++a) {
        const Chain* chain=assigned[a];
        if(chain && f.stage[a]<int(chain->goals.size())) {
            active_chain[a]=chain;
            cost_table[a]=chain->cached_row(g,f.stage[a]);
        }
    }
    auto cost=[&](int a,int v,int d) {
        if(cost_table[a])return cost_table[a][v*4+d];
        if(active_chain[a])return active_chain[a]->cost(g,f.stage[a],v,d);
        return cfg.idle_eviction*g.pocket_depth[v];
    };
    auto allowed=[&](int a,int d) {return moving[a]?d==f.dir[a]:turn(d,f.dir[a])<=1;};
    auto& idle_heading=scratch.idle_heading;idle_heading.resize(n);
    auto& forced_heading=scratch.forced_heading;forced_heading.assign(n,-1);
    auto& base_cost=scratch.base_cost;base_cost.resize(n);
    auto& priorities=scratch.priorities;priorities.resize(n);
    auto& candidates=scratch.candidates;candidates.resize(n);
    auto& candidate_count=scratch.candidate_count;candidate_count.resize(n);
    auto& ranking_slots=scratch.ranking_slots;ranking_slots.assign(n,nullptr);
    auto& ranking_hits=scratch.ranking_hits;ranking_hits.assign(n,0);
    // Push loss depends on another robot's state, so that optional policy uses
    // the original path. All other ranking inputs are captured below; priorities
    // and collision resolution are always recomputed for the current future.
    CachedRanking* cache=cfg.candidate_cache && cfg.push_price==0 && !candidate_rankings_.empty()
        ?candidate_rankings_[omp_get_thread_num()].data():nullptr;
    for(int i=0;i<n;++i) {
        if(cache) {
            uint64_t key=(uint64_t(uint32_t(f.stage[i]))<<32)|uint32_t(p[i]*8+f.dir[i]*2+moving[i]);
            size_t slot=(key*0x9e3779b97f4a7c15ULL)>>58;
            auto* entry=&cache[size_t(i)*ranking_cache_slots+slot];ranking_slots[i]=entry;
            if(entry->epoch==ranking_epoch_ && entry->key==key && entry->chain==active_chain[i]) {
                ranking_hits[i]=1;idle_heading[i]=entry->idle_heading;base_cost[i]=entry->base_cost;
            } else {
                entry->epoch=ranking_epoch_;entry->key=key;entry->chain=active_chain[i];
            }
        }
        if(!ranking_hits[i]) {
            int best_dir=f.dir[i];float best=cost(i,p[i],best_dir);
            if(!moving[i])for(int q:{(f.dir[i]+1)%4,(f.dir[i]+3)%4}) {
                float x=cost(i,p[i],q)+0.05f*g.weight[p[i]][4];
                if(x<best-1e-5f) {best=x;best_dir=q;}
            }
            idle_heading[i]=best_dir;
            base_cost[i]=cost(i,p[i],best_dir);
            if(ranking_slots[i]) {
                ranking_slots[i]->idle_heading=idle_heading[i];ranking_slots[i]->base_cost=base_cost[i];
            }
        }
        int age=cfg.rollout_age?f.age[i]:age_[i];
        if(cfg.age_cap>0)age=std::min(age,cfg.age_cap);
        float priority=age+offsets[i];
        const bool active=assigned[i] && f.stage[i]<int(assigned[i]->goals.size());
        if(!active)priority-=100000;
        if(cfg.deadends && g.pocket[p[i]] &&
           (!active || g.pocket[assigned[i]->goals[f.stage[i]]]!=g.pocket[p[i]]))priority+=1000000;
        priorities[i]=priority;
    }
    mark_policy(0);
    auto propose_cycles=[&](bool blocked_only) {
        struct Proposal { float gain;const std::vector<int>* ring;int sign;bool ready; };
        std::vector<Proposal> proposals;
        for(const auto& ring:g.cycles) {
            const int count=int(ring.size());bool idle=true;
            for(int v:ring)if(owner[v]<0 || moving[owner[v]] || (blocked_only && chosen[owner[v]]!=p[owner[v]])){idle=false;break;}
            if(!idle)continue;
            for(int sign:{1,count-1}) {
                float gain=0;bool ready=true;
                for(int k=0;k<count;++k) {
                    int a=owner[ring[k]],v=ring[(k+sign)%count],d=g.direction(ring[k],v);
                    ready=ready && allowed(a,d);
                    // Compare against the best heading available during an idle
                    // step, so rotations alone do not make a cycle profitable.
                    float before=base_cost[a];
                    if(cycle_mode>=2)for(int q=0;q<4;++q)before=std::min(before,cost(a,ring[k],q));
                    gain+=before-cost(a,v,d);
                }
                if((ready || cycle_mode>=2) && gain>cfg.loop_threshold+cfg.pre_cycle_gain*count)
                    proposals.push_back({gain,&ring,sign,ready});
            }
        }
        std::stable_sort(proposals.begin(),proposals.end(),[](const Proposal& a,const Proposal& b){return a.gain>b.gain;});
        for(const auto& proposal:proposals) {
            const auto& ring=*proposal.ring;const int count=int(ring.size());bool free=true;
            for(int v:ring)if(blocked_only ? (chosen[owner[v]]!=p[owner[v]] || forced_heading[owner[v]]>=0) : chosen[owner[v]]>=0){free=false;break;}
            if(!free)continue;
            for(int k=0;k<count;++k) {
                int a=owner[ring[k]],target=ring[(k+proposal.sign)%count];
                forced_heading[a]=g.direction(ring[k],target);
                chosen[a]=proposal.ready?target:ring[k];reserve[chosen[a]]=a;
            }
        }
    };
    // Spatial intent and executable PIBT use identical costs and priorities.
    // Build their shared ordering once, then filter infeasible headings while
    // visiting candidates. Stable sorting preserves the previous tie order.
    for(int i=0;i<n;++i) {
        auto& cand=candidates[i];int count=0;
        if(ranking_hits[i]) {
            const auto& entry=*ranking_slots[i];candidate_count[i]=entry.count;
            std::copy_n(entry.candidates.begin(),entry.count,cand.begin());continue;
        }
        for(int d=0;d<4;++d) {
            int v=g.next[p[i]][d];if(v<0 || (!cfg.intent_rotation && !allowed(i,d)))continue;
            float score=cost(i,v,d)+g.weight[p[i]][d];
            int b=owner[v];
            if(cfg.push_price>0 && b>=0 && b!=i) {
                float loss=INF;
                for(int q=0;q<4;++q)if(g.next[v][q]>=0 && allowed(b,q))
                    loss=std::min(loss,cost(b,g.next[v][q],q)+g.weight[v][q]-base_cost[b]);
                score+=cfg.push_price*std::max(0.0f,std::min(100.0f,loss));
            }
            cand[count++]={v,d,score};
        }
        // A small tie preference preserves useful forward chains.
        float wait_value=base_cost[i];
        if(cfg.prospective_wait) {
            // The future wait is itself an opportunity to rotate. A robot
            // moving now can turn once then; one idle now can turn twice.
            for(int d=0;d<4;++d)if(!moving[i] || turn(d,f.dir[i])<=1)
                wait_value=std::min(wait_value,cost(i,p[i],d));
        }
        cand[count++]={p[i],idle_heading[i],wait_value+cfg.wait_cost+0.001f};
        // At most five entries: stable insertion sort avoids a temporary
        // allocation for every robot in every simulated policy step.
        for(int k=1;k<count;++k) {
            MoveCandidate value=cand[k];int j=k;
            while(j>0 && value.score<cand[j-1].score){cand[j]=cand[j-1];--j;}
            cand[j]=value;
        }
        candidate_count[i]=count;
        if(ranking_slots[i]) {
            auto& entry=*ranking_slots[i];entry.count=count;
            std::copy_n(cand.begin(),count,entry.candidates.begin());
        }
    }
    mark_policy(1);
    auto& order=scratch.order;
    order_priorities(priorities,cfg.packed_order,cfg.radix_order,order,scratch.priority_keys,scratch.radix_buffer);
    mark_policy(2);
    auto& prepared=scratch.prepared;prepared.clear();
    auto choose=[&](bool kinematic) {
    std::fill(chosen.begin(),chosen.end(),-1);
    std::fill(reserve.begin(),reserve.end(),-1);
    if(kinematic && cfg.intent_mode==2)for(int i=0;i<n;++i)if(prepared[i]!=p[i]) {
        chosen[i]=prepared[i];reserve[chosen[i]]=i;
    }
    if(kinematic && cycle_mode>0 && cycle_mode<3)propose_cycles(false);
    int expansions=0;
    auto pibt=[&](auto&& self,int a)->bool {
        ++expansions;
        for(int k=0;k<candidate_count[a];++k) {
            int v=candidates[a][k].v;
            if(kinematic && v!=p[a] && !allowed(a,candidates[a][k].d))continue;
            if(expansions>cfg.expansion_limit && v!=p[a])continue;
            if(reserve[v]>=0)continue;
            int b=owner[v];
            if(b>=0 && b!=a && chosen[b]==p[a])continue;
            chosen[a]=v;reserve[v]=a;
            if(b>=0 && b!=a && chosen[b]<0 && !self(self,b)) {
                // A failed child commits to staying, overriding our tentative
                // claim on its cell. Keep that blocker; do not explore it again.
                if(reserve[v]==a)reserve[v]=-1;
                chosen[a]=-1;
                continue;
            }
            return true;
        }
        // Priority inheritance propagates failure back to the requesting
        // parent, which must release its attempted move into this cell.
        chosen[a]=p[a];reserve[p[a]]=a;
        return false;
    };
    for(int a:order)if(chosen[a]<0)pibt(pibt,a);
    expansion_count+=expansions;
    };
    auto& intent=scratch.intent;intent.clear();
    if(cfg.intent_rotation){choose(false);intent=chosen;}
    if(cfg.intent_mode) {
        // The collision-free spatial assignment decomposes into disjoint chains
        // ending at holes and cycles. Commit a component only when every member
        // can execute its intended heading after the current action.
        std::vector<int> parent(n);std::iota(parent.begin(),parent.end(),0);
        auto root=[&](int a) {
            while(parent[a]!=a){parent[a]=parent[parent[a]];a=parent[a];}
            return a;
        };
        for(int i=0;i<n;++i)if(intent[i]!=p[i]) {
            int b=owner[intent[i]];
            if(b>=0)parent[root(i)]=root(b);
        }
        std::vector<unsigned char> ready(n,1);
        for(int i=0;i<n;++i)if(intent[i]!=p[i] && !allowed(i,g.direction(p[i],intent[i])))
            ready[root(i)]=0;
        prepared=p;
        for(int i=0;i<n;++i)if(ready[root(i)])prepared[i]=intent[i];
    }
    mark_policy(3);
    // Mode 1 executes only complete ready components. Mode 2 pins those moves
    // and fills the remaining space with the usual kinematic PIBT policy.
    if(cfg.intent_mode==1)chosen=prepared;else choose(true);
    mark_policy(4);
    if(cycle_mode==3)propose_cycles(true);
    if(cfg.loops) {
        // Geometry is precomputed; evaluate only obstacle-free perimeters.
        for(const auto& v:g.cycles) {
            const int count=int(v.size());std::array<int,32> a{};bool good=true;
            for(int k=0;k<count;++k) {
                if(owner[v[k]]<0){good=false;break;}
                a[k]=owner[v[k]];
                if(chosen[a[k]]!=v[k] || forced_heading[a[k]]>=0){good=false;break;}
            }
            if(!good)continue;
            float best_gain=cfg.loop_threshold;int sign=0;
            for(int s:{1,count-1}) {
                float gain=0;bool ok=true;
                for(int k=0;k<count;++k) {
                    int u=v[(k+s)%count],d=g.direction(v[k],u);
                    if(!allowed(a[k],d)){ok=false;break;}
                    gain+=base_cost[a[k]]-cost(a[k],u,d);
                }
                if(ok && gain>best_gain){best_gain=gain;sign=s;}
            }
            if(sign)for(int k=0;k<count;++k)chosen[a[k]]=v[(k+sign)%count];
        }
    }
    mark_policy(5);
    actions.assign(n,W);
    for(int i=0;i<n;++i) {
        if(moving[i])actions[i]=FW;
        else {
            int d=chosen[i]==p[i]?idle_heading[i]:g.direction(p[i],chosen[i]);
            int wanted=-1;
            if(chosen[i]==p[i]) {
                if(forced_heading[i]>=0)wanted=forced_heading[i];
                else if(cfg.intent_rotation && intent[i]!=p[i])wanted=g.direction(p[i],intent[i]);
            }
            if(wanted>=0) {
                if(turn(wanted,f.dir[i])==2) {
                    int right=(f.dir[i]+1)%4,left=(f.dir[i]+3)%4;
                    d=cost(i,p[i],right)<=cost(i,p[i],left)?right:left;
                } else d=wanted;
            }
            int delta=(d-f.dir[i]+4)%4;
            if(delta==2)throw std::runtime_error("pipeline requested an impossible half-turn");
            if(delta==1)actions[i]=CR;
            if(delta==3)actions[i]=CCR;
            f.dir[i]=d;
        }
    }
    if(cfg.reverse_penalty>0) {
        for(int i=0;i<n;++i)
            f.reverse_turns+=(actions[i]==CR && f.last_actions[i]==CCR) ||
                             (actions[i]==CCR && f.last_actions[i]==CR);
        f.last_actions=actions;
    }
    f.loc.swap(p);f.pending.swap(chosen);
    mark_policy(6);
}

void Engine::match_future(Frame& frame) const {
    const auto& g=*graph;
    std::vector<int> ready;
    for(int a=0;a<int(frame.loc.size());++a) {
        const Chain* task=frame.active_chains[a];
        if(task && frame.stage[a]>=int(task->goals.size())) {
            ready.push_back(a);frame.active_chains[a]=nullptr;
            if(score_graph_)frame.plain_chains[a]=nullptr;
            frame.stage[a]=0;
        }
    }
    // Match only newly finished robots to tasks visible and unassigned at the
    // real step's start. Do not inspect hidden future tasks or alter the real
    // schedule. Each rollout owns its availability mask.
    while(!ready.empty()) {
        float best=INF;int robot=-1,task=-1,position=-1;
        for(int k=0;k<int(ready.size());++k) {
            int a=ready[k],p=frame.loc[a],goal;
            for(int j=0;j<int(future_tasks_.size());++j)if(frame.free_tasks[j]) {
                goal=future_tasks_[j]->goals[0];
                float approach=g.hop(goal,p);
                if(cfg.guided_matching) {
                    approach=INF;
                    for(int d=0;d<4;++d)approach=std::min(approach,g.dist(goal*4+d,p*4+frame.dir[a])/2);
                }
                float cost=approach+cfg.length_weight*future_lengths_[j];
                if(cost<best){best=cost;robot=a;task=j;position=k;}
            }
        }
        if(task<0)break;
        frame.active_chains[robot]=future_tasks_[task];
        if(score_graph_)frame.plain_chains[robot]=future_plain_[task];
        frame.free_tasks[task]=0;ready.erase(ready.begin()+position);
    }
}

Rollout Engine::rollout(Frame frame,const std::vector<float>& offsets,bool cycle_moves,
                        const Continuation* continuation,RolloutPrefix* save,
                        const RolloutPrefix* resume) const {
    const auto& g=*graph;Rollout r;r.offsets=offsets;r.cycle_moves=cycle_moves;
    auto total_cost=[&](const Frame& f) {
        double guided=0,plain=0;
        const auto& assigned=cfg.rollout_match?f.active_chains:assigned_;
        const auto& plain_assigned=cfg.rollout_match?f.plain_chains:score_assigned_;
        for(int i=0;i<int(f.loc.size());++i)if(assigned[i]) {
            guided+=assigned[i]->cost(g,f.stage[i],f.loc[i],f.dir[i]);
            if(score_graph_)plain+=plain_assigned[i]->cost(*score_graph_,f.stage[i],f.loc[i],f.dir[i]);
        }
        return score_graph_?guided*(1-cfg.plain_score)+plain*cfg.plain_score:guided;
    };
    auto completed=[&](const Frame& f) {
        int count=0;
        const auto& assigned=cfg.rollout_match?f.active_chains:assigned_;
        for(int i=0;i<int(f.loc.size());++i)
            count+=assigned[i] && f.stage[i]>=int(assigned[i]->goals.size());
        return count;
    };
    int completions=resume?resume->completions:0;
    double initial=resume?resume->initial:total_cost(frame);
    double previous=resume?resume->previous:initial,progress=resume?resume->progress:0;
    double discounted=resume?resume->discounted:0,weight=resume?resume->weight:1;
    double weight_sum=resume?resume->weight_sum:0;
    if(resume){r.first=resume->first;r.actions=resume->actions;}
    std::vector<Action> actions;
    std::vector<float> future_offsets;
    if(continuation)future_offsets=offsets;
    for(int t=resume?resume->time:0;t<cfg.depth;++t) {
        if(continuation)for(const auto& change:continuation->at(t))
            future_offsets[change.agent]=change.offset;
        const auto& priorities=continuation?future_offsets:offsets;
        const int before_completed=cfg.completion_bonus>0?completed(frame):0;
        if(cfg.operation_depth)advance_operations(frame,priorities,actions,r.expansions);
        else advance(frame,priorities,actions,r.expansions,cycle_moves);
        if(t==0){r.first=frame;r.actions=actions;}
        if(cfg.completion_bonus>0)completions+=completed(frame)-before_completed;
        if(cfg.progress_discount<1 || cfg.rollout_match) {
            double current=total_cost(frame);
            progress+=previous-current;
            discounted+=weight*(previous-current);weight_sum+=weight;
            weight*=cfg.progress_discount;previous=current;
        }
        if(cfg.rollout_match && t+1<cfg.depth) {
            match_future(frame);
            // New task cost is a new baseline, not negative progress. Credit
            // only distance actually reduced while executing each chain.
            previous=total_cost(frame);
        }
        if(save && t+1==cfg.continuation_start) {
            save->frame=frame;save->first=r.first;save->actions=r.actions;
            save->time=t+1;save->completions=completions;
            save->initial=initial;save->previous=previous;save->progress=progress;
            save->discounted=discounted;save->weight=weight;save->weight_sum=weight_sum;
        }
    }
    r.score=(cfg.rollout_match?progress:initial-total_cost(frame))/2.0;
    if(cfg.progress_discount<1)r.score=discounted*cfg.depth/(2*weight_sum);
    r.score-=cfg.reverse_penalty*frame.reverse_turns;
    // Finished agents have no replacement task inside these short rollouts.
    // An optional terminal reward tests whether pure distance decrease therefore
    // undervalues completing a chain relative to advancing an unfinished one.
    if(cfg.completion_bonus>0)r.score+=cfg.completion_bonus*completions;
    if(cfg.dispersion) {
        int pairs=0;
        if(cfg.fast_dispersion)pairs=g.nearby_pairs(frame.loc);
        else {
            std::vector<int> occupancy(g.from_grid.size(),0);
            for(int v:frame.loc)occupancy[g.to_grid[v]]=1;
            for(int v:frame.loc) {
                int p=g.to_grid[v],x=p%g.cols,y=p/g.cols;
                for(int yy=std::max(0,y-2);yy<=std::min(g.rows-1,y+2);++yy)
                    for(int xx=std::max(0,x-2);xx<=std::min(g.cols-1,x+2);++xx)
                        if(yy*g.cols+xx>p)pairs+=occupancy[yy*g.cols+xx];
            }
        }
        r.score-=cfg.dispersion*pairs;
    }
    return r;
}
Rollout Engine::evaluate(const Frame& frame,const std::vector<float>& offsets,
                         const std::vector<Continuation>& continuations,bool cycle_moves) const {
    RolloutPrefix prefix;
    const bool shared=cfg.share_prefix && !continuations.empty();
    Rollout result=rollout(frame,offsets,cycle_moves,nullptr,shared?&prefix:nullptr);
    if(continuations.empty())return result;
    if(shared && prefix.time!=cfg.continuation_start)
        throw std::runtime_error("missing shared rollout prefix");
    double score=result.score,mean=result.score,variance_sum=0;int count=1;
    for(const auto& continuation:continuations) {
        Rollout branch=shared
            ?rollout(prefix.frame,offsets,cycle_moves,&continuation,nullptr,&prefix)
            :rollout(frame,offsets,cycle_moves,&continuation);
        // A branch evaluates the root's decision; it cannot silently substitute
        // a different first action or promise while contributing to its score.
        if(branch.actions!=result.actions || branch.first.loc!=result.first.loc ||
           branch.first.dir!=result.first.dir || branch.first.pending!=result.first.pending ||
           branch.first.stage!=result.first.stage || branch.first.operations!=result.first.operations)
            throw std::runtime_error("continuation changed the first decision");
        score+=branch.score;result.expansions+=branch.expansions;
        if(cfg.continuation_risk!=0) {
            ++count;double delta=branch.score-mean;mean+=delta/count;
            variance_sum+=delta*(branch.score-mean);
        }
    }
    result.score=score/(continuations.size()+1);
    // Positive coefficients penalize sensitivity to future priorities; negative
    // coefficients test optimism about the next real step's re-optimization.
    // Both still execute every configured continuation. Zero preserves the mean.
    if(cfg.continuation_risk!=0)
        result.score-=cfg.continuation_risk*std::sqrt(std::max(0.0,variance_sum/count));
    return result;
}

void Engine::compute(SharedEnvironment* env,std::vector<Action>& plan,std::vector<int>& schedule) {
    // Invalidate between real steps, including task swaps/configuration changes.
    ++ranking_epoch_;
    policy_profile_active_=cfg.policy_profile && (env->curr_timestep<5 || env->curr_timestep%100==0);
    if(policy_profile_active_)policy_timings_.assign(cfg.threads,PolicyTiming{});
    std::chrono::steady_clock::time_point measured;
    if(cfg.profile)measured=std::chrono::steady_clock::now();
    std::array<double,5> phase_ms{};
    auto mark=[&](int phase) {
        if(!cfg.profile)return;
        const auto now=std::chrono::steady_clock::now();
        phase_ms[phase]+=std::chrono::duration<double,std::milli>(now-measured).count();
        measured=now;
    };
    const auto& g=*graph;const int n=env->num_of_agents;
    Frame frame;frame.loc.resize(n);frame.dir.resize(n);frame.stage.resize(n);
    for(int a=0;a<n;++a) {
        frame.loc[a]=g.from_grid[env->curr_states[a].location];frame.dir[a]=env->curr_states[a].orientation;
        if(!predicted_loc_.empty() && (predicted_loc_[a]!=frame.loc[a] || predicted_dir_[a]!=frame.dir[a]))
            throw std::runtime_error("simulator diverged from pipeline prediction");
    }
    if(pending_.empty())pending_=frame.loc;
    frame.pending=pending_;
    if(cfg.operation_depth)frame.operations=operations_;
    match(env,schedule);mark(0);
    for(auto it=chains_.begin();it!=chains_.end();) {
        if(!env->task_pool.count(it->first))it=chains_.erase(it);else ++it;
    }
    for(auto it=score_chains_.begin();it!=score_chains_.end();) {
        if(!env->task_pool.count(it->first))it=score_chains_.erase(it);else ++it;
    }
    assigned_.assign(n,nullptr);score_assigned_.assign(n,nullptr);triaged_=0;
    for(int a=0;a<n;++a) {
        int id=schedule[a];++age_[a];
        if(previous_task_[a]>=0 && !env->task_pool.count(previous_task_[a]))age_[a]=0;
        if(id>=0) {
            const auto& task=env->task_pool.at(id);
            frame.stage[a]=task.idx_next_loc;
            if(id==previous_task_[a] && task.idx_next_loc>previous_stage_[a])age_[a]=0;
            auto& chain=chains_[id];if(!chain)chain=std::make_shared<Chain>(g,task,cfg.cost_cache);
            assigned_[a]=chain.get();
            if(score_graph_) {
                auto& score_chain=score_chains_[id];
                if(!score_chain)score_chain=std::make_shared<Chain>(*score_graph_,task,cfg.cost_cache);
                score_assigned_[a]=score_chain.get();
            }
            if(cfg.horizon>0) {
                double remaining=0;int p=frame.loc[a];
                for(int k=task.idx_next_loc;k<int(chain->goals.size());++k) {
                    remaining+=g.hop(chain->goals[k],p);p=chain->goals[k];
                }
                double steps_per_cell=total_forward_?double(total_agent_steps_)/total_forward_:2;
                if(remaining*steps_per_cell*cfg.triage_scale>cfg.horizon-env->curr_timestep) {
                    assigned_[a]=nullptr;++triaged_;
                }
            }
        }
        previous_task_[a]=id;previous_stage_[a]=frame.stage[a];
    }
    if(cfg.rollout_match) {
        frame.active_chains=assigned_;if(score_graph_)frame.plain_chains=score_assigned_;
        future_tasks_.clear();future_plain_.clear();future_lengths_.clear();
        std::unordered_set<int> used(schedule.begin(),schedule.end());
        std::vector<int> ids;
        for(const auto& item:env->task_pool)if(!used.count(item.first) && item.second.idx_next_loc==0)
            ids.push_back(item.first);
        std::sort(ids.begin(),ids.end());
        for(int id:ids) {
            const auto& task=env->task_pool.at(id);
            auto& chain=chains_[id];if(!chain)chain=std::make_shared<Chain>(g,task,cfg.cost_cache);
            future_tasks_.push_back(chain.get());
            float length=0;
            for(int k=1;k<int(chain->goals.size());++k)length+=g.hop(chain->goals[k],chain->goals[k-1]);
            future_lengths_.push_back(length);
            if(score_graph_) {
                auto& plain=score_chains_[id];
                if(!plain)plain=std::make_shared<Chain>(*score_graph_,task,cfg.cost_cache);
                future_plain_.push_back(plain.get());
            }
        }
        frame.free_tasks.assign(future_tasks_.size(),1);
    }
    mark(1);
    frame.age=age_;
    if(cfg.reverse_penalty>0)frame.last_actions=last_actions_;
    const int roots=cfg.futures/cfg.continuations;
    std::vector<std::vector<float>> offsets(roots);
    // Independent per-step streams preserve candidate prefixes across K and
    // keep local-refinement draws independent of the number of global futures.
    auto mix=[](uint64_t x) {
        x+=0x9e3779b97f4a7c15ULL;x=(x^(x>>30))*0xbf58476d1ce4e5b9ULL;
        x=(x^(x>>27))*0x94d049bb133111ebULL;return x^(x>>31);
    };
    std::mt19937 step_random,local_random;
    if(cfg.random_by_step) {
        uint64_t key=(uint64_t(uint32_t(cfg.seed))<<32)|uint32_t(env->curr_timestep);
        step_random.seed(uint32_t(mix(key)));local_random.seed(uint32_t(mix(key^0xd1b54a32d192ed03ULL)));
    }
    auto& global_rng=cfg.random_by_step?step_random:rng_;
    auto& local_rng=cfg.random_by_step?local_random:rng_;
    std::uniform_real_distribution<float> unit(0,1),noise(-cfg.noise,cfg.noise);
    // Common continuation draws compare roots under the same future priority
    // perturbations. A separate stream leaves root and local draws unchanged.
    // Branch zero keeps the original constant-offset rollout.
    std::vector<Continuation> continuations(cfg.continuations-1,Continuation(cfg.depth));
    if(!continuations.empty()) {
        uint64_t key=(uint64_t(uint32_t(cfg.seed))<<32)|uint32_t(env->curr_timestep);
        std::mt19937 future_random(uint32_t(mix(key^0x94d049bb133111ebULL)));
        for(auto& branch:continuations)for(int t=cfg.continuation_start;t<cfg.depth;++t)
            for(int a=0;a<n;++a)if(unit(future_random)<cfg.future_mutation)
                branch[t].push_back({a,noise(future_random)});
    }
    std::vector<Rollout> results(roots);
    std::vector<std::exception_ptr> errors(roots);
    int best=0;
    for(int generation=0;generation<cfg.generations;++generation) {
        const int begin=generation*roots/cfg.generations;
        const int end=(generation+1)*roots/cfg.generations;
        // Keep the total number of complete rollouts fixed. Later batches
        // refine this step's incumbent; one generation preserves the original
        // random draws, candidate order, and equal-score acceptance behavior.
        const auto& parent=generation?results[best].offsets:best_offsets_;
        for(int k=begin;k<end;++k) {
            offsets[k]=parent;
            if(k>begin) {
                // Keep one quarter of the portfolio global. Other futures can
                // change one spatial neighborhood while preserving its context.
                int center=-1;
                if(cfg.mutation_radius>0 && k%4!=0)
                    center=g.to_grid[frame.loc[std::uniform_int_distribution<int>(0,n-1)(global_rng)]];
                for(int a=0;a<n;++a) {
                    if(center>=0) {
                        int p=g.to_grid[frame.loc[a]];
                        if(std::abs(p/g.cols-center/g.cols)>cfg.mutation_radius ||
                           std::abs(p%g.cols-center%g.cols)>cfg.mutation_radius)continue;
                    }
                    if(k%4==0 || unit(global_rng)<cfg.mutation)offsets[k][a]=noise(global_rng);
                }
            }
        }
        mark(2);
        #pragma omp parallel for num_threads(cfg.threads) schedule(static)
        for(int k=begin;k<end;++k) {
            try { results[k]=evaluate(frame,offsets[k],continuations,!cfg.cycle_portfolio || k%2==1); }
            catch(...) { errors[k]=std::current_exception(); }
        }
        mark(3);
        for(int k=begin;k<end;++k) {
            if(errors[k])std::rethrow_exception(errors[k]);
            if(results[k].score>results[best].score+1e-7 ||
               (cfg.accept_equal && results[k].score>=results[best].score-1e-7))best=k;
        }
    }
    // Local refinement also spends complete continuation groups, without
    // exceeding its existing rollout allowance. Unused remainders stay unused.
    for(int trial=0;trial<cfg.local_trials/cfg.continuations;++trial) {
        auto local=results[best].offsets;
        int center=std::uniform_int_distribution<int>(0,n-1)(local_rng);
        int p=g.to_grid[frame.loc[center]];
        for(int a=0;a<n;++a) {
            int q=g.to_grid[frame.loc[a]];
            if(std::abs(p/g.cols-q/g.cols)<=2 && std::abs(p%g.cols-q%g.cols)<=2)local[a]=noise(local_rng);
        }
        Rollout candidate=evaluate(frame,local,continuations,results[best].cycle_moves);
        if(candidate.score>results[best].score+1e-7 ||
           (cfg.accept_equal && candidate.score>=results[best].score-1e-7))results[best]=std::move(candidate);
    }
    auto& selected=results[best];
    certify(g,frame.loc,selected.first.loc);
    certify(g,selected.first.loc,selected.first.pending);
    if(cfg.operation_depth) {
        // Certify the selected suffix, including its terminal wait, before any
        // real action is returned. Failed revisions always retain this fallback.
        std::vector<int> previous=selected.first.loc,next(n);
        for(int t=0;t<OperationModel::horizon;++t) {
            for(int a=0;a<n;++a)next[a]=operation_model_->path(
                selected.first.loc[a]*4+selected.first.dir[a],selected.first.operations[a])[t]/4;
            certify(g,previous,next);previous=next;
        }
        operations_=selected.first.operations;
    }
    plan=selected.actions;pending_=selected.first.pending;best_offsets_=selected.offsets;
    predicted_loc_=selected.first.loc;predicted_dir_=selected.first.dir;
    if(cfg.reverse_penalty>0)last_actions_=selected.actions;
    total_agent_steps_+=n;total_forward_+=std::count(plan.begin(),plan.end(),FW);
    mark(4);
    if(cfg.profile && (env->curr_timestep<5 || env->curr_timestep%100==0))
        std::fprintf(stderr,"R05_PROFILE t=%d assignment_ms=%.3f task_cost_ms=%.3f candidates_ms=%.3f lookahead_ms=%.3f final_ms=%.3f\n",
                     env->curr_timestep,phase_ms[0],phase_ms[1],phase_ms[2],phase_ms[3],phase_ms[4]);
    if(policy_profile_active_) {
        PolicyTiming total;
        for(const auto& timing:policy_timings_) {
            total.calls+=timing.calls;total.samples+=timing.samples;
            for(int k=0;k<7;++k)total.nanoseconds[k]+=timing.nanoseconds[k];
        }
        // Sampled elapsed worker time, not isolated CPU utilization. Separate
        // columns identify expensive policy phases without timing every call.
        std::fprintf(stderr,"R05_POLICY_PROFILE t=%d calls=%llu samples=%llu setup_ns=%llu candidates_ns=%llu order_ns=%llu intent_ns=%llu kinematic_ns=%llu cycles_ns=%llu actions_ns=%llu\n",
            env->curr_timestep,(unsigned long long)total.calls,(unsigned long long)total.samples,
            (unsigned long long)total.nanoseconds[0],(unsigned long long)total.nanoseconds[1],
            (unsigned long long)total.nanoseconds[2],(unsigned long long)total.nanoseconds[3],
            (unsigned long long)total.nanoseconds[4],(unsigned long long)total.nanoseconds[5],
            (unsigned long long)total.nanoseconds[6]);
    }
    if(env->curr_timestep%100==0) {
        int moves=std::count(plan.begin(),plan.end(),FW);uint64_t expanded=0;
        for(const auto& r:results)expanded+=r.expansions;
        std::fprintf(stderr,"R05_STEP t=%d moves=%d score=%.3f expansions=%llu K=%d triaged=%d\n",
                     env->curr_timestep,moves,selected.score,(unsigned long long)expanded,cfg.futures,triaged_);
    }
}
}
