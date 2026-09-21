// Read-only trace diagnosis. No actions or throughput scores are produced.
#include "engine.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
using namespace r05;
using json=nlohmann::json;
static json read(const std::string& path) {
    std::ifstream stream(path);json value;stream>>value;
    if(!stream)throw std::runtime_error("cannot read "+path);return value;
}
static std::vector<std::string> split(const std::string& text) {
    std::stringstream stream(text);std::string item;std::vector<std::string> items;
    while(std::getline(stream,item,','))items.push_back(item);return items;
}
static int turns(int a,int b) {int d=(a-b+4)%4;return std::min(d,4-d);}
int main(int argc,char** argv) {
    try {
        if(argc!=4)throw std::invalid_argument("usage: random05_goal_probe CASE_JSON RESULT_JSON OUTPUT_JSON");
        const auto config=read(argv[1]),result=read(argv[2]);
        const auto input=read(config.at("input"));
        for(auto it=config.at("env").begin();it!=config.at("env").end();++it)
            setenv(it.key().c_str(),it.value().get<std::string>().c_str(),1);
        SharedEnvironment env;env.num_of_agents=result.at("teamSize");
        env.trick_instance=config.at("trick").is_boolean()?
            (config.at("trick").get<bool>()?"RANDOM-05":""):config.at("trick").get<std::string>();
        const auto map_path=std::filesystem::path(config.at("input").get<std::string>()).parent_path()/input.at("mapFile").get<std::string>();
        std::ifstream map(map_path);std::string label,line;
        std::getline(map,line);map>>label>>env.rows>>label>>env.cols>>label;std::getline(map,line);
        env.map.reserve(env.rows*env.cols);
        for(int y=0;y<env.rows;++y) {
            std::getline(map,line);if(int(line.size())<env.cols)throw std::runtime_error("short map");
            for(int x=0;x<env.cols;++x)env.map.push_back(line[x]=='@' || line[x]=='T');
        }
        Config cfg=Config::environment(env);
        if(cfg.window || cfg.operation_depth || cfg.guidance_distance_mix || cfg.push_price || cfg.prospective_wait)
            throw std::runtime_error("this diagnostic requires the ordinary fixed-cost pipeline");
        Graph g(env,cfg);
        std::unordered_map<int,std::unique_ptr<Chain>> chains;
        for(const auto& item:result.at("tasks")) {
            Task task;task.task_id=item.at(0);
            const auto coordinates=item.at(2).get<std::vector<int>>();
            for(size_t k=0;k<coordinates.size();k+=2)task.locations.push_back(coordinates.at(k)*env.cols+coordinates.at(k+1));
            chains[task.task_id]=std::make_unique<Chain>(g,task,false);
        }
        const int n=env.num_of_agents,steps=config.at("steps");
        std::vector<std::vector<std::string>> paths;
        for(const auto& row:result.at("actualPaths")) {
            paths.push_back(split(row));if(int(paths.back().size())!=steps)throw std::runtime_error("truncated path");
        }
        if(int(paths.size())!=n)throw std::runtime_error("wrong team size");
        std::vector<std::vector<std::pair<int,int>>> changes(steps+1);
        std::vector<std::vector<std::array<int,3>>> events(steps+1);
        for(int a=0;a<n;++a)for(const auto& item:split(result.at("actualSchedule").at(a))) {
            const auto colon=item.find(':');int time=std::stoi(item.substr(0,colon)),task=std::stoi(item.substr(colon+1));
            changes.at(time).emplace_back(a,task);
        }
        for(const auto& event:result.at("events"))events.at(event.at(0)).push_back({event.at(1),event.at(2),event.at(3)});
        std::vector<int> loc(n),dir(n),previous_dir(n),assignment(n,-1),from(n),to(n);
        std::vector<bool> previous_forward(n,false);std::unordered_map<int,int> stage;
        for(int a=0;a<n;++a) {
            const auto& start=result.at("start").at(a);
            loc[a]=g.from_grid.at(start.at(0).get<int>()*env.cols+start.at(1).get<int>());
            dir[a]=int(std::string("ESWN").find(start.at(2).get<std::string>()));previous_dir[a]=dir[a];
        }
        using Counts=std::map<std::string,uint64_t>;
        std::map<std::string,std::map<std::string,Counts>> counts;
        int finished=0;
        for(int time=1;time<=steps;++time) {
            for(auto [agent,task]:changes[time])assignment[agent]=task;
            from=loc;to=loc;
            for(int a=0;a<n;++a) {
                const char action=paths[a][time-1].at(0);const int start_dir=dir[a];
                if(action=='F')to[a]=g.next.at(loc[a]).at(dir[a]);
                else if(action=='R')dir[a]=(dir[a]+1)%4;
                else if(action=='C')dir[a]=(dir[a]+3)%4;
                else if(action!='W')throw std::runtime_error("unknown action");
                const int task=assignment[a];
                if(action=='F' && task>=0 && stage[task]>0) {
                    const auto& chain=*chains.at(task);const int k=stage[task],p=loc[a],v=to[a];
                    const int goal=chain.goals.at(k),h=g.hop(goal,p);
                    auto cost=[&](int cell,int heading){return chain.cost(g,k,cell,heading);};
                    const bool moving=previous_forward[a];const int heading=previous_dir[a];
                    int idle_heading=heading;float idle=cost(p,heading);
                    if(!moving)for(int q:{(heading+1)%4,(heading+3)%4}) {
                        float value=cost(p,q)+.05f*g.weight[p][4];
                        if(value<idle-1e-5f){idle=value;idle_heading=q;}
                    }
                    float best_all=cost(p,idle_heading)+cfg.wait_cost+.001f,best_allowed=best_all;
                    for(int d=0;d<4;++d)if(g.next[p][d]>=0) {
                        const float value=cost(g.next[p][d],d)+g.forward_weight(goal,p,d);
                        best_all=std::min(best_all,value);
                        if(moving?d==heading:turns(d,heading)<=1)best_allowed=std::min(best_allowed,value);
                    }
                    const float actual=cost(v,start_dir)+g.forward_weight(goal,p,start_dir);
                    const bool away=g.hop(goal,v)>h;
                    const std::string bin=h<=2?std::to_string(h):h<=4?"3-4":h<=8?"5-8":"9+";
                    std::vector<std::string> phases={"all"};if(time<=700)phases.push_back("first700");
                    for(const auto& phase:phases) {
                        auto& c=counts[phase][bin];++c["loaded_forward"];
                        if(away) {
                            ++c["away"];
                            if(actual<=best_all+1e-4f)++c["away_minimum_static_score"];
                            else if(actual<=best_allowed+1e-4f)++c["away_only_kinematically_minimum"];
                            else ++c["away_not_minimum_eligible_score"];
                        }
                    }
                }
                previous_dir[a]=start_dir;previous_forward[a]=action=='F';
            }
            Engine::certify(g,from,to);loc=to;
            for(auto event:events[time]) {
                const int agent=event[0],task=event[1],next=event[2];
                if(assignment[agent]!=task || next!=stage[task]+1 || loc[agent]!=chains.at(task)->goals.at(stage[task]))
                    throw std::runtime_error("event replay mismatch");
                stage[task]=next;finished+=next==int(chains.at(task)->goals.size());
            }
        }
        if(finished!=result.at("numTaskFinished"))throw std::runtime_error("completion count mismatch");
        json output={{"case",argv[1]},{"result",argv[2]},{"tasks",finished},{"steps",steps},{"counts",counts},
            {"caveat","Offline score classification, not causal attribution. Eligible headings reconstruct the previous pipeline decision. Nonminimum choices can arise from collision resolution, cycles or suppressed goals. The first700-step subtotal limits late-horizon confounding. No benchmark policy is changed."}};
        std::ofstream stream(argv[3]);stream<<output.dump(2)<<'\n';if(!stream)throw std::runtime_error("cannot write output");
        std::cout<<output.at("counts").at("first700").dump(2)<<'\n';
    } catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
}
