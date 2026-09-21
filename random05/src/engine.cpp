#include "engine.hpp"
#include <algorithm>
#include <cmath>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <filesystem>
#include <fstream>
#include <limits>
#include <numeric>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <unordered_set>
#include <omp.h>

namespace r05 {
namespace {
constexpr float INF=1e20f;
int integer(const char* key,int value) { const char* v=std::getenv(key);return v?std::stoi(v):value; }
float real(const char* key,float value) { const char* v=std::getenv(key);return v?std::stof(v):value; }
int turn(int a,int b) { const int d=(a-b+4)%4;return std::min(d,4-d); }
// K always counts completed branch evaluations. Screening spends s branches
// on q roots, then B-s additional branches on one survivor: q*s+B-s work.
int search_roots(const Config& cfg,int futures) {
    if(futures<1 || cfg.continuations<1)throw std::invalid_argument("search work and continuations must be positive");
    if(!cfg.screen_branches) {
        if(futures%cfg.continuations)throw std::invalid_argument("continuations must divide K");
        return futures/cfg.continuations;
    }
    if(cfg.screen_branches<1 || cfg.screen_branches>=cfg.continuations ||
       cfg.screen_keep<2 || cfg.screen_keep>64 || cfg.generations<1 || cfg.branch_diagnostics)
        throw std::invalid_argument("screening needs 0<s<B, retention denominator2..64, and no branch diagnostic");
    const int64_t group=int64_t(cfg.screen_keep)*cfg.screen_branches+cfg.continuations-cfg.screen_branches;
    if(futures%cfg.generations || (futures/cfg.generations)%group)
        throw std::invalid_argument("each generation's K must divide into complete screening groups");
    if((futures/cfg.generations)/group<2)
        throw std::invalid_argument("screening needs at least two finalists per generation for anchor and best");
    return int((futures/group)*cfg.screen_keep);
}
struct PolicyScratch {
    std::vector<int> p,moving,owner,chosen,reserve,idle_heading,forced_heading,candidate_count,order,prepared,intent;
    std::vector<float> base_cost,priorities;
    std::vector<const Chain*> active_chain;
    std::vector<const float*> cost_table;
    std::vector<std::array<MoveCandidate,5>> candidates;
    std::vector<uint64_t> priority_keys, radix_buffer;
    std::vector<CachedRanking*> ranking_slots;
    std::vector<unsigned char> ranking_hits;
    std::vector<unsigned int> kinematic_masks;
    std::vector<uint64_t> cycle_ready;
};
}
Config Config::environment(const SharedEnvironment& env) {
    const bool random_trick=env.trick_instance=="RANDOM-01" || env.trick_instance=="RANDOM-02" ||
        env.trick_instance=="RANDOM-03" || env.trick_instance=="RANDOM-04" || env.trick_instance=="RANDOM-05";
    Config c;
    c.futures=integer("R05_K",c.futures);c.depth=integer("R05_DEPTH",c.depth);
    c.first_futures=integer("R05_FIRST_K",0);
    c.generations=integer("R05_GENERATIONS",1);c.elites=integer("R05_ELITES",1);
    c.persist_elites=integer("R05_PERSIST_ELITES",1);
    c.continuations=integer("R05_CONTINUATIONS",1);
    c.continuation_start=integer("R05_CONTINUATION_START",1);
    c.screen_branches=integer("R05_SCREEN_BRANCHES",0);
    c.screen_keep=integer("R05_SCREEN_KEEP",4);
    c.component_trials=integer("R05_COMPONENT_TRIALS",0);
    c.component_rounds=integer("R05_COMPONENT_ROUNDS",2);
    c.component_parents=integer("R05_COMPONENT_PARENTS",8);
    c.component_min_agents=integer("R05_COMPONENT_MIN_AGENTS",1);
    if(c.component_trials<0 || c.component_trials>1024 || c.component_rounds<1 || c.component_rounds>8 ||
       c.component_parents<2 || c.component_parents>64 || c.component_min_agents<1)
        throw std::invalid_argument("invalid motion-component search settings");
    c.replan_roots=integer("R05_REPLAN_ROOTS",0);
    c.replan_futures=integer("R05_REPLAN_FUTURES",1);
    c.replan_k=integer("R05_REPLAN_K",32);
    c.replan_steps=integer("R05_REPLAN_STEPS",8);
    c.replan_continuations=integer("R05_REPLAN_CONTINUATIONS",4);
    c.replan_start=integer("R05_REPLAN_START",0);
    c.replan_threads=integer("R05_REPLAN_THREADS",1);
    c.replan_policy=integer("R05_REPLAN_POLICY",0);
    c.rescore_roots=integer("R05_RESCORE_ROOTS",0);
    c.rescore_branches=integer("R05_RESCORE_BRANCHES",64);
    c.rescore_blend=real("R05_RESCORE_BLEND",0);
    c.rescore_static_weight=real("R05_RESCORE_STATIC_WEIGHT",-1);
    if(!std::isfinite(c.rescore_static_weight) ||
       (c.rescore_static_weight!=-1 && (c.rescore_static_weight<0 || c.rescore_static_weight>1)))
        throw std::invalid_argument("static future weight must be -1 or in [0,1]");
    if(c.rescore_roots<0 || c.rescore_roots>128 || c.rescore_branches<2 || c.rescore_branches>1024 ||
       !std::isfinite(c.rescore_blend) || c.rescore_blend<0 || c.rescore_blend>1 ||
       (c.rescore_roots>0 && (c.replan_roots || c.component_trials || c.continuation_start<1 || c.continuation_start>=c.depth)))
        throw std::invalid_argument("invalid independent-future rescoring settings");
    if(c.replan_roots<0 || c.replan_roots>32 || c.replan_futures<1 || c.replan_futures>8 ||
       c.replan_k<1 || c.replan_k>1024 || c.replan_steps<2 || c.replan_steps>32 ||
       c.replan_continuations<1 || c.replan_k%c.replan_continuations || c.replan_start<0)
        throw std::invalid_argument("invalid closed-loop forecast settings");
    c.branch_diagnostics=integer("R05_BRANCH_DIAGNOSTICS",0);
    c.snapshot_interval=integer("R05_SNAPSHOT_EVERY",0);
    c.snapshot_candidates=integer("R05_SNAPSHOT_CANDIDATES",8);
    if(const char* directory=std::getenv("R05_SNAPSHOT_DIR"))c.snapshot_directory=directory;
    if(c.snapshot_interval<0 || c.snapshot_candidates<1 || c.snapshot_candidates>64 ||
       (c.snapshot_interval && c.local_trials))
        throw std::invalid_argument("invalid decision snapshot settings");
    if(c.branch_diagnostics<0)throw std::invalid_argument("branch diagnostic interval must be nonnegative");
    c.future_mutation=real("R05_FUTURE_MUTATION",0.3);
    c.future_elite_blend=real("R05_FUTURE_ELITE_BLEND",0);
    if(!std::isfinite(c.future_elite_blend) || c.future_elite_blend<0 || c.future_elite_blend>1 ||
       (c.future_elite_blend>0 && c.persist_elites<2))
        throw std::invalid_argument("future elite blend must be in [0,1] and needs at least two retained vectors");
    c.continuation_risk=real("R05_CONTINUATION_RISK",0);
    if(c.rescore_static_weight>=0 && c.continuation_risk!=0)
        throw std::invalid_argument("weighted static rescoring currently requires mean-only future scoring");
    if(!std::isfinite(c.continuation_risk))
        throw std::invalid_argument("continuation coefficient must be finite");
    c.share_prefix=integer("R05_SHARE_PREFIX",0);
    c.packed_order=integer("R05_PACKED_ORDER",0);c.fast_dispersion=integer("R05_FAST_DISPERSION",0);
    c.scratch_reuse=integer("R05_SCRATCH_REUSE",0);c.profile=integer("R05_PROFILE",0);
    c.goal_cache=integer("R05_GOAL_CACHE",0);
    c.policy_profile=integer("R05_POLICY_PROFILE",0);
    c.radix_order=integer("R05_RADIX_ORDER",0);
    c.candidate_cache=integer("R05_CANDIDATE_CACHE",0);
    c.fuse_cache_hits=integer("R05_FUSE_CACHE_HITS",0);
    c.lazy_cost_rows=integer("R05_LAZY_COST_ROWS",0);
    c.shared_rankings_mb=integer("R05_SHARED_RANKINGS_MB",0);
    c.shared_orders=integer("R05_SHARED_ORDERS",0);
    if(c.shared_rankings_mb<0 || c.shared_rankings_mb>16384)
        throw std::invalid_argument("shared ranking cache budget must be 0..16384 MiB");
    c.kinematic_mask=integer("R05_KINEMATIC_MASK",0);
    c.cycle_mask=integer("R05_CYCLE_MASK",0);
    c.cache_slots=integer("R05_CACHE_SLOTS",64);
    if(c.cache_slots<8 || c.cache_slots>1024 || (c.cache_slots&(c.cache_slots-1)))
        throw std::invalid_argument("candidate cache slots must be a power of two in [8,1024]");
    const int roots=search_roots(c,c.futures);
    const int finalists=c.screen_branches?roots/c.screen_keep:roots;
    if(c.generations<1 || c.generations>roots || c.elites<1 ||
       c.elites>finalists || c.persist_elites<1 || c.persist_elites>finalists || c.continuation_start<1 ||
       (c.continuations>1 && c.continuation_start>=c.depth) ||
       !std::isfinite(c.future_mutation) || c.future_mutation<0 || c.future_mutation>1)
        throw std::invalid_argument("continuations must preserve the first decision and enough complete finalists");
    if(c.first_futures<0 || c.first_futures>c.futures)
        throw std::invalid_argument("first-step K must not exceed regular K");
    if(c.first_futures>0) {
        const int first_roots=search_roots(c,c.first_futures);
        const int first_finalists=c.screen_branches?first_roots/c.screen_keep:first_roots;
        if(first_roots<c.generations || first_finalists<std::max(c.elites,c.persist_elites))
            throw std::invalid_argument("first-step K needs enough complete finalists");
    }
    c.window=integer("R05_WINDOW",0);
    c.window_keep=integer("R05_WINDOW_KEEP",6);
    c.window_islands=integer("R05_WINDOW_ISLANDS",32);
    c.window_iterations=integer("R05_WINDOW_ITERS",24);
    c.window_first_iterations=integer("R05_WINDOW_FIRST_ITERS",0);
    c.window_initial_steps=integer("R05_WINDOW_INITIAL_STEPS",1);
    c.window_neighborhood=integer("R05_WINDOW_NEIGHBORHOOD",8);
    c.window_expansions=integer("R05_WINDOW_EXPANSIONS",20000);
    c.window_starts=integer("R05_WINDOW_STARTS",1);
    c.window_rounds=integer("R05_WINDOW_ROUNDS",1);
    c.window_reuse=integer("R05_WINDOW_REUSE",0);
    c.window_fast_groups=integer("R05_WINDOW_FAST_GROUPS",0);
    c.window_heap4=integer("R05_WINDOW_HEAP4",0);
    c.window_merge=integer("R05_WINDOW_MERGE",0);
    c.window_temperature=real("R05_WINDOW_TEMPERATURE",0);
    c.window_blockers=integer("R05_WINDOW_BLOCKERS",0);
    c.window_blocker_rotation=integer("R05_WINDOW_BLOCKER_ROTATION",0);
    c.window_equal=integer("R05_WINDOW_EQUAL",0);
    if(c.window<0 || c.window>32 || c.window_keep<0 || (c.window && c.window_keep>=c.window) ||
       c.window_islands<1 || c.window_islands>128 || c.window_iterations<0 || c.window_iterations>16384 ||
       c.window_neighborhood<1 || c.window_neighborhood>64 || c.window_expansions<1 || c.window_starts<1 || c.window_starts>1024 ||
       c.window_rounds<1 || c.window_rounds>32 || c.window_iterations%c.window_rounds ||
       c.window_first_iterations<0 || c.window_first_iterations>c.window_iterations || c.window_first_iterations%c.window_rounds ||
       c.window_initial_steps<1 || c.window_initial_steps>128 ||
       c.window_blocker_rotation<0 || c.window_blocker_rotation>2 ||
       !std::isfinite(c.window_temperature) || c.window_temperature<0 || c.window_temperature>100)
        throw std::invalid_argument("invalid windowed search configuration");
    c.threads=integer("R05_THREADS",c.threads);c.seed=integer("R05_SEED",c.seed);
    c.noise=real("R05_NOISE",c.noise);c.mutation=real("R05_MUTATION",c.mutation);
    c.priority_remaining_weight=real("R05_PRIORITY_REMAINING",0);
    c.priority_remaining_steps=integer("R05_PRIORITY_REMAINING_STEPS",0);
    if(!std::isfinite(c.priority_remaining_weight) || c.priority_remaining_weight<0 ||
       c.priority_remaining_steps<0 || (c.priority_remaining_weight>0 && !random_trick))
        throw std::invalid_argument("remaining-work priority requires a nonnegative weight and explicit --trick RANDOM-01..05");
    c.restart_period=integer("R05_RESTART_PERIOD",4);
    c.elite_decision_distance=real("R05_ELITE_DECISION_DISTANCE",0);
    if(c.restart_period<0 || !std::isfinite(c.elite_decision_distance) ||
       c.elite_decision_distance<0 || c.elite_decision_distance>1)
        throw std::invalid_argument("invalid restart period or elite decision distance");
    c.move_bias=real("R05_MOVE_BIAS",0);
    c.move_bias_mode=integer("R05_MOVE_BIAS_MODE",0);
    if(c.move_bias_mode<0 || c.move_bias_mode>2)
        throw std::invalid_argument("move proposal mode must be compass, available edges, or edges and waiting");
    if(!std::isfinite(c.move_bias) || c.move_bias<0 || c.move_bias>4)
        throw std::invalid_argument("move proposal bias must be in [0,4]");
    c.move_bias_fraction=real("R05_MOVE_BIAS_FRACTION",0.25f);
    if(!std::isfinite(c.move_bias_fraction) || c.move_bias_fraction<0 || c.move_bias_fraction>1)
        throw std::invalid_argument("move proposal fraction must be in [0,1]");
    c.mutation_decay=real("R05_MUTATION_DECAY",1);
    if(!std::isfinite(c.mutation_decay) || c.mutation_decay<=0 || c.mutation_decay>1)
        throw std::invalid_argument("mutation decay must be in (0,1]");
    c.blocker_mutation_size=integer("R05_BLOCKER_MUTATION_SIZE",0);
    c.blocker_mutation_period=integer("R05_BLOCKER_MUTATION_PERIOD",2);
    c.blocker_mutation_edges=integer("R05_BLOCKER_MUTATION_EDGES",1);
    if(c.blocker_mutation_size<0 || c.blocker_mutation_size>512 ||
       c.blocker_mutation_period<1 || c.blocker_mutation_period>64 ||
       c.blocker_mutation_edges<1 || c.blocker_mutation_edges>4)
        throw std::invalid_argument("invalid blocker-directed priority mutation settings");
    c.mutation_radius=integer("R05_MUTATION_RADIUS",0);
    if(c.mutation_radius<0)throw std::invalid_argument("mutation radius must be nonnegative");
    c.dispersion=real("R05_DISPERSION",c.dispersion);c.push_price=real("R05_PUSH",c.push_price);
    c.loop_threshold=real("R05_LOOP_THRESHOLD",c.loop_threshold);
    c.length_weight=real("R05_LENGTH_WEIGHT",c.length_weight);c.keep_bonus=real("R05_KEEP_BONUS",c.keep_bonus);
    c.active_task_cap=integer("R05_ACTIVE_TASK_CAP",0);
    c.active_cap_steps=integer("R05_ACTIVE_CAP_STEPS",0);
    c.active_cap_triage_credit=real("R05_ACTIVE_CAP_TRIAGE_CREDIT",0);
    c.fast_admission=integer("R05_FAST_ADMISSION",0);
    c.admission_price=real("R05_ADMISSION_PRICE",-1);
    c.admission_price_steps=integer("R05_ADMISSION_PRICE_STEPS",0);
    if(c.admission_price_steps<0 || (c.admission_price_steps>0 && c.admission_price<0))
        throw std::invalid_argument("startup admission price needs a nonnegative duration and an enabled price");
    if(!std::isfinite(c.admission_price) || c.admission_price<-1 ||
       (c.admission_price<0 && c.admission_price!=-1) ||
       (c.admission_price>=0 && !random_trick))
        throw std::invalid_argument("optional assignment price requires a nonnegative price and explicit trick instance");
    if(c.active_task_cap<0 || c.active_cap_steps<0 || (c.active_task_cap>0 && !random_trick))
        throw std::invalid_argument("active task admission requires nonnegative settings and an explicit --trick RANDOM-01..05");
    c.destination_load=real("R05_DESTINATION_LOAD",0);
    if(!std::isfinite(c.destination_load) || c.destination_load<0)
        throw std::invalid_argument("destination load coefficient must be finite and nonnegative");
    c.initial_length_weight=real("R05_INITIAL_LENGTH_WEIGHT",-1);
    c.initial_length_steps=integer("R05_INITIAL_LENGTH_STEPS",250);
    if((c.initial_length_weight<0 && c.initial_length_weight!=-1) || c.initial_length_steps<0)
        throw std::invalid_argument("invalid initial task-length preference");
    if(c.initial_length_weight>=0 && !random_trick)
        throw std::invalid_argument("initial task-length preference requires an explicit --trick RANDOM-01..05 instance");
    c.turn_cost=real("R05_TURN_COST",c.turn_cost);c.wait_cost=real("R05_WAIT_COST",c.wait_cost);
    c.matching=integer("R05_MATCH",1);c.loops=integer("R05_LOOPS",1);c.deadends=integer("R05_DEADENDS",1);
    c.progress_discount=real("R05_PROGRESS_DISCOUNT",1);c.flow_turn_load=real("R05_FLOW_TURN_LOAD",0);
    c.plain_score=real("R05_PLAIN_SCORE",0);
    c.guidance_distance_mix=real("R05_GUIDANCE_DISTANCE_MIX",0);
    c.guidance_edge_mix=real("R05_GUIDANCE_EDGE_MIX",0);
    if(!std::isfinite(c.guidance_edge_mix) || c.guidance_edge_mix<0 || c.guidance_edge_mix>1 ||
       (c.guidance_edge_mix>0 && c.guidance_distance_mix>0))
        throw std::invalid_argument("edge mixture must be in [0,1] and separate from potential interpolation");
    if(!std::isfinite(c.guidance_distance_mix) || c.guidance_distance_mix<0 || c.guidance_distance_mix>1)
        throw std::invalid_argument("guidance distance mixture must be in [0,1]");
    c.reverse_penalty=real("R05_REVERSE_PENALTY",0);
    c.completion_bonus=real("R05_COMPLETE_BONUS",0);
    c.progress_softcap=real("R05_PROGRESS_SOFTCAP",0);
    if(!std::isfinite(c.progress_softcap) || c.progress_softcap<0 ||
       (c.progress_softcap>0 && !random_trick))
        throw std::invalid_argument("nonlinear short-task preference requires a nonnegative scale and explicit trick instance");
    c.score_rank_power=real("R05_SCORE_RANK_POWER",0);
    c.score_rank_steps=integer("R05_SCORE_RANK_STEPS",0);
    if(c.score_rank_steps<0 || (c.score_rank_steps>0 && c.score_rank_power<=0))
        throw std::invalid_argument("startup score window needs nonnegative steps and positive rank power");
    if(!std::isfinite(c.score_rank_power) || c.score_rank_power<0 || c.score_rank_power>4)
        throw std::invalid_argument("task progress rank power must be in [0,4]");
    if(c.score_rank_power>0 && !random_trick)
        throw std::invalid_argument("short-remaining-task score preference requires an explicit --trick RANDOM-01..05 instance");
    if(c.completion_bonus<0)throw std::invalid_argument("completion bonus must be nonnegative");
    if(c.reverse_penalty<0)throw std::invalid_argument("reverse-turn penalty must be nonnegative");
    if(c.plain_score<0 || c.plain_score>1)
        throw std::invalid_argument("plain score blend must be in [0,1]");
    if(c.progress_discount<=0 || c.progress_discount>1 || c.flow_turn_load<0)
        throw std::invalid_argument("discount must be in (0,1] and turn-load multiplier nonnegative");
    c.cycle_portfolio=integer("R05_CYCLE_PORTFOLIO",0);
    c.early_fill=integer("R05_EARLY_FILL",0);c.early_fill_gain=real("R05_EARLY_FILL_GAIN",0);
    c.early_root_period=integer("R05_EARLY_ROOT_PERIOD",0);
    if(c.early_root_period<0)throw std::invalid_argument("early root period must be nonnegative");
    if(c.early_fill_gain<0)throw std::invalid_argument("early-fill threshold must be nonnegative");
    c.operation_depth=integer("R05_OPERATIONS",0);
    if(c.operation_depth && c.move_bias>0)
        throw std::invalid_argument("move proposal bias is only implemented for the pipelined policy");
    c.operation_revisits=integer("R05_OPERATION_REVISITS",4);
    c.operation_inherit=integer("R05_OPERATION_INHERIT",1);
    c.operation_moving=integer("R05_OPERATION_MOVING",0);
    c.operation_protect=integer("R05_OPERATION_PROTECT",0);
    c.operation_finish_move=integer("R05_OPERATION_FINISH_MOVE",0);
    c.operation_cost_weight=real("R05_OPERATION_COST",0);
    if((c.operation_depth!=0 && c.operation_depth!=3) || c.operation_revisits<1 || c.operation_cost_weight<0)
        throw std::invalid_argument("operations must be off or length3, with positive revisit limit and nonnegative cost weight");
    c.idle_eviction=real("R05_IDLE_EVICTION",0);
    c.pre_cycles=integer("R05_PRE_CYCLES",0);c.pre_cycle_gain=real("R05_PRE_CYCLE_GAIN",0);
    c.random_by_step=integer("R05_RANDOM_BY_STEP",0);c.age_cap=integer("R05_AGE_CAP",0);
    c.waypoint_age_retain=real("R05_WAYPOINT_AGE_RETAIN",0);
    if(!std::isfinite(c.waypoint_age_retain) || c.waypoint_age_retain<0 || c.waypoint_age_retain>1)
        throw std::invalid_argument("waypoint age retention must be in [0,1]");
    if(c.age_cap>0 && !random_trick)
        throw std::invalid_argument("capped priority aging requires an explicit --trick RANDOM-01..05 instance");
    c.chain_matching=integer("R05_SCHED_CHAIN",0);c.hungarian_limit=integer("R05_HUNGARIAN",0);c.prospective_wait=integer("R05_PROSPECTIVE_WAIT",0);
    c.local_trials=integer("R05_LOCAL",0);c.horizon=integer("R05_HORIZON",0);
    if(!std::isfinite(c.active_cap_triage_credit) || c.active_cap_triage_credit<0 ||
       c.active_cap_triage_credit>1 || (c.active_cap_triage_credit>0 &&
       (!random_trick || c.horizon<=0 || c.active_task_cap<=0)))
        throw std::invalid_argument("triaged-task admission credit requires a fraction in [0,1], a cap, a horizon and explicit trick instance");
    c.active_travel_rate=integer("R05_ACTIVE_TRAVEL_RATE",0);
    if(c.active_travel_rate && (!random_trick || c.horizon<=0))
        throw std::invalid_argument("active travel calibration requires a declared horizon and explicit trick instance");
    c.match_horizon_weight=real("R05_MATCH_HORIZON_WEIGHT",0);
    if(!std::isfinite(c.match_horizon_weight) || c.match_horizon_weight<0 ||
       (c.match_horizon_weight>0 && (!random_trick || c.horizon<=0)))
        throw std::invalid_argument("matching horizon weight requires a declared horizon and --trick RANDOM-01..05");
    if(c.snapshot_interval && c.local_trials)throw std::invalid_argument("decision snapshots require local search off");
    c.triage_scale=real("R05_TRIAGE_SCALE",c.triage_scale);c.accept_equal=integer("R05_EQUAL",0);
    c.triage_guided_mix=real("R05_TRIAGE_GUIDED_MIX",0);
    if(!std::isfinite(c.triage_guided_mix) || c.triage_guided_mix<0 || c.triage_guided_mix>1 ||
       (c.triage_guided_mix>0 && c.horizon<=0))
        throw std::invalid_argument("guided triage mix must be in [0,1] and requires a declared horizon");
    if(c.horizon>0 && !random_trick)
        throw std::invalid_argument("known-horizon triage requires an explicit --trick RANDOM-01..05 instance");
    c.intent_rotation=integer("R05_INTENT_ROTATION",1);
    c.intent_mode=integer("R05_INTENT_MODE",0);
    if(c.intent_mode<0 || c.intent_mode>2 || (c.intent_mode && !c.intent_rotation))
        throw std::invalid_argument("intent mode must be 0..2 and requires intent rotation");
    c.flow_seed=integer("R05_FLOW_SEED",c.flow_seed);c.flow_iterations=integer("R05_FLOW_ITERS",c.flow_iterations);
    c.rollout_age=integer("R05_ROLLOUT_AGE",0);c.rollout_match=integer("R05_ROLLOUT_MATCH",0);c.cost_cache=integer("R05_COST_CACHE",0);
    c.pocket_components=integer("R05_POCKET_COMPONENTS",0);
    c.goal_local_radius=integer("R05_GOAL_LOCAL_RADIUS",0);
    c.goal_local_mix=real("R05_GOAL_LOCAL_MIX",0);
    if(c.goal_local_radius<0 || !std::isfinite(c.goal_local_mix) ||
       c.goal_local_mix<0 || c.goal_local_mix>1 ||
       (c.goal_local_mix>0 && (c.goal_local_radius==0 || !random_trick)))
        throw std::invalid_argument("local goal guidance requires a positive radius, mix in [0,1] and explicit trick instance");
    if(c.goal_local_mix>0 && (c.window || c.operation_depth || c.guidance_distance_mix>0))
        throw std::invalid_argument("local goal guidance currently requires the pipelined policy and exact goal costs");
    c.flow_turn=real("R05_FLOW_TURN",0);
    c.flow_power=real("R05_FLOW_POWER",1);c.flow_alpha=real("R05_FLOW_ALPHA",1);
    c.flow_confidence_power=real("R05_FLOW_CONFIDENCE_POWER",0);
    if(!std::isfinite(c.flow_confidence_power) || c.flow_confidence_power<0)
        throw std::invalid_argument("flow confidence exponent must be finite and nonnegative");
    c.flow_betweenness=real("R05_FLOW_BETWEENNESS",0);
    c.flow_average=integer("R05_FLOW_AVERAGE",0);c.flow_normalize=integer("R05_FLOW_NORMALIZE",0);c.loop_extent=integer("R05_LOOP_EXTENT",2);
    c.predict_matching=integer("R05_SCHED_PREDICT",0);
    c.flow_penalty=real("R05_FLOW_PENALTY",c.flow_penalty);c.guided_matching=integer("R05_SCHED_GUIDE",0);
    c.flow_output_penalty=real("R05_FLOW_OUTPUT_PENALTY",-1);
    c.flow_normalize_ref=real("R05_FLOW_NORMALIZE_REF",-1);
    if(c.flow_normalize_ref<0 && c.flow_normalize_ref!=-1)
        throw std::invalid_argument("flow normalization reference must be nonnegative or -1");
    if(c.flow_output_penalty<0 && c.flow_output_penalty!=-1)
        throw std::invalid_argument("flow output penalty must be nonnegative or -1 for the assignment penalty");
    if(const char* v=std::getenv("R05_GUIDANCE")) c.guidance=v;
    if(const char* v=std::getenv("R05_WEIGHTS")) c.weights=v;
    c.flow_flips=integer("R05_FLOW_FLIPS",0);c.flow_flip_seed=integer("R05_FLOW_FLIP_SEED",1);
    c.flow_reverse=integer("R05_FLOW_REVERSE",0);
    if(c.flow_reverse && (!random_trick || c.guidance=="none"))
        throw std::invalid_argument("guidance reversal requires weighted guidance and an explicit trick instance");
    if(c.flow_flips<0 || (c.flow_flips && c.guidance!="flow"))
        throw std::invalid_argument("field flips require flow guidance and a nonnegative count");
    if(c.guidance_edge_mix>0 && c.guidance=="none")
        throw std::invalid_argument("edge/action mixture requires weighted guidance");
    if(c.guidance_distance_mix>0 && (c.guidance=="none" || c.window))
        throw std::invalid_argument("mixed guidance potentials require weighted guidance and a non-windowed policy");
    if(c.guidance!="none" && !random_trick)
        throw std::invalid_argument("guidance experiments require --trick RANDOM-05");
    if((c.active_task_cap || c.admission_price>=0) && c.rollout_match)
        throw std::invalid_argument("active task admission does not support virtual task replacement forecasts");
    if(c.blocker_mutation_size && (c.window || c.operation_depth || c.mutation_radius))
        throw std::invalid_argument("blocker-directed mutations require the ordinary pipeline without spatial mutation");
    if(c.early_root_period && (c.early_fill || c.operation_depth || c.window || c.component_trials || c.replan_roots))
        throw std::invalid_argument("optional immediate moves require the ordinary pipelined portfolio");
    if(c.component_trials && (c.early_fill || c.operation_depth))
        throw std::invalid_argument("motion-component search requires the ordinary fixed first-position pipeline");
    if(c.progress_softcap>0 && (c.window || c.rollout_match || c.replan_roots ||
       c.plain_score>0 || c.score_rank_power>0 || c.completion_bonus>0))
        throw std::invalid_argument("nonlinear progress requires fixed-chain pipeline scoring without other task rewards");
    if(c.score_rank_power>0 && (c.rollout_match || c.replan_roots || c.completion_bonus>0))
        throw std::invalid_argument("rank-weighted scoring currently requires fixed-chain futures without reranking or completion bonus");
    if(c.replan_roots && (c.operation_depth || c.plain_score || c.reverse_penalty || c.progress_discount!=1))
        throw std::invalid_argument("replanning forecast currently needs pipeline and undiscounted guided scoring");
    if(c.replan_threads<1 || c.replan_threads>c.threads)
        throw std::invalid_argument("inner forecast workers must fit the declared total worker count");
    if(c.priority_remaining_weight>0 && (c.operation_depth || c.window))
        throw std::invalid_argument("remaining-work priority is implemented for the pipelined policy only");
    if(c.window && (c.operation_depth || c.rollout_match || c.replan_roots || c.rescore_roots ||
       c.component_trials || c.snapshot_interval || c.reverse_penalty || c.plain_score))
        throw std::invalid_argument("windowed search requires fixed-chain costs without other experimental search modes");
    if(c.futures<1 || c.generations<1 || c.generations>c.futures || c.depth<1 || c.threads<1 || c.depth>64 || c.turn_cost<=0 ||
       c.wait_cost<=0 || c.mutation<0 || c.mutation>1)
        throw std::invalid_argument("invalid R05 configuration");
    return c;
}

// Negative entries are inactive goals. Equal remaining costs share a rank;
// normalize active weights to mean1 to preserve the dispersion/cost scale.
std::vector<double> rank_progress_weights(const std::vector<float>& remaining,float power) {
    if(power==0)return std::vector<double>(remaining.size(),1);
    std::vector<int> order;
    for(int a=0;a<int(remaining.size());++a)if(remaining[a]>=0)order.push_back(a);
    std::sort(order.begin(),order.end(),[&](int a,int b) {
        if(remaining[a]!=remaining[b])return remaining[a]<remaining[b];
        return a<b;
    });
    std::vector<double> weights(remaining.size(),0);double sum=0;
    const int count=int(order.size());
    for(int begin=0;begin<count;) {
        int end=begin+1;
        while(end<count && remaining[order[end]]==remaining[order[begin]])++end;
        const double rank=(begin+end-1)*.5;
        const double weight=std::pow((count-rank)/count,power);
        for(int k=begin;k<end;++k){weights[order[k]]=weight;sum+=weight;}
        begin=end;
    }
    if(sum>0)for(double& weight:weights)weight*=count/sum;
    return weights;
}

static void order_priorities(const std::vector<float>& priorities,bool packed,bool radix,
                             std::vector<int>& order,std::vector<uint64_t>& keys,
                             std::vector<uint64_t>& buffer) {
    order.resize(priorities.size());
    std::iota(order.begin(),order.end(),0);
    if((!packed && !radix) || !std::all_of(priorities.begin(),priorities.end(),[](float p){return std::isfinite(p);})) {
        std::stable_sort(order.begin(),order.end(),[&](int a,int b){return priorities[a]>priorities[b];});
        return;
    }
    static_assert(sizeof(float)==sizeof(uint32_t) && std::numeric_limits<float>::is_iec559,
                  "packed priorities require IEEE binary32");
    // Sort the float value and original agent ID together. This avoids an
    // indirect float lookup per comparison and preserves stable tie ordering.
    // Normalize signed zero because the original float comparator equates them.
    keys.resize(priorities.size());
    for(size_t a=0;a<priorities.size();++a) {
        float p=priorities[a]==0?0.0f:priorities[a];uint32_t bits;
        std::memcpy(&bits,&p,sizeof(bits));
        uint32_t ascending=(bits&0x80000000u)?~bits:(bits^0x80000000u);
        keys[a]=(uint64_t(~ascending)<<32)|uint32_t(a);
    }
    if(radix) {
        // Input is already in ascending agent-ID order. Four stable byte
        // passes on the high (float-priority) word therefore produce exactly
        // the packed 64-bit order, including equal priorities and signed zero.
        std::array<std::array<uint32_t,256>,4> counts{};
        for(uint64_t key:keys)for(int pass=0;pass<4;++pass)
            ++counts[pass][(key>>(32+pass*8))&255];
        buffer.resize(keys.size());
        for(int pass=0;pass<4;++pass) {
            uint32_t sum=0;int occupied=0;
            for(auto& count:counts[pass]) {
                occupied+=count>0;uint32_t next=sum+count;count=sum;sum=next;
            }
            if(occupied<=1)continue;
            for(uint64_t key:keys)buffer[counts[pass][(key>>(32+pass*8))&255]++]=key;
            keys.swap(buffer);
        }
    } else std::sort(keys.begin(),keys.end());
    for(size_t k=0;k<keys.size();++k)order[k]=int(uint32_t(keys[k]));
}
std::vector<int> priority_order(const std::vector<float>& priorities,bool packed,bool radix) {
    std::vector<int> order;std::vector<uint64_t> keys,buffer;
    order_priorities(priorities,packed,radix,order,keys,buffer);
    return order;
}

int Graph::nearby_pairs(const std::vector<int>& locations) const {
    std::vector<unsigned char> occupied(cells,0);
    for(int v:locations)occupied[v]=1;
    int pairs=0;
    if(locations.size()<=size_t(cells/2)) {
        for(int v:locations)for(int u:nearby[v])if(u>v)pairs+=occupied[u];
    } else {
        // Count all free-cell pairs, remove edges incident to a hole, then add
        // back hole-hole pairs that the degree subtraction removed twice.
        pairs=all_nearby_pairs;
        for(int v=0;v<cells;++v)if(!occupied[v]) {
            pairs-=int(nearby[v].size());
            for(int u:nearby[v])if(u>v && !occupied[u])++pairs;
        }
    }
    return pairs;
}

Graph::Graph(const SharedEnvironment& env,const Config& cfg) {
    goal_local_radius=cfg.goal_local_radius;goal_local_mix=cfg.goal_local_mix;
    rows=env.rows;cols=env.cols;from_grid.assign(env.map.size(),-1);
    for(int i=0;i<int(env.map.size());++i) if(!env.map[i]) {
        from_grid[i]=int(to_grid.size());to_grid.push_back(i);
    }
    cells=int(to_grid.size());states=4*cells;
    if(cells>4096) throw std::invalid_argument("initial exact-table implementation limited to 4096 free cells");
    next.resize(cells);weight.resize(cells);degree.assign(cells,0);
    nearby.resize(cells);
    for(int v=0;v<cells;++v) {
        const int p=to_grid[v],x=p%cols,y=p/cols;
        for(int yy=std::max(0,y-2);yy<=std::min(rows-1,y+2);++yy)
            for(int xx=std::max(0,x-2);xx<=std::min(cols-1,x+2);++xx) {
                int u=from_grid[yy*cols+xx];if(u<0 || u==v)continue;
                nearby[v].push_back(u);all_nearby_pairs+=u>v;
            }
    }
    for(int v=0;v<cells;++v) {
        int p=to_grid[v],x=p%cols,y=p/cols;
        const int dx[]={1,0,-1,0},dy[]={0,1,0,-1};
        for(int d=0;d<4;++d) {
            int xx=x+dx[d],yy=y+dy[d];
            int u=(xx<0 || xx>=cols || yy<0 || yy>=rows)?-1:from_grid[yy*cols+xx];
            next[v][d]=u;degree[v]+=u>=0;
            weight[v][d]=2;
            if(cfg.guidance=="lanes") {
                bool preferred=d%2==0 ? d==((y%2)?2:0) : d==((x%2)?1:3);
                weight[v][d]=preferred?2:6;
            }
        }
        weight[v][4]=cfg.turn_cost;
    }
    if(cfg.guidance=="file") {
        std::ifstream f(cfg.weights);
        if(!f) throw std::invalid_argument("cannot open guidance weights");
        nlohmann::json j;f>>j;
        if(j.is_object()) j=j.at("weights");
        if(j.size()!=env.map.size()*5) throw std::invalid_argument("guidance array must have five weights per grid cell");
        double sum=0;int count=0;
        for(int v=0;v<cells;++v)for(int d=0;d<4;++d)if(next[v][d]>=0) {
            sum+=j.at(size_t(to_grid[v])*5+d).get<float>();++count;
        }
        float scale=2*count/sum;
        for(int v=0;v<cells;++v)for(int d=0;d<5;++d) {
            float w=j.at(size_t(to_grid[v])*5+d).get<float>();
            if(!std::isfinite(w)||w<=0)throw std::invalid_argument("guidance costs must be finite and positive");
            weight[v][d]=std::max(0.01f,w*scale);
        }
    } else if(cfg.guidance!="none" && cfg.guidance!="lanes" && cfg.guidance!="flow")
        throw std::invalid_argument("R05_GUIDANCE must be none, lanes, flow or file");
    if(cfg.guidance=="flow") {
        // Traffic assignment with uniform all-pairs demand. Opposing traffic is
        // priced more strongly than co-flow, then the resulting dominant edge
        // directions are rounded into soft one-way preferences.
        std::mt19937 random(cfg.flow_seed);
        std::uniform_real_distribution<float> jitter(0,0.2f);
        std::vector<std::array<float,4>> price(cells),bias(cells);
        std::vector<std::array<double,4>> flow(cells),average(cells);
        for(int v=0;v<cells;++v)for(int d=0;d<4;++d)price[v][d]=bias[v][d]=1+jitter(random);
        for(int iteration=0;iteration<cfg.flow_iterations;++iteration) {
            std::vector<std::vector<std::array<double,4>>> partial(
                cfg.threads,std::vector<std::array<double,4>>(cells));
            #pragma omp parallel for num_threads(cfg.threads) schedule(static)
            for(int source=0;source<cells;++source) {
                auto& accumulator=partial[omp_get_thread_num()];
                if(cfg.flow_turn>0) {
                    // Route demand in orientation space so field construction
                    // prices the turns required by the actual robot kinematics.
                    std::vector<float> d(states,INF);
                    std::vector<int> parent(states,-1),parent_dir(states,-1),order;
                    std::vector<double> demand(states,0);
                    using Item=std::pair<float,int>;
                    std::priority_queue<Item,std::vector<Item>,std::greater<Item>> q;
                    for(int o=0;o<4;++o){d[source*4+o]=0;q.emplace(0,source*4+o);}
                    while(!q.empty()) {
                        auto [cost,state]=q.top();q.pop();if(cost!=d[state])continue;
                        order.push_back(state);int v=state/4,o=state%4;
                        auto relax=[&](int dest,float w,int dir) {
                            float value=cost+w;
                            if(value<d[dest]){d[dest]=value;parent[dest]=state;parent_dir[dest]=dir;q.emplace(value,dest);}
                        };
                        int u=next[v][o];if(u>=0)relax(u*4+o,price[v][o],o);
                        relax(v*4+(o+1)%4,cfg.flow_turn,4);relax(v*4+(o+3)%4,cfg.flow_turn,4);
                    }
                    for(int v=0;v<cells;++v) {
                        int end=v*4;for(int o=1;o<4;++o)if(d[v*4+o]<d[end])end=v*4+o;
                        demand[end]=1;
                    }
                    for(auto it=order.rbegin();it!=order.rend();++it) {
                        int v=*it,u=parent[v];if(u<0)continue;
                        if(parent_dir[v]<4)accumulator[u/4][parent_dir[v]]+=demand[v];
                        demand[u]+=demand[v];
                    }
                } else {
                std::vector<float> d(cells,INF);
                std::vector<int> parent(cells,-1),parent_dir(cells,-1),order;
                std::vector<double> demand(cells,1);
                using Item=std::pair<float,int>;
                std::priority_queue<Item,std::vector<Item>,std::greater<Item>> q;
                d[source]=0;q.emplace(0,source);
                while(!q.empty()) {
                    auto [cost,v]=q.top();q.pop();if(cost!=d[v])continue;
                    order.push_back(v);
                    for(int dir=0;dir<4;++dir) {
                        int u=next[v][dir];if(u<0)continue;
                        float candidate=cost+price[v][dir];
                        if(candidate<d[u]) {d[u]=candidate;parent[u]=v;parent_dir[u]=dir;q.emplace(candidate,u);}
                    }
                }
                for(auto it=order.rbegin();it!=order.rend();++it) {
                    int v=*it,u=parent[v];if(u<0)continue;
                    accumulator[u][parent_dir[v]]+=demand[v];demand[u]+=demand[v];
                }
                }
            }
            double sum=0;int edges=0;
            for(int v=0;v<cells;++v)for(int d=0;d<4;++d) {
                flow[v][d]=0;
                for(int t=0;t<cfg.threads;++t)flow[v][d]+=partial[t][v][d];
                if(cfg.flow_average) {
                    average[v][d]+=(flow[v][d]-average[v][d])/(iteration+1);
                    flow[v][d]=average[v][d];
                }
                if(next[v][d]>=0){sum+=flow[v][d];++edges;}
            }
            const double mean=std::max(1.0,sum/edges);
            for(int v=0;v<cells;++v)for(int d=0;d<4;++d) {
                int u=next[v][d];if(u<0)continue;
                float target=bias[v][d]+float((flow[v][d]+cfg.flow_penalty*flow[u][(d+2)%4])/mean);
                if(cfg.flow_power!=1 || cfg.flow_alpha!=1)
                    target=bias[v][d]+cfg.flow_alpha*std::pow(float(flow[v][d]/mean),cfg.flow_power)
                         +cfg.flow_penalty*std::pow(float(flow[u][(d+2)%4]/mean),cfg.flow_power);
                price[v][d]=0.65f*price[v][d]+0.35f*target;
            }
        }
        std::vector<double> load(cells,0);double total_load=0;
        for(int v=0;v<cells;++v)for(int d=0;d<4;++d)if(next[v][d]>=0)
            load[v]+=flow[v][d]+flow[next[v][d]][(d+2)%4];
        for(double x:load)total_load+=x;
        const double mean_load=std::max(1.0,total_load/cells);
        if(cfg.flow_turn_load>0)for(int v=0;v<cells;++v)
            weight[v][4]*=float((1+cfg.flow_turn_load*load[v]/mean_load)/(1+cfg.flow_turn_load));
        // Separate layout construction from its final directional contrast.
        // This allows controlled cost-field comparisons on the same streets.
        const float output_penalty=cfg.flow_output_penalty<0?cfg.flow_penalty:cfg.flow_output_penalty;
        double weight_sum=0;int weight_count=0;
        for(int v=0;v<cells;++v)for(int d=0;d<4;++d) {
            int u=next[v][d];if(u<0)continue;
            // Near-balanced aggregate demand provides weak evidence for a
            // preferred direction. Optionally soften only those opposing-edge
            // penalties while preserving the constructed direction ordering.
            const double forward=flow[v][d],reverse=flow[u][(d+2)%4];
            const float confidence=cfg.flow_confidence_power>0
                ?std::pow(float(std::abs(forward-reverse)/std::max(1.0,forward+reverse)),cfg.flow_confidence_power):1;
            weight[v][d]=2*(forward>=reverse?1:1+output_penalty*confidence);
            const float load_factor=1+cfg.flow_betweenness*float((load[v]+load[u])/(2*mean_load));
            weight[v][d]*=load_factor;
            if(cfg.flow_normalize_ref>=0) {
                // Hold the preferred-direction scale fixed while changing the
                // price of opposing traffic. Otherwise global normalization
                // also changes the relative prices of turns, waits and matching.
                float reference=2*(forward>=reverse?1:1+cfg.flow_normalize_ref*confidence);
                reference*=load_factor;weight_sum+=reference;
            } else weight_sum+=weight[v][d];
            ++weight_count;
        }
        if(cfg.flow_normalize) {
            float scale=float(2*weight_count/weight_sum);
            for(auto& w:weight)for(int d=0;d<4;++d)w[d]*=scale;
        }
    }
    if(cfg.flow_flips) {
        // Mutate the existing field without changing its cost scale or removing
        // physical edges. Equal-cost pairs have no direction to reverse.
        std::vector<std::pair<int,int>> edges;
        for(int v=0;v<cells;++v)for(int d=0;d<4;++d) {
            int u=next[v][d];
            if(u>v && weight[v][d]!=weight[u][(d+2)%4])edges.emplace_back(v,d);
        }
        if(cfg.flow_flips>int(edges.size()))throw std::invalid_argument("too many distinct guidance flips");
        std::mt19937 random(cfg.flow_flip_seed);std::shuffle(edges.begin(),edges.end(),random);
        for(int k=0;k<cfg.flow_flips;++k) {
            auto [v,d]=edges[k];int u=next[v][d];
            std::swap(weight[v][d],weight[u][(d+2)%4]);
        }
    }
    if(cfg.flow_reverse) {
        // Reverse every directional preference after local mutations, preserving
        // each physical edge pair's two prices, turns and overall cost scale.
        for(int v=0;v<cells;++v)for(int d=0;d<4;++d) {
            const int u=next[v][d];
            if(u>v)std::swap(weight[v][d],weight[u][(d+2)%4]);
        }
    }
    if(cfg.loop_extent<2 || cfg.loop_extent>8)throw std::invalid_argument("cycle extent must be 2..8");
    if(cfg.guidance_edge_mix>0) {
        // Coherent alternative to blending two distance tables: price each
        // forward/quarter-turn edge, then solve exact oriented shortest paths.
        // Every physical edge remains present; positive contrast keeps the
        // original preferred direction unless the mixture is entirely physical.
        const float fraction=cfg.guidance_edge_mix;
        for(auto& prices:weight)for(float& price:prices)price=(1-fraction)*price+fraction*2;
    }
    if(cfg.loops)for(int height=2;height<=cfg.loop_extent;++height)
    for(int width=2;width<=cfg.loop_extent;++width)
    for(int y=0;y+height<=rows;++y)for(int x=0;x+width<=cols;++x) {
        std::vector<int> ring;
        for(int xx=x;xx<x+width;++xx)ring.push_back(from_grid[y*cols+xx]);
        for(int yy=y+1;yy<y+height;++yy)ring.push_back(from_grid[yy*cols+x+width-1]);
        for(int xx=x+width-2;xx>=x;--xx)ring.push_back(from_grid[(y+height-1)*cols+xx]);
        for(int yy=y+height-2;yy>y;--yy)ring.push_back(from_grid[yy*cols+x]);
        if(std::all_of(ring.begin(),ring.end(),[](int v){return v>=0;}))cycles.push_back(std::move(ring));
    }
    if(cfg.cycle_mask) {
        cycle_masks.reserve(cycles.size());
        for(const auto& ring:cycles) {
            std::vector<CycleWordMask> masks;
            for(int v:ring) {
                const size_t word=size_t(v)/64;const uint64_t bit=uint64_t(1)<<(v%64);
                auto found=std::find_if(masks.begin(),masks.end(),[&](const auto& item){return item.word==word;});
                if(found==masks.end())masks.push_back({word,bit});else found->bits|=bit;
            }
            // Reject a nonstationary perimeter cheaply; checking denser words
            // first changes no cycle ordering or subsequent gain arithmetic.
            std::stable_sort(masks.begin(),masks.end(),[](const auto& a,const auto& b){
                return __builtin_popcountll(a.bits)>__builtin_popcountll(b.bits);
            });
            cycle_masks.push_back(std::move(masks));
        }
    }
    // Peel tree pockets. Agents exiting these get precedence over agents entering.
    pocket.assign(cells,0);
    auto deg=degree;std::queue<int> leaves;
    for(int v=0;v<cells;++v)if(deg[v]<=1)leaves.push(v);
    while(!leaves.empty()) {
        int v=leaves.front();leaves.pop();pocket[v]=1;
        for(int u:next[v])if(u>=0 && !pocket[u] && --deg[u]==1)leaves.push(u);
    }
    if(cfg.pocket_components) {
        auto peeled=pocket;std::fill(pocket.begin(),pocket.end(),0);int component=0;
        for(int seed=0;seed<cells;++seed)if(peeled[seed] && !pocket[seed]) {
            ++component;std::vector<int> q={seed};pocket[seed]=component;
            for(size_t k=0;k<q.size();++k)for(int u:next[q[k]])
                if(u>=0 && peeled[u] && !pocket[u]){pocket[u]=component;q.push_back(u);}
        }
    }
    // A goal-less robot in a peeled tree pocket must prefer leaving it over
    // waiting. Otherwise its exit priority reserves the doorway indefinitely.
    pocket_depth.assign(cells,0);
    std::vector<int> exits;
    for(int v=0;v<cells;++v)if(!pocket[v])exits.push_back(v);
    for(size_t k=0;k<exits.size();++k)for(int u:next[exits[k]])
        if(u>=0 && pocket[u] && pocket_depth[u]==0) {
            pocket_depth[u]=pocket_depth[exits[k]]+1;exits.push_back(u);
        }
    hops.assign(size_t(cells)*cells,65535);
    #pragma omp parallel for num_threads(cfg.threads) schedule(static)
    for(int target=0;target<cells;++target) {
        auto* h=hops.data()+size_t(target)*cells;
        std::vector<int> q(cells);int read=0,end=1;q[0]=target;h[target]=0;
        while(read<end) {
            int v=q[read++];
            for(int u:next[v])if(u>=0 && h[u]==65535) { h[u]=h[v]+1;q[end++]=u; }
        }
    }
    distance.assign(size_t(states)*states,INF);
    #pragma omp parallel for num_threads(cfg.threads) schedule(dynamic,8)
    for(int target=0;target<states;++target) {
        float* d=distance.data()+size_t(target)*states;
        using Item=std::pair<float,int>;
        std::priority_queue<Item,std::vector<Item>,std::greater<Item>> q;
        d[target]=0;q.emplace(0,target);
        while(!q.empty()) {
            auto [cost,state]=q.top();q.pop();if(cost!=d[state])continue;
            const int v=state/4,o=state%4;
            auto relax=[&](int pred,float w) {
                const float value=cost+w;
                if(value<d[pred]) { d[pred]=value;q.emplace(value,pred); }
            };
            relax(v*4+(o+1)%4,weight[v][4]);relax(v*4+(o+3)%4,weight[v][4]);
            int u=next[v][(o+2)%4];
            if(u>=0)relax(u*4+o,forward_weight(target/4,u,o));
        }
    }
    if(cfg.goal_cache) {
        // This table depends only on the map and its costs, so preprocessing
        // may build it before any tasks or starting states are revealed.
        any_heading_distance.resize(size_t(cells)*states);
        #pragma omp parallel for num_threads(cfg.threads) schedule(static)
        for(int target=0;target<cells;++target)for(int source=0;source<states;++source) {
            float best=INF;
            for(int d=0;d<4;++d)best=std::min(best,dist(target*4+d,source));
            any_heading_distance[size_t(target)*states+source]=best;
        }
    }
}
void Graph::blend_distances(const Graph& other,float fraction,int threads) {
    if(!std::isfinite(fraction) || fraction<0 || fraction>1 || threads<1 ||
       states!=other.states || to_grid!=other.to_grid || distance.size()!=other.distance.size())
        throw std::invalid_argument("incompatible guidance potential mixture");
    if(fraction==0)return;
    // Mix oriented cost-to-go potentials while retaining the original lane
    // prices for move ranking. These are not exact distances under those edge
    // prices; the environment guard excludes windowed A* for this experiment.
    #pragma omp parallel for num_threads(threads) schedule(static)
    for(size_t i=0;i<distance.size();++i)
        distance[i]=fraction==1?other.distance[i]:(1-fraction)*distance[i]+fraction*other.distance[i];
    if(!any_heading_distance.empty()) {
        #pragma omp parallel for num_threads(threads) schedule(static)
        for(int target=0;target<cells;++target)for(int source=0;source<states;++source) {
            float best=INF;
            for(int d=0;d<4;++d)best=std::min(best,dist(target*4+d,source));
            any_heading_distance[size_t(target)*states+source]=best;
        }
    }
}
float Graph::approach(int target,int source) const {
    if(!any_heading_distance.empty())return any_heading_distance[size_t(target)*states+source];
    float best=INF;
    for(int d=0;d<4;++d)best=std::min(best,dist(target*4+d,source));
    return best;
}
int Graph::direction(int a,int b) const {
    for(int d=0;d<4;++d)if(next[a][d]==b)return d;
    return -1;
}
Chain::Chain(const Graph& g,const Task& task,bool cache) {
    for(int p:task.locations) {
        if(p<0 || p>=int(g.from_grid.size()) || g.from_grid[p]<0)throw std::invalid_argument("task on blocked cell");
        goals.push_back(g.from_grid[p]);
    }
    tail.resize(goals.size());
    for(int k=int(goals.size())-2;k>=0;--k)
        for(int o=0;o<4;++o) {
            float best=INF;
            for(int q=0;q<4;++q)best=std::min(best,g.dist(goals[k+1]*4+q,goals[k]*4+o)+tail[k+1][q]);
            tail[k][o]=best;
        }
    if(cache) {
        values.resize(goals.size());
        for(int k=0;k<int(goals.size());++k) {
            // The final errand has no remaining tail. Borrow the immutable
            // map-level row instead of rebuilding a copy for every task.
            if(k+1==int(goals.size()) && !g.any_heading_distance.empty())continue;
            values[k].resize(g.states);
            for(int state=0;state<g.states;++state) {
                float best=INF;
                for(int q=0;q<4;++q)best=std::min(best,g.dist(goals[k]*4+q,state)+tail[k][q]);
                values[k][state]=best;
            }
        }
    }
}
const float* Chain::cached_row(const Graph& g,int stage) const {
    if(!values.empty() && !values[stage].empty())return values[stage].data();
    if(stage+1==int(goals.size()) && !g.any_heading_distance.empty())
        return g.any_heading_distance.data()+size_t(goals[stage])*g.states;
    return nullptr;
}
float Chain::cost(const Graph& g,int stage,int cell,int direction) const {
    if(stage>=int(goals.size()))return 0;
    if(const float* row=cached_row(g,stage))return row[cell*4+direction];
    float best=INF;
    for(int q=0;q<4;++q)best=std::min(best,g.dist(goals[stage]*4+q,cell*4+direction)+tail[stage][q]);
    return best;
}
void Engine::initialize(SharedEnvironment* env) {
    // Nested teams are bounded below by outer_workers*inner_workers<=threads.
    // They permit a few more faithful forecasts to use the existing allocation.
    if(cfg.replan_roots && cfg.replan_threads>1)omp_set_max_active_levels(2);
    rng_.seed(cfg.seed);auto prepared_graph=std::make_shared<Graph>(*env,cfg);
    if(cfg.plain_score>0 || cfg.guidance_distance_mix>0) {
        // Physical action costs may inform only evaluation, or optionally the
        // policy's distance potential. Both are built before any tasks appear.
        Config metric=cfg;metric.guidance="none";metric.turn_cost=2;metric.loops=false;
        metric.flow_flips=0;metric.flow_reverse=false;metric.guidance_distance_mix=0;metric.guidance_edge_mix=0;
        auto physical=std::make_unique<Graph>(*env,metric);
        if(cfg.guidance_distance_mix>0)prepared_graph->blend_distances(*physical,cfg.guidance_distance_mix,cfg.threads);
        if(cfg.plain_score>0)score_graph_=std::move(physical);
    }
    // Publish immutable shared graph storage only after preprocessing is done.
    graph=std::move(prepared_graph);
    const int n=env->num_of_agents;
    if(cfg.candidate_cache && cfg.push_price==0) {
        // Bounded per-worker storage is independent of map area/task history.
        // Allocation uses no task/start information and belongs to preprocessing.
        candidate_rankings_.resize(cfg.threads);
        #pragma omp parallel for num_threads(cfg.threads) schedule(static)
        for(int worker=0;worker<cfg.threads;++worker)
            candidate_rankings_[worker].resize(size_t(n)*cfg.cache_slots);
    }
    if(cfg.operation_depth) {operation_model_=std::make_unique<OperationModel>(*graph);operations_.assign(n,OperationModel::waiting);}
    age_.assign(n,0);previous_task_.assign(n,-1);previous_stage_.assign(n,0);
    last_actions_.assign(n,W);
    past_offsets_.clear();
    best_offsets_.resize(n);
    for(float& x:best_offsets_)x=std::uniform_real_distribution<float>(0,1)(rng_);
    std::fprintf(stderr,"R05_INIT agents=%d cells=%d K=%d depth=%d threads=%d guidance=%s seed=%d table_mb=%.1f\n",
                 n,graph->cells,cfg.futures,cfg.depth,cfg.threads,cfg.guidance.c_str(),cfg.seed,
                 graph->distance.size()*sizeof(float)/1e6);
}
double Engine::travel_steps_per_cell() const {
    const uint64_t forward=cfg.active_travel_rate?active_forward_:total_forward_;
    const uint64_t steps=cfg.active_travel_rate?active_agent_steps_:total_agent_steps_;
    return forward?double(steps)/forward:2;
}

void Engine::record_travel(const std::vector<Action>& actions) {
    total_agent_steps_+=actions.size();total_forward_+=std::count(actions.begin(),actions.end(),FW);
    if(cfg.active_travel_rate)for(size_t a=0;a<actions.size();++a)if(assigned_[a]) {
        ++active_agent_steps_;active_forward_+=actions[a]==FW;
    }
}

std::vector<int> hungarian_assignment(const std::vector<float>& matrix,int nr,int nc,
    int dummy_columns,bool fast_dummy_prefix) {
    if(nr<0 || nc<nr || matrix.size()!=size_t(nr)*nc || dummy_columns<0 || dummy_columns>nr)
        throw std::invalid_argument("invalid rectangular matching problem");
    std::vector<double> u(nr+1),v(nc+1);
    std::vector<int> owner(nc+1),previous(nc+1);
    int first_row=1;
    if(fast_dummy_prefix && dummy_columns) {
        const int real_columns=nc-dummy_columns;
        const float idle_cost=matrix[real_columns];
        // Preconditions make every dummy strictly cheaper than every real
        // pair, with identical cost for every row and dummy column.
        for(int row=0;row<nr;++row)for(int j=0;j<nc;++j)
            if(j<real_columns ? !(matrix[size_t(row)*nc+j]>idle_cost) : matrix[size_t(row)*nc+j]!=idle_cost)
                throw std::invalid_argument("dummy-prefix matching preconditions do not hold");
        // The ordinary augmentations assign row k to dummy k for this prefix,
        // with u[k]=idle_cost and all nonzero-column potentials unchanged.
        // Equal-distance scans never replace that row's direct predecessor.
        for(int row=1;row<=dummy_columns;++row) {
            owner[real_columns+row]=row;u[row]=idle_cost;v[0]-=idle_cost;
        }
        first_row=dummy_columns+1;
    }
    for(int row=first_row;row<=nr;++row) {
        owner[0]=row;int column=0;
        std::vector<double> distance(nc+1,1e30);std::vector<bool> visited(nc+1,false);
        do {
            visited[column]=true;int active=owner[column],next_column=0;double delta=1e30;
            for(int j=1;j<=nc;++j)if(!visited[j]) {
                double reduced=matrix[size_t(active-1)*nc+j-1]-u[active]-v[j];
                if(reduced<distance[j]){distance[j]=reduced;previous[j]=column;}
                if(distance[j]<delta){delta=distance[j];next_column=j;}
            }
            for(int j=0;j<=nc;++j) {
                if(visited[j]){u[owner[j]]+=delta;v[j]-=delta;}
                else distance[j]-=delta;
            }
            column=next_column;
        } while(owner[column]);
        do {int prev=previous[column];owner[column]=owner[prev];column=prev;}while(column);
    }
    std::vector<int> assignment(nr,-1);
    for(int j=1;j<=nc;++j)if(owner[j])assignment[owner[j]-1]=j-1;
    return assignment;
}

void Engine::match(SharedEnvironment* env,std::vector<int>& schedule) {
    std::chrono::steady_clock::time_point match_start;
    if(cfg.profile)match_start=std::chrono::steady_clock::now();
    const auto& g=*graph;const int n=env->num_of_agents;
    schedule=env->curr_task_schedule;
    const float admission_price=(!cfg.admission_price_steps || env->curr_timestep<cfg.admission_price_steps)
        ?cfg.admission_price:-1;
    const float length_weight=cfg.initial_length_weight>=0 && env->curr_timestep<cfg.initial_length_steps
        ?cfg.initial_length_weight:cfg.length_weight;
    std::vector<int> agents, tasks;std::unordered_set<int> locked;int suppressed_opened=0;
    for(int i=0;i<n;++i) {
        int id=schedule[i];
        bool started=id>=0 && env->task_pool.at(id).idx_next_loc>0;
        if(started || (!cfg.matching && id>=0)) {
            locked.insert(id);
            if(cfg.active_cap_triage_credit>0 && suppressed_tasks_.size()==size_t(n) && suppressed_tasks_[i]==id)
                ++suppressed_opened;
            continue;
        }
        agents.push_back(i);schedule[i]=-1;
    }
    const bool capped=cfg.active_task_cap>0 &&
        (!cfg.active_cap_steps || env->curr_timestep<cfg.active_cap_steps);
    // Opened tasks remain protected even if they temporarily exceed the cap.
    // All robots remain in the collision planner and may yield or be pushed.
    const int credited_opened=capped?int(std::floor(cfg.active_cap_triage_credit*suppressed_opened)):0;
    const int capacity=capped?std::min(int(agents.size()),std::max(0,cfg.active_task_cap-int(locked.size())+credited_opened)):int(agents.size());
    for(const auto& kv:env->task_pool)if(!locked.count(kv.first))tasks.push_back(kv.first);
    std::sort(tasks.begin(),tasks.end());
    std::vector<float> length(tasks.size(),0);
    for(size_t j=0;j<tasks.size();++j) {
        const auto& stops=env->task_pool.at(tasks[j]).locations;
        for(size_t k=1;k<stops.size();++k)length[j]+=g.hop(g.from_grid[stops[k]],g.from_grid[stops[k-1]]);
    }
    std::vector<std::array<float,4>> continuation;
    if(cfg.chain_matching) {
        continuation.resize(tasks.size());
        for(int j=0;j<int(tasks.size());++j) {
            Chain chain(g,env->task_pool.at(tasks[j]));
            continuation[j]=chain.tail[0];
        }
    }
    std::vector<float> destination_pressure;
    if(cfg.destination_load>0) {
        destination_pressure.assign(g.cells,0);
        // Only already opened tasks are fixed demand. Unopened assignments are
        // being rematched now and must not bias their own replacement costs.
        for(int a=0;a<n;++a) {
            const int id=env->curr_task_schedule[a];
            if(id<0)continue;
            const auto& task=env->task_pool.at(id);
            if(task.idx_next_loc==0)continue;
            const int goal=g.from_grid[task.locations.at(task.idx_next_loc)];
            const int from=g.from_grid[env->curr_states[a].location];
            destination_pressure[goal]+=1.f/(1+g.hop(goal,from));
        }
    }
    const double match_steps_per_cell=travel_steps_per_cell();
    const double remaining_steps=cfg.horizon-env->curr_timestep;
    struct Pair { float cost;int agent,task; };
    const int dummy_columns=(capped || admission_price>=0)?int(agents.size())-std::min(capacity,int(tasks.size())):0;
    // Additional idle columns make the remaining slots optional at this price.
    // The original, strictly cheaper cap dummies stay last so their exact
    // Hungarian-prefix optimization remains valid.
    const int optional_columns=admission_price>=0?std::min(capacity,int(tasks.size())):0;
    const int columns=int(tasks.size())+optional_columns+dummy_columns;
    const bool exact=cfg.hungarian_limit>0 && int(agents.size())<=cfg.hungarian_limit && columns>=int(agents.size());
    std::vector<float> matrix(exact?agents.size()*columns:0);
    std::vector<Pair> pairs;if(!exact)pairs.reserve(agents.size()*tasks.size());
    for(int row=0;row<int(agents.size());++row) {
        int a=agents[row];
        int p=g.from_grid[env->curr_states[a].location];
        if(cfg.predict_matching && !pending_.empty())p=pending_[a];
        for(int j=0;j<int(tasks.size());++j) {
            int t=tasks[j];const auto& task=env->task_pool.at(t);
            float cost=g.hop(g.from_grid[task.locations[task.idx_next_loc]],p)+length_weight*length[j];
            if(cfg.guided_matching) {
                const int goal=g.from_grid[task.locations[task.idx_next_loc]];
                cost=g.approach(goal,p*4+env->curr_states[a].orientation)/2+length_weight*length[j];
            }
            if(cfg.chain_matching) {
                const int goal=g.from_grid[task.locations[0]];
                cost=INF;
                for(int d=0;d<4;++d)cost=std::min(cost,
                    (g.dist(goal*4+d,p*4+env->curr_states[a].orientation)
                     +length_weight*continuation[j][d])/2);
            }
            if(!destination_pressure.empty())
                cost+=cfg.destination_load*destination_pressure[g.from_grid[task.locations[task.idx_next_loc]]];
            if(cfg.match_horizon_weight>0) {
                // Use only this visible unopened chain. Opened tasks remain
                // locked above. Penalize estimated work beyond the explicitly
                // declared end, so matching can avoid immediately triaged pairs.
                const double hops=g.hop(g.from_grid[task.locations[task.idx_next_loc]],p)+length[j];
                const double estimate=hops*match_steps_per_cell*cfg.triage_scale;
                cost+=cfg.match_horizon_weight*float(std::max(0.0,estimate-remaining_steps));
            }
            if(t==env->curr_task_schedule[a])cost-=cfg.keep_bonus;
            if(exact)matrix[size_t(row)*columns+j]=cost;
            else pairs.push_back({cost,a,j});
        }
    }
    if(exact && optional_columns) {
        for(size_t row=0;row<agents.size();++row)
            std::fill(matrix.begin()+row*columns+tasks.size(),
                      matrix.begin()+row*columns+tasks.size()+optional_columns,admission_price);
    }
    if(exact && dummy_columns) {
        float minimum=0;
        for(size_t row=0;row<agents.size();++row)for(size_t j=0;j<tasks.size()+optional_columns;++j)
            minimum=std::min(minimum,matrix[row*columns+j]);
        // Every dummy is cheaper than every real pair, so an optimum fills all
        // fixed dummies. Remaining slots admit minimum-cost real pairs or,
        // when enabled, optional idle columns at the declared price.
        const float idle_cost=minimum-std::max(1.f,std::abs(minimum)*1e-6f);
        for(size_t row=0;row<agents.size();++row)
            std::fill(matrix.begin()+row*columns+tasks.size()+optional_columns,matrix.begin()+(row+1)*columns,idle_cost);
    }
    std::chrono::steady_clock::time_point matrix_done;
    if(cfg.profile)matrix_done=std::chrono::steady_clock::now();
    auto report_match=[&]() {
        if(cfg.profile && (env->curr_timestep<5 || env->curr_timestep%100==0)) {
            const auto now=std::chrono::steady_clock::now();
            std::fprintf(stderr,"R05_MATCH_PROFILE t=%d agents=%zu tasks=%zu matrix_ms=%.3f solve_ms=%.3f\n",
                env->curr_timestep,agents.size(),tasks.size(),
                std::chrono::duration<double,std::milli>(matrix_done-match_start).count(),
                std::chrono::duration<double,std::milli>(now-matrix_done).count());
        }
    };
    if(exact) {
        const auto selected=hungarian_assignment(matrix,int(agents.size()),columns,dummy_columns,cfg.fast_admission);
        for(size_t row=0;row<agents.size();++row)if(selected[row]>=0 && selected[row]<int(tasks.size()))
            schedule[agents[row]]=tasks[selected[row]];
        if(cfg.profile && (env->curr_timestep<5 || env->curr_timestep%100==0)) {
            std::vector<float> admitted_costs;
            for(size_t row=0;row<agents.size();++row)if(selected[row]>=0 && selected[row]<int(tasks.size()))
                admitted_costs.push_back(matrix[row*columns+selected[row]]);
            std::sort(admitted_costs.begin(),admitted_costs.end());
            auto quantile=[&](double q){return admitted_costs.empty()?0.f:admitted_costs[size_t(q*(admitted_costs.size()-1))];};
            std::fprintf(stderr,"R05_ADMISSION_PROFILE t=%d locked=%zu admitted=%zu capacity=%d credited=%d cost_p50=%.3f cost_p90=%.3f cost_max=%.3f\n",
                env->curr_timestep,locked.size(),admitted_costs.size(),capacity,credited_opened,quantile(.5),quantile(.9),quantile(1));
        }
        report_match();return;
    }
    std::sort(pairs.begin(),pairs.end(),[](const Pair& a,const Pair& b) {
        if(a.cost!=b.cost)return a.cost<b.cost;
        return std::tie(a.agent,a.task)<std::tie(b.agent,b.task);
    });
    std::vector<bool> used(tasks.size(),false);int admitted=0;
    for(const auto& p:pairs) {
        if(admitted>=capacity || (admission_price>=0 && p.cost>admission_price))break;
        if(schedule[p.agent]<0 && !used[p.task]) {
            schedule[p.agent]=tasks[p.task];used[p.task]=true;++admitted;
        }
    }
    report_match();
}
void Engine::certify(const Graph& g,const std::vector<int>& from,const std::vector<int>& to) {
    if(from.size()!=to.size())throw std::runtime_error("invalid move count");
    std::vector<int> owner(g.cells,-1),dest(g.cells,-1);
    for(int i=0;i<int(from.size());++i) {
        if(from[i]<0 || from[i]>=g.cells || to[i]<0 || to[i]>=g.cells)throw std::runtime_error("out-of-map move");
        if(owner[from[i]]>=0)throw std::runtime_error("duplicate source");
        owner[from[i]]=i;
        if(dest[to[i]]>=0)throw std::runtime_error("vertex conflict");
        dest[to[i]]=i;
        if(from[i]!=to[i] && g.direction(from[i],to[i])<0)throw std::runtime_error("nonadjacent move");
    }
    for(int i=0;i<int(from.size());++i)if(to[i]!=from[i]) {
        int j=owner[to[i]];
        if(j>=0 && to[j]==from[i])throw std::runtime_error("edge swap");
    }
}

void Engine::fill_ready_moves(const Frame& f,const std::vector<float>& offsets,std::vector<int>& to) const {
    const auto& g=*graph;const int n=int(f.loc.size());
    const auto& assigned=cfg.rollout_match?f.active_chains:assigned_;
    std::vector<int> owner(g.cells,-1),next(n,-1),order(n),stamp(n,-1),index(n);
    std::vector<unsigned char> eligible(n,0);
    std::vector<float> gain(n),priority(n);
    for(int a=0;a<n;++a)owner[to[a]]=a;
    for(int a=0;a<n;++a) {
        const Chain* chain=assigned[a];
        const bool active=chain && f.stage[a]<int(chain->goals.size());
        auto value=[&](int v,int d) {
            return active?chain->cost(g,f.stage[a],v,d):cfg.idle_eviction*g.pocket_depth[v];
        };
        int age=cfg.rollout_age?f.age[a]:age_[a];if(cfg.age_cap>0)age=std::min(age,cfg.age_cap);
        priority[a]=age+offsets[a]-(active?0:100000);
        if(cfg.deadends && g.pocket[f.loc[a]] &&
           (!active || g.pocket[chain->goals[f.stage[a]]]!=g.pocket[f.loc[a]]))priority[a]+=1000000;
        // Preserve every already promised forward move. An idle robot can only
        // add a forward move in its current heading; no turn-and-move shortcut.
        if(to[a]!=f.loc[a])continue;
        int v=g.next[f.loc[a]][f.dir[a]];if(v<0)continue;
        next[a]=v;eligible[a]=1;
        float before=value(f.loc[a],f.dir[a]);
        for(int d:{(f.dir[a]+1)%4,(f.dir[a]+3)%4})before=std::min(before,value(f.loc[a],d));
        gain[a]=before-value(v,f.dir[a]);
    }
    std::iota(order.begin(),order.end(),0);
    std::stable_sort(order.begin(),order.end(),[&](int a,int b){return priority[a]>priority[b];});
    std::vector<int> chain;chain.reserve(n);
    for(int root:order)if(eligible[root]) {
        chain.clear();int a=root,begin=0;bool possible=false;
        while(eligible[a]) {
            if(stamp[a]==root) {
                begin=index[a];possible=int(chain.size())-begin>=3;break;
            }
            stamp[a]=root;index[a]=int(chain.size());chain.push_back(a);
            int b=owner[next[a]];
            if(b<0){possible=true;break;}
            a=b;
        }
        if(!possible)continue;
        float total=0;for(int k=begin;k<int(chain.size());++k)total+=gain[chain[k]];
        if(total<=cfg.early_fill_gain+1e-5f)continue;
        // A cycle cannot accept its incoming tail; a chain must end at an
        // unreserved cell after the existing promises. Two-agent swaps fail
        // the minimum cycle-length check above.
        for(int k=begin;k<int(chain.size());++k)owner[f.loc[chain[k]]]=-1;
        for(int k=begin;k<int(chain.size());++k) {
            int b=chain[k];to[b]=next[b];owner[to[b]]=b;eligible[b]=0;
        }
    }
}

void Engine::advance(Frame& f,const std::vector<float>& offsets,std::vector<Action>& actions,
                     uint64_t& expansion_count,bool cycle_moves,bool early_moves) const {
    PolicyTiming* timing=policy_profile_active_?&policy_timings_[omp_get_thread_num()]:nullptr;
    const bool sampled=timing && ++timing->calls%64==0;
    std::chrono::steady_clock::time_point measured;
    if(sampled){++timing->samples;measured=std::chrono::steady_clock::now();}
    auto mark_policy=[&](int phase) {
        if(!sampled)return;
        const auto now=std::chrono::steady_clock::now();
        timing->nanoseconds[phase]+=std::chrono::duration_cast<std::chrono::nanoseconds>(now-measured).count();
        measured=now;
    };
    const auto& assigned=cfg.rollout_match?f.active_chains:assigned_;
    const int cycle_mode=cycle_moves?cfg.pre_cycles:0;
    const auto& g=*graph;const int n=int(f.loc.size());
    // pending is the already promised forward/wait move. Plan the following
    // movement on its exact resulting occupancy, while current idle robots turn.
    PolicyScratch fresh;
    thread_local PolicyScratch reused;
    auto& scratch=cfg.scratch_reuse?reused:fresh;
    auto& p=scratch.p;p=f.pending;
    auto& moving=scratch.moving;moving.resize(n);
    auto& owner=scratch.owner;owner.assign(g.cells,-1);
    auto& chosen=scratch.chosen;chosen.resize(n);
    auto& reserve=scratch.reserve;reserve.resize(g.cells);
    if(cfg.early_fill || early_moves)fill_ready_moves(f,offsets,p);
    for(int i=0;i<n;++i) {
        moving[i]=p[i]!=f.loc[i];owner[p[i]]=i;
        if(moving[i] && g.next[f.loc[i]][f.dir[i]]!=p[i])
            throw std::runtime_error("pending move not aligned with heading");
        bool arrived=assigned[i] && f.stage[i]<int(assigned[i]->goals.size()) &&
                     p[i]==assigned[i]->goals[f.stage[i]];
        if(arrived)++f.stage[i];
        if(cfg.rollout_age)f.age[i]=arrived?int((f.age[i]+1)*cfg.waypoint_age_retain):f.age[i]+1;
    }
    // Stage is fixed throughout this policy step. Resolve the active chain
    // and its cached row once, rather than for every candidate lookup.
    auto& active_chain=scratch.active_chain;active_chain.assign(n,nullptr);
    auto& cost_table=scratch.cost_table;cost_table.assign(n,nullptr);
    for(int a=0;a<n;++a) {
        const Chain* chain=assigned[a];
        if(chain && f.stage[a]<int(chain->goals.size())) {
            active_chain[a]=chain;
            if(!cfg.lazy_cost_rows)cost_table[a]=chain->cached_row(g,f.stage[a]);
        }
    }
    auto cost=[&](int a,int v,int d) {
        // Shared ranking hits often need no raw cost lookup at all. Defer its
        // row pointer until a cycle, rotation or cache miss actually needs it.
        if(cfg.lazy_cost_rows && !cost_table[a] && active_chain[a])
            cost_table[a]=active_chain[a]->cached_row(g,f.stage[a]);
        if(cost_table[a])return cost_table[a][v*4+d];
        if(active_chain[a])return active_chain[a]->cost(g,f.stage[a],v,d);
        return cfg.idle_eviction*g.pocket_depth[v];
    };
    auto forward_weight=[&](int a,int cell,int d) {
        return g.forward_weight(active_chain[a]?active_chain[a]->goals[f.stage[a]]:-1,cell,d);
    };
    auto allowed=[&](int a,int d) {return moving[a]?d==f.dir[a]:turn(d,f.dir[a])<=1;};
    auto& idle_heading=scratch.idle_heading;idle_heading.resize(n);
    auto& forced_heading=scratch.forced_heading;forced_heading.assign(n,-1);
    auto& base_cost=scratch.base_cost;base_cost.resize(n);
    auto& priorities=scratch.priorities;priorities.resize(n);
    auto& candidates=scratch.candidates;candidates.resize(n);
    auto& candidate_count=scratch.candidate_count;candidate_count.resize(n);
    auto& ranking_slots=scratch.ranking_slots;ranking_slots.assign(n,nullptr);
    auto& ranking_hits=scratch.ranking_hits;ranking_hits.assign(n,0);
    auto& kinematic_masks=scratch.kinematic_masks;
    if(cfg.kinematic_mask)kinematic_masks.resize(n);
    // Push loss depends on another robot's state, so that optional policy uses
    // the original path. All other ranking inputs are captured below; priorities
    // and collision resolution are always recomputed for the current future.
    CachedRanking* cache=cfg.candidate_cache && cfg.push_price==0 && !candidate_rankings_.empty()
        && ranking_epoch_<=std::numeric_limits<uint32_t>::max()
        ?candidate_rankings_[omp_get_thread_num()].data():nullptr;
    const int cache_shift=64-__builtin_ctz(unsigned(cfg.cache_slots));
    const bool shared=cfg.shared_rankings_mb>0 && cfg.push_price==0 && !cfg.rollout_match && !cfg.replan_roots;
    for(int i=0;i<n;++i) {
        if(shared && cfg.shared_orders && cfg.move_bias==0 && active_chain[i] && !active_chain[i]->order_rankings.empty()) {
            const auto& entry=active_chain[i]->order_rankings[(size_t(f.stage[i])*g.cells+p[i])*8+f.dir[i]*2+moving[i]];
            ranking_hits[i]=2;idle_heading[i]=entry.idle_heading();base_cost[i]=entry.base_cost;
            candidate_count[i]=entry.count();
            if(cfg.kinematic_mask)kinematic_masks[i]=entry.kinematic_mask();
            entry.load(candidates[i],p[i],g.next[p[i]]);
        } else if(shared && active_chain[i] && !active_chain[i]->rankings.empty()) {
            const auto& entry=active_chain[i]->rankings[(size_t(f.stage[i])*g.cells+p[i])*8+f.dir[i]*2+moving[i]];
            ranking_hits[i]=2;idle_heading[i]=entry.idle_heading;base_cost[i]=entry.base_cost;
            candidate_count[i]=entry.count;
            if(cfg.kinematic_mask)kinematic_masks[i]=entry.kinematic_mask;
            entry.load(candidates[i],p[i],g.next[p[i]]);
        } else if(cache && uint32_t(f.stage[i])<=std::numeric_limits<uint16_t>::max()) {
            const uint32_t pose=uint32_t(p[i]*8+f.dir[i]*2+moving[i]);
            // Preserve the original hash/address sequence. The stored key only
            // removes unused bits; very long chains use the uncached path.
            const uint64_t hash_key=(uint64_t(uint32_t(f.stage[i]))<<32)|pose;
            const uint32_t key=(uint32_t(f.stage[i])<<16)|pose;
            size_t slot=(hash_key*0x9e3779b97f4a7c15ULL)>>cache_shift;
            auto* entry=&cache[size_t(i)*cfg.cache_slots+slot];ranking_slots[i]=entry;
            if(entry->epoch==ranking_epoch_ && entry->key==key && entry->chain==active_chain[i]) {
                ranking_hits[i]=1;idle_heading[i]=entry->idle_heading;base_cost[i]=entry->base_cost;
                if(cfg.fuse_cache_hits) {
                    // Copy while this sparse cache entry is already being read.
                    // Later collision/priority resolution still runs unchanged.
                    candidate_count[i]=entry->count;
                    if(cfg.kinematic_mask)kinematic_masks[i]=entry->kinematic_mask;
                    entry->load(candidates[i]);
                }
            } else {
                entry->epoch=uint32_t(ranking_epoch_);entry->key=key;entry->chain=active_chain[i];
            }
        }
        if(!ranking_hits[i]) {
            int best_dir=f.dir[i];float best=cost(i,p[i],best_dir);
            if(!moving[i])for(int q:{(f.dir[i]+1)%4,(f.dir[i]+3)%4}) {
                float x=cost(i,p[i],q)+0.05f*g.weight[p[i]][4];
                if(x<best-1e-5f) {best=x;best_dir=q;}
            }
            idle_heading[i]=best_dir;
            base_cost[i]=cost(i,p[i],best_dir);
            if(ranking_slots[i]) {
                ranking_slots[i]->idle_heading=idle_heading[i];ranking_slots[i]->base_cost=base_cost[i];
            }
        }
        int age=cfg.rollout_age?f.age[i]:age_[i];
        if(cfg.age_cap>0)age=std::min(age,cfg.age_cap);
        float priority=age+offsets[i];
        const bool active=assigned[i] && f.stage[i]<int(assigned[i]->goals.size());
        if(!active)priority-=100000;
        else if(priority_remaining_scale_>0)priority-=priority_remaining_scale_*base_cost[i];
        if(cfg.deadends && g.pocket[p[i]] &&
           (!active || g.pocket[assigned[i]->goals[f.stage[i]]]!=g.pocket[p[i]]))priority+=1000000;
        priorities[i]=priority;
    }
    mark_policy(0);
    auto propose_cycles=[&](bool blocked_only) {
        struct Proposal { float gain;const std::vector<int>* ring;int sign;bool ready; };
        std::vector<Proposal> proposals;
        for(const auto& ring:g.cycles) {
            const int count=int(ring.size());bool idle=true;
            for(int v:ring)if(owner[v]<0 || moving[owner[v]] || (blocked_only && chosen[owner[v]]!=p[owner[v]])){idle=false;break;}
            if(!idle)continue;
            for(int sign:{1,count-1}) {
                float gain=0;bool ready=true;
                for(int k=0;k<count;++k) {
                    int a=owner[ring[k]],v=ring[(k+sign)%count],d=g.direction(ring[k],v);
                    ready=ready && allowed(a,d);
                    // Compare against the best heading available during an idle
                    // step, so rotations alone do not make a cycle profitable.
                    float before=base_cost[a];
                    if(cycle_mode>=2)for(int q=0;q<4;++q)before=std::min(before,cost(a,ring[k],q));
                    gain+=before-cost(a,v,d);
                }
                if((ready || cycle_mode>=2) && gain>cfg.loop_threshold+cfg.pre_cycle_gain*count)
                    proposals.push_back({gain,&ring,sign,ready});
            }
        }
        std::stable_sort(proposals.begin(),proposals.end(),[](const Proposal& a,const Proposal& b){return a.gain>b.gain;});
        for(const auto& proposal:proposals) {
            const auto& ring=*proposal.ring;const int count=int(ring.size());bool free=true;
            for(int v:ring)if(blocked_only ? (chosen[owner[v]]!=p[owner[v]] || forced_heading[owner[v]]>=0) : chosen[owner[v]]>=0){free=false;break;}
            if(!free)continue;
            for(int k=0;k<count;++k) {
                int a=owner[ring[k]],target=ring[(k+proposal.sign)%count];
                forced_heading[a]=g.direction(ring[k],target);
                chosen[a]=proposal.ready?target:ring[k];reserve[chosen[a]]=a;
            }
        }
    };
    // Spatial intent and executable PIBT use identical costs and priorities.
    // Build their shared ordering once, then filter infeasible headings while
    // visiting candidates. Stable sorting preserves the previous tie order.
    for(int i=0;i<n;++i) {
        auto& cand=candidates[i];int count=0;
        if(ranking_hits[i]) {
            if(ranking_hits[i]==1 && !cfg.fuse_cache_hits) {
                const auto& entry=*ranking_slots[i];candidate_count[i]=entry.count;
                if(cfg.kinematic_mask)kinematic_masks[i]=entry.kinematic_mask;
                entry.load(cand);
            }
            continue;
        }
        for(int d=0;d<4;++d) {
            int v=g.next[p[i]][d];if(v<0 || (!cfg.intent_rotation && !allowed(i,d)))continue;
            float score=cost(i,v,d)+forward_weight(i,p[i],d);
            int b=owner[v];
            if(cfg.push_price>0 && b>=0 && b!=i) {
                float loss=INF;
                for(int q=0;q<4;++q)if(g.next[v][q]>=0 && allowed(b,q))
                    loss=std::min(loss,cost(b,g.next[v][q],q)+forward_weight(b,v,q)-base_cost[b]);
                score+=cfg.push_price*std::max(0.0f,std::min(100.0f,loss));
            }
            cand[count++]={v,d,score};
        }
        // A small tie preference preserves useful forward chains.
        float wait_value=base_cost[i];
        if(cfg.prospective_wait) {
            // The future wait is itself an opportunity to rotate. A robot
            // moving now can turn once then; one idle now can turn twice.
            for(int d=0;d<4;++d)if(!moving[i] || turn(d,f.dir[i])<=1)
                wait_value=std::min(wait_value,cost(i,p[i],d));
        }
        cand[count++]={p[i],idle_heading[i],wait_value+cfg.wait_cost+0.001f};
        // At most five entries: stable insertion sort avoids a temporary
        // allocation for every robot in every simulated policy step.
        for(int k=1;k<count;++k) {
            MoveCandidate value=cand[k];int j=k;
            while(j>0 && value.score<cand[j-1].score){cand[j]=cand[j-1];--j;}
            cand[j]=value;
        }
        candidate_count[i]=count;
        if(cfg.kinematic_mask) {
            // Eligibility depends only on the same pose/moving-state key as
            // the cached ranking. Preserve sorted indices, including waiting.
            unsigned int mask=0;
            for(int k=0;k<count;++k)
                if(cand[k].v==p[i] || allowed(i,cand[k].d))mask|=1u<<k;
            kinematic_masks[i]=mask;
        }
        if(ranking_slots[i]) {
            auto& entry=*ranking_slots[i];entry.count=count;
            if(cfg.kinematic_mask)entry.kinematic_mask=kinematic_masks[i];
            entry.save(cand);
        }
    }
    // Explore nearby routing alternatives as well as priority orders. The
    // deterministic root vector supplies proposal randomness, not score credit.
    // Keep cached rankings unbiased: mutate only this rollout's local copy.
    const uint64_t bias_cutoff=uint64_t(double(cfg.move_bias_fraction)*4294967296.0);
    if(cfg.move_bias>0 && bias_cutoff>0)for(int a=0;a<n;++a) {
        uint32_t bits;std::memcpy(&bits,&offsets[a],sizeof(bits));
        uint32_t key=bits^(uint32_t(a)+1)*0x9e3779b9U;
        key^=key>>16;key*=0x7feb352dU;key^=key>>15;key*=0x846ca68bU;key^=key>>16;
        // Separate the activation bits from the preferred-direction bits.
        // The default quarter exactly preserves the old low-two-bits test.
        const uint32_t activation=((key&3u)<<30)|((key>>4)<<2);
        if(uint64_t(activation)>=bias_cutoff)continue;
        int preferred=int((key>>2)&3u);
        if(cfg.move_bias_mode) {
            // Sample a real edge instead of wasting proposals on walls. Mode2
            // also permits waiting; legality still comes from the joint PIBT
            // and kinematic checks, and the rollout score remains unbiased.
            std::array<int,5> directions{};int choices=0;
            for(int d=0;d<4;++d)if(g.next[p[a]][d]>=0)directions[choices++]=d;
            if(cfg.move_bias_mode==2)directions[choices++]=4;
            if(!choices)continue;
            uint32_t choice=key^0xa511e9b3u;choice^=choice>>16;choice*=0x7feb352du;choice^=choice>>15;
            preferred=directions[size_t((uint64_t(choice)*choices)>>32)];
        }
        auto& cand=candidates[a];const int count=candidate_count[a];
        for(int k=0;k<count;++k)
            if(cand[k].v==p[a]?preferred==4:cand[k].d==preferred)
                cand[k].score-=cfg.move_bias;
        for(int k=1;k<count;++k) {
            MoveCandidate value=cand[k];int j=k;
            while(j>0 && value.score<cand[j-1].score){cand[j]=cand[j-1];--j;}
            cand[j]=value;
        }
        if(cfg.kinematic_mask) {
            unsigned int mask=0;
            for(int k=0;k<count;++k)
                if(cand[k].v==p[a] || allowed(a,cand[k].d))mask|=1u<<k;
            kinematic_masks[a]=mask;
        }
    }
    mark_policy(1);
    auto& order=scratch.order;
    order_priorities(priorities,cfg.packed_order,cfg.radix_order,order,scratch.priority_keys,scratch.radix_buffer);
    mark_policy(2);
    auto& prepared=scratch.prepared;prepared.clear();
    auto choose=[&](auto mode_tag) {
    constexpr int mode=decltype(mode_tag)::value;
    constexpr bool kinematic=mode!=0;
    std::fill(chosen.begin(),chosen.end(),-1);
    std::fill(reserve.begin(),reserve.end(),-1);
    if(kinematic && cfg.intent_mode==2)for(int i=0;i<n;++i)if(prepared[i]!=p[i]) {
        chosen[i]=prepared[i];reserve[chosen[i]]=i;
    }
    if(kinematic && cycle_mode>0 && cycle_mode<3)propose_cycles(false);
    int expansions=0;
    auto pibt=[&](auto&& self,int a)->bool {
        ++expansions;
        unsigned int remaining=0;
        if constexpr(mode==2)remaining=kinematic_masks[a];
        for(int ordinal=0;mode==2?remaining!=0:ordinal<candidate_count[a];++ordinal) {
            int k=ordinal;
            if constexpr(mode==2) {
                k=__builtin_ctz(remaining);remaining&=remaining-1;
            }
            int v=candidates[a][k].v;
            if constexpr(mode==1)
                if(v!=p[a] && !allowed(a,candidates[a][k].d))continue;
            if(expansions>cfg.expansion_limit && v!=p[a])continue;
            if(reserve[v]>=0)continue;
            int b=owner[v];
            if(b>=0 && b!=a && chosen[b]==p[a])continue;
            chosen[a]=v;reserve[v]=a;
            if(b>=0 && b!=a && chosen[b]<0 && !self(self,b)) {
                // A failed child commits to staying, overriding our tentative
                // claim on its cell. Keep that blocker; do not explore it again.
                if(reserve[v]==a)reserve[v]=-1;
                chosen[a]=-1;
                continue;
            }
            return true;
        }
        // Priority inheritance propagates failure back to the requesting
        // parent, which must release its attempted move into this cell.
        chosen[a]=p[a];reserve[p[a]]=a;
        return false;
    };
    for(int a:order)if(chosen[a]<0)pibt(pibt,a);
    expansion_count+=expansions;
    };
    auto& intent=scratch.intent;intent.clear();
    if(cfg.intent_rotation){choose(std::integral_constant<int,0>{});intent=chosen;}
    if(cfg.intent_mode) {
        // The collision-free spatial assignment decomposes into disjoint chains
        // ending at holes and cycles. Commit a component only when every member
        // can execute its intended heading after the current action.
        std::vector<int> parent(n);std::iota(parent.begin(),parent.end(),0);
        auto root=[&](int a) {
            while(parent[a]!=a){parent[a]=parent[parent[a]];a=parent[a];}
            return a;
        };
        for(int i=0;i<n;++i)if(intent[i]!=p[i]) {
            int b=owner[intent[i]];
            if(b>=0)parent[root(i)]=root(b);
        }
        std::vector<unsigned char> ready(n,1);
        for(int i=0;i<n;++i)if(intent[i]!=p[i] && !allowed(i,g.direction(p[i],intent[i])))
            ready[root(i)]=0;
        prepared=p;
        for(int i=0;i<n;++i)if(ready[root(i)])prepared[i]=intent[i];
    }
    mark_policy(3);
    // Mode 1 executes only complete ready components. Mode 2 pins those moves
    // and fills the remaining space with the usual kinematic PIBT policy.
    if(cfg.intent_mode==1)chosen=prepared;
    else if(cfg.kinematic_mask)choose(std::integral_constant<int,2>{});
    else choose(std::integral_constant<int,1>{});
    mark_policy(4);
    if(cycle_mode==3)propose_cycles(true);
    if(cfg.loops) {
        auto& ready=scratch.cycle_ready;
        if(cfg.cycle_mask) {
            ready.assign((g.cells+63)/64,0);
            for(int i=0;i<n;++i)if(chosen[i]==p[i] && forced_heading[i]<0)
                ready[size_t(p[i])/64]|=uint64_t(1)<<(p[i]%64);
        }
        // Visit the same geometric rings in the same order. A word mask only
        // replaces the test that every vertex has an eligible stationary robot.
        for(size_t ring_index=0;ring_index<g.cycles.size();++ring_index) {
            const auto& v=g.cycles[ring_index];
            const int count=int(v.size());std::array<int,32> a;bool good=true;
            if(cfg.cycle_mask) {
                for(const auto& mask:g.cycle_masks[ring_index])
                    if((ready[mask.word]&mask.bits)!=mask.bits){good=false;break;}
                if(!good)continue;
                for(int k=0;k<count;++k)a[k]=owner[v[k]];
            } else {
                for(int k=0;k<count;++k) {
                    if(owner[v[k]]<0){good=false;break;}
                    a[k]=owner[v[k]];
                    if(chosen[a[k]]!=v[k] || forced_heading[a[k]]>=0){good=false;break;}
                }
                if(!good)continue;
            }
            float best_gain=cfg.loop_threshold;int sign=0;
            for(int s:{1,count-1}) {
                float gain=0;bool ok=true;
                for(int k=0;k<count;++k) {
                    int u=v[(k+s)%count],d=g.direction(v[k],u);
                    if(!allowed(a[k],d)){ok=false;break;}
                    gain+=base_cost[a[k]]-cost(a[k],u,d);
                }
                if(ok && gain>best_gain){best_gain=gain;sign=s;}
            }
            if(sign)for(int k=0;k<count;++k) {
                chosen[a[k]]=v[(k+sign)%count];
                // Later overlapping rings must see these robots as committed,
                // just as the original chosen-position checks did.
                if(cfg.cycle_mask)ready[size_t(v[k])/64]&=~(uint64_t(1)<<(v[k]%64));
            }
        }
    }
    mark_policy(5);
    actions.assign(n,W);
    for(int i=0;i<n;++i) {
        if(moving[i])actions[i]=FW;
        else {
            int d=chosen[i]==p[i]?idle_heading[i]:g.direction(p[i],chosen[i]);
            int wanted=-1;
            if(chosen[i]==p[i]) {
                if(forced_heading[i]>=0)wanted=forced_heading[i];
                else if(cfg.intent_rotation && intent[i]!=p[i])wanted=g.direction(p[i],intent[i]);
            }
            if(wanted>=0) {
                if(turn(wanted,f.dir[i])==2) {
                    int right=(f.dir[i]+1)%4,left=(f.dir[i]+3)%4;
                    d=cost(i,p[i],right)<=cost(i,p[i],left)?right:left;
                } else d=wanted;
            }
            int delta=(d-f.dir[i]+4)%4;
            if(delta==2)throw std::runtime_error("pipeline requested an impossible half-turn");
            if(delta==1)actions[i]=CR;
            if(delta==3)actions[i]=CCR;
            f.dir[i]=d;
        }
    }
    if(cfg.reverse_penalty>0) {
        for(int i=0;i<n;++i)
            f.reverse_turns+=(actions[i]==CR && f.last_actions[i]==CCR) ||
                             (actions[i]==CCR && f.last_actions[i]==CR);
        f.last_actions=actions;
    }
    f.loc.swap(p);f.pending.swap(chosen);
    mark_policy(6);
}

void Engine::match_future(Frame& frame) const {
    const auto& g=*graph;
    std::vector<int> ready;
    for(int a=0;a<int(frame.loc.size());++a) {
        const Chain* task=frame.active_chains[a];
        if(task && frame.stage[a]>=int(task->goals.size())) {
            ready.push_back(a);frame.active_chains[a]=nullptr;
            if(score_graph_)frame.plain_chains[a]=nullptr;
            frame.stage[a]=0;
        }
    }
    // Match only newly finished robots to tasks visible and unassigned at the
    // real step's start. Do not inspect hidden future tasks or alter the real
    // schedule. Each rollout owns its availability mask.
    while(!ready.empty()) {
        float best=INF;int robot=-1,task=-1,position=-1;
        for(int k=0;k<int(ready.size());++k) {
            int a=ready[k],p=frame.loc[a],goal;
            for(int j=0;j<int(future_tasks_.size());++j)if(frame.free_tasks[j]) {
                goal=future_tasks_[j]->goals[0];
                float approach=g.hop(goal,p);
                if(cfg.guided_matching) {
                    approach=INF;
                    for(int d=0;d<4;++d)approach=std::min(approach,g.dist(goal*4+d,p*4+frame.dir[a])/2);
                }
                float cost=approach+cfg.length_weight*future_lengths_[j];
                if(cost<best){best=cost;robot=a;task=j;position=k;}
            }
        }
        if(task<0)break;
        frame.active_chains[robot]=future_tasks_[task];
        if(score_graph_)frame.plain_chains[robot]=future_plain_[task];
        frame.free_tasks[task]=0;ready.erase(ready.begin()+position);
    }
}

Rollout Engine::rollout(Frame frame,const std::vector<float>& offsets,bool cycle_moves,
                        const Continuation* continuation,RolloutPrefix* save,
                        const RolloutPrefix* resume,const Rollout* forced_first,bool early_moves) const {
    const auto& g=*graph;Rollout r;r.offsets=offsets;r.cycle_moves=cycle_moves;r.early_moves=early_moves;
    auto total_cost=[&](const Frame& f) {
        double guided=0,plain=0;
        const auto& assigned=cfg.rollout_match?f.active_chains:assigned_;
        if(cfg.progress_softcap>0) {
            const double scale=cfg.progress_softcap;
            for(int i=0;i<int(f.loc.size());++i)if(assigned[i]) {
                const double remaining=assigned[i]->cost(g,f.stage[i],f.loc[i],f.dir[i]);
                guided+=scale*remaining/(scale+remaining);
            }
            return guided*progress_normalization_;
        }
        const auto& plain_assigned=cfg.rollout_match?f.plain_chains:score_assigned_;
        for(int i=0;i<int(f.loc.size());++i)if(assigned[i]) {
            const double weight=score_weights_.empty()?1:score_weights_[i];
            guided+=weight*assigned[i]->cost(g,f.stage[i],f.loc[i],f.dir[i]);
            if(score_graph_)plain+=weight*plain_assigned[i]->cost(*score_graph_,f.stage[i],f.loc[i],f.dir[i]);
        }
        return score_graph_?guided*(1-cfg.plain_score)+plain*cfg.plain_score:guided;
    };
    auto completed=[&](const Frame& f) {
        int count=0;
        const auto& assigned=cfg.rollout_match?f.active_chains:assigned_;
        for(int i=0;i<int(f.loc.size());++i)
            count+=assigned[i] && f.stage[i]>=int(assigned[i]->goals.size());
        return count;
    };
    int completions=resume?resume->completions:0;
    double initial=resume?resume->initial:total_cost(frame);
    double previous=resume?resume->previous:initial,progress=resume?resume->progress:0;
    double discounted=resume?resume->discounted:0,weight=resume?resume->weight:1;
    double weight_sum=resume?resume->weight_sum:0;
    if(resume){r.first=resume->first;r.actions=resume->actions;}
    std::vector<Action> actions;
    std::vector<float> future_offsets;
    if(continuation)future_offsets=offsets;
    for(int t=resume?resume->time:0;t<cfg.depth;++t) {
        if(continuation)for(const auto& change:continuation->at(t))
            future_offsets[change.agent]=change.offset;
        const auto& priorities=continuation?future_offsets:offsets;
        const int before_completed=cfg.completion_bonus>0?completed(frame):0;
        if(t==0 && forced_first) {
            frame=forced_first->first;actions=forced_first->actions;
        } else if(cfg.operation_depth)advance_operations(frame,priorities,actions,r.expansions);
        else advance(frame,priorities,actions,r.expansions,cycle_moves,early_moves && t==0);
        if(t==0){r.first=frame;r.actions=actions;}
        if(cfg.completion_bonus>0)completions+=completed(frame)-before_completed;
        if(cfg.progress_discount<1 || cfg.rollout_match) {
            double current=total_cost(frame);
            progress+=previous-current;
            discounted+=weight*(previous-current);weight_sum+=weight;
            weight*=cfg.progress_discount;previous=current;
        }
        if(cfg.rollout_match && t+1<cfg.depth) {
            match_future(frame);
            // New task cost is a new baseline, not negative progress. Credit
            // only distance actually reduced while executing each chain.
            previous=total_cost(frame);
        }
        if(save && t+1==cfg.continuation_start) {
            save->frame=frame;save->first=r.first;save->actions=r.actions;
            save->time=t+1;save->completions=completions;
            save->initial=initial;save->previous=previous;save->progress=progress;
            save->discounted=discounted;save->weight=weight;save->weight_sum=weight_sum;
        }
    }
    r.score=(cfg.rollout_match?progress:initial-total_cost(frame))/2.0;
    if(cfg.progress_discount<1)r.score=discounted*cfg.depth/(2*weight_sum);
    r.score-=cfg.reverse_penalty*frame.reverse_turns;
    // Finished agents have no replacement task inside these short rollouts.
    // An optional terminal reward tests whether pure distance decrease therefore
    // undervalues completing a chain relative to advancing an unfinished one.
    if(cfg.completion_bonus>0)r.score+=cfg.completion_bonus*completions;
    if(cfg.dispersion) {
        int pairs=0;
        if(cfg.fast_dispersion)pairs=g.nearby_pairs(frame.loc);
        else {
            std::vector<int> occupancy(g.from_grid.size(),0);
            for(int v:frame.loc)occupancy[g.to_grid[v]]=1;
            for(int v:frame.loc) {
                int p=g.to_grid[v],x=p%g.cols,y=p/g.cols;
                for(int yy=std::max(0,y-2);yy<=std::min(g.rows-1,y+2);++yy)
                    for(int xx=std::max(0,x-2);xx<=std::min(g.cols-1,x+2);++xx)
                        if(yy*g.cols+xx>p)pairs+=occupancy[yy*g.cols+xx];
            }
        }
        r.score-=cfg.dispersion*pairs;
    }
    return r;
}
Rollout Engine::evaluate(const Frame& frame,const std::vector<float>& offsets,
                         const std::vector<Continuation>& continuations,bool cycle_moves,
                         std::vector<double>* branch_scores,const Rollout* forced_first,bool early_moves) const {
    RolloutPrefix prefix;
    const bool shared=cfg.share_prefix && !continuations.empty();
    Rollout result=rollout(frame,offsets,cycle_moves,nullptr,shared?&prefix:nullptr,nullptr,forced_first,early_moves);
    if(branch_scores){branch_scores->clear();branch_scores->reserve(continuations.size()+1);branch_scores->push_back(result.score);}
    if(continuations.empty())return result;
    if(shared && prefix.time!=cfg.continuation_start)
        throw std::runtime_error("missing shared rollout prefix");
    double score=result.score,mean=result.score,variance_sum=0;int count=1;
    for(const auto& continuation:continuations) {
        Rollout branch=shared
            ?rollout(prefix.frame,offsets,cycle_moves,&continuation,nullptr,&prefix,nullptr,early_moves)
            :rollout(frame,offsets,cycle_moves,&continuation,nullptr,nullptr,forced_first,early_moves);
        // A branch evaluates the root's decision; it cannot silently substitute
        // a different first action or promise while contributing to its score.
        if(branch.actions!=result.actions || branch.first.loc!=result.first.loc ||
           branch.first.dir!=result.first.dir || branch.first.pending!=result.first.pending ||
           branch.first.stage!=result.first.stage || branch.first.operations!=result.first.operations)
            throw std::runtime_error("continuation changed the first decision");
        score+=branch.score;result.expansions+=branch.expansions;
        result.evaluated_branches+=branch.evaluated_branches;
        if(branch_scores)branch_scores->push_back(branch.score);
        if(cfg.continuation_risk!=0) {
            ++count;double delta=branch.score-mean;mean+=delta/count;
            variance_sum+=delta*(branch.score-mean);
        }
    }
    result.score=score/(continuations.size()+1);
    // Positive coefficients penalize sensitivity to future priorities; negative
    // coefficients test optimism about the next real step's re-optimization.
    // Both still execute every configured continuation. Zero preserves the mean.
    if(cfg.continuation_risk!=0)
        result.score-=cfg.continuation_risk*std::sqrt(std::max(0.0,variance_sum/count));
    return result;
}


// Resume the same root under later common continuation draws. Accumulated
// sums preserve branch order; partial means never compete with full means.
void Engine::evaluate_until(const Frame& frame,const std::vector<float>& offsets,
                            const std::vector<Continuation>& continuations,bool cycle_moves,
                            int branches,ScreenedRollout& state,bool early_moves) const {
    const bool shared=cfg.share_prefix && !continuations.empty();
    if(!state.count) {
        state.result=rollout(frame,offsets,cycle_moves,nullptr,shared?&state.prefix:nullptr,nullptr,nullptr,early_moves);
        state.sum=state.mean=state.result.score;state.count=1;
        if(shared && state.prefix.time!=cfg.continuation_start)
            throw std::runtime_error("missing screened rollout prefix");
    }
    while(state.count<branches) {
        const auto& continuation=continuations.at(state.count-1);
        Rollout branch=shared
            ?rollout(state.prefix.frame,offsets,cycle_moves,&continuation,nullptr,&state.prefix,nullptr,early_moves)
            :rollout(frame,offsets,cycle_moves,&continuation,nullptr,nullptr,nullptr,early_moves);
        auto& result=state.result;
        if(branch.actions!=result.actions || branch.first.loc!=result.first.loc ||
           branch.first.dir!=result.first.dir || branch.first.pending!=result.first.pending ||
           branch.first.stage!=result.first.stage || branch.first.operations!=result.first.operations)
            throw std::runtime_error("screened continuation changed the first decision");
        state.sum+=branch.score;result.expansions+=branch.expansions;
        result.evaluated_branches+=branch.evaluated_branches;++state.count;
        if(cfg.continuation_risk!=0) {
            double delta=branch.score-state.mean;state.mean+=delta/state.count;
            state.variance_sum+=delta*(branch.score-state.mean);
        }
    }
    state.result.score=state.sum/state.count;
    if(cfg.continuation_risk!=0)
        state.result.score-=cfg.continuation_risk*std::sqrt(std::max(0.0,state.variance_sum/state.count));
    state.result.fully_evaluated=state.count==cfg.continuations;
}

// Select distinct evaluated vectors with the chosen incumbent first. Reused
// across generations and between real steps; scores never cross a real step.
std::vector<std::vector<int>> priority_dependencies(const Graph& g,const Frame& f,
    const std::vector<const Chain*>& assigned,int preferred_edges) {
    const int n=int(f.loc.size());
    if(preferred_edges<1 || preferred_edges>4 || int(f.pending.size())!=n ||
       int(f.stage.size())!=n || int(assigned.size())!=n)
        throw std::invalid_argument("invalid priority dependency frame");
    std::vector<int> owner(g.cells,-1);
    for(int a=0;a<n;++a) {
        int v=f.pending[a];
        if(v<0 || v>=g.cells || owner[v]>=0)throw std::invalid_argument("invalid promised occupancy");
        owner[v]=a;
    }
    std::vector<std::vector<int>> edges(n);
    for(int a=0;a<n;++a) {
        const auto* chain=assigned[a];if(!chain)continue;
        int stage=f.stage[a],v=f.pending[a];
        if(stage<int(chain->goals.size()) && v==chain->goals[stage])++stage;
        if(stage>=int(chain->goals.size()))continue;
        std::array<std::pair<float,int>,4> choices{};int count=0;
        // Include intended routes before rotation constraints, as in the
        // spatial intent policy. This is a mutation graph, never a move plan.
        for(int d=0;d<4;++d) {
            const int u=g.next[v][d];if(u<0)continue;
            choices[count++]={chain->cost(g,stage,u,d)+g.weight[v][d],d};
        }
        std::sort(choices.begin(),choices.begin()+count);
        for(int k=0;k<std::min(count,preferred_edges);++k) {
            const int b=owner[g.next[v][choices[k].second]];
            if(b<0 || b==a)continue;
            edges[a].push_back(b);edges[b].push_back(a);
        }
    }
    for(auto& neighbors:edges) {
        std::sort(neighbors.begin(),neighbors.end());
        neighbors.erase(std::unique(neighbors.begin(),neighbors.end()),neighbors.end());
    }
    return edges;
}

std::vector<int> dependency_neighborhood(const std::vector<std::vector<int>>& edges,
    int center,int limit) {
    if(center<0 || center>=int(edges.size()) || limit<1)
        throw std::invalid_argument("invalid priority mutation neighborhood");
    std::vector<unsigned char> seen(edges.size(),0);seen[center]=1;
    std::vector<int> group{center};
    for(size_t i=0;i<group.size() && int(group.size())<limit;++i)
        for(int b:edges[group[i]]) {
            if(b<0 || b>=int(edges.size()))throw std::invalid_argument("invalid priority dependency");
            if(!seen[b]){seen[b]=1;group.push_back(b);if(int(group.size())==limit)break;}
        }
    return group;
}

std::vector<int> select_rollout_elites(const std::vector<Rollout>& results,int used,
                                      int best,int limit,bool accept_equal,float decision_distance) {
    std::vector<int> parents{best};
    if(limit==1)return parents;
    std::vector<int> ranked(used);std::iota(ranked.begin(),ranked.end(),0);
    std::sort(ranked.begin(),ranked.end(),[&](int a,int b) {
        if(results[a].score!=results[b].score)return results[a].score>results[b].score;
        return accept_equal?a>b:a<b;
    });
    for(int candidate:ranked) {
        if(!results[candidate].fully_evaluated)continue;
        bool duplicate=false;
        for(int old:parents) {
            if(results[candidate].offsets==results[old].offsets){duplicate=true;break;}
            if(decision_distance>0) {
                const auto& a=results[candidate].first.pending;
                const auto& b=results[old].first.pending;
                if(a.size()!=b.size())throw std::logic_error("incompatible elite decisions");
                const int minimum=int(std::ceil(decision_distance*a.size()));
                int different=0;
                for(size_t i=0;i<a.size() && different<minimum;++i)different+=a[i]!=b[i];
                if(different<minimum){duplicate=true;break;}
            }
        }
        if(!duplicate)parents.push_back(candidate);
        if(int(parents.size())>=limit)break;
    }
    return parents;
}

void Engine::compute(SharedEnvironment* env,std::vector<Action>& plan,std::vector<int>& schedule,int forced_candidate) {
    replan_stats_=ReplanStats{};rescore_stats_=RescoreStats{};
    // Recompute the declared startup preference on every real call. It changes
    // proposals only; complete rollout scoring and the fixed work remain intact.
    priority_remaining_scale_=(!cfg.priority_remaining_steps || env->curr_timestep<cfg.priority_remaining_steps)
        ?cfg.priority_remaining_weight:0;
    const bool save_snapshot=cfg.snapshot_interval>0 && env->curr_timestep>0 &&
                             env->curr_timestep%cfg.snapshot_interval==0;
    nlohmann::json snapshot;
    if(save_snapshot)snapshot=checkpoint(*env);
    if(forced_candidate>=0 && cfg.local_trials)
        throw std::invalid_argument("forced diagnostic root requires local search off");
    // Invalidate between real steps, including task swaps/configuration changes.
    ++ranking_epoch_;
    policy_profile_active_=cfg.policy_profile && (env->curr_timestep<5 || env->curr_timestep%100==0);
    if(policy_profile_active_)policy_timings_.assign(cfg.threads,PolicyTiming{});
    std::chrono::steady_clock::time_point measured;
    if(cfg.profile)measured=std::chrono::steady_clock::now();
    std::array<double,5> phase_ms{};
    auto mark=[&](int phase) {
        if(!cfg.profile)return;
        const auto now=std::chrono::steady_clock::now();
        phase_ms[phase]+=std::chrono::duration<double,std::milli>(now-measured).count();
        measured=now;
    };
    const auto& g=*graph;const int n=env->num_of_agents;
    Frame frame;frame.loc.resize(n);frame.dir.resize(n);frame.stage.resize(n);
    for(int a=0;a<n;++a) {
        frame.loc[a]=g.from_grid[env->curr_states[a].location];frame.dir[a]=env->curr_states[a].orientation;
        if(!predicted_loc_.empty() && (predicted_loc_[a]!=frame.loc[a] || predicted_dir_[a]!=frame.dir[a]))
            throw std::runtime_error("simulator diverged from pipeline prediction");
    }
    if(pending_.empty())pending_=frame.loc;
    frame.pending=pending_;
    if(cfg.operation_depth)frame.operations=operations_;
    match(env,schedule);mark(0);
    for(auto it=chains_.begin();it!=chains_.end();) {
        if(!env->task_pool.count(it->first))it=chains_.erase(it);else ++it;
    }
    for(auto it=score_chains_.begin();it!=score_chains_.end();) {
        if(!env->task_pool.count(it->first))it=score_chains_.erase(it);else ++it;
    }
    assigned_.assign(n,nullptr);score_assigned_.assign(n,nullptr);triaged_=0;
    std::vector<double> triage_hops,triage_guided;
    double total_triage_hops=0,total_triage_guided=0;
    if(cfg.horizon>0 && cfg.triage_guided_mix>0) {
        triage_hops.resize(n);triage_guided.resize(n);
    }
    for(int a=0;a<n;++a) {
        int id=schedule[a];++age_[a];
        if(previous_task_[a]>=0 && !env->task_pool.count(previous_task_[a]))age_[a]=0;
        if(id>=0) {
            const auto& task=env->task_pool.at(id);
            frame.stage[a]=task.idx_next_loc;
            if(id==previous_task_[a] && task.idx_next_loc>previous_stage_[a])
                age_[a]=int(age_[a]*cfg.waypoint_age_retain);
            auto& chain=chains_[id];if(!chain)chain=std::make_shared<Chain>(g,task,cfg.cost_cache);
            assigned_[a]=chain.get();
            if(score_graph_) {
                auto& score_chain=score_chains_[id];
                if(!score_chain)score_chain=std::make_shared<Chain>(*score_graph_,task,cfg.cost_cache);
                score_assigned_[a]=score_chain.get();
            }
            if(cfg.horizon>0) {
                double remaining=0;int p=frame.loc[a];
                for(int k=task.idx_next_loc;k<int(chain->goals.size());++k) {
                    remaining+=g.hop(chain->goals[k],p);p=chain->goals[k];
                }
                if(cfg.triage_guided_mix>0) {
                    triage_hops[a]=remaining;total_triage_hops+=remaining;
                    triage_guided[a]=chain->cost(g,frame.stage[a],frame.loc[a],frame.dir[a]);
                    total_triage_guided+=triage_guided[a];
                } else {
                    double steps_per_cell=travel_steps_per_cell();
                    if(remaining*steps_per_cell*cfg.triage_scale>cfg.horizon-env->curr_timestep) {
                        assigned_[a]=nullptr;++triaged_;
                    }
                }
            }
        }
        previous_task_[a]=id;previous_stage_[a]=frame.stage[a];
    }
    if(!triage_hops.empty()) {
        // Keep the total estimated work equal to the hop-based baseline, while
        // allowing direction and remaining turns to redistribute it across tasks.
        // These are currently assigned, visible chains only; no future stream.
        const double normalization=total_triage_guided>0?total_triage_hops/total_triage_guided:0;
        const double steps_per_cell=travel_steps_per_cell();
        for(int a=0;a<n;++a)if(assigned_[a]) {
            const double remaining=(1-cfg.triage_guided_mix)*triage_hops[a]+
                cfg.triage_guided_mix*normalization*triage_guided[a];
            if(remaining*steps_per_cell*cfg.triage_scale>cfg.horizon-env->curr_timestep) {
                assigned_[a]=nullptr;++triaged_;
            }
        }
    }
    // Remember actual deferred task IDs, not robot flags: a completed or
    // reassigned task must never inherit an old admission credit. This is a
    // one-step-delayed admission estimate; opened assignments remain locked.
    suppressed_tasks_.clear();
    if(cfg.active_cap_triage_credit>0) {
        suppressed_tasks_.assign(n,-1);
        for(int a=0;a<n;++a)if(schedule[a]>=0 && !assigned_[a])suppressed_tasks_[a]=schedule[a];
    }
    score_weights_.clear();progress_normalization_=1;
    if(cfg.progress_softcap>0) {
        // Keep the mean initial marginal value equal to one. This preserves
        // the local distance/dispersion scale while giving nearer completions
        // smoothly increasing value along each simulated future.
        double slopes=0;int active=0;
        for(int a=0;a<n;++a)if(assigned_[a]) {
            const double remaining=assigned_[a]->cost(g,frame.stage[a],frame.loc[a],frame.dir[a]);
            const double ratio=cfg.progress_softcap/(cfg.progress_softcap+remaining);
            slopes+=ratio*ratio;++active;
        }
        if(active)progress_normalization_=active/slopes;
    }
    // A declared startup window returns to equal progress weights at its exact
    // boundary. Rebuild every step so previous weighted scores cannot leak.
    if(cfg.score_rank_power>0 && (!cfg.score_rank_steps || env->curr_timestep<cfg.score_rank_steps)) {
        std::vector<float> remaining(n,-1);
        for(int a=0;a<n;++a)if(assigned_[a])
            remaining[a]=assigned_[a]->cost(g,frame.stage[a],frame.loc[a],frame.dir[a]);
        score_weights_=rank_progress_weights(remaining,cfg.score_rank_power);
    }
    if(cfg.rollout_match) {
        frame.active_chains=assigned_;if(score_graph_)frame.plain_chains=score_assigned_;
        future_tasks_.clear();future_plain_.clear();future_lengths_.clear();
        std::unordered_set<int> used(schedule.begin(),schedule.end());
        std::vector<int> ids;
        for(const auto& item:env->task_pool)if(!used.count(item.first) && item.second.idx_next_loc==0)
            ids.push_back(item.first);
        std::sort(ids.begin(),ids.end());
        for(int id:ids) {
            const auto& task=env->task_pool.at(id);
            auto& chain=chains_[id];if(!chain)chain=std::make_shared<Chain>(g,task,cfg.cost_cache);
            future_tasks_.push_back(chain.get());
            float length=0;
            for(int k=1;k<int(chain->goals.size());++k)length+=g.hop(chain->goals[k],chain->goals[k-1]);
            future_lengths_.push_back(length);
            if(score_graph_) {
                auto& plain=score_chains_[id];
                if(!plain)plain=std::make_shared<Chain>(*score_graph_,task,cfg.cost_cache);
                future_plain_.push_back(plain.get());
            }
        }
        frame.free_tasks.assign(future_tasks_.size(),1);
    }
    prepare_shared_rankings(env->curr_timestep);
    const auto mutation_dependencies=cfg.blocker_mutation_size>0?
        priority_dependencies(g,frame,assigned_,cfg.blocker_mutation_edges):std::vector<std::vector<int>>{};
    mark(1);
    frame.age=age_;
    if(cfg.reverse_penalty>0)frame.last_actions=last_actions_;
    if(cfg.window) {
        if(forced_candidate>=0)throw std::invalid_argument("forced root is unavailable for windowed search");
        window_plan(frame,*env,plan);
        record_travel(plan);
        return;
    }
    // Initialization of the first task pool has extra matching/cost work.
    // A declared first-step budget can reserve room for that work. Every
    // configured rollout is still completed; elapsed time never changes K.
    const int futures=env->curr_timestep==0 && cfg.first_futures>0?cfg.first_futures:cfg.futures;
    const int roots=search_roots(cfg,futures);
    std::vector<std::vector<float>> offsets(roots);
    std::vector<unsigned char> root_early(roots,0);
    // Independent per-step streams preserve candidate prefixes across K and
    // keep local-refinement draws independent of the number of global futures.
    auto mix=[](uint64_t x) {
        x+=0x9e3779b97f4a7c15ULL;x=(x^(x>>30))*0xbf58476d1ce4e5b9ULL;
        x=(x^(x>>27))*0x94d049bb133111ebULL;return x^(x>>31);
    };
    std::mt19937 step_random,local_random;
    if(cfg.random_by_step) {
        uint64_t key=(uint64_t(uint32_t(cfg.seed))<<32)|uint32_t(env->curr_timestep);
        step_random.seed(uint32_t(mix(key)));local_random.seed(uint32_t(mix(key^0xd1b54a32d192ed03ULL)));
    }
    auto& global_rng=cfg.random_by_step?step_random:rng_;
    auto& local_rng=cfg.random_by_step?local_random:rng_;
    std::uniform_real_distribution<float> unit(0,1),noise(-cfg.noise,cfg.noise);
    // Common continuation draws compare roots under the same future priority
    // perturbations. A separate stream leaves root and local draws unchanged.
    // Branch zero keeps the original constant-offset rollout.
    std::vector<Continuation> continuations(cfg.continuations-1,Continuation(cfg.depth));
    if(!continuations.empty()) {
        uint64_t key=(uint64_t(uint32_t(cfg.seed))<<32)|uint32_t(env->curr_timestep);
        std::mt19937 future_random(uint32_t(mix(key^0x94d049bb133111ebULL)));
        for(size_t b=0;b<continuations.size();++b) {
            // A retained vector can supply correlated future priorities. It is
            // only a control proposal: simulate every action and cost afresh.
            // Keep mutation masks and random draws identical when disabled;
            // before history exists, fall back to the original random futures.
            const auto* elite=cfg.future_elite_blend>0 && !past_offsets_.empty()?
                &past_offsets_[b%past_offsets_.size()]:nullptr;
            for(int t=cfg.continuation_start;t<cfg.depth;++t)
                for(int a=0;a<n;++a)if(unit(future_random)<cfg.future_mutation) {
                    float value=noise(future_random);
                    if(elite) value=cfg.future_elite_blend==1?(*elite)[a]:
                        (1-cfg.future_elite_blend)*value+cfg.future_elite_blend*(*elite)[a];
                    continuations[b][t].push_back({a,value});
                }
        }
    }
    std::vector<Rollout> results(roots);
    const bool diagnose_branches=cfg.branch_diagnostics>0 && env->curr_timestep%cfg.branch_diagnostics==0;
    std::vector<std::vector<double>> branch_scores(diagnose_branches?roots:0);
    std::vector<std::exception_ptr> errors(roots);
    int best=0;float generation_mutation=cfg.mutation;
    for(int generation=0;generation<cfg.generations;++generation) {
        const int begin=generation*roots/cfg.generations;
        const int end=(generation+1)*roots/cfg.generations;
        // Later generations can preserve more of their evaluated parents.
        // Full random restarts keep their original amplitude and frequency.
        if(generation && cfg.mutation_decay!=1)generation_mutation*=cfg.mutation_decay;
        // Keep the total number of complete rollouts fixed. Later batches
        // refine this step's incumbent; one generation preserves the original
        // random draws, candidate order, and equal-score acceptance behavior.
        std::vector<int> parents;
        if(generation && cfg.elites>1) {
            // Retain several distinct evaluated priority vectors instead of
            // making every later mutation descend from one incumbent. Keeping
            // the incumbent first preserves the same unmodified anchor.
            parents=select_rollout_elites(results,begin,best,cfg.elites,cfg.accept_equal,cfg.elite_decision_distance);
        }
        int exploitation=0,history_children=0,local_mutations=0;
        for(int k=begin;k<end;++k) {
            const bool global=k>begin && cfg.restart_period>0 && k%cfg.restart_period==0;
            int parent=best;
            if(!parents.empty()) {
                // Fully random candidates have no inherited values. Do not
                // let their every-fourth positions starve one elite of trials.
                parent=parents[global?0:exploitation++%parents.size()];
            }
            // An optional alternative adds only legal immediate forward moves
            // to this root. Its continuation returns to the ordinary pipeline.
            // Keep a later generation's incumbent mode with its unchanged anchor.
            root_early[k]=cfg.early_root_period>0 &&
                (cfg.early_root_period==1 || k%cfg.early_root_period==1);
            if(generation && k==begin)root_early[k]=results[best].early_moves;
            offsets[k]=generation?results[parent].offsets:best_offsets_;
            bool history_anchor=false;
            if(!generation && cfg.persist_elites>1 && !past_offsets_.empty() &&
               !global) {
                // Try each retained vector unchanged once, then mutate parents
                // in rotation. Declared fully random restarts are retained.
                // All candidates are evaluated again from the current frame.
                history_anchor=history_children<int(past_offsets_.size());
                offsets[k]=past_offsets_[history_children++%past_offsets_.size()];
            }
            if(k>begin && !history_anchor) {
                if(cfg.blocker_mutation_size>0 && !global &&
                   local_mutations++%cfg.blocker_mutation_period==0) {
                    const int center=std::uniform_int_distribution<int>(0,n-1)(global_rng);
                    for(int a:dependency_neighborhood(mutation_dependencies,center,cfg.blocker_mutation_size))
                        offsets[k][a]=noise(global_rng);
                    continue;
                }
                // Keep the declared restart share global. Other futures can
                // change one spatial neighborhood while preserving its context.
                int center=-1;
                if(cfg.mutation_radius>0 && !global)
                    center=g.to_grid[frame.loc[std::uniform_int_distribution<int>(0,n-1)(global_rng)]];
                for(int a=0;a<n;++a) {
                    if(center>=0) {
                        int p=g.to_grid[frame.loc[a]];
                        if(std::abs(p/g.cols-center/g.cols)>cfg.mutation_radius ||
                           std::abs(p%g.cols-center%g.cols)>cfg.mutation_radius)continue;
                    }
                    if(global || unit(global_rng)<generation_mutation)offsets[k][a]=noise(global_rng);
                }
            }
        }
        mark(2);
        if(!cfg.screen_branches) {
            #pragma omp parallel for num_threads(cfg.threads) schedule(static)
            for(int k=begin;k<end;++k) {
                try { results[k]=evaluate(frame,offsets[k],continuations,!cfg.cycle_portfolio || k%2==1,
                                         diagnose_branches?&branch_scores[k]:nullptr,nullptr,root_early[k]); }
                catch(...) { errors[k]=std::current_exception(); }
            }
        } else {
            std::vector<ScreenedRollout> screened(end-begin);
            #pragma omp parallel for num_threads(cfg.threads) schedule(static)
            for(int k=begin;k<end;++k) {
                try { evaluate_until(frame,offsets[k],continuations,!cfg.cycle_portfolio || k%2==1,
                                     cfg.screen_branches,screened[k-begin],root_early[k]); }
                catch(...) { errors[k]=std::current_exception(); }
            }
            for(int k=begin;k<end;++k)if(errors[k])std::rethrow_exception(errors[k]);
            std::vector<int> survivors(end-begin);std::iota(survivors.begin(),survivors.end(),begin);
            std::sort(survivors.begin(),survivors.end(),[&](int a,int b) {
                const double x=screened[a-begin].result.score,y=screened[b-begin].result.score;
                if(x!=y)return x>y;
                return cfg.accept_equal?a>b:a<b;
            });
            survivors.resize((end-begin)/cfg.screen_keep);
            // Keep the unmodified incumbent anchor in every generation.
            if(std::find(survivors.begin(),survivors.end(),begin)==survivors.end())survivors.back()=begin;
            std::sort(survivors.begin(),survivors.end());
            #pragma omp parallel for num_threads(cfg.threads) schedule(static)
            for(size_t j=0;j<survivors.size();++j) {
                int k=survivors[j];
                try { evaluate_until(frame,offsets[k],continuations,!cfg.cycle_portfolio || k%2==1,
                                     cfg.continuations,screened[k-begin],root_early[k]); }
                catch(...) { errors[k]=std::current_exception(); }
            }
            for(int k=begin;k<end;++k)results[k]=std::move(screened[k-begin].result);
        }
        mark(3);
        for(int k=begin;k<end;++k) {
            if(errors[k])std::rethrow_exception(errors[k]);
            if(!results[k].fully_evaluated)continue;
            if(results[k].score>results[best].score+1e-7 ||
               (cfg.accept_equal && results[k].score>=results[best].score-1e-7))best=k;
        }
        if(diagnose_branches) {
            // Buffer one generation: stderr is unbuffered, and one write per
            // number would make this observational diagnostic dominate a step.
            // There are still no extra draws, rollouts or selection changes.
            std::ostringstream diagnostic;diagnostic.precision(17);
            for(int k=begin;k<end;++k) {
                diagnostic<<"R05_BRANCH_SCORE {\"t\":"<<env->curr_timestep
                          <<",\"generation\":"<<generation<<",\"candidate\":"<<k
                          <<",\"value\":"<<results[k].score<<",\"scores\":[";
                for(size_t b=0;b<branch_scores[k].size();++b) {
                    if(b)diagnostic<<',';
                    diagnostic<<branch_scores[k][b];
                }
                diagnostic<<"]}\n";
            }
            const auto text=diagnostic.str();
            std::fwrite(text.data(),1,text.size(),stderr);
        }
    }
    int evaluated=0;
    for(const auto& result:results)evaluated+=result.evaluated_branches;
    if(evaluated!=futures)throw std::runtime_error("fixed search work count changed");
    // Local refinement also spends complete continuation groups, without
    // exceeding its existing rollout allowance. Unused remainders stay unused.
    for(int trial=0;trial<cfg.local_trials/cfg.continuations;++trial) {
        auto local=results[best].offsets;
        int center=std::uniform_int_distribution<int>(0,n-1)(local_rng);
        int p=g.to_grid[frame.loc[center]];
        for(int a=0;a<n;++a) {
            int q=g.to_grid[frame.loc[a]];
            if(std::abs(p/g.cols-q/g.cols)<=2 && std::abs(p%g.cols-q%g.cols)<=2)local[a]=noise(local_rng);
        }
        Rollout candidate=evaluate(frame,local,continuations,results[best].cycle_moves,nullptr,nullptr,results[best].early_moves);
        if(candidate.score>results[best].score+1e-7 ||
           (cfg.accept_equal && candidate.score>=results[best].score-1e-7))results[best]=std::move(candidate);
    }
    if(cfg.component_trials) {
        // Freeze several distinct first decisions from the completed portfolio.
        // Whole connected motion groups can be substituted independently; the
        // subsequent rollout still evaluates their future interactions jointly.
        std::vector<int> ranking;
        for(int k=0;k<roots;++k)if(results[k].fully_evaluated)ranking.push_back(k);
        std::sort(ranking.begin(),ranking.end(),[&](int a,int b) {
            if(results[a].score!=results[b].score)return results[a].score>results[b].score;
            return cfg.accept_equal?a>b:a<b;
        });
        std::vector<Rollout> donors{results[best]};int next_rank=0;
        for(int rank=0;rank<int(ranking.size()) && int(donors.size())<cfg.component_parents;++rank) {
            if(rank<next_rank)continue;
            const auto& candidate=results[ranking[rank]];bool duplicate=false;
            for(const auto& old:donors)if(candidate.actions==old.actions &&
                                        candidate.first.pending==old.first.pending)duplicate=true;
            if(duplicate)continue;
            donors.push_back(candidate);next_rank=next_rank?next_rank*2:2;
        }
        uint64_t hybrid_expansions=0;int accepted=0,component_evaluations=0,available=0;
        for(int round=0;round<cfg.component_rounds;++round) {
            const auto anchor=results[best];
            struct Proposal {int donor;std::vector<int> agents;};
            std::vector<Proposal> proposals;
            for(int donor=0;donor<int(donors.size());++donor)
                for(auto group:decision_components(g,anchor,donors[donor]))
                    if(int(group.size())>=cfg.component_min_agents)proposals.push_back({donor,std::move(group)});
            available+=int(proposals.size());
            std::shuffle(proposals.begin(),proposals.end(),local_rng);
            std::vector<Rollout> hybrids(cfg.component_trials),trials(cfg.component_trials);
            std::vector<std::exception_ptr> trial_errors(cfg.component_trials);
            for(int trial=0;trial<cfg.component_trials;++trial) {
                auto& hybrid=hybrids[trial];hybrid=anchor;
                if(!proposals.empty()) {
                    const auto& proposal=proposals[trial%proposals.size()];const auto& donor=donors[proposal.donor];
                    for(int a:proposal.agents) {
                        hybrid.first.dir[a]=donor.first.dir[a];hybrid.first.pending[a]=donor.first.pending[a];
                        hybrid.actions[a]=donor.actions[a];hybrid.offsets[a]=donor.offsets[a];
                        if(!hybrid.first.last_actions.empty())hybrid.first.last_actions[a]=donor.first.last_actions[a];
                    }
                    if(cfg.reverse_penalty>0) {
                        hybrid.first.reverse_turns=0;
                        for(int a=0;a<n;++a)hybrid.first.reverse_turns+=
                            (hybrid.actions[a]==CR && frame.last_actions[a]==CCR) ||
                            (hybrid.actions[a]==CCR && frame.last_actions[a]==CR);
                    }
                }
                certify(g,frame.loc,hybrid.first.loc);certify(g,hybrid.first.loc,hybrid.first.pending);
            }
            #pragma omp parallel for num_threads(cfg.threads) schedule(static)
            for(int trial=0;trial<cfg.component_trials;++trial) {
                try {trials[trial]=evaluate(frame,hybrids[trial].offsets,continuations,
                                           hybrids[trial].cycle_moves,nullptr,&hybrids[trial]);}
                catch(...) {trial_errors[trial]=std::current_exception();}
            }
            for(int trial=0;trial<cfg.component_trials;++trial) {
                if(trial_errors[trial])std::rethrow_exception(trial_errors[trial]);
                auto& candidate=trials[trial];component_evaluations+=candidate.evaluated_branches;
                hybrid_expansions+=candidate.expansions;
                if(candidate.score>results[best].score+1e-7 ||
                   (cfg.accept_equal && candidate.score>=results[best].score-1e-7)) {
                    ++accepted;results[best]=std::move(candidate);
                }
            }
        }
        if(component_evaluations!=cfg.component_trials*cfg.component_rounds*cfg.continuations)
            throw std::runtime_error("motion-component search changed its fixed work count");
        if(env->curr_timestep%100==0)std::fprintf(stderr,
            "R05_COMPONENT t=%d rounds=%d trials=%d branches=%d donors=%zu proposals=%d accepted=%d expansions=%llu\n",
            env->curr_timestep,cfg.component_rounds,cfg.component_trials,component_evaluations,
            donors.size(),available,accepted,(unsigned long long)hybrid_expansions);
    }
    if(cfg.rescore_roots)best=rescore_candidates(*env,frame,results,best);
    if(cfg.replan_roots && env->curr_timestep>=cfg.replan_start)
        best=rank_replanned(*env,schedule,results,best);
    if(save_snapshot) {
        // Observe only already-completed roots. Powers-of-two ranks cover both
        // close alternatives and a wider score range; duplicate first decisions
        // carry no extra causal information. No search draws or scores change.
        std::vector<int> ranked;
        for(int k=0;k<roots;++k)if(results[k].fully_evaluated)ranked.push_back(k);
        std::sort(ranked.begin(),ranked.end(),[&](int a,int b) {
            if(results[a].score!=results[b].score)return results[a].score>results[b].score;
            return cfg.accept_equal?a>b:a<b;
        });
        std::vector<int> selected{best};
        int target=0;
        for(int rank=0;rank<int(ranked.size()) && int(selected.size())<cfg.snapshot_candidates;++rank) {
            if(rank<target)continue;
            int k=ranked[rank];bool duplicate=false;
            for(int old:selected)if(results[k].actions==results[old].actions &&
                                    results[k].first.pending==results[old].first.pending)duplicate=true;
            if(duplicate)continue;
            selected.push_back(k);target=target?target*2:2;
        }
        snapshot["selected_candidate"]=best;
        snapshot["candidates"]=nlohmann::json::array();
        for(int k:selected)snapshot["candidates"].push_back({
            {"candidate",k},{"score",results[k].score},{"actions",results[k].actions},
            {"pending",results[k].first.pending},{"schedule",schedule},
            {"rank",std::find(ranked.begin(),ranked.end(),k)-ranked.begin()}});
        std::filesystem::create_directories(cfg.snapshot_directory);
        std::string path=cfg.snapshot_directory+"/step-"+std::to_string(env->curr_timestep)+".json";
        std::ofstream output(path);output<<snapshot.dump()<<'\n';output.close();
        if(!output)throw std::runtime_error("could not write decision snapshot");
    }
    if(forced_candidate>=0) {
        if(forced_candidate>=roots || !results[forced_candidate].fully_evaluated)
            throw std::invalid_argument("forced root is not a fully evaluated candidate");
        best=forced_candidate;
    }
    if(cfg.persist_elites>1) {
        past_offsets_.clear();
        for(int parent:select_rollout_elites(results,roots,best,cfg.persist_elites,cfg.accept_equal,cfg.elite_decision_distance))
            past_offsets_.push_back(results[parent].offsets);
    }
    auto& selected=results[best];
    certify(g,frame.loc,selected.first.loc);
    certify(g,selected.first.loc,selected.first.pending);
    if(cfg.operation_depth) {
        // Certify the selected suffix, including its terminal wait, before any
        // real action is returned. Failed revisions always retain this fallback.
        std::vector<int> previous=selected.first.loc,next(n);
        for(int t=0;t<OperationModel::horizon;++t) {
            for(int a=0;a<n;++a)next[a]=operation_model_->path(
                selected.first.loc[a]*4+selected.first.dir[a],selected.first.operations[a])[t]/4;
            certify(g,previous,next);previous=next;
        }
        operations_=selected.first.operations;
    }
    plan=selected.actions;pending_=selected.first.pending;best_offsets_=selected.offsets;
    predicted_loc_=selected.first.loc;predicted_dir_=selected.first.dir;
    if(cfg.reverse_penalty>0)last_actions_=selected.actions;
    record_travel(plan);
    mark(4);
    if(cfg.profile && (env->curr_timestep<5 || env->curr_timestep%100==0))
        std::fprintf(stderr,"R05_PROFILE t=%d assignment_ms=%.3f task_cost_ms=%.3f candidates_ms=%.3f lookahead_ms=%.3f final_ms=%.3f\n",
                     env->curr_timestep,phase_ms[0],phase_ms[1],phase_ms[2],phase_ms[3],phase_ms[4]);
    if(policy_profile_active_) {
        PolicyTiming total;
        for(const auto& timing:policy_timings_) {
            total.calls+=timing.calls;total.samples+=timing.samples;
            for(int k=0;k<7;++k)total.nanoseconds[k]+=timing.nanoseconds[k];
        }
        // Sampled elapsed worker time, not isolated CPU utilization. Separate
        // columns identify expensive policy phases without timing every call.
        std::fprintf(stderr,"R05_POLICY_PROFILE t=%d calls=%llu samples=%llu setup_ns=%llu candidates_ns=%llu order_ns=%llu intent_ns=%llu kinematic_ns=%llu cycles_ns=%llu actions_ns=%llu\n",
            env->curr_timestep,(unsigned long long)total.calls,(unsigned long long)total.samples,
            (unsigned long long)total.nanoseconds[0],(unsigned long long)total.nanoseconds[1],
            (unsigned long long)total.nanoseconds[2],(unsigned long long)total.nanoseconds[3],
            (unsigned long long)total.nanoseconds[4],(unsigned long long)total.nanoseconds[5],
            (unsigned long long)total.nanoseconds[6]);
    }
    if(!quiet_ && env->curr_timestep%100==0) {
        if(cfg.screen_branches)
            std::fprintf(stderr,"R05_SCREEN t=%d roots=%d finalists=%d branch_evaluations=%d screen_branches=%d full_branches=%d\n",
                         env->curr_timestep,roots,roots/cfg.screen_keep,evaluated,cfg.screen_branches,cfg.continuations);
        int moves=std::count(plan.begin(),plan.end(),FW);uint64_t expanded=0;
        for(const auto& r:results)expanded+=r.expansions;
        std::fprintf(stderr,"R05_STEP t=%d moves=%d score=%.3f expansions=%llu K=%d triaged=%d early_root=%d\n",
                     env->curr_timestep,moves,selected.score,(unsigned long long)expanded,futures,triaged_,int(selected.early_moves));
    }
}
}
