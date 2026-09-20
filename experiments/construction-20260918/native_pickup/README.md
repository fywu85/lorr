# Warehouse pickup-distance weight after the NMS target

The winning native-band/matching/mean-margin policy spends 9.32 million of its
50 million robot steps travelling empty. Earlier native-field matching tests
found that pickup weight 5 versus 1 reduced empty travel on both seeds, with
full throughput effects +482/-18. Only weights 1 and 5 were tested in that field.

This controlled follow-up compares weights 5 (exact winning control), 8 and 12
on full seeds 0/2. All other settings retain the frozen V92 winning profile:
ordinary HRRN and forced-oldest admission enabled, short preference OFF,
CGAR primary/recovery protection, matching64, one allowed unopened retarget,
known horizon5000 and prospective mean completion margin. This is tuning the
existing pickup term in the HRRN denominator, not disabling age or filtering
tasks by chain length. It is a Warehouse TRICK profile requiring --trick WAREHOUSE.

Use the frozen V93 implementation binary (source bdf5e7a), complete fixed
4M global work and two regional rounds. Four distinct bound physical GRID cores
per run, shared5s development,32decimalGB RSS. Verify both control totals and
complete trajectories against V92, source/binary identity, movement accounting,
fixed-work receipts and task waiting tails. Report all arms, including regressions.
No generic default change or claimed improvement before full validation.

## Verified result and combination

V97 all6full cases pass, with exact controls. Weight8 gives154929/154965, gains
40/409; both exceed NMS154795. Empty work falls27184/21623robot steps, agep90
falls11/14, and all first-half tasks still finish. Weight12 gives154820/154844
(-69/+288), so8is the stronger tested candidate. This is a two-seed result, not
an increase over the separate p90 single-run peak155057.

Next compare p90+weight5 versus p90+weight8 on full seeds0/2 using the frozen V96
binary. Require exact complete p90 controls against their verified trajectories.
All other settings, fairness, resource limits and fixed search work remain
unchanged. The two component gains need not add: assignment changes calibration
data, traffic and the task pool. Record the measured interaction instead.
