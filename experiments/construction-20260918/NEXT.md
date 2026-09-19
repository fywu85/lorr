# Continuing warehouse work

Goal: at least 152,981 completed tasks over all 5,000 steps, repeatably over six
seeds, complete one-second decisions, and measured process RSS below
32,000,000,000 bytes. No map-name branches, aisle templates, supplied weights or
map-specific fleet caps. The local leader uses more cores and exceeds the memory
target; retain that difference when comparing. The active goal is not complete.

## Established full results

- **New strongest seed 0: 122,896**, using executed-flow strength 1, margin 50,
  freeze 128, minimum 8 samples and fixed 4M candidate work. No warm start or
  regions. All 5,000 entries pass, max 0.881398 seconds, RSS 12.106 GB, one core.
  Its paired 4M control is 109,244 (+12.50%); earlier regional best is 112,164.
  Final-window count improves 22,456 to 25,254 and task-age p90 850 to 743.
  Confirmation seeds 1/2 reach 122,195 and 70,171. The three-seed mean is only
  105,087.3, with a severe late collapse in seed 2. **Do not promote this profile.**
  Its strongest seed remains 30,085 below the leader. See FLOW_MARGIN.md.
- The no-margin flow field completes at only 46,186 and degrades late. A late
  freeze with margin 50 reaches 110,649. All pass deadlines; better timing alone
  does not make a routing policy useful. The 4M control exactly matches v8.
- Equal-weight 50k averages 107,413.3 over seeds 0–2. Compact seed 0 preserves
  every full trajectory field at 107,457 and reduces RSS from 16.208 to 11.883 GB.
- Two regional rounds reach 112,164, max entry 0.891 seconds. One round reaches
  111,411 at 0.680 seconds. Global 25k plus two rounds reaches 111,997 at 0.725
  seconds, four cores and about 11.82 GB RSS. No six-seed confirmation.
- Regional temperature 100/0 yields 111,573/111,289 in one round and 112,131 at
  zero with two rounds. No material gain. Warm 50k reaches 108,492 (+0.96%), but
  warm 25k/10k fall to 104,548/94,510. Neither changes the leading profile.
- Turn cost 2 and distance scales 256/1024 lose throughput; larger turn costs
  and the original 50k learned-flow profiles fail deadlines. Failures have no
  partial score. V18 cuts isolated table-build time 19–26% with exact full control
  trajectories; no whole-planner speedup follows from that component measurement.

## Active full queue

All full matrices use two independent single-core EPYC 9354 instances and
24 GiB aggregate reserved memory, one-second decisions and 5,000 steps.

1. **8898550/8898552**, frozen v26-r1, running on research31: corrected route-to-go metric, comparing
   disabled control, unit routes, opposing-cost routes, reconnection, and bounded
   refinement 64. All five screens pass. First full pair: control 109,244 and
   unit guides 39,066, both valid. Unit guides collapse late (2,778 tasks in the
   final window, age p90 4,505); they are not promoted. Opposing-cost guides also
   complete at 42,051 and reconnection at 28,071, both valid but negative. The
   refinement case is still running.
2. **8898558/8898559**, frozen v20-r1: matching no-flow 4M controls at seeds 1/2,
   to distinguish the fragile flow policy from its different fixed-work setting.
3. **8898554/8898555**, frozen v20-r1: margin-50 control versus margins 25/75
   and strength 2. Moved while pending after the matched controls; all three
   feasibility screens pass. See results/flow-neighbors-queue-update.json.
4. **8898566/8898567**, frozen v27: exact-policy validation of global 4M and
   global 25k plus two regional rounds after the kernel optimization. This matrix
   uses two four-core instances and 24 GiB total; the others above use two cores.
5. **8898606/8898607**, frozen v28: matched one-/four-worker preparation for
   global 4M and global 25k/two regional rounds. Two four-core instances, 24 GiB.

The old v21/v24 guide full matrices and analysis jobs
8898527/8898528/8898535/8898536 were confirmed pending and canceled. No running
job was interrupted; all frozen source/specifications and completed screens
remain available. See results/guide-pending-matrices-superseded.json.

## Review and new source

Fable's new CLI run emitted six findings but failed its configured cost budget;
it is not a successful final review. Visible findings, actual usage/status and
an independent assessment are in fable-guides/. No retry was submitted.

Its waypoint-scoring defect is independently reproduced: the old code prefers
stopping at a nearby waypoint to passing it. V26 seeds all remaining route states
in the local box and fixes that ranking. A bounded shortest-connector BFS keeps
reconnection independent of the new route-to-go score. V25 separately adds
optional, fixed-count refinement that accepts only complete strictly cheaper
routes, excluding self flow exactly. All changes remain optional with guides.

The complete corrected suite passes, including 3,912 oriented window states,
four rotated forward-over-parking checks, exact flow conservation, explicit
timeout propagation, fixed-work fairness, and nonzero serial/four-thread
production refinement with warm starts and protected actions. All five 200-step
screens pass at max 0.849 seconds; disabled guidance exactly preserves the prior
screen trajectory. These establish correctness/feasibility, not throughput gain.
See GUIDE_PATHS.md and the source-hash-verified build archives.

After each matrix: preserve failures, check every entry sample, errors/timeouts,
actual RSS, trajectory fingerprints, final-window rates and movement efficiency.
Do not rank policies by short prefixes. Confirm additional seeds before promoting
an improvement, and six before claiming the goal has been achieved. Future
combinations of flow, regional repair or warm reuse require independent validation.


An isolated all-search candidate index preserves exact results but makes
global/warm kernels 1.9%/1.5% slower. A regional-only template specialization
preserves all checksums and reduces isolated regional time 6.9%, global time
about 2%. The exact tested header is integrated in v27. All regressions and both
production screens pass with identical trajectories; full validation is queued
above. No end-to-end speedup is established. See prefilter/.

V28 parallelizes candidate scoring with serial, ordered LRU lookup and immutable
worker inputs. Build 8898592 and four warehouse screens 8898601 pass. Each
one-/four-thread pair exactly preserves all prior trajectory fields; single
200-step pairs reduce wall time 13.9% / 8.3%. Full validation is queued, and
no throughput gain is claimed. See PARALLEL_PREPARATION.md.

Guide load-cost feasibility screen **8898605** follows the preparation screen,
using frozen v26-r1, 200 steps, one core and 8 GiB. It tests load cost 1 at
heuristic weights 2/4, without reconnection and with reconnection/refinement.
It is a concentration/coverage/deadline test, not a short throughput ranking.
