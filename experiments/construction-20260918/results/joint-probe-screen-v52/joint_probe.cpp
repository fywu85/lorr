// Offline complete enumeration of captured two/three-robot candidate products.
// Outside reservations are fixed. No result from this program is a planner input.
#include "nlohmann/json.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>
using json = nlohmann::ordered_json;
struct Choice {
    std::array<int,5> cells, edges, cell_owners, edge_owners;
    long long cost; int operation;
};
struct Robot { int id, selected; double power; std::vector<Choice> choices; };
void require(bool ok, const char* message) { if (!ok) throw std::runtime_error(message); }
bool compatible(const Choice& a, const Choice& b) {
    for (int t=0;t<5;++t)
        if (a.cells[t]==b.cells[t] || (a.edges[t]>=0 && a.edges[t]==b.edges[t])) return false;
    return true;
}
json solve(const json& group) {
    require(group.at("schema")==1,"unsupported snapshot schema");
    const auto& records=group.at("robots");
    const int n=records.size(),unit=group.at("unit_cost"),scale=group.at("distance_scale");
    require(n>=2&&n<=3&&unit>0&&scale>0,"invalid group dimensions or cost units");
    std::vector<Robot> robots; std::set<int> members;
    for(const auto& row:records){
        Robot r{row.at("robot"),row.at("selected"),row.at("power"),{}};
        require(r.id>=0&&members.insert(r.id).second&&!row.at("fixed").get<bool>(),"duplicate or protected member");
        require(std::isfinite(r.power)&&r.power>=0,"invalid robot power");
        for(const auto& c:row.at("choices")){
            require(c.at("valid").get<bool>(),"invalid candidate path");
            Choice choice{c.at("cells"),c.at("edges"),c.at("cell_owners"),c.at("edge_owners"),c.at("cost"),c.at("operation")};
            require(choice.operation>=0&&choice.operation<129,"invalid operation id");
            for(int t=0;t<5;++t) require(choice.cells[t]>=0&&choice.edges[t]>=-1&&
                choice.cell_owners[t]>=-1&&choice.edge_owners[t]>=-1&&
                (choice.edges[t]>=0||choice.edge_owners[t]==-1),"invalid footprint or owner");
            r.choices.push_back(choice);
        }
        require(!r.choices.empty()&&r.choices.size()<=129&&r.selected>=0&&r.selected<int(r.choices.size()),"invalid candidate domain");
        robots.push_back(r);
    }
    require(robots[0].id==group.at("root").get<int>()&&robots[0].power>0,"root is not the first active member");
    std::vector<std::vector<int>> domains(n);
    auto outside=[&](const Choice& c){
        for(int t=0;t<5;++t) for(int owner:{c.cell_owners[t],c.edge_owners[t]})
            if(owner>=0&&!members.count(owner)) return true;
        return false;
    };
    auto physical=[&](const Choice& c){return c.cost+static_cast<long long>(c.operation)*unit;};
    const auto& root_old=robots[0].choices[robots[0].selected];
    const int start=records[0].at("start"), trigger=group.at("trigger");
    require(root_old.cells[0]==start&&trigger>0&&trigger<int(robots[0].choices.size()),"invalid stationary root or trigger");
    const auto& trigger_choice=robots[0].choices[trigger];
    std::set<int> trigger_owners;
    for(int t=0;t<5;++t) for(int owner:{trigger_choice.cell_owners[t],trigger_choice.edge_owners[t]})
        if(owner>=0&&owner!=robots[0].id) trigger_owners.insert(owner);
    auto expected_owners=members;expected_owners.erase(robots[0].id);
    require(trigger_owners==expected_owners&&trigger_choice.cells[0]!=start&&trigger_choice.cost<root_old.cost&&
            physical(root_old)-physical(trigger_choice)>=static_cast<long long>(scale)*unit,"trigger differs from captured group");
    json domain_counts=json::array();
    for(int r=0;r<n;++r){
        const auto& old=robots[r].choices[robots[r].selected];
        require(!outside(old),"selected path has an outside conflict");
        for(int t=0;t<5;++t) require(old.cell_owners[t]==robots[r].id&&
            (old.edges[t]<0||old.edge_owners[t]==robots[r].id),"selected path ownership inconsistent");
        for(int s=0;s<r;++s)require(compatible(old,robots[s].choices[robots[s].selected]),"selected group is not feasible");
        int allowed=0;
        for(int k=0;k<int(robots[r].choices.size());++k){const auto& c=robots[r].choices[k];
            if(outside(c))continue;
            ++allowed;
            // Keep the root's full-unit forward objective fixed; owners may take any complete candidate.
            if(r==0&&(c.cells[0]==start||c.cost>=root_old.cost||
                physical(root_old)-physical(c)<static_cast<long long>(scale)*unit))continue;
            domains[r].push_back(k);
        }
        domain_counts.push_back({{"robot",robots[r].id},{"all",robots[r].choices.size()},
            {"outside_compatible",allowed},{"searched",domains[r].size()}});
    }
    // Precompute pairwise feasibility independently of the planner's reservation code.
    std::array<std::array<std::vector<std::vector<char>>,3>,3> pairs;
    for(int a=0;a<n;++a)for(int b=a+1;b<n;++b){auto& table=pairs[a][b];
        table.assign(robots[a].choices.size(),std::vector<char>(robots[b].choices.size()));
        for(int i:domains[a])for(int j:domains[b])table[i][j]=compatible(robots[a].choices[i],robots[b].choices[j]);
    }
    unsigned long long product=1,feasible=0,scalar_count=0,physical_count=0,both_count=0;
    for(const auto& domain:domains)product*=domain.size();
    std::vector<int> selected(n);json best=nullptr,best_both=nullptr;
    double best_gain=-INFINITY,best_both_gain=-INFINITY;
    auto enumerate=[&](auto&& self,int r,double gain,long long physical_gain,double weighted_physical)->void{
        if(r==n){
            ++feasible;const bool scalar=gain>1e-6,physical_ok=physical_gain>=static_cast<long long>(scale)*unit;
            scalar_count+=scalar;physical_count+=physical_ok;both_count+=scalar&&physical_ok;
            if(gain>best_gain||(scalar&&physical_ok&&gain>best_both_gain)){
                json witness={{"choices",selected},{"weighted_scalar_gain",gain},{"physical_potential_gain",physical_gain},
                              {"weighted_physical_potential_gain",weighted_physical}};
                if(gain>best_gain){best_gain=gain;best=witness;}
                if(scalar&&physical_ok&&gain>best_both_gain){best_both_gain=gain;best_both=witness;}
            }
            return;
        }
        const auto& old=robots[r].choices[robots[r].selected];
        for(int k:domains[r]){
            bool ok=true;for(int a=0;a<r;++a)if(!pairs[a][r][selected[a]][k]){ok=false;break;}
            if(!ok)continue;
            selected[r]=k;const auto& c=robots[r].choices[k];const auto delta=physical(old)-physical(c);
            self(self,r+1,gain+robots[r].power*(old.cost-c.cost),physical_gain+delta,weighted_physical+robots[r].power*delta);
        }
    };
    enumerate(enumerate,0,0,0,0);
    return {{"step",group.at("step")},{"root",robots[0].id},{"group_size",n},{"domains",domain_counts},
        {"complete_domain_product",product},{"feasible_forward_replacements",feasible},
        {"scalar_improving",scalar_count},{"physical_improving",physical_count},{"both_improving",both_count},
        {"best_scalar_witness",best},{"best_both_witness",best_both},{"exhaustive",true},
        {"scope","Fixed outside reservations, captured candidates, same five-slot horizon. No production reachability or future throughput claim."}};
}

