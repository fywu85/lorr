#include "engine.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
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
}
Config Config::environment(const SharedEnvironment& env) {
    Config c;
    c.futures=integer("R05_K",c.futures);c.depth=integer("R05_DEPTH",c.depth);
    c.threads=integer("R05_THREADS",c.threads);c.seed=integer("R05_SEED",c.seed);
    c.noise=real("R05_NOISE",c.noise);c.mutation=real("R05_MUTATION",c.mutation);
    c.dispersion=real("R05_DISPERSION",c.dispersion);c.push_price=real("R05_PUSH",c.push_price);
    c.loop_threshold=real("R05_LOOP_THRESHOLD",c.loop_threshold);
    c.length_weight=real("R05_LENGTH_WEIGHT",c.length_weight);c.keep_bonus=real("R05_KEEP_BONUS",c.keep_bonus);
    c.turn_cost=real("R05_TURN_COST",c.turn_cost);c.wait_cost=real("R05_WAIT_COST",c.wait_cost);
    c.matching=integer("R05_MATCH",1);c.loops=integer("R05_LOOPS",1);c.deadends=integer("R05_DEADENDS",1);
    c.cycle_portfolio=integer("R05_CYCLE_PORTFOLIO",0);
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
    c.flow_seed=integer("R05_FLOW_SEED",c.flow_seed);c.flow_iterations=integer("R05_FLOW_ITERS",c.flow_iterations);
    c.rollout_age=integer("R05_ROLLOUT_AGE",0);c.cost_cache=integer("R05_COST_CACHE",0);
    c.pocket_components=integer("R05_POCKET_COMPONENTS",0);
    c.flow_turn=real("R05_FLOW_TURN",0);
    c.flow_power=real("R05_FLOW_POWER",1);c.flow_alpha=real("R05_FLOW_ALPHA",1);
    c.flow_betweenness=real("R05_FLOW_BETWEENNESS",0);
    c.flow_average=integer("R05_FLOW_AVERAGE",0);c.flow_normalize=integer("R05_FLOW_NORMALIZE",0);c.loop_extent=integer("R05_LOOP_EXTENT",2);
    c.predict_matching=integer("R05_SCHED_PREDICT",0);
    c.flow_penalty=real("R05_FLOW_PENALTY",c.flow_penalty);c.guided_matching=integer("R05_SCHED_GUIDE",0);
    if(const char* v=std::getenv("R05_GUIDANCE")) c.guidance=v;
    if(const char* v=std::getenv("R05_WEIGHTS")) c.weights=v;
    if(c.guidance!="none" && env.trick_instance!="RANDOM-05")
        throw std::invalid_argument("guidance experiments require --trick RANDOM-05");
    if(c.futures<1 || c.depth<1 || c.threads<1 || c.depth>64 || c.turn_cost<=0 ||
       c.wait_cost<=0 || c.mutation<0 || c.mutation>1)
        throw std::invalid_argument("invalid R05 configuration");
    return c;
}

