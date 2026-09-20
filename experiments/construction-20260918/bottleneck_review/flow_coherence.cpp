// Read-only trajectory replay. No result from this program is a planner input.
#include "nlohmann/json.hpp"
#include "flow_guidance.hpp"
#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>
using json = nlohmann::ordered_json;
using Counts = std::vector<std::array<uint64_t, 2>>;

struct Topology {
    struct Edge { int u, v, d, group = -1; };
    int rows, cols;
    std::vector<char> free;
    std::vector<int> edge_at, group_at;
    std::vector<Edge> edges;
    std::vector<std::vector<int>> groups;
    int straight_cells = 0;
    int neighbor(int u, int d) const {
        if(d==0)return u%cols+1<cols?u+1:-1;
        if(d==1)return u/cols+1<rows?u+cols:-1;
        if(d==2)return u%cols?u-1:-1;
        return u/cols?u-cols:-1;
    }
    Topology(std::vector<char> mask, int r, int c):rows(r),cols(c),free(std::move(mask)) {
        if(rows<1||cols<1||free.size()!=size_t(rows)*cols)throw std::runtime_error("topology dimensions");
        const int n=free.size();std::vector<int> axis(n,-1);group_at.assign(n,-1);edge_at.assign(n*4,-1);
        for(int u=0;u<n;++u)if(free[u]){
            int mask=0;for(int d=0;d<4;++d){int v=neighbor(u,d);if(v>=0&&free[v])mask|=1<<d;}
            if(mask==5)axis[u]=0;else if(mask==10)axis[u]=1;
            straight_cells+=axis[u]>=0;
        }
        for(int u=0;u<n;++u)if(axis[u]>=0&&group_at[u]<0){
            const int group=groups.size();groups.push_back({});
            std::vector<int> stack{u};group_at[u]=group;
            while(!stack.empty()){
                int v=stack.back();stack.pop_back();
                for(int d:{axis[v],axis[v]+2}){int w=neighbor(v,d);
                    if(w>=0&&axis[w]==axis[v]&&group_at[w]<0){group_at[w]=group;stack.push_back(w);}}
            }
        }
        for(int u=0;u<n;++u)if(free[u])for(int d=0;d<2;++d){int v=neighbor(u,d);if(v<0||!free[v])continue;
            int a=group_at[u],b=group_at[v];if(a>=0&&b>=0&&a!=b)throw std::runtime_error("adjacent chain groups differ");
            int group=std::max(a,b),id=edges.size();edges.push_back({u,v,d,group});
            edge_at[u*4+d]=edge_at[v*4+d+2]=id;if(group>=0)groups[group].push_back(id);
        }
    }
    json summary()const{
        int incident=0;json histogram=json::object();std::vector<int> lengths(groups.size());
        for(int group:group_at)if(group>=0)++lengths[group];
        for(int length:lengths){std::string key=std::to_string(length);histogram[key]=histogram.value(key,0)+1;}
        for(const auto&e:edges)incident+=e.group>=0;
        return {{"free_cells",std::count(free.begin(),free.end(),1)}, {"undirected_edges",edges.size()},
                {"straight_degree2_cells",straight_cells},{"chains",groups.size()},
                {"chain_length_histogram",histogram},{"incident_edges",incident}};
    }
};

json traffic(const Topology& g,const Counts& counts){
    uint64_t all=0,covered=0,edge_minority=0,chain_minority=0;int used=0,opposed=0;
    for(const auto&x:counts)all+=x[0]+x[1];
    for(const auto&group:g.groups){uint64_t f=0,r=0;
        for(int id:group){f+=counts[id][0];r+=counts[id][1];edge_minority+=std::min(counts[id][0],counts[id][1]);}
        covered+=f+r;chain_minority+=std::min(f,r);used+=f+r>0;opposed+=f>0&&r>0;
    }
    return {{"all_forward_moves",all},{"corridor_forward_moves",covered},
        {"movement_coverage",all?json(double(covered)/all):json(nullptr)},
        {"sum_chain_minority_moves",chain_minority},{"sum_edge_minority_moves",edge_minority},
        {"chain_minority_share",covered?json(double(chain_minority)/covered):json(nullptr)},
        {"edge_minority_share",covered?json(double(edge_minority)/covered):json(nullptr)},
        {"visited_chains",used},{"bidirectional_chains",opposed}};
}

