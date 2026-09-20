# Four global workers followed by regional repair

Compare the confirmed regional profile with one versus four independent global
searches, all four global searches completing before the best complete result
enters the unchanged four-region/two-round repair. Each global worker retains the
4M candidate quota; this increases prescribed work, not an anytime deadline.
The calling thread participates, and search states and RNGs are private while
geometry/candidate costs are shared. Any failed worker fails the entire entry.
No map identity, unfair task preference or extra assignment feature is enabled.

The earlier global-worker comparisons preceded the regional policy. This tests
whether the combination helps; higher internal search scores alone are not a
throughput result. Changing worker count also consumes more seeds from the common
temporal RNG and shifts later regional seeds. It is a whole-policy comparison,
not an isolated causal effect of the best initial solution or an equal-work speedup.

Use the V60 executable/source4964309, four reserved physical cores per process,
shared-host5s complete decisions and32GB RSS. First200 steps validate feasibility,
exact native control, all prescribed worker participation and unchanged regional
quotas. Then full seeds0/2; a promising pair requires the other four seeds before
promotion. Report totals, final1000steps, age tails, empty work, latency/CPU/RSS.

The screen8899342/analysis8899343 passes: exact1-worker control, all200/800 prescribed
complete worker runs accounted for, unchanged regional quotas, valid actions and
32GB RSS. Maximum entry times are927.247ms/936.475ms. Full8899344 now compares both
policies at seeds0/2 on shared research52 with16boundphysicalcores,4percase. The
startup totals2888/2912 are feasibility evidence only. Full scores remain pending.
