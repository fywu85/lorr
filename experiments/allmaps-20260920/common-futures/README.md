# Complete CGAR proposals under common continuations

Declared 2026-09-21T08:58:56.352855+00:00. Source 965756fdbf5111441d2f40bf97d3b604f8a85a0b; build/regression qualification
must pass before submission. Fixed full archived horizons1000/2000, seeds0/10 on
RANDOM-04/05 respectively, four bound physical cores, strict1000ms and32decimalGB.
All existing guidance, scheduling and horizon tricks remain explicitly gated.
These are CGAR runs; the independent PILOT solver is untouched.

Control is each instance's current selected profile. The three treatments compare
4or8complete CGAR roots over10actions or4roots over15actions, with two common
continuations each. Root zero is the existing repaired plan. Others are complete
global starts sorted by their original five-step score. Common continuations
use exact revealed-chain cost and the same branch priority perturbation, seed and
fixed displacement work across roots. All branches and chunks must finish before
choosing. Paid actions plus remaining chain is the declared aggregate objective;
incumbent wins ties. Only a selected original CGAR first action is executed.

Protected root paths stay fixed; their forecast waits after action five. No
unrevealed tasks or future primary/recovery changes are simulated. This therefore
transfers the common-continuation idea, not PILOT's two-phase movement pipeline.
It is a general default-off mechanism composed with existing instance tricks.
Measure complete throughput before claiming that its forecast objective helps.

RANDOM-04 full seed0:1999control,1928with4roots/H10,1890with8roots/H10,
**2023with4roots/H15** (268.97ms maximum). The H15case changes1239first robot
actions over700000actions. All fixed evaluations finish; source/binary checked.
This selected-seed record needs replication. Declared next: seeds2/4matched
control versus H15, plus seed0 H15/B4, H20/B2 and H15/B2with five-step chain
scoring. All other factors retained, strict1s/four-core/full1000steps.