int surcharge(uint64_t forward,uint64_t reverse,uint64_t minimum){
    const uint64_t total=forward+reverse;
    if(total<minimum||reverse<=forward||(reverse-forward)*100<=25*total)return 0;
    return int((4*(reverse-forward)+total-1)/total);
}

json field_summary(const Topology&g,const Counts&counts,const std::vector<uint8_t>&costs){
    int mixed=0,directional=0,all_neutral=0,fully_directional=0,partial=0,flips=0,adjacent=0;
    int neutral_edges=0,directional_edges=0,changed=0,newly_penalized=0,removed=0,penalized_all=0;
    uint64_t mixed_traffic=0,covered=0,changes_traffic=0;
    for(const auto&e:g.edges)for(int k=0;k<2;++k)penalized_all+=costs[(k?e.v:e.u)*4+(k?e.d+2:e.d)]>4;
    for(const auto&group:g.groups){
        bool plus=false,minus=false;int zeros=0,previous=0;uint64_t f=0,r=0;
        for(int id:group){const auto&e=g.edges[id];f+=counts[id][0];r+=counts[id][1];
            int p=costs[e.u*4+e.d]-4,m=costs[e.v*4+e.d+2]-4;
            if(p&&m)throw std::runtime_error("both edge directions penalized");
            // Positive sign means preferred movement in the increasing coordinate.
            int sign=m?1:p?-1:0;plus|=sign>0;minus|=sign<0;zeros+=!sign;
            neutral_edges+=!sign;directional_edges+=bool(sign);
            if(previous&&sign){++adjacent;flips+=previous!=sign;}previous=sign;
        }
        directional+=plus||minus;all_neutral+=!(plus||minus);fully_directional+=!zeros;partial+=zeros>0&&zeros<int(group.size());
        mixed+=plus&&minus;covered+=f+r;if(plus&&minus)mixed_traffic+=f+r;
        // Offline pooling proposal: keep the minimum mean evidence per edge8,
        // margin25 and strength4, avoiding a threshold reduction from summing.
        const int pooled[2]={surcharge(f,r,8*group.size()),surcharge(r,f,8*group.size())};
        for(int id:group){const auto&e=g.edges[id];for(int k=0;k<2;++k){
            const int old=costs[(k?e.v:e.u)*4+(k?e.d+2:e.d)]-4;
            changed+=old!=pooled[k];newly_penalized+=!old&&pooled[k];removed+=old&&!pooled[k];
            if(old!=pooled[k])changes_traffic+=counts[id][k];
        }}
    }
    return {{"penalized_edges",penalized_all},{"corridor_directional_edges",directional_edges},
        {"corridor_neutral_edges",neutral_edges},{"directional_chains",directional},
        {"fully_neutral_chains",all_neutral},{"fully_directional_chains",fully_directional},
        {"partly_neutral_chains",partial},{"mixed_sign_chains",mixed},
        {"adjacent_directional_pairs",adjacent},{"opposite_adjacent_pairs",flips},
        {"cumulative_corridor_moves",covered},{"cumulative_moves_in_mixed_sign_chains",mixed_traffic},
        {"mixed_sign_traffic_share",covered?json(double(mixed_traffic)/covered):json(nullptr)},
        {"pooled_counterfactual_changed_directed_edges",changed},{"pooled_new_penalties",newly_penalized},
        {"pooled_removed_penalties",removed},{"historical_crossings_on_changed_directions",changes_traffic}};
}

std::vector<uint8_t> independent_field(const Topology&g,const Counts&counts){
    std::vector<uint8_t> result(g.free.size()*4,4);
    for(size_t id=0;id<g.edges.size();++id){const auto&e=g.edges[id];
        result[e.u*4+e.d]+=surcharge(counts[id][0],counts[id][1],8);
        result[e.v*4+e.d+2]+=surcharge(counts[id][1],counts[id][0],8);}
    return result;
}

