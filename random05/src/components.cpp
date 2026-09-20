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
}