json choice(std::array<int,5> cells,std::array<int,5> owners,long long cost,int operation){
    return {{"cells",cells},{"edges",std::array<int,5>{-1,-1,-1,-1,-1}},{"cell_owners",owners},
        {"edge_owners",std::array<int,5>{-1,-1,-1,-1,-1}},{"cost",cost},{"operation",operation},{"valid",true}};
}
void fixtures(){
    auto robot=[](int id,json choices){return json{{"robot",id},{"selected",0},{"power",1.0},{"fixed",false},
                                                  {"start",id},{"choices",choices}};};
    json group={{"schema",1},{"step",1},{"root",0},{"trigger",1},{"unit_cost",1},{"distance_scale",50},
        {"robots",json::array({robot(0,json::array({choice({0,0,0,0,0},{0,0,0,0,0},1000,0),choice({1,2,2,2,2},{1,2,2,2,2},0,1)})),
            robot(1,json::array({choice({1,1,1,1,1},{1,1,1,1,1},0,0),choice({6,6,6,6,6},{-1,-1,-1,-1,-1},100,1)})),
            robot(2,json::array({choice({2,2,2,2,2},{2,2,2,2,2},0,0),choice({7,7,7,7,7},{-1,-1,-1,-1,-1},100,1)}))})}};
    auto result=solve(group);
    require(result.at("feasible_forward_replacements")==1&&result.at("both_improving")==1&&
        result.at("best_scalar_witness").at("weighted_scalar_gain")==800.0&&
        result.at("best_scalar_witness").at("physical_potential_gain")==797,"joint-gain fixture failed");
    auto outside=group;outside["robots"][2]["choices"][1]["cell_owners"]=std::array<int,5>{9,9,9,9,9};
    require(solve(outside).at("feasible_forward_replacements")==0,"outside reservation fixture failed");
    auto collision=group;collision["robots"][2]["choices"][1]["cells"]=std::array<int,5>{6,6,6,6,6};
    require(solve(collision).at("feasible_forward_replacements")==0,"joint vertex-conflict fixture failed");
    auto edge=group;edge["robots"][1]["choices"][1]["edges"]=std::array<int,5>{33,-1,-1,-1,-1};
    edge["robots"][2]["choices"][1]["edges"]=std::array<int,5>{33,-1,-1,-1,-1};
    require(solve(edge).at("feasible_forward_replacements")==0,"joint edge-conflict fixture failed");
    auto scalar=group;scalar["robots"][0]["choices"][1]["cost"]=800;scalar["robots"][0]["choices"][1]["operation"]=128;
    scalar["robots"][1]["choices"][1]["cost"]=75;scalar["robots"][2]["choices"][1]["cost"]=75;
    result=solve(scalar);require(result.at("scalar_improving")==1&&result.at("both_improving")==0,"scalar/physical distinction fixture failed");
    auto bad=group;bad["robots"][1]["fixed"]=true;bool rejected=false;
    try{solve(bad);}catch(const std::runtime_error&){rejected=true;}
    require(rejected,"protected member was accepted");
}
int main(int argc,char** argv){try{
    fixtures();
    if(argc==2&&std::string(argv[1])=="--self-test"){std::cout<<"JOINT_PROBE_FIXTURES passed cases=6\n";return 0;}
    require(argc==3,"usage: joint_probe LOG OUTPUT");std::ifstream input(argv[1]);require(bool(input),"cannot open log");
    const std::string prefix="[cgar-temporal-group] ";std::string line;json results=json::array();
    while(std::getline(input,line))if(line.compare(0,prefix.size(),prefix)==0)results.push_back(solve(json::parse(line.substr(prefix.size()))));
    require(!results.empty(),"no group snapshots");
    std::ofstream output(argv[2]);require(bool(output),"cannot open output");
    output<<json{{"fixtures_passed",6},{"groups",results},{"all_groups_exhaustive",true}}.dump(2)<<'\n';
    require(bool(output),"failed writing output");std::cout<<"JOINT_PROBE_COMPLETE groups="<<results.size()<<'\n';
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
