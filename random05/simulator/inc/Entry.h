#pragma once
#include "SharedEnv.h"
#include "ActionModel.h"
#include "engine.hpp"
class Entry {
public:
    SharedEnvironment* env;
    Entry():env(new SharedEnvironment()){}
    explicit Entry(SharedEnvironment* e):env(e){}
    virtual ~Entry(){ delete env; }
    virtual void initialize(int preprocess_time_limit);
    virtual void compute(int time_limit,std::vector<Action>& plan,std::vector<int>& schedule);
    double last_compute_seconds() const { return seconds_; }
private:
    std::unique_ptr<r05::Engine> engine_;
    double seconds_=-1;
};
