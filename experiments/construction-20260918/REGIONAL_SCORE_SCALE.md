# Physical cost before operation-index preference

The existing candidate cost is S times the physical heuristic term, minus op*U.
With the confirmed learned-field unit U=4 and operation indices0..128, that secondary
term spans512 raw score units. At S=50 it can override a physical heuristic difference
of up to10 integer metric units (2.5 base action costs). Directed traffic tolls can
make the physical metric differ by only one integer unit. S=513 makes any such
integer physical difference outrank the entire operation-index span. This statement
concerns the supplied five-step heuristic, not true global task completion time.

The bounded experiment compares existing CGAR_TEMPORAL_DISTANCE_SCALE values50
and513 under the newly confirmed regional/graded-flow/64-field policy. All other
parameters stay fixed, including two regional rounds and one4M global worker.
Next-errand scoring and peak auditing are OFF. No production change is needed.
Older256/1024 no-flow experiments were negative; they do not settle this interaction.
Larger S changes choices and search behavior, so a gain is a hypothesis, not an
arithmetic consequence or a claim that the existing preference is a correctness bug.

First require a200-step deadline/RSS screen and exact scale50 control trajectory.
Then full matched seeds0/2; any useful candidate needs the remaining four seeds.
All complete fixed work must finish within1s or fail, processRSS<32decimalGB,
four isolated physical cores per process. [Profiles](regional-score-scale-variants.json).

Screen8899165 passed both profiles with four disjoint physical cores and no CPU
quota. Scale50 exactly reproduces the regional prefix; maxima972.25ms versus
943.05ms for513, peakRSS5.292GB. Diagnostics confirm the selected scale, all22
source/test hashes and the binary match exact8ba2484. Prefix throughput is not
quality evidence. [Screen verification](results/regional-score-scale-screen-v48/equivalence.json).

Full matrix8899167/analysis8899168 started2026-09-19T22:28:04UTC: four simultaneous
seeds0/2 x50/513 runs, four physical cores per process,16cores/64GiB aggregate.
Raw runs/cgar-regional-score-scale-full-v48-20260919, future archive
results/regional-score-scale-full-v48. Scale50 full controls must reproduce the
confirmed regional trajectories. Full results are pending.

The full scale513seed0 treatment failed explicitly at timestep2230:
CGAR_TIMEOUT elapsed_ms=1003.435 stage=temporal_region_repair. Its maximum sampled
RSS was9.324GB. It cannot satisfy the1s acceptance criterion; no partial-throughput
comparison is accepted. Other cases continue. The failure occurred after the2176
field refresh; sampled regional time had risen to395.2ms at step2200 versus140.5ms
at step200. These sparse observations do not isolate the cause of the failing tick.
