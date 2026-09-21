#include "engine.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace r05 {
static int turn_distance(int a,int b) {int d=(a-b+4)%4;return std::min(d,4-d);}
JointMoves joint_move_assignment(const Graph& g,const Config& cfg,const Frame& frame,
    const std::vector<const Chain*>& assigned,float movement_price) {
    const int n=int(frame.loc.size());
    if(int(frame.pending.size())!=n || int(frame.dir.size())!=n || int(frame.stage.size())!=n ||
       int(assigned.size())!=n || n>g.cells || movement_price<0 || !std::isfinite(movement_price))
        throw std::invalid_argument("invalid joint movement proposal");
    const auto& p=frame.pending;
    std::vector<int> owner(g.cells,-1),idle_heading(n);
    std::vector<float> matrix(size_t(n)*g.cells,1e12f);
    for(int a=0;a<n;++a) {
        if(p[a]<0 || p[a]>=g.cells || owner[p[a]]>=0)throw std::invalid_argument("invalid promised occupancy");
        owner[p[a]]=a;
        const bool moving=p[a]!=frame.loc[a];const int heading=frame.dir[a];
        const Chain* chain=assigned[a];int stage=frame.stage[a];
        if(chain && stage<int(chain->goals.size()) && p[a]==chain->goals[stage])++stage;
        if(chain && stage>=int(chain->goals.size()))chain=nullptr;
        const auto* row=chain?chain->cached_row(g,stage):nullptr;
        auto cost=[&](int v,int d) {
            if(row)return row[v*4+d];
            return chain?chain->cost(g,stage,v,d):cfg.idle_eviction*g.pocket_depth[v];
        };
        int idle=heading;float best=cost(p[a],heading);
        if(!moving)for(int d:{(heading+1)%4,(heading+3)%4}) {
            const float value=cost(p[a],d)+.05f*g.weight[p[a]][4];
            if(value<best-1e-5f){best=value;idle=d;}
        }
        idle_heading[a]=idle;
        const float base=cost(p[a],heading);
        // Subtract a per-row constant for numerical conditioning. Both models
        // minimize aggregate remaining cost; one also charges half the lane
        // price. Idle robots prefer waiting unless moving helps the joint plan.
        matrix[size_t(a)*g.cells+p[a]]=cost(p[a],idle)-base+(chain?.001f:0.f);
        for(int d=0;d<4;++d) {
            const int v=g.next[p[a]][d];
            if(v<0 || (moving?d!=heading:turn_distance(d,heading)>1))continue;
            const float lane=g.forward_weight(chain?chain->goals[stage]:-1,p[a],d);
            matrix[size_t(a)*g.cells+v]=cost(v,d)-base+movement_price*lane+(chain?0.f:.0001f);
        }
    }
    JointMoves result;result.targets=hungarian_assignment(matrix,n,g.cells);result.headings.resize(n);
    for(int a=0;a<n;++a) {
        const int v=result.targets[a];
        if(v<0 || v>=g.cells || matrix[size_t(a)*g.cells+v]>=1e11f)
            throw std::runtime_error("joint movement assignment used a forbidden edge");
    }
    // The matching consists of disjoint paths to holes and directed cycles.
    // Canceling a two-cycle frees neither endpoint to another component, so
    // replacing both moves by waits preserves every other assignment.
    for(int a=0;a<n;++a) {
        const int b=owner[result.targets[a]];
        if(b>=0 && b!=a && result.targets[b]==p[a]) {
            result.targets[a]=p[a];result.targets[b]=p[b];++result.canceled_swaps;
        }
    }
    for(int a=0;a<n;++a) {
        result.headings[a]=result.targets[a]==p[a]?idle_heading[a]:g.direction(p[a],result.targets[a]);
        if((p[a]!=frame.loc[a] && result.headings[a]!=frame.dir[a]) ||
           turn_distance(result.headings[a],frame.dir[a])>1)
            throw std::runtime_error("joint movement assignment violates pipeline headings");
    }
    Engine::certify(g,p,result.targets);
    return result;
}
}
