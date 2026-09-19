void temporal_parallel_regression() {
 setenv("CGAR_TEMPORAL","1",1);setenv("CGAR_ORIENTATION_GUIDANCE","1",1);
 setenv("CGAR_TEMPORAL_WORKERS","4",1);setenv("CGAR_TEMPORAL_STEPS","256",1);
 SharedEnvironment e;e.rows=8;e.cols=8;e.num_of_agents=24;e.map.assign(64,0);e.curr_task_schedule.assign(24,-1);e.goal_locations.resize(24);
 for(int i=0;i<24;++i){e.curr_states.emplace_back(i,0,i%4);e.goal_locations[i]={{63-i,0}};}
 setenv("CGAR_TEMPORAL_THREADS","1",1);Cgar serial;serial.initialize(&e,1000);
 setenv("CGAR_TEMPORAL_THREADS","4",1);Cgar parallel;parallel.initialize(&e,1000);
 int comparisons=0;
 for(int t=0;t<80;++t){
  e.curr_timestep=t;std::vector<Action>a,b;serial.plan(&e,1000,a);parallel.plan(&e,1000,b);
  if(a!=b)throw std::runtime_error("temporal parallel scheduling changed decisions");
  auto next=step(e,e.curr_states,a);if(next.empty())throw std::runtime_error("temporal parallel collision");e.curr_states=next;
  for(int i=0;i<24;++i)if(e.curr_states[i].location==e.goal_locations[i][0].first)e.goal_locations[i][0].first=(e.goal_locations[i][0].first+37)%64;
  comparisons+=a.size();
 }
 for(const char* name:{"CGAR_TEMPORAL","CGAR_ORIENTATION_GUIDANCE","CGAR_TEMPORAL_WORKERS","CGAR_TEMPORAL_STEPS","CGAR_TEMPORAL_THREADS"})unsetenv(name);
 std::cout<<"TEMPORAL_PARALLEL passed workers=4 serial_vs_parallel_robot_decisions="<<comparisons<<"\n";
}
