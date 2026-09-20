    // Exact unweighted spatial bounds on first admissions, using the independent
    // complete BFS queries above. This is stronger than production's cache-miss
    // Manhattan fallback. Report stock witnesses, never a throughput upper bound.
    const int horizon = report.at("steps").get<int>();
    std::vector<long long> chain_service(tasks.size());
    for (size_t i = 0; i < tasks.size(); ++i) {
        chain_service[i] = tasks[i].chain;
        for (size_t k = 1; k < tasks[i].stops.size(); ++k)
            chain_service[i] += tasks[i].stops[k] == tasks[i].stops[k-1];
    }
    std::vector<int> order;
    for (size_t i = 0; i < tasks.size(); ++i) if (!tasks[i].assignments.empty()) order.push_back(i);
    std::sort(order.begin(), order.end(), [&](int a, int b) {
        const auto& x = tasks[a].assignments.front(); const auto& y = tasks[b].assignments.front();
        return std::make_tuple(x.step,x.robot,tasks[a].id) < std::make_tuple(y.step,y.robot,tasks[b].id);
    });
    json cohorts;
    std::vector<int> thresholds{std::max(1,horizon-1000),std::max(1,horizon-400),std::max(1,horizon-250),std::max(1,horizon-100)};
    std::sort(thresholds.begin(),thresholds.end());thresholds.erase(std::unique(thresholds.begin(),thresholds.end()),thresholds.end());
    for (int threshold : thresholds) {
        long long completed=0,impossible=0,slack=0,completed_after_rescue=0;
        for (int i : order) {
            const auto& t=tasks[i];const int step=std::max(1,t.assignments.front().step);
            if(step<threshold)continue;
            const long long bound=std::max(1,t.first_pickup_distance)+chain_service[i];
            const int remaining=horizon-step+1;
            if(t.finished>=0){
                const int last_step=std::max(1,t.assignments.back().step);
                const long long last_bound=std::max(1,t.last_pickup_distance)+chain_service[i];
                if(last_bound>horizon-last_step+1)throw std::runtime_error("completed final holder violates exact spatial service lower bound");
                if(bound>remaining){
                    if(t.assignments.size()<2)throw std::runtime_error("completed unchanged holder violates exact spatial service lower bound");
                    ++completed_after_rescue;
                }
                ++completed;
            }else if(bound>remaining)++impossible;else ++slack;
        }
        cohorts[std::to_string(threshold)]={{"completed",completed},{"completed_initial_pair_impossible_but_retargeted",completed_after_rescue},{"uncompleted_initial_pair_impossible",impossible},{"uncompleted_lb_feasible",slack}};
    }
    std::vector<char> used(tasks.size(),false);
    long long exposed=0,overlap_skipped=0,witnesses=0,searches=0,candidates=0;
    json examples=json::array();
    for (int i : order) {
        const auto& t=tasks[i];const auto& assignment=t.assignments.front();const int step=std::max(1,assignment.step);
        if(step<thresholds.front()||t.finished>=0)continue;
        const long long bound=std::max(1,t.first_pickup_distance)+chain_service[i];const int remaining=horizon-step+1;
        if(bound<=remaining)continue;
        ++exposed;if(used[i]){++overlap_skipped;continue;}
        std::vector<int> possible;
        for (size_t j=0;j<tasks.size();++j) {
            const auto& alt=tasks[j];
            // Conservatively omit every task actually assigned in this same
            // batch, as well as previously assigned or not-yet-revealed tasks.
            if(used[j]||alt.revealed>=step||(!alt.assignments.empty()&&alt.assignments.front().step<=step)||chain_service[j]+1>remaining)continue;
            const int first=alt.stops.front();
            const int manhattan=std::abs(first/cols-assignment.cell/cols)+std::abs(first%cols-assignment.cell%cols);
            if(std::max(1,manhattan)+chain_service[j]<=remaining)possible.push_back(j);
        }
        candidates+=possible.size();if(possible.empty())continue;
        ++searches;std::fill(distance.begin(),distance.end(),-1);distance[assignment.cell]=0;queue.assign(1,assignment.cell);
        for(size_t head=0;head<queue.size();++head){const int from=queue[head];
            for(int direction=0;direction<4;++direction){const int to=neighbor(from,direction);
                if(to>=0&&free[to]&&distance[to]<0){distance[to]=distance[from]+1;queue.push_back(to);}}}
        int best=-1;long long best_bound=0;
        for(int j:possible){const int d=distance[tasks[j].stops.front()];if(d<0)continue;
            const long long b=std::max(1,d)+chain_service[j];if(b>remaining)continue;
            if(best<0||std::make_pair(b,tasks[j].id)<std::make_pair(best_bound,tasks[best].id)){best=j;best_bound=b;}}
        if(best<0)continue;
        used[i]=used[best]=true;++witnesses;
        if(examples.size()<20)examples.push_back({{"assignment_step",step},{"decision_tick",step-1},{"robot",assignment.robot},
            {"actual_task",t.id},{"actual_bound",bound},{"remaining",remaining},{"alternative_task",tasks[best].id},{"alternative_bound",best_bound}});
    }
    json completed_residuals;
    for (int cutoff : {1,std::max(1,horizon-1000)}) {
        std::array<std::vector<long long>,5> excess_by_bound;
        std::array<long long,5> duration_sum{},bound_sum{};
        for (int i:order) {
            const auto& task=tasks[i];
            if(task.finished<0||task.assignments.size()!=1)continue;
            const int start=std::max(1,task.assignments.front().step);if(start<cutoff)continue;
            const long long bound=std::max(1,task.first_pickup_distance)+chain_service[i];
            const long long duration=task.finished-start+1;
            if(duration<bound)throw std::runtime_error("unchanged completed holder beats its spatial service lower bound");
            const int bin=bound<50?0:bound<100?1:bound<200?2:bound<400?3:4;
            excess_by_bound[bin].push_back(duration-bound);duration_sum[bin]+=duration;bound_sum[bin]+=bound;
        }
        const std::array<std::string,5> labels{"0-49","50-99","100-199","200-399","400+"};
        for(int bin=0;bin<5;++bin)completed_residuals[std::to_string(cutoff)][labels[bin]]={
            {"excess",distribution(excess_by_bound[bin])},{"duration_sum",duration_sum[bin]},{"bound_sum",bound_sum[bin]}};
    }
    report["known_horizon_bounds"]={{"configured_horizon",horizon},{"cohorts",cohorts},
        {"completed_single_holder_residuals_by_bound",completed_residuals},
        {"chronological_witnesses",witnesses},{"exposed_impossible_unfinished_admissions",exposed},{"overlapping_actual_tasks_skipped",overlap_skipped},
        {"bfs_searches",searches},{"manhattan_candidate_exposures",candidates},{"examples",examples},
        {"limits","Exact spatial bounds conditional on keeping the holder, stronger than production cache misses. Later matching may rescue an initially impossible pair. Sequential task-disjoint witnesses against the unchanged pool; same-batch assignments conservatively excluded. Not a counterfactual rollout, complete opportunity count, throughput estimate or upper bound. Reveals and later assignments would change under a new policy."}};
