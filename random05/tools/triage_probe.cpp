// Read-only reconstruction of current horizon suppression in a certified trace.
// It observes recorded assignments/actions; it never generates alternatives.
#include "engine.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
using namespace r05;
using json=nlohmann::json;
static json read(const std::string& path) {
    std::ifstream stream(path);json value;stream>>value;
    if(!stream)throw std::runtime_error("cannot read "+path);return value;
}
static void check(bool ok,const std::string& message) {if(!ok)throw std::runtime_error(message);}
static std::vector<std::string> split(const std::string& text) {
    std::stringstream stream(text);std::string item;std::vector<std::string> items;
    while(std::getline(stream,item,','))items.push_back(item);return items;
}
struct Counts {
    std::map<std::string,uint64_t> values;
    void add(const std::string& key,uint64_t count=1){values[key]+=count;}
};
int main(int argc,char** argv) {
    try {
        if(argc!=4)throw std::invalid_argument("usage: random05_triage_probe CASE_JSON RESULT_JSON OUTPUT_JSON");
        const auto config=read(argv[1]),result=read(argv[2]),input=read(config.at("input"));
        for(auto it=config.at("env").begin();it!=config.at("env").end();++it)
            setenv(it.key().c_str(),it.value().get<std::string>().c_str(),1);
        SharedEnvironment env;env.num_of_agents=result.at("teamSize");
        env.trick_instance=config.at("trick").is_boolean()?
            (config.at("trick").get<bool>()?"RANDOM-05":""):config.at("trick").get<std::string>();
        const auto map_path=std::filesystem::path(config.at("input").get<std::string>()).parent_path()/input.at("mapFile").get<std::string>();
        env.map_name=map_path.filename().string();
        std::ifstream map(map_path);std::string label,line;
        std::getline(map,line);map>>label>>env.rows>>label>>env.cols>>label;std::getline(map,line);
        for(int y=0;y<env.rows;++y) {
            std::getline(map,line);check(int(line.size())>=env.cols,"short map");
            for(int x=0;x<env.cols;++x)env.map.push_back(line[x]=='@' || line[x]=='T');
        }
        Config cfg=Config::environment(env);
        check(!cfg.guidance_distance_mix,"blended guidance distances are not supported by this diagnostic");
        Graph g(env,cfg);Config physical_cfg;physical_cfg.threads=cfg.threads;physical_cfg.loops=false;
        Graph physical(env,physical_cfg);
        std::map<int,std::unique_ptr<Chain>> chains,physical_chains;
        for(const auto& item:result.at("tasks")) {
            Task task;task.task_id=item.at(0);
            const auto coordinates=item.at(2).get<std::vector<int>>();
            for(size_t k=0;k<coordinates.size();k+=2)task.locations.push_back(coordinates.at(k)*env.cols+coordinates.at(k+1));
            chains[task.task_id]=std::make_unique<Chain>(g,task,false);
            physical_chains[task.task_id]=std::make_unique<Chain>(physical,task,false);
        }
        const int n=env.num_of_agents,steps=config.at("steps");
        std::vector<std::vector<std::string>> paths;
        for(const auto& row:result.at("actualPaths")) {
            paths.push_back(split(row));check(int(paths.back().size())==steps,"truncated path");
        }
        check(int(paths.size())==n,"wrong team size");
        std::vector<std::vector<std::pair<int,int>>> changes(steps+1);
        std::vector<std::vector<std::array<int,3>>> events(steps+1);
        std::map<int,int> completion;
        for(int a=0;a<n;++a)for(const auto& item:split(result.at("actualSchedule").at(a))) {
            const auto colon=item.find(':');int time=std::stoi(item.substr(0,colon)),task=std::stoi(item.substr(colon+1));
            changes.at(time).emplace_back(a,task);
        }
        check(changes.at(0).empty(),"time-zero schedule needs explicit handling");
        for(const auto& event:result.at("events")) {
            const int time=event.at(0),agent=event.at(1),task=event.at(2),stage=event.at(3);
            events.at(time).push_back({agent,task,stage});
            if(stage==int(chains.at(task)->goals.size()))completion[task]=time;
        }
        std::vector<int> loc(n),dir(n),assignment(n,-1),previous(n,-1),samples(n),from,to;
        std::vector<double> last_remaining(n),rates(n);
        std::map<int,int> stages,first_suppressed;
        std::map<int,Counts> task_counts;
        for(int a=0;a<n;++a) {
            const auto& start=result.at("start").at(a);
            loc[a]=g.from_grid.at(start.at(0).get<int>()*env.cols+start.at(1).get<int>());
            dir[a]=int(std::string("ESWN").find(start.at(2).get<std::string>()));
            check(loc[a]>=0 && dir[a]>=0 && dir[a]<4,"bad start");
        }
        Counts all;std::map<int,Counts> bins;json masks=json::array(),snapshots=json::object();
        uint64_t total_forward=0,active_forward=0,active_steps=0;int finished=0;
        for(int step=0;step<steps;++step) {
            const int time=step+1;
            for(auto [agent,task]:changes[time])assignment.at(agent)=task;
            std::vector<double> hops(n),guided(n),work(n),estimate(n);
            std::vector<unsigned char> active(n);double total_hops=0,total_guided=0;
            for(int a=0;a<n;++a) {
                const int task=assignment[a];
                if(task<0) {last_remaining[a]=rates[a]=0;samples[a]=0;previous[a]=task;continue;}
                const auto& chain=*chains.at(task);const int stage=stages[task];
                check(stage<int(chain.goals.size()),"completed order remains assigned");
                guided[a]=chain.cost(g,stage,loc[a],dir[a]);
                if(cfg.triage_progress_mix>0) {
                    if(task!=previous[a]) {rates[a]=0;samples[a]=0;}
                    else {
                        const double delta=last_remaining[a]-guided[a],alpha=2.0/(cfg.triage_progress_window+1);
                        rates[a]=samples[a]?(1-alpha)*rates[a]+alpha*delta:delta;
                        samples[a]=std::min(cfg.triage_progress_window,samples[a]+1);
                    }
                    last_remaining[a]=guided[a];
                }
                int cell=loc[a];
                for(int k=stage;k<int(chain.goals.size());++k) {hops[a]+=g.hop(chain.goals[k],cell);cell=chain.goals[k];}
                total_hops+=hops[a];total_guided+=guided[a];previous[a]=task;active[a]=1;
            }
            const double normalization=total_guided>0?total_hops/total_guided:0;
            const uint64_t forward=cfg.active_travel_rate?active_forward:total_forward;
            const double travel=forward?double(cfg.active_travel_rate?active_steps:uint64_t(n)*step)/forward:2;
            for(int a=0;a<n;++a)if(active[a])work[a]=(1-cfg.triage_guided_mix)*hops[a]+cfg.triage_guided_mix*normalization*guided[a];
            std::vector<double> factors(n,1);
            if(cfg.triage_progress_mix>0)factors=progress_time_factors(work,rates,samples,cfg.triage_progress_window,cfg.triage_progress_mix);
            int suppressed=0;uint64_t mask=14695981039346656037ull;
            for(int a=0;a<n;++a) {
                estimate[a]=work[a]*travel*cfg.triage_scale*factors[a];
                if(active[a] && cfg.horizon>0 && estimate[a]>cfg.horizon-step) {active[a]=0;++suppressed;}
                mask^=active[a]?1:0;mask*=1099511628211ull;
            }
            masks.push_back({{"step",step},{"suppressed",suppressed},{"active_mask",std::to_string(mask)}});
            Counts snapshot;from=loc;to=loc;
            for(int a=0;a<n;++a) {
                const int task=assignment[a];const bool loaded=task>=0 && stages[task]>0;
                const std::string phase=task<0?"idle":loaded?"loaded":"unopened";
                const std::string mode=task<0?"idle":active[a]?"active":"suppressed";
                const std::string outcome=task<0?"idle":completion.count(task)?"finishes":"unfinished";
                const auto& action=paths[a][step];check(action.size()==1,"bad action");
                const std::string key=phase+"_"+mode+"_"+outcome+"_"+action;
                all.add(key);bins[step/100*100].add(key);snapshot.add(phase+"_"+mode+"_"+outcome);
                if(task>=0) {
                    auto& tc=task_counts[task];tc.add(phase+"_"+mode+"_"+action);
                    const double bound=physical_chains.at(task)->cost(physical,stages[task],loc[a],dir[a])/2;
                    const bool lower_fits=bound<=steps-step;
                    snapshot.add(phase+"_"+mode+(lower_fits?"_physical_bound_fits":"_physical_bound_exceeds"));
                    if(!active[a]) {
                        all.add(phase+"_suppressed_"+outcome+(lower_fits?"_bound_fits_steps":"_bound_exceeds_steps"));
                        if(!first_suppressed.count(task))first_suppressed[task]=step;
                    }
                }
                if(action=="F") {to[a]=g.next.at(loc[a]).at(dir[a]);++total_forward;}
                else if(action=="R")dir[a]=(dir[a]+1)%4;
                else if(action=="C")dir[a]=(dir[a]+3)%4;
                else check(action=="W","unknown action");
                if(active[a]) {++active_steps;active_forward+=action=="F";}
            }
            if(step%100==0 || step>=steps-10)snapshots[std::to_string(step)]={{"counts",snapshot.values},{"steps_per_forward",travel}};
            Engine::certify(g,from,to);loc=to;
            for(auto event:events[time]) {
                const int agent=event[0],task=event[1],next=event[2];
                check(assignment.at(agent)==task && next==stages[task]+1 && loc.at(agent)==chains.at(task)->goals.at(stages[task]),"event replay mismatch");
                stages[task]=next;
                if(next==int(chains.at(task)->goals.size())) {++finished;bins[step/100*100].add("completed");}
            }
        }
        check(finished==result.at("numTaskFinished"),"completion count mismatch");
        Counts order_summary,terminal;
        std::map<std::string,std::vector<double>> terminal_remaining;
        for(const auto& entry:chains) {
            const int id=entry.first;const bool done=completion.count(id);
            const std::string state=done?"finished":stages[id]>0?"unfinished_opened":"unfinished_unopened";
            order_summary.add(state);
            if(first_suppressed.count(id))order_summary.add(state+"_ever_suppressed");
        }
        for(int a=0;a<n;++a) {
            const int task=assignment[a];if(task<0 || stages[task]==int(chains.at(task)->goals.size()))continue;
            const std::string phase=stages[task]>0?"loaded":"unopened";
            terminal_remaining[phase].push_back(physical_chains.at(task)->cost(physical,stages[task],loc[a],dir[a])/2);
            for(const auto& item:task_counts[task].values)terminal.add(item.first,item.second);
        }
        json bin_json=json::object(),terminal_distribution=json::object();
        for(const auto& entry:bins)bin_json[std::to_string(entry.first)]=entry.second.values;
        for(auto& entry:terminal_remaining) {
            auto& values=entry.second;std::sort(values.begin(),values.end());
            double sum=0;for(double value:values)sum+=value;
            terminal_distribution[entry.first]={{"count",values.size()},{"sum",sum},{"min",values.front()},
                {"median",values[values.size()/2]},{"max",values.back()},
                {"at_most5",std::count_if(values.begin(),values.end(),[](double x){return x<=5;})},
                {"at_most10",std::count_if(values.begin(),values.end(),[](double x){return x<=10;})}};
        }
        json output={{"case",argv[1]},{"result",argv[2]},{"tasks",finished},{"steps",steps},
            {"counts",all.values},{"bins",bin_json},{"snapshots",snapshots},{"suppression_masks",masks},
            {"orders",order_summary.values},{"terminal_assigned_order_actions",terminal.values},{"terminal_physical_remaining",terminal_distribution},
            {"caveat","Retrospective observation of the recorded policy, not an intervention or recoverable-throughput estimate. Unit-action remaining cost ignores other robots and permits zero-time repeated waypoints. A fitting lower bound does not prove deadline feasibility. Suppression masks must be checked against the corresponding solver diagnostics."}};
        std::ofstream stream(argv[3]);stream<<output.dump(2)<<'\n';check(bool(stream),"cannot write output");
        std::cout<<output.at("orders").dump(2)<<'\n'<<output.at("terminal_physical_remaining").dump(2)<<'\n';
    } catch(const std::exception& error) {std::cerr<<error.what()<<'\n';return 1;}
}
