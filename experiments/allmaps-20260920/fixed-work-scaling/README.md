# Fixed-work scaling and RANDOM03 paired replication

Test additional complete search work on selected RANDOM03/04/05 profiles;
these selected profiles include explicit tricks. No wall-clock quality cutoff.
RANDOM03 compares4M(one proposal) against8M,16M and four4M proposals. Dense
controls have eight500k proposals; alternatives use2M/4M per proposal, or keep
those controls and double each regional candidate cap to8M. Actual work is
recorded because step caps may bind before candidate caps. Same four physical
cores, full800/1000/2000steps, strict1s; timeouts are failed runs, not scores.

Separately compare RANDOM03 generic chain-rank1613 versus KK-field/pickup4
1890 configurations on seeds2/4. This is a bundle comparison, not attribution
to each component. Sourceacaf634 and frozen reference buildv2. The independent
RANDOM05 solver and its streams remain untouched.

RANDOM05's8M regional cap reaches2684 at seed0 versus2574 (selected prior
maximum2608 was seed2), max227.23ms. More global work2544/2559 loses.
Next compare8M control with16M,32M, and8M x4rounds under the same strict1s
limit. Replicate4M versus8M on seeds2/4. All fixed work must complete.
RANDOM04 scaling loses:1622 control vs1556/1601/1582; keep1645seed2 overall.
