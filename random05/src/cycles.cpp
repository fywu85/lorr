#include "engine.hpp"
#include <algorithm>
#include <set>
#include <stdexcept>

namespace r05 {
std::vector<std::vector<int>> grid_face_cycles(const Graph& graph,int maximum_length) {
    if(maximum_length<4 || maximum_length>32)
        throw std::invalid_argument("face cycles require a bound of4..32 vertices");
    // Every directed edge belongs to exactly one face boundary of the planar
    // free-cell graph. Follow the rightmost outgoing edge at each vertex.
    // Walk an entire boundary even when it exceeds the requested cycle bound.
    std::vector<unsigned char> visited(size_t(graph.cells)*4,0);
    std::set<std::vector<int>> cycles;
    for(int start=0;start<graph.cells;++start)for(int initial=0;initial<4;++initial) {
        if(graph.next[start][initial]<0 || visited[size_t(start)*4+initial])continue;
        int cell=start,direction=initial;std::vector<int> ring;
        do {
            const size_t edge=size_t(cell)*4+direction;
            if(visited[edge])throw std::runtime_error("face traversal entered another boundary");
            visited[edge]=1;ring.push_back(cell);
            const int next=graph.next[cell][direction];
            if(next<0)throw std::runtime_error("face traversal crossed a blocked edge");
            int outgoing=-1;
            for(int delta:{1,0,3,2}) {
                const int d=(direction+delta)%4;
                if(graph.next[next][d]>=0){outgoing=d;break;}
            }
            if(outgoing<0)throw std::runtime_error("face edge lost its reverse neighbor");
            cell=next;direction=outgoing;
        } while(cell!=start || direction!=initial);
        if(ring.size()<4 || ring.size()>size_t(maximum_length))continue;
        auto vertices=ring;std::sort(vertices.begin(),vertices.end());
        // Articulation/bridge boundaries can revisit a vertex. They are walks,
        // not simultaneous rotation cycles, and must never become proposals.
        if(std::adjacent_find(vertices.begin(),vertices.end())!=vertices.end())continue;
        std::rotate(ring.begin(),std::min_element(ring.begin(),ring.end()),ring.end());
        auto reverse=ring;std::reverse(reverse.begin()+1,reverse.end());
        cycles.insert(std::min(ring,reverse));
    }
    return {cycles.begin(),cycles.end()};
}
}
