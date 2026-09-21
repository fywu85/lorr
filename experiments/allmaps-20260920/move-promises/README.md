# One-action CGAR motion commitments

Declared before benchmarking. This general opt-in transfers only the movement
commitment idea from the separately developed pipelined policy. It continues to
use CGAR primary selection, protected recovery, certificates, scheduler and
complete temporal construction/repair. It does not substitute PILOT's planner.

Earlier after-turn promises required the old four-action suffix to fit current
candidates and reservations. This version stores the next occupied cell and
replans every later action. Mode1 carries forward moves; mode2 also carries
stationary cells but allows new rotations there. Both require exact observed
pose and the same task and goal. Idle robots can retain displacement promises.
Current CGAR protected first actions override history. Replacing a conflicting
old promise with its safe seed recursively resets dependent old promises; an
independent component can survive. Every ordinary fallback and every searchable
candidate then respects the retained one-action occupancy. Full joint seeds and
returned plans are checked. All declared work completes or raises timeout.

Default0 preserves old whole trajectories and RNG streams. Rolling-window repair
and legacy history are rejected. Common-future scoring, regional repair, paid
chain scoring and bounded branch transactions compose without breaking the
current action constraint. The actual objective and task admission remain fixed.

Tests exercise a fully occupied four-agent cycle; dependency resets from a new
protection, task, goal or permission; independent idle components; stale pose and
time; deadline propagation; complete production action validity; serial/parallel
identity; default identity; repeated service stops; and CGAR primary, pocket,
capacity and recovery behavior. No throughput claim before full GRID regression
and frozen-source qualification.

Declared full1000/2000-step matrices use RANDOM04 seed0 control2059 and RANDOM05
seed10 control3065, each against modes1and2 alone. Existing selected explicit
`--trick RANDOM-04/05` profiles are retained. Four bound physical cores per case,
strict1000ms complete entry,32decimalGB and node-local log spool. Any positive
signal needs additional matched seeds. PILOT source provenance is recorded in
../pilot-transfer/source-review-20260921-1112.json; independent jobs and outputs
remain untouched.

First full RANDOM04 result: control2059, forward-only2190, forward/wait2237.
All three pass strict1s and full1000-step verification; control repeats the entire
prior selected trajectory. Mode2 improves178tasks(+8.65%) on seed0, maximum
302.54ms. This is a general optional commitment mechanism composed with the
existing explicitly gated map tricks. Replication of all three arms on seeds2/4
is declared before seeing those outcomes. No mean-gain claim yet.

A separate mode2 forecast-depth comparison is declared on seed0: currentH20,
H15,H25,and no common futures. This tests an interaction introduced by carrying
movement between actual calls; earlier depth tests had no such commitments.
The no-futures arm removes only future settings and its otherwise unused chain
oracle settings. Fixed temporal construction, repair, priorities, scheduling and
all map tricks remain identical. The selected2237control must repeat in full.

Depth comparison: H20control2237, H15=2212, H25=2234, no-futures2239.
All four full runs valid; the control repeats its whole selected trajectory.
No-futures is only two tasks better on this seed, with278.68ms maximum entry time.
It is recorded under the user's selected-seed rule, without a robust-gain claim.
The H20/no-futures pair is declared for seeds2/4. Separately, on seed0 the new
no-futures commitment profile is compared with exact terminal-chain scoring and
paid-plus-chain scoring. These previously lost without motion commitments; this
one bounded interaction test checks whether execution consistency changes that.

The original commitment comparison now replicates across seeds0/2/4. Controls:
2059/1901/1843. Forward-only:2190/2172/2110 (+11.53%aggregate). Forward/wait:
2237/2298/2215 (+16.32%aggregate). All six new runs are valid, both modes improve
all three seeds, and the selected maximum becomes2298 atseed2 (306.43ms max).
The new best profile retains H20; the two-task no-futures seed0 gain is still only
a single-seed observation. See random04-three-seed-summary.json.

RANDOM05 seed10 full results: control3065, forward-only3391, **forward/wait3571**.
All three pass full2000-step strict1s verification; control repeats its prior whole
uncapped trajectory. The selected run peaks at712.32ms and clears the3355target
(+10%over historical max(NMS,KK)=3050). Its3571score is17.08%above that published
baseline, not a matched competitor result. It uses no new task-cap trick; existing
static guidance, horizon admission and31-robot capacity parking remain. Three-arm
replication on seeds0/2 is declared. A separate two-arm seed10 comparison tests
whether composing mode2 with cap700 improves3571. No other factors change.

RANDOM04 H20/no-futures replication:2237/2298/2215 versus2239/2219/2270
on seeds0/2/4. Removing futures loses22 aggregate tasks; retain the selected H20
profile. Terminal-chain and paid-chain interaction arms also lose:2231/2044
versus2239control. All controls repeat exact prior trajectories; all full runs
pass strict1s. See horizon-replication-results.json and chain-results.json.