void fixtures(){
    Topology corridor(std::vector<char>(5,1),1,5);if(corridor.groups.size()!=1||corridor.straight_cells!=3||corridor.edges.size()!=4)throw std::runtime_error("line topology fixture");
    Topology corner(std::vector<char>(4,1),2,2);if(!corner.groups.empty())throw std::runtime_error("corner fixture");
    Topology junction({0,1,0,1,1,1,0,1,0},3,3);if(!junction.groups.empty())throw std::runtime_error("junction fixture");
    Counts counts(4);for(auto&x:counts)x={10,2};auto field=independent_field(corridor,counts);
    auto coherent=field_summary(corridor,counts,field);auto flow=traffic(corridor,counts);
    if(coherent["mixed_sign_chains"]!=0||coherent["fully_directional_chains"]!=1||flow["sum_chain_minority_moves"]!=8||flow["corridor_forward_moves"]!=48)throw std::runtime_error("coherent-count fixture");
    counts={{{8,0}},{{8,0}},{{0,8}},{{0,8}}};auto mixed=field_summary(corridor,counts,independent_field(corridor,counts));flow=traffic(corridor,counts);
    if(mixed["mixed_sign_chains"]!=1||mixed["opposite_adjacent_pairs"]!=1||mixed["pooled_removed_penalties"]!=4||flow["sum_chain_minority_moves"]!=16||flow["sum_edge_minority_moves"]!=0)throw std::runtime_error("opposing-count fixture");
    counts={{{12,0}},{{12,0}},{{12,0}},{{0,0}}};auto sparse=field_summary(corridor,counts,independent_field(corridor,counts));
    if(sparse["partly_neutral_chains"]!=1||sparse["pooled_new_penalties"]!=1)throw std::runtime_error("neutral-gap fixture");
    for(auto&x:counts)x={5,4};auto neutral=field_summary(corridor,counts,independent_field(corridor,counts));
    if(neutral["fully_neutral_chains"]!=1||neutral["pooled_counterfactual_changed_directed_edges"]!=0)throw std::runtime_error("margin fixture");
    std::cout<<"FLOW_COHERENCE_FIXTURES_PASS 7\n";
}

