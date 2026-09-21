// Read the same first arrow layout as NMS and execute its unchanged constructor.
#include <Objects/Environment/graph_guidance.hpp>
#include <Objects/Environment/guidance_map.hpp>
#include <Objects/Environment/map.hpp>
#include <Objects/Basic/position.hpp>
#include <fstream>
#include <stdexcept>
#include <string>
int main(int argc, char** argv) {
    if (argc != 3) throw std::invalid_argument("usage: dump_native MAP ARROWS");
    std::ifstream input(argv[1]);std::string word,row;SharedEnvironment env;
    input >> word >> word >> word >> env.rows >> word >> env.cols >> word;
    if(!input || env.rows!=32 || env.cols!=32 || word!="map")throw std::runtime_error("unexpected random map");
    for(int r=0;r<32;++r){input>>row;if(row.size()!=32)throw std::runtime_error("row width");
        for(char c:row)env.map.push_back(c=='@'||c=='T');}
    get_map()=Map(env);std::ifstream arrows(argv[2]);GuidanceMap field;arrows>>field;
    if(!arrows||field.get_rows()!=32||field.get_cols()!=32)throw std::runtime_error("arrow dimensions");
    GraphGuidance guidance(field);
    const int delta[]={1,32,-1,-32};
    for(int d=0;d<4;++d){Position p(20,20,d);if(int(p.move_forward().get_pos())!=int(p.get_pos())+delta[d])throw std::runtime_error("direction mismatch");}
    for(int cell=0;cell<1024;++cell){
        if((field.get(cell/32,cell%32)=='@')!=bool(env.map[cell]))throw std::runtime_error("arrow occupancy mismatch");
        for(int d=0;d<4;++d){
            for(int a=1;a<4;++a)if(guidance.get(cell+1,d,a)!=(env.map[cell]?0u:2u))throw std::runtime_error("nonforward mismatch");
            std::cout<<guidance.get(cell+1,d,0)<<(d==3?'\n':' ');
        }
    }
}
