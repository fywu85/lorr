#include "engine.hpp"
#include <algorithm>
#include <numeric>
#include <stdexcept>
namespace r05 {
std::vector<std::vector<int>> decision_components(const Graph& graph,const Rollout& left,const Rollout& right) {
    const int n=int(left.first.loc.size());
    if(left.first.loc!=right.first.loc || left.first.pending.size()!=size_t(n) ||
       right.first.pending.size()!=size_t(n) || left.actions.size()!=size_t(n) || right.actions.size()!=size_t(n))
        throw std::invalid_argument("motion components require common first positions");
    std::vector<int> parent(n),owner(graph.cells,-1),destination_owner(graph.cells,-1);
    std::iota(parent.begin(),parent.end(),0);
    auto root=[&](int a) {while(parent[a]!=a){parent[a]=parent[parent[a]];a=parent[a];}return a;};
    auto unite=[&](int a,int b) {a=root(a);b=root(b);if(a!=b)parent[a]=b;};
    for(int a=0;a<n;++a)owner.at(left.first.loc[a])=a;
    for(int a=0;a<n;++a)for(int v:{left.first.pending[a],right.first.pending[a]}) {
        if(owner.at(v)>=0)unite(a,owner[v]);
        // A currently empty cell can be wanted by different robots in the two
        // parents. They must belong to the same component as well.
        if(destination_owner.at(v)>=0)unite(a,destination_owner[v]);
        else destination_owner[v]=a;
    }
    std::vector<std::vector<int>> groups(n),result;
    for(int a=0;a<n;++a)groups[root(a)].push_back(a);
    for(auto& group:groups) {
        bool changed=false;
        for(int a:group)changed|=left.first.pending[a]!=right.first.pending[a] || left.actions[a]!=right.actions[a];
        if(changed)result.push_back(std::move(group));
    }
    return result;
}
// Connect exactly the cross-parent vertex and reverse-edge conflicts. Choosing
// either complete parent's paths for each component cannot introduce a conflict
// between components, since both unchanged parents are already feasible.
std::vector<std::vector<int>> window_conflict_components(const Graph& graph,
    const std::vector<std::vector<int>>& left,const std::vector<std::vector<int>>& right) {
    const int n=int(left.size());
    if(right.size()!=left.size())throw std::invalid_argument("window component team mismatch");
    if(n==0)return {};
    const int steps=int(left[0].size());
    if(steps==0)throw std::invalid_argument("empty window component path");
    for(int a=0;a<n;++a)if(int(left[a].size())!=steps || int(right[a].size())!=steps || left[a][0]!=right[a][0])
        throw std::invalid_argument("window components require common starts and horizons");
    std::vector<int> parent(n),owner(graph.cells,-1);
    std::iota(parent.begin(),parent.end(),0);
    auto root=[&](int a) {while(parent[a]!=a){parent[a]=parent[parent[a]];a=parent[a];}return a;};
    auto unite=[&](int a,int b) {a=root(a);b=root(b);if(a!=b)parent[a]=b;};
    for(int t=1;t<steps;++t) {
        std::fill(owner.begin(),owner.end(),-1);
        for(int a=0;a<n;++a)owner.at(left[a][t]/4)=a;
        for(int b=0;b<n;++b) {
            int a=owner.at(right[b][t]/4);
            if(a>=0)unite(a,b);
            // A left path ends where the right path started, and vice versa.
            a=owner.at(right[b][t-1]/4);
            if(a>=0 && left[a][t-1]/4==right[b][t]/4)unite(a,b);
        }
    }
    std::vector<std::vector<int>> groups(n),result;
    for(int a=0;a<n;++a)groups[root(a)].push_back(a);
    for(auto& group:groups) {
        bool changed=false;for(int a:group)changed|=left[a]!=right[a];
        if(changed)result.push_back(std::move(group));
    }
    return result;
}

// Pairwise scans avoid allocating/clearing a map-sized owner table for every
// tiny LNS repair. Unlike whole-plan mixing, the right choice can be a legal
// planned prefix of agents plus old paths for unplanned members. Any resulting
// right-right conflict then also appears as a cross-choice conflict below.
std::vector<std::vector<int>> window_repair_components(
    const std::vector<std::vector<int>>& left,const std::vector<std::vector<int>>& right) {
    const int n=int(left.size());
    if(right.size()!=left.size())throw std::invalid_argument("repair component team mismatch");
    if(!n)return {};
    const int steps=int(left[0].size());
    if(!steps)throw std::invalid_argument("empty repair component path");
    for(int a=0;a<n;++a)if(int(left[a].size())!=steps || int(right[a].size())!=steps || left[a][0]!=right[a][0])
        throw std::invalid_argument("repair components require common starts and horizons");
    std::vector<int> parent(n);std::iota(parent.begin(),parent.end(),0);
    auto root=[&](int a){while(parent[a]!=a){parent[a]=parent[parent[a]];a=parent[a];}return a;};
    for(int a=0;a<n;++a)for(int b=a+1;b<n;++b) {
        bool conflict=false;
        for(int t=1;t<steps && !conflict;++t) {
            const int la=left[a][t]/4,lb=left[b][t]/4,ra=right[a][t]/4,rb=right[b][t]/4;
            conflict=la==rb || lb==ra ||
                (la==right[b][t-1]/4 && left[a][t-1]/4==rb) ||
                (lb==right[a][t-1]/4 && left[b][t-1]/4==ra);
        }
        if(conflict){int x=root(a),y=root(b);if(x!=y)parent[x]=y;}
    }
    std::vector<std::vector<int>> groups(n),result;
    for(int a=0;a<n;++a)groups[root(a)].push_back(a);
    for(auto& group:groups)if(!group.empty())result.push_back(std::move(group));
    return result;
}

}