Graph::Graph(const SharedEnvironment& env,const Config& cfg) {
    rows=env.rows;cols=env.cols;from_grid.assign(env.map.size(),-1);
    for(int i=0;i<int(env.map.size());++i) if(!env.map[i]) {
        from_grid[i]=int(to_grid.size());to_grid.push_back(i);
    }
    cells=int(to_grid.size());states=4*cells;
    if(cells>4096) throw std::invalid_argument("initial exact-table implementation limited to 4096 free cells");
    next.resize(cells);weight.resize(cells);degree.assign(cells,0);
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
        double weight_sum=0;int weight_count=0;
        for(int v=0;v<cells;++v)for(int d=0;d<4;++d) {
            int u=next[v][d];if(u<0)continue;
            weight[v][d]=2*(flow[v][d]>=flow[u][(d+2)%4]?1:1+cfg.flow_penalty);
            weight[v][d]*=1+cfg.flow_betweenness*float((load[v]+load[u])/(2*mean_load));
            weight_sum+=weight[v][d];++weight_count;
        }
        if(cfg.flow_normalize) {
            float scale=float(2*weight_count/weight_sum);
            for(auto& w:weight)for(int d=0;d<4;++d)w[d]*=scale;
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
        values.resize(goals.size(),std::vector<float>(g.states));
        for(int k=0;k<int(goals.size());++k)for(int state=0;state<g.states;++state) {
            float best=INF;
            for(int q=0;q<4;++q)best=std::min(best,g.dist(goals[k]*4+q,state)+tail[k][q]);
            values[k][state]=best;
        }
    }
}
float Chain::cost(const Graph& g,int stage,int cell,int direction) const {
    if(stage>=int(goals.size()))return 0;
    if(!values.empty())return values[stage][cell*4+direction];
    float best=INF;
    for(int q=0;q<4;++q)best=std::min(best,g.dist(goals[stage]*4+q,cell*4+direction)+tail[stage][q]);
    return best;
}
void Engine::initialize(SharedEnvironment* env) {
    rng_.seed(cfg.seed);graph=std::make_unique<Graph>(*env,cfg);
    const int n=env->num_of_agents;
    age_.assign(n,0);previous_task_.assign(n,-1);previous_stage_.assign(n,0);
    best_offsets_.resize(n);
    for(float& x:best_offsets_)x=std::uniform_real_distribution<float>(0,1)(rng_);
    std::fprintf(stderr,"R05_INIT agents=%d cells=%d K=%d depth=%d threads=%d guidance=%s seed=%d table_mb=%.1f\n",
                 n,graph->cells,cfg.futures,cfg.depth,cfg.threads,cfg.guidance.c_str(),cfg.seed,
                 graph->distance.size()*sizeof(float)/1e6);
}
void Engine::match(SharedEnvironment* env,std::vector<int>& schedule) {
    const auto& g=*graph;const int n=env->num_of_agents;
    schedule=env->curr_task_schedule;
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
            float cost=g.hop(g.from_grid[task.locations[task.idx_next_loc]],p)+cfg.length_weight*length[j];
            if(cfg.guided_matching) {
                const int goal=g.from_grid[task.locations[task.idx_next_loc]];
                float approach=INF;
                for(int d=0;d<4;++d)approach=std::min(approach,g.dist(goal*4+d,p*4+env->curr_states[a].orientation));
                cost=approach/2+cfg.length_weight*length[j];
            }
            if(cfg.chain_matching) {
                const int goal=g.from_grid[task.locations[0]];
                cost=INF;
                for(int d=0;d<4;++d)cost=std::min(cost,
                    (g.dist(goal*4+d,p*4+env->curr_states[a].orientation)
                     +cfg.length_weight*continuation[j][d])/2);
            }
            if(t==env->curr_task_schedule[a])cost-=cfg.keep_bonus;
            if(exact)matrix[size_t(row)*tasks.size()+j]=cost;
            else pairs.push_back({cost,a,j});
        }
    }
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
        return;
    }
    std::sort(pairs.begin(),pairs.end(),[](const Pair& a,const Pair& b) {
        if(a.cost!=b.cost)return a.cost<b.cost;
        return std::tie(a.agent,a.task)<std::tie(b.agent,b.task);
    });
    std::vector<bool> used(tasks.size(),false);
    for(const auto& p:pairs)if(schedule[p.agent]<0 && !used[p.task]) {
        schedule[p.agent]=tasks[p.task];used[p.task]=true;
    }
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

