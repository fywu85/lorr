# Moving fixed search work into parallel regional repair

The confirmed reference uses one global worker with a4million candidate limit,
then two rounds of four regions, each with25,000 fixed repair attempts. Fable turn15
suggested reducing serial global work and spending that time on additional regional
rounds. Compare the reference with3million/four rounds and2million/four rounds.
All arms reserve four physical cores and use the same frozen V48 binary,64 complete
pickup fields,1000ppm regional temperature and read-only peak auditing. Next-errand
scoring stays off. Each decision completes its fixed work or fails explicitly.

The global candidate limit is a work limit, not a guaranteed exact number of
candidates: construction can itself exceed it. More regional rounds can also add
serial candidate-partition preparation. The200-step screen checks cold-entry timing,
full action validity, memory, exact reference trajectory and feature activation.
It does not rank throughput. If feasible, compare full5,000-step seeds0/2; a useful
candidate then needs the remaining four seeds before promotion.

Four rounds can revisit the same geometric partition, but plans, boundary-crossing
robots and RNG state have changed. Repeating a partition does not make the round
inert. The policy derives partitions generically from dimensions; it contains no
map identity or manually specified aisle pattern. Extra internal score gain is not
itself evidence of more completed tasks.

All22 frozen source/test hashes match [8ba2484](https://github.com/fywu85/lorr/commit/8ba2484dd5f1b9bfc719f5f1e8f4533d7543a7bd).
[Profiles](regional-work-balance-variants.json), [Fable review](fable-flow-session/turn15/).

Screen8899177 was submitted2026-09-19 at22:44UTC: three simultaneous profiles,
four isolated physical cores each,12cores/48GiB aggregate. Raw
runs/cgar-regional-work-balance-screen-v48-20260919. Full throughput is pending.

Screen8899177 is complete and independently verified: all22 source/test hashes,
frozen binary, four disjoint physical cores per case, no CPU quota, complete fixed
repair counts and exact reference trajectory. Maxima962.235/969.932/993.561ms,
peakRSS5.300GB. The2M candidate has only6.4ms observed margin; a later timeout remains
possible. [Screen checks](results/regional-work-balance-screen-v48/equivalence.json).

Full matrix8899182/analysis8899183 started2026-09-19T22:50:01UTC. Six simultaneous
runs (seeds0/2 xthreeprofiles), four physical cores each,24cores/96GiB aggregate,
32decimalGB cap per process. Raw runs/cgar-regional-work-balance-full-v48-20260919,
future archive results/regional-work-balance-full-v48. Compare with
compare_regional.py --variable CGAR_TEMPORAL_CANDIDATE_LIMIT --control4000000.

The2M/four-round seed0 full case fails explicitly at timestep36, elapsed1000.580ms,
stage planning_complete. The other five cases continue. No partial result is used;
this arm cannot meet the acceptance criterion as measured. Use compare_failed_regional.py
for the completed matrix so the failure remains visible alongside verified valid runs.
