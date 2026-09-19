// Standalone diagnostic compiled against both frozen old and corrected headers.
#include "guide_routes.hpp"
#include "temporal_geometry.hpp"
#include <iostream>
#include <string>
int main(int argc,char** argv) {
 using namespace cgar;
 std::vector<char> core(49,true);GuideRoutes guides;guides.initialize(core,1,49,1);
 auto heuristic=[](int,int cell,int){return std::abs(40-cell);};
 guides.update({0},{0},{40},{true},heuristic,[]{});
 guides.update({5},{0},{40},{true},heuristic,[]{});
 TemporalGeometry geometry;geometry.initialize(core,1,49,[]{});
 const auto& paths=geometry.paths(5,0);const auto& operations=TemporalGeometry::operations();
 int forward=-1,park=-1;
 for(int op=0;op<int(operations.size());++op){
  if(operations[op]==std::array<uint8_t,5>{0,0,0,0,0})forward=op;
  if(operations[op]==std::array<uint8_t,5>{0,0,0,3,3})park=op;
 }
 auto distance=[&](int cell,int dir){return guides.distance(0,cell,dir);};
 const auto a=TemporalGeometry::cost(paths[forward],forward,40,1,distance);
 const auto b=TemporalGeometry::cost(paths[park],park,40,1,distance);
 std::cout<<"{\"start_cell\":5,\"cached_waypoint\":8,\"goal\":40,\"forward_end_distance\":"<<distance(10,0)
          <<",\"parking_end_distance\":"<<distance(8,0)<<",\"forward_cost\":"<<a<<",\"parking_cost\":"<<b
          <<",\"prefers_forward\":"<<(a<b?"true":"false")<<"}\n";
 const bool expect_old=argc>1&&std::string(argv[1])=="old";
 return expect_old?(a>b&&distance(10,0)==38?0:1):(a<b&&distance(10,0)==30?0:1);
}