void Engine::advance(Frame& f,const std::vector<float>& offsets,std::vector<Action>& actions,
                     uint64_t& expansion_count,bool cycle_moves) const {
    const int cycle_mode=cycle_moves?cfg.pre_cycles:0;
    const auto& g=*graph;const int n=int(f.loc.size());
    // pending is the already promised forward/wait move. Plan the following
    // movement on its exact resulting occupancy, while current idle robots turn.
    std::vector<int> p=f.pending, moving(n),owner(g.cells,-1),chosen(n,-1),reserve(g.cells,-1);
    for(int i=0;i<n;++i) {
        moving[i]=p[i]!=f.loc[i];owner[p[i]]=i;
        if(moving[i] && g.next[f.loc[i]][f.dir[i]]!=p[i])
            throw std::runtime_error("pending move not aligned with heading");
        bool arrived=assigned_[i] && f.stage[i]<int(assigned_[i]->goals.size()) &&
                     p[i]==assigned_[i]->goals[f.stage[i]];
        if(arrived)++f.stage[i];
        if(cfg.rollout_age)f.age[i]=arrived?0:f.age[i]+1;
    }
    auto cost=[&](int a,int v,int d) {
        if(assigned_[a] && f.stage[a]<int(assigned_[a]->goals.size()))
            return assigned_[a]->cost(g,f.stage[a],v,d);
        return cfg.idle_eviction*g.pocket_depth[v];
    };
    auto allowed=[&](int a,int d) {return moving[a]?d==f.dir[a]:turn(d,f.dir[a])<=1;};
    std::vector<int> idle_heading(n),forced_heading(n,-1);
    std::vector<float> base_cost(n),priorities(n);
    struct Candidate { int v,d;float score; };
    std::vector<std::array<Candidate,5>> candidates(n);
    std::vector<int> candidate_count(n);
    for(int i=0;i<n;++i) {
        int best_dir=f.dir[i];float best=cost(i,p[i],best_dir);
        if(!moving[i])for(int q:{(f.dir[i]+1)%4,(f.dir[i]+3)%4}) {
            float x=cost(i,p[i],q)+0.05f*g.weight[p[i]][4];
            if(x<best-1e-5f) {best=x;best_dir=q;}
        }
        idle_heading[i]=best_dir;
        base_cost[i]=cost(i,p[i],best_dir);
        int age=cfg.rollout_age?f.age[i]:age_[i];
        if(cfg.age_cap>0)age=std::min(age,cfg.age_cap);
        float priority=age+offsets[i];
        const bool active=assigned_[i] && f.stage[i]<int(assigned_[i]->goals.size());
        if(!active)priority-=100000;
        if(cfg.deadends && g.pocket[p[i]] &&
           (!active || g.pocket[assigned_[i]->goals[f.stage[i]]]!=g.pocket[p[i]]))priority+=1000000;
        priorities[i]=priority;
    }
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
    auto choose=[&](bool kinematic) {
    std::fill(chosen.begin(),chosen.end(),-1);
    std::fill(reserve.begin(),reserve.end(),-1);
    for(int i=0;i<n;++i) {
        auto& cand=candidates[i];int count=0;
        for(int d=0;d<4;++d) {
            int v=g.next[p[i]][d];if(v<0 || (kinematic && !allowed(i,d)))continue;
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
        std::stable_sort(cand.begin(),cand.begin()+count,[](const Candidate& a,const Candidate& b){return a.score<b.score;});
        candidate_count[i]=count;
    }
    if(kinematic && cycle_mode>0 && cycle_mode<3)propose_cycles(false);
    std::vector<int> order(n);std::iota(order.begin(),order.end(),0);
    std::stable_sort(order.begin(),order.end(),[&](int a,int b){return priorities[a]>priorities[b];});
    int expansions=0;
    std::function<bool(int)> pibt=[&](int a)->bool {
        ++expansions;
        for(int k=0;k<candidate_count[a];++k) {
            int v=candidates[a][k].v;
            if(expansions>cfg.expansion_limit && v!=p[a])continue;
            if(reserve[v]>=0)continue;
            int b=owner[v];
            if(b>=0 && b!=a && chosen[b]==p[a])continue;
            chosen[a]=v;reserve[v]=a;
            if(b>=0 && b!=a && chosen[b]<0 && !pibt(b)) {
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
    for(int a:order)if(chosen[a]<0)pibt(a);
    expansion_count+=expansions;
    return chosen;
    };
    std::vector<int> intent;
    if(cfg.intent_rotation)intent=choose(false);
    chosen=choose(true);
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
    f.loc=std::move(p);f.pending=std::move(chosen);
}

Rollout Engine::rollout(Frame frame,const std::vector<float>& offsets,bool cycle_moves) const {
    const auto& g=*graph;Rollout r;r.offsets=offsets;r.cycle_moves=cycle_moves;
    auto total_cost=[&](const Frame& f) {
        double s=0;
        for(int i=0;i<int(f.loc.size());++i)if(assigned_[i])s+=assigned_[i]->cost(g,f.stage[i],f.loc[i],f.dir[i]);
        return s;
    };
    double initial=total_cost(frame);
    std::vector<Action> actions;
    for(int t=0;t<cfg.depth;++t) {
        advance(frame,offsets,actions,r.expansions,cycle_moves);
        if(t==0){r.first=frame;r.actions=actions;}
    }
    r.score=(initial-total_cost(frame))/2.0;
    if(cfg.dispersion) {
        std::vector<int> occupancy(g.from_grid.size(),0);
        for(int v:frame.loc)occupancy[g.to_grid[v]]=1;
        int pairs=0;
        for(int v:frame.loc) {
            int p=g.to_grid[v],x=p%g.cols,y=p/g.cols;
            for(int yy=std::max(0,y-2);yy<=std::min(g.rows-1,y+2);++yy)
                for(int xx=std::max(0,x-2);xx<=std::min(g.cols-1,x+2);++xx)
                    if(yy*g.cols+xx>p)pairs+=occupancy[yy*g.cols+xx];
        }
        r.score-=cfg.dispersion*pairs;
    }
    return r;
}
void Engine::compute(SharedEnvironment* env,std::vector<Action>& plan,std::vector<int>& schedule) {
    const auto& g=*graph;const int n=env->num_of_agents;
    Frame frame;frame.loc.resize(n);frame.dir.resize(n);frame.stage.resize(n);
    for(int a=0;a<n;++a) {
        frame.loc[a]=g.from_grid[env->curr_states[a].location];frame.dir[a]=env->curr_states[a].orientation;
        if(!predicted_loc_.empty() && (predicted_loc_[a]!=frame.loc[a] || predicted_dir_[a]!=frame.dir[a]))
            throw std::runtime_error("simulator diverged from pipeline prediction");
    }
    if(pending_.empty())pending_=frame.loc;
    frame.pending=pending_;
    match(env,schedule);
    for(auto it=chains_.begin();it!=chains_.end();) {
        if(!env->task_pool.count(it->first))it=chains_.erase(it);else ++it;
    }
    assigned_.assign(n,nullptr);triaged_=0;
    for(int a=0;a<n;++a) {
        int id=schedule[a];++age_[a];
        if(previous_task_[a]>=0 && !env->task_pool.count(previous_task_[a]))age_[a]=0;
        if(id>=0) {
            const auto& task=env->task_pool.at(id);
            frame.stage[a]=task.idx_next_loc;
            if(id==previous_task_[a] && task.idx_next_loc>previous_stage_[a])age_[a]=0;
            auto& chain=chains_[id];if(!chain)chain=std::make_shared<Chain>(g,task,cfg.cost_cache);
            assigned_[a]=chain.get();
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
    frame.age=age_;
    std::vector<std::vector<float>> offsets(cfg.futures,best_offsets_);
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
    for(int k=1;k<cfg.futures;++k)for(int a=0;a<n;++a)
        if(k%4==0 || unit(global_rng)<cfg.mutation)offsets[k][a]=noise(global_rng);
    std::vector<Rollout> results(cfg.futures);
    std::vector<std::exception_ptr> errors(cfg.futures);
    #pragma omp parallel for num_threads(cfg.threads) schedule(static)
    for(int k=0;k<cfg.futures;++k) {
        try { results[k]=rollout(frame,offsets[k],!cfg.cycle_portfolio || k%2==1); }
        catch(...) { errors[k]=std::current_exception(); }
    }
    int best=0;
    for(int k=0;k<cfg.futures;++k) {
        if(errors[k])std::rethrow_exception(errors[k]);
        if(results[k].score>results[best].score+1e-7 ||
           (cfg.accept_equal && results[k].score>=results[best].score-1e-7))best=k;
    }
    for(int trial=0;trial<cfg.local_trials;++trial) {
        auto local=results[best].offsets;
        int center=std::uniform_int_distribution<int>(0,n-1)(local_rng);
        int p=g.to_grid[frame.loc[center]];
        for(int a=0;a<n;++a) {
            int q=g.to_grid[frame.loc[a]];
            if(std::abs(p/g.cols-q/g.cols)<=2 && std::abs(p%g.cols-q%g.cols)<=2)local[a]=noise(local_rng);
        }
        Rollout candidate=rollout(frame,local,results[best].cycle_moves);
        if(candidate.score>results[best].score+1e-7 ||
           (cfg.accept_equal && candidate.score>=results[best].score-1e-7))results[best]=std::move(candidate);
    }
    auto& selected=results[best];
    certify(g,frame.loc,selected.first.loc);
    certify(g,selected.first.loc,selected.first.pending);
    plan=selected.actions;pending_=selected.first.pending;best_offsets_=selected.offsets;
    predicted_loc_=selected.first.loc;predicted_dir_=selected.first.dir;
    total_agent_steps_+=n;total_forward_+=std::count(plan.begin(),plan.end(),FW);
    if(env->curr_timestep%100==0) {
        int moves=std::count(plan.begin(),plan.end(),FW);uint64_t expanded=0;
        for(const auto& r:results)expanded+=r.expansions;
        std::fprintf(stderr,"R05_STEP t=%d moves=%d score=%.3f expansions=%llu K=%d triaged=%d\n",
                     env->curr_timestep,moves,selected.score,(unsigned long long)expanded,cfg.futures,triaged_);
    }
}
}
