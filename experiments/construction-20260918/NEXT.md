# Continuing warehouse work

The goal remains 152,981 or more completed tasks over the full 5,000 steps,
repeatably across six seeds, with complete one-second decisions and measured
process memory below 32,000,000,000 bytes. Keep policies generic: no map-name
branches, aisle templates, supplied map weights or map-specific fleet caps.

The validated checkpoint is the equal-weight 50k temporal profile: 107,413.3
mean over seeds 0–2, and an identical seed-0 timing repeat at 107,457 with a
0.5051-second maximum entry time. Six-seed confirmation remains outstanding.

Current experiment queue:

- `runs/cgar-temporal-scheduler-v7-20260918`, job 8898307: five full seed-0 cases
  run sequentially. Global-64/direct-cost/weight-5 has finished at 109,676;
  its same-build control has finished at 107,457. Direct cost with weights 1
  and 5 has finished at 107,083 and 109,836. HRRN plus global-64 is still running.
- `runs/cgar-temporal-work-full-v8-20260918`, job 8898333: held behind that
  scheduler matrix, so full benchmark allocations do not overlap. It tests
  4M candidates with and without the global-64 scheduler, plus the 50k control.
  It requires the EPYC 9354 CPU model used by the local leader reference.
- Both matrices reserve one physical core and 24 GiB total job memory. Short
  diagnostics and analysis use smaller allocations. Record all failed cases.

The 4M work setting passed a 50-step cold-start screen on EPYC 9354, but this is
not full-run validation. The 5M and 6M settings leave little deadline margin;
8M and several slower-CPU checks failed explicitly. Do not report their prefix
task counts as performance scores.

After each full matrix, run the compact analyzer with one reserved core, check
exact entry timing, memory, errors and complete trajectory hashes, and compare
final-1,000-step rates and all seed effects. Confirm the strongest candidate
across six seeds before claiming repeatability.

The next substantive target is loaded-motion efficiency. The travel audit finds
1.4888 steps per shortest-path cell for CGAR versus 1.0799 for KittyKnight, with
only a 2.7% difference in completed chain length. Actual pre-pickup robot time,
including abandoned assignments, is approximately one fifth for both. Stronger
coordination and generic route guidance deserve priority over a larger independent
portfolio, whose quality gain was only about 0.5%.

Weighted turn guidance and deterministic parallel regional repair are now
implemented as optional experiments, with regression and cold-deadline checks.
Their full matrices are queued as jobs 8898338 and 8898343 respectively; see
[ROUTING.md](ROUTING.md). Neither has a full-run performance result yet. Learned
traffic costs remain an unimplemented possible follow-up. Preserve the protected
CGAR progress mechanism and explicit failure contract in every variant.