int main(int argc,char**argv){
    fixtures();if(argc==2&&std::string(argv[1])=="--self-test")return 0;
    if(argc!=5)throw std::runtime_error("usage: coherence MAP RESULT OUTPUT LEARNED(0/1)");
    const bool learned=std::string(argv[4])=="1";
    std::ifstream map_file(argv[1]);std::string word;int rows=0,cols=0;
    while(map_file>>word&&word!="map"){if(word=="height")map_file>>rows;else if(word=="width")map_file>>cols;}
    if(!rows||!cols)throw std::runtime_error("invalid map header");
    std::vector<char> free(rows*cols);for(int r=0;r<rows;++r){map_file>>word;if(int(word.size())!=cols)throw std::runtime_error("map row");for(int c=0;c<cols;++c)free[r*cols+c]=word[c]!='@'&&word[c]!='T';}
    Topology g(free,rows,cols);json data;std::ifstream(argv[2])>>data;
    json report={{"team_size",data.at("teamSize")},{"steps",data.at("makespan")},{"tasks",data.at("numTaskFinished")},
        {"planner_errors",data.at("numPlannerErrors")},{"schedule_errors",data.at("numScheduleErrors")},{"entry_timeouts",data.at("numEntryTimeouts")},
        {"topology",g.summary()},{"learned_field_reconstruction",learned},{"fixtures_passed",7}};
    const int n=data.at("teamSize"),steps=data.at("makespan");
    std::vector<int> locations,headings;std::vector<std::string> paths;
    for(int i=0;i<n;++i){const auto&s=data.at("start").at(i);locations.push_back(s.at(0).get<int>()*cols+s.at(1).get<int>());
        int d=s.at(2).is_number_integer()?s.at(2).get<int>():s.at(2)=="E"?0:s.at(2)=="S"?1:s.at(2)=="W"?2:s.at(2)=="N"?3:-1;
        if(d<0||d>3)throw std::runtime_error("unknown heading");headings.push_back(d);
        paths.push_back(data.at("actualPaths").at(i).get<std::string>());
        if(paths.back().size()!=size_t(2*steps-1))throw std::runtime_error("path length");
    }
    data.clear();Counts counts(g.edges.size()),after1000(g.edges.size()),final1000(g.edges.size());
    std::array<uint64_t,5> actions{};uint64_t active_moves=0,active_corridor_moves=0,tolled_moves=0,tolled_corridor_moves=0;
    cgar::FlowGuidance flow;if(learned)flow.initialize(free,rows,cols,128,4,8,25,512,4);
    std::vector<int> occupied(free.size(),-1),next_occupied(free.size(),-1),next(n);
    report["publications"]=json::array();
    for(int t=0;t<steps;++t){
        if(learned&&flow.observe(t,locations)){
            if(flow.costs()!=independent_field(g,counts))throw std::runtime_error("independent published field mismatch");
            auto row=field_summary(g,counts,flow.costs());row["step"]=t;row["moves"]=flow.moves();row["samples"]=flow.samples();row["publications"]=flow.publications();
            report["publications"].push_back(std::move(row));
        }
        std::fill(occupied.begin(),occupied.end(),-1);std::fill(next_occupied.begin(),next_occupied.end(),-1);
        for(int i=0;i<n;++i){int u=locations[i];if(u<0||u>=int(free.size())||!free[u]||occupied[u]>=0)throw std::runtime_error("invalid current occupancy");occupied[u]=i;}
        for(int i=0;i<n;++i){const char a=paths[i][2*t];const int u=locations[i],d=headings[i];int v=u;
            if(t+1<steps&&paths[i][2*t+1]!=',')throw std::runtime_error("action separator");
            if(a=='F'){
                ++actions[0];v=g.neighbor(u,d);const int id=g.edge_at[u*4+d];if(id<0)throw std::runtime_error("forward into blocked edge");
                const int k=d<2?0:1;++counts[id][k];if(t>=1000)++after1000[id][k];if(t>=steps-1000)++final1000[id][k];
                if(learned&&flow.publications()){
                    const bool covered=g.edges[id].group>=0,tolled=flow.costs()[u*4+d]>4;
                    ++active_moves;active_corridor_moves+=covered;tolled_moves+=tolled;tolled_corridor_moves+=covered&&tolled;
                }
            }else if(a=='R'){++actions[1];headings[i]=(d+1)%4;}
            else if(a=='C'){++actions[2];headings[i]=(d+3)%4;}
            else if(a=='W')++actions[3];else if(a=='T')++actions[4];else throw std::runtime_error("unknown action token");
            if(v<0||v>=int(free.size())||!free[v]||next_occupied[v]>=0)throw std::runtime_error("invalid next occupancy");
            next[i]=v;next_occupied[v]=i;
        }
        for(int i=0;i<n;++i){int other=occupied[next[i]];if(other>=0&&other!=i&&next[other]==locations[i])throw std::runtime_error("edge swap");}
        locations.swap(next);
    }
    report["actions"]={{"fw",actions[0]},{"cr",actions[1]},{"ccr",actions[2]},{"wait",actions[3]},{"other",actions[4]}};
    if(std::accumulate(actions.begin(),actions.end(),uint64_t(0))!=uint64_t(n)*steps)throw std::runtime_error("action conservation");
    report["traffic"]={{"all",traffic(g,counts)},{"after1000",traffic(g,after1000)},{"final1000",traffic(g,final1000)}};
    report["learned_active_forward_moves"]=active_moves;report["learned_active_corridor_moves"]=active_corridor_moves;
    report["tolled_forward_moves"]=tolled_moves;report["tolled_corridor_moves"]=tolled_corridor_moves;
    report["limits"]={"Observed movement only; task cohorts, resources and guidance differ across planners.",
        "Minority shares count edge crossings, not distinct trips; stops inside a chain can change edge balances.",
        "Pooled fields are offline counterfactuals on the observed trace, not measured policy gains.",
        "Competitor replay is diagnostic and does not qualify its deadline or memory usage."};
    std::ofstream(argv[3])<<report.dump(2)<<'\n';
}
