#include "cgar.hpp"
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <map>
using json=nlohmann::json;
using namespace cgar;
int main(int argc,char**argv){
 if(argc!=3)throw std::runtime_error("input and output paths required");
 std::ifstream in(argv[1]);json data;in>>data;
 const int rows=data.at("rows"),cols=data.at("cols"),horizon=data.at("horizon");
 auto map=data.at("map").get<std::vector<int>>();
 auto starts=data.at("starts");const int robots=starts.size();
 auto cert=build_certificate(map,rows,cols,robots);
 if(!cert.valid)throw std::runtime_error("uncertified replay input");
 std::vector<int> locations(robots),headings(robots);
 std::vector<std::string> actions=data.at("actions").get<std::vector<std::string>>();
 for(int r=0;r<robots;++r){locations[r]=starts[r][0];headings[r]=starts[r][1];if(actions[r].size()!=size_t(horizon))throw std::runtime_error("action horizon mismatch");}
 const auto config=data.at("flow");
 const int unit=config.at("cost_scale"),turn=config.at("turn_cost");
 FlowGuidance flow;flow.initialize(cert.free,rows,cols,config.at("warmup"),config.at("strength"),config.at("minimum_samples"),config.at("margin_percent"),config.at("refresh_interval"),unit);
 DistanceOracle spatial;spatial.init(&cert,256ULL<<20);
 TurnDistanceOracle oriented;oriented.init(&cert,512ULL<<20,turn,true,unit);
 const auto deadline=std::chrono::steady_clock::now()+std::chrono::minutes(20);
 std::map<int,std::vector<json>> queries;
 for(const auto&q:data.at("queries"))queries[int(q.at("publication"))].push_back(q);
 json result={{"queries",json::array()},{"publications",json::array()},{"certificate_core",cert.core_size},{"certificate_free",std::count(cert.free.begin(),cert.free.end(),true)}};
 const auto expected=data.at("expected_publications");
 auto neighbor=[&](int u,int d){int v=u+(d==0?1:d==1?cols:d==2?-1:-cols);if(v<0||v>=rows*cols||((d==0||d==2)&&v/cols!=u/cols))return -1;return v;};
 for(int t=0;t<horizon;++t){
  if(t){for(int r=0;r<robots;++r){
   switch(actions[r][t-1]){
    case 'F':{int next=neighbor(locations[r],headings[r]);if(next<0||!cert.free[next])throw std::runtime_error("replay forward collision with map");locations[r]=next;break;}
    case 'R':headings[r]=(headings[r]+1)%4;break;
    case 'C':headings[r]=(headings[r]+3)%4;break;
    case 'W':break;
    default:throw std::runtime_error("unknown replay action");
   }
  }}
  if(!flow.observe(t,locations))continue;
  const int publication=flow.publications();
  if(publication>int(expected.size()))throw std::runtime_error("extra publication");
  const auto&e=expected[publication-1];
  if(t!=int(e.at("step"))||flow.samples()!=int(e.at("samples"))||flow.moves()!=uint64_t(e.at("moves"))||flow.penalized_edges()!=int(e.at("penalized_edges"))||publication!=int(e.at("publications")))throw std::runtime_error("publication differs from recorded planner");
  uint64_t hash=1469598103934665603ULL;for(auto cost:flow.costs()){hash^=cost;hash*=1099511628211ULL;}
  result["publications"].push_back({{"step",t},{"publication",publication},{"samples",flow.samples()},{"moves",flow.moves()},{"penalized_edges",flow.penalized_edges()},{"costs_fnv1a64",hash}});
  oriented.set_forward_costs(flow.costs());
  for(auto q:queries[publication]){
   const auto cells=q.at("locations").get<std::vector<int>>();long long unit_chain=0,oriented_chain=0;
   for(size_t k=1;k<cells.size();++k){
    const auto&plain=spatial.table(cells[k]);int d=spatial.distance_from(plain,cells[k-1]);
    const auto*table=oriented.table(cells[k],deadline);int best=kInf;
    for(int h=0;h<4;++h)best=std::min(best,oriented.value(*table,cells[k-1],h));
    if(d>=kInf||best>=kInf||best<unit*d)throw std::runtime_error("infinite or inconsistent task leg");
    unit_chain+=d;oriented_chain+=best;
   }
   q["unit_chain"]=unit_chain;q["oriented_chain"]=oriented_chain;q["unit_scale"]=unit;
   result["queries"].push_back(q);spatial.trim();oriented.trim();
  }
 }
 if(result["publications"].size()!=expected.size()||result["queries"].size()!=data.at("queries").size())throw std::runtime_error("incomplete replay");
 result["all_publication_counters_exact"]=true;result["horizon"]=horizon;
 std::ofstream output(argv[2]);output<<result.dump(2)<<std::endl;
}
