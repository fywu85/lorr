void temporal_kernel_regression() {
 auto validate=[](const TemporalPibt& search,const std::vector<int>& starts,int cells){
  std::vector<int> previous=starts,owner(cells,-1);
  for(int t=0;t<5;++t){
   for(size_t i=0;i<starts.size();++i){int v=search.choice(i).path->cells[t];if(v<0||v>=cells||owner[v]>=0)throw std::runtime_error("temporal test vertex conflict");owner[v]=i;}
   for(size_t i=0;i<starts.size();++i){int j=owner[previous[i]];if(j>=0&&j!=static_cast<int>(i)&&previous[j]==search.choice(i).path->cells[t])throw std::runtime_error("temporal test swap");}
   for(size_t i=0;i<starts.size();++i){previous[i]=search.choice(i).path->cells[t];owner[previous[i]]=-1;}
  }
 };
 // A displacement chain longer than all earlier prototype depth limits.
 const int n=1500;TemporalGeometry geometry;geometry.initialize(std::vector<char>(n+1,1),1,n+1,[]{});
 std::vector<std::vector<TemporalChoice>> choices(n);std::vector<int> starts(n),order(n);std::vector<char> fixed(n,0);std::vector<double> power(n,1);
 int forward=-1;const auto& ops=TemporalGeometry::operations();
 for(int k=1;k<129;++k)if(ops[k]==std::array<uint8_t,5>{0,3,3,3,3})forward=k;
 if(forward<0)throw std::runtime_error("missing forward temporal operation");
 for(int i=0;i<n;++i){starts[i]=order[i]=i;const auto& paths=geometry.paths(i,0);choices[i]={{&paths[0],0,0},{&paths[forward],-1,forward}};}
 TemporalPibt full(n+1,choices,fixed,power,8192,0);full.construct(order,[]{});validate(full,starts,n+1);
 if(full.stats.max_depth!=n-1)throw std::runtime_error("temporal chain was truncated");
 for(int i=0;i<n;++i)if(full.choice(i).path->cells[0]!=i+1)throw std::runtime_error("temporal chain did not advance");
 TemporalPibt bounded(n+1,choices,fixed,power,8,0);bounded.construct(order,[]{});validate(bounded,starts,n+1);
 if(!bounded.stats.budget_exhausted||bounded.selected(0)!=0)throw std::runtime_error("temporal bounded rollback failed");
 bool timeout=false;try{TemporalPibt x(n+1,choices,fixed,power,8192,0);x.construct(order,[]{throw Timeout("temporal_test");});}catch(const Timeout&){timeout=true;}
 if(!timeout)throw std::runtime_error("temporal deadline did not propagate");
 // Random legal geometries, movable waits, immutable protected robots, and
 // independent collision checks after both construction and fixed-count search.
 TemporalGeometry small;small.initialize(std::vector<char>(36,1),6,6,[]{});
 std::mt19937 random(202);
 for(int seed=0;seed<48;++seed){
  const int count=12;std::vector<int> locations(36);std::iota(locations.begin(),locations.end(),0);std::shuffle(locations.begin(),locations.end(),random);locations.resize(count);
  std::vector<std::vector<TemporalChoice>> options(count);std::vector<char> protected_robot(count,0);protected_robot[seed%count]=1;
  std::vector<double> weights(count,1);std::vector<int> ranks(count);std::iota(ranks.begin(),ranks.end(),0);
  for(int i=0;i<count;++i){const auto& paths=small.paths(locations[i],random()%4);int goal=random()%36;
   auto cost=[&](const TemporalPath& path,int op){int cell=path.cells[4];return int64_t(50)*(std::abs(cell/6-goal/6)+std::abs(cell%6-goal%6))-op;};
   options[i].push_back({&paths[0],cost(paths[0],0),0});
   for(int k=1;k<129;++k)if(paths[k].valid)options[i].push_back({&paths[k],cost(paths[k],k),k});
   std::sort(options[i].begin()+1,options[i].end(),[](const auto& a,const auto& b){return std::tie(a.cost,a.operation)<std::tie(b.cost,b.operation);});
  }
  TemporalPibt search(36,options,protected_robot,weights,8192,seed);search.construct(ranks,[]{});validate(search,locations,36);search.repair(256,[]{});validate(search,locations,36);
  if(search.selected(seed%count)!=0||search.stats.repairs!=256)throw std::runtime_error("temporal fixed-work or protection failed");
 }
 std::cout<<"TEMPORAL_KERNEL passed chain_robots=1500 random_cases=48 bounded_rollback=1 explicit_timeout=1\n";
}

void temporal_primary_regression() {
 setenv("CGAR_TEMPORAL","1",1);setenv("CGAR_ORIENTATION_GUIDANCE","1",1);setenv("CGAR_TEMPORAL_STEPS","128",1);
 SharedEnvironment e;e.num_of_agents=2;e.rows=3;e.cols=4;e.map.assign(12,0);
 e.curr_states={State(6,0,3),State(5,0,2)};e.goal_locations={{},{{6,0}}};e.curr_task_schedule={-1,-1};
 Cgar c;c.initialize(&e,1000);std::vector<Action> actions;
 bool reached=false;
 for(int t=0;t<30;++t){
  e.curr_timestep=t;if(t==1)e.goal_locations[0]={{4,0}};
  c.plan(&e,1000,actions);auto next=step(e,e.curr_states,actions);
  if(next.empty())throw std::runtime_error("temporal primary fixture invalid");
  e.curr_states=next;
  if(e.curr_states[1].location==6){reached=true;break;}
 }
 if(!reached)throw std::runtime_error("temporal primary intent was not completed");
 // The original pocket/recovery fixtures must also finish with the layer active.
 pocket_case();persistent_primary();capacity_bootstrap();
 unsetenv("CGAR_TEMPORAL");unsetenv("CGAR_ORIENTATION_GUIDANCE");unsetenv("CGAR_TEMPORAL_STEPS");
 std::cout<<"TEMPORAL_INTEGRATION passed primary_intent=1 pockets=1 capacity=1\n";
}
