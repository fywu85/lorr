# General RANDOM-05 committed-motion record

Verified full archived 2,000-step run: **2,314 tasks**, +3.953% over the prior
2,226 general profile, still **27.049% below matched max(NMS, Kitty Knight)=3,172**.
The selected trick record remains4,254. This general profile uses neither
map-specific guidance nor a known horizon and has no trick flag.

Finished UTC: 2026-09-22T02:07:14.726513+00:00.
Source: [30a56647](https://github.com/fywu85/lorr/commit/30a5664757b8ffad1150f5eb763c747ba1001bed), plannerseed5.
The only changed setting is R05_TERMINAL_PENDING=1. Complete fixed search work
is preserved. Full movement/collision/scheduling/task/waiting replay passes;
all five general disabled controls match their archived runs in all six fields.

Mean entry518.413ms; maximum559.538ms; peakRSS490.627MB. Bound16physical
EPYC9354cores/32SMTworkers, no CPU quota; strict1s entries and32decimalGB limit.
Longest completed order1,968steps; completedmean558.604/p951,374steps.
There are206initial orders unfinished,131unopened; oldest unfinished age is
censored at2,000steps. Higher throughput is not a fairness guarantee.

Promoted as a general development record, with exact repeat and planner-seed
qualification pending. Seven frozen runs repeatseed5 and compare credit0/1on
seeds6/7/8; the four-seed aggregate includes the originalseed5pair. No fresh-input
qualification or broad gain at other densities is claimed. Completed general
01/02/03/04 alternatives lose; selected04 alternatives also lose.

[Independent audit](audit.json), [five exact controls](general-controls-partial.json),
[record summary](general-random-05-terminal-pending-1/summary.json).
