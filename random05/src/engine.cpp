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
    c.flow_seed=integer("R05_FLOW_SEED",c.flow_seed);c.flow_iterations=integer("R05_FLOW_ITERS",c.flow_iterations);
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
        std::vector<std::array<double,4>> flow(cells);
        for(int v=0;v<cells;++v)for(int d=0;d<4;++d)price[v][d]=bias[v][d]=1+jitter(random);
        for(int iteration=0;iteration<cfg.flow_iterations;++iteration) {
            std::vector<std::vector<std::array<double,4>>> partial(
                cfg.threads,std::vector<std::array<double,4>>(cells));
            #pragma omp parallel for num_threads(cfg.threads) schedule(static)
            for(int source=0;source<cells;++source) {
                auto& accumulator=partial[omp_get_thread_num()];
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
            double sum=0;int edges=0;
            for(int v=0;v<cells;++v)for(int d=0;d<4;++d) {
                flow[v][d]=0;
                for(int t=0;t<cfg.threads;++t)flow[v][d]+=partial[t][v][d];
                if(next[v][d]>=0){sum+=flow[v][d];++edges;}
            }
            const double mean=std::max(1.0,sum/edges);
            for(int v=0;v<cells;++v)for(int d=0;d<4;++d) {
                int u=next[v][d];if(u<0)continue;
                float target=bias[v][d]+float((flow[v][d]+cfg.flow_penalty*flow[u][(d+2)%4])/mean);
                price[v][d]=0.65f*price[v][d]+0.35f*target;
            }
        }
        for(int v=0;v<cells;++v)for(int d=0;d<4;++d) {
            int u=next[v][d];if(u<0)continue;
            weight[v][d]=2*(flow[v][d]>=flow[u][(d+2)%4]?1:1+cfg.flow_penalty);
        }
    }
    // Peel tree pockets. Agents exiting these get precedence over agents entering.
    pocket.assign(cells,0);
    auto deg=degree;std::queue<int> leaves;
    for(int v=0;v<cells;++v)if(deg[v]<=1)leaves.push(v);
    while(!leaves.empty()) {
        int v=leaves.front();leaves.pop();pocket[v]=1;
        for(int u:next[v])if(u>=0 && !pocket[u] && --deg[u]==1)leaves.push(u);
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
Chain::Chain(const Graph& g,const Task& task) {
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
}
float Chain::cost(const Graph& g,int stage,int cell,int direction) const {
    if(stage>=int(goals.size()))return 0;
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
    struct Pair { float cost;int agent,task; };
    std::vector<Pair> pairs;pairs.reserve(agents.size()*tasks.size());
    for(int a:agents) {
        int p=g.from_grid[env->curr_states[a].location];
        for(int j=0;j<int(tasks.size());++j) {
            int t=tasks[j];const auto& task=env->task_pool.at(t);
            float cost=g.hop(g.from_grid[task.locations[task.idx_next_loc]],p)+cfg.length_weight*length[j];
            if(cfg.guided_matching) {
                const int goal=g.from_grid[task.locations[task.idx_next_loc]];
                float approach=INF;
                for(int d=0;d<4;++d)approach=std::min(approach,g.dist(goal*4+d,p*4+env->curr_states[a].orientation));
                cost=approach/2+cfg.length_weight*length[j];
            }
            if(t==env->curr_task_schedule[a])cost-=cfg.keep_bonus;
            pairs.push_back({cost,a,j});
        }
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
                     uint64_t& expansion_count) const {
    const auto& g=*graph;const int n=int(f.loc.size());
    // pending is the already promised forward/wait move. Plan the following
    // movement on its exact resulting occupancy, while current idle robots turn.
    std::vector<int> p=f.pending, moving(n),owner(g.cells,-1),chosen(n,-1),reserve(g.cells,-1);
    for(int i=0;i<n;++i) {
        moving[i]=p[i]!=f.loc[i];owner[p[i]]=i;
        if(moving[i] && g.next[f.loc[i]][f.dir[i]]!=p[i])
            throw std::runtime_error("pending move not aligned with heading");
        if(assigned_[i] && f.stage[i]<int(assigned_[i]->goals.size()) &&
           p[i]==assigned_[i]->goals[f.stage[i]])++f.stage[i];
    }
    auto cost=[&](int a,int v,int d) {return assigned_[a]?assigned_[a]->cost(g,f.stage[a],v,d):0.0f;};
    auto allowed=[&](int a,int d) {return moving[a]?d==f.dir[a]:turn(d,f.dir[a])<=1;};
    std::vector<int> idle_heading(n);
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
        float priority=age_[i]+offsets[i];
        const bool active=assigned_[i] && f.stage[i]<int(assigned_[i]->goals.size());
        if(!active)priority-=100000;
        if(cfg.deadends && g.pocket[p[i]] &&
           (!active || !g.pocket[assigned_[i]->goals[f.stage[i]]]))priority+=1000000;
        priorities[i]=priority;
    }
    for(int i=0;i<n;++i) {
        auto& cand=candidates[i];int count=0;
        for(int d=0;d<4;++d) {
            int v=g.next[p[i]][d];if(v<0 || !allowed(i,d))continue;
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
        cand[count++]={p[i],idle_heading[i],base_cost[i]+cfg.wait_cost+0.001f};
        std::stable_sort(cand.begin(),cand.begin()+count,[](const Candidate& a,const Candidate& b){return a.score<b.score;});
        candidate_count[i]=count;
    }
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
    if(cfg.loops) {
        // 2x2 rectangular cycles. All four destinations must currently be waits.
        for(int y=0;y+1<g.rows;++y)for(int x=0;x+1<g.cols;++x) {
            int grid[]={y*g.cols+x,y*g.cols+x+1,(y+1)*g.cols+x+1,(y+1)*g.cols+x};
            int v[4],a[4];bool good=true;
            for(int k=0;k<4;++k) {
                v[k]=g.from_grid[grid[k]];
                if(v[k]<0 || owner[v[k]]<0){good=false;break;}
                a[k]=owner[v[k]];
                if(chosen[a[k]]!=v[k]){good=false;break;}
            }
            if(!good)continue;
            float best_gain=cfg.loop_threshold;int sign=0;
            for(int s:{1,3}) {
                float gain=0;bool ok=true;
                for(int k=0;k<4;++k) {
                    int u=v[(k+s)%4],d=g.direction(v[k],u);
                    if(!allowed(a[k],d)){ok=false;break;}
                    gain+=base_cost[a[k]]-cost(a[k],u,d);
                }
                if(ok && gain>best_gain){best_gain=gain;sign=s;}
            }
            if(sign)for(int k=0;k<4;++k)chosen[a[k]]=v[(k+sign)%4];
        }
    }
    actions.assign(n,W);
    for(int i=0;i<n;++i) {
        if(moving[i])actions[i]=FW;
        else {
            int d=chosen[i]==p[i]?idle_heading[i]:g.direction(p[i],chosen[i]);
            int delta=(d-f.dir[i]+4)%4;
            if(delta==2)throw std::runtime_error("pipeline requested an impossible half-turn");
            if(delta==1)actions[i]=CR;
            if(delta==3)actions[i]=CCR;
            f.dir[i]=d;
        }
    }
    f.loc=std::move(p);f.pending=std::move(chosen);
}

Rollout Engine::rollout(Frame frame,const std::vector<float>& offsets) const {
    const auto& g=*graph;Rollout r;r.offsets=offsets;
    auto total_cost=[&](const Frame& f) {
        double s=0;
        for(int i=0;i<int(f.loc.size());++i)if(assigned_[i])s+=assigned_[i]->cost(g,f.stage[i],f.loc[i],f.dir[i]);
        return s;
    };
    double initial=total_cost(frame);
    std::vector<Action> actions;
    for(int t=0;t<cfg.depth;++t) {
        advance(frame,offsets,actions,r.expansions);
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
    assigned_.assign(n,nullptr);
    for(int a=0;a<n;++a) {
        int id=schedule[a];++age_[a];
        if(previous_task_[a]>=0 && !env->task_pool.count(previous_task_[a]))age_[a]=0;
        if(id>=0) {
            const auto& task=env->task_pool.at(id);
            frame.stage[a]=task.idx_next_loc;
            if(id==previous_task_[a] && task.idx_next_loc>previous_stage_[a])age_[a]=0;
            auto& chain=chains_[id];if(!chain)chain=std::make_shared<Chain>(g,task);
            assigned_[a]=chain.get();
        }
        previous_task_[a]=id;previous_stage_[a]=frame.stage[a];
    }
    std::vector<std::vector<float>> offsets(cfg.futures,best_offsets_);
    std::uniform_real_distribution<float> unit(0,1),noise(-cfg.noise,cfg.noise);
    for(int k=1;k<cfg.futures;++k)for(int a=0;a<n;++a)
        if(k%4==0 || unit(rng_)<cfg.mutation)offsets[k][a]=noise(rng_);
    std::vector<Rollout> results(cfg.futures);
    std::vector<std::exception_ptr> errors(cfg.futures);
    #pragma omp parallel for num_threads(cfg.threads) schedule(static)
    for(int k=0;k<cfg.futures;++k) {
        try { results[k]=rollout(frame,offsets[k]); }
        catch(...) { errors[k]=std::current_exception(); }
    }
    int best=0;
    for(int k=0;k<cfg.futures;++k) {
        if(errors[k])std::rethrow_exception(errors[k]);
        if(results[k].score>results[best].score+1e-7)best=k;
    }
    auto& selected=results[best];
    certify(g,frame.loc,selected.first.loc);
    certify(g,selected.first.loc,selected.first.pending);
    plan=selected.actions;pending_=selected.first.pending;best_offsets_=selected.offsets;
    predicted_loc_=selected.first.loc;predicted_dir_=selected.first.dir;
    if(env->curr_timestep%100==0) {
        int moves=std::count(plan.begin(),plan.end(),FW);uint64_t expanded=0;
        for(const auto& r:results)expanded+=r.expansions;
        std::fprintf(stderr,"R05_STEP t=%d moves=%d score=%.3f expansions=%llu K=%d\n",
                     env->curr_timestep,moves,selected.score,(unsigned long long)expanded,cfg.futures);
    }
}
}
