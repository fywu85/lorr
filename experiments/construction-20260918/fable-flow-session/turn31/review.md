get of 3. A budget of 1 reproduces today's rule, so current regressions stay meaningful.
  - By the token arithmetic, 3 is the smallest budget at which tying needs about 15 realized steps per event instead of 43 or more.
  - Clear commitments on retarget, as the existing passes do.

- **Guards.**
  - Add a third selector read in `tricks::options`: 0 off, 1 shadow, 2 active.
  - Print it in `[CGAR_TRICK_COMPONENTS]` together with quota and budget, and have the benchmark expect it.
  - It is the only path. `CGAR_REASSIGN` and `CGAR_REASSIGN_POOL` stay rejected under `--trick`.

- **Protections, unchanged from `unopened_candidates`.**
  - Started tasks, and a robot standing on its pickup cell.
  - The primary and the next-oldest ticket.
  - Recovery robots and transaction cells.
  - Parked, locked and non-core robots, and robots with an outstanding commitment.
  - Fair tasks when short is 0.
  - No task is deleted. Released tasks keep their metadata.

- **Fixed work.**
  - Quota 256 holders per tick under the 5 s development limit. About 2,600 robots are empty at any time in the short arms (`empty_steps/5000`), so each holder gets a look roughly every ten ticks.
  - That is four more 64-field batches. I have no timing for it, so measure it.
  - No partial results. An overrun fails the run, as now.
  - Fields are transient, so RSS should not move. Confirm it.

- **Arms.**
  - lanes+short+retarget and lanes+retarget, on seeds 0 and 2.
  - Compare against your existing lanes+short and lanes rows. That is four runs.

- **Acceptance.**
  - Outcome: tasks above lanes-only on both seeds.
  - Mechanism, from the replay and not from logged savings:
    - realized empty steps per completed task;
    - the ratio of realized empty-step change to summed logged saving;
    - retargets per assignment, and budget exhaustion counts;
    - completed chain and loaded means;
    - unpicked age tails;
    - maximum decision time.

- **Strongest reason it fails.**
  - Each fresh task is visible to only the quota, about a tenth of holders per tick, and there is no within-tick cascade. KK and NMS re-match every unopened holder every tick.
  - Second, estimated savings may not survive as realized time. A retarget clears a commitment, can add turns, and resets ticket and stall.

- **What the acceptance rule does not give you.**
  - It lowers one robot's estimated cost at one instant.
  - It guarantees neither less realized empty time nor more throughput.
  - Liveness rests on the unchanged protections and the finite budget, not on the cost decrease.
