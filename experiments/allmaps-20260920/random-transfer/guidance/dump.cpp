#include "engine.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

int main(int argc,char** argv) {
    if(argc!=3)throw std::invalid_argument("expected map and thread count");
    std::ifstream stream(argv[1]);std::string key,value;
    SharedEnvironment env;
    stream>>key>>value>>key>>env.rows>>key>>env.cols>>key;
    if(!stream || env.rows!=32 || env.cols!=32 || key!="map")throw std::runtime_error("unexpected RANDOM layout header");
    for(int row=0;row<env.rows;++row) {
        stream>>value;if(int(value.size())!=env.cols)throw std::runtime_error("truncated map");
        for(char cell:value)env.map.push_back(cell=='@'||cell=='T');
    }
    r05::Config cfg;
    cfg.guidance="flow";cfg.threads=std::stoi(argv[2]);cfg.turn_cost=0.6f;
    cfg.flow_seed=15;cfg.flow_iterations=20;cfg.flow_average=true;cfg.flow_normalize=true;
    cfg.flow_power=3;cfg.flow_betweenness=0.75f;cfg.flow_output_penalty=2.4f;
    cfg.flow_flips=1;cfg.flow_flip_seed=5;
    const r05::Graph graph(env,cfg);
    if(graph.cells!=819)throw std::runtime_error("unexpected RANDOM free-cell count");
    std::cout<<env.rows<<' '<<env.cols<<' '<<graph.cells<<'\n'<<std::setprecision(9);
    for(int cell=0;cell<int(env.map.size());++cell) {
        const int vertex=graph.from_grid[cell];
        for(int direction=0;direction<5;++direction)
            std::cout<<(direction?" ":"")<<(vertex<0?0.0f:graph.weight[vertex][direction]);
        std::cout<<'\n';
    }
}
