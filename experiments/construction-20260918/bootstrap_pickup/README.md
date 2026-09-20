# Offline initial pickup matching

A read-only diagnostic for the time-zero Warehouse dispatch. `run.py` freezes the C++ probe, production distance/permutation/field headers and verified full-run references, then submits four bound CPU cores. The probe explicitly requires `--trick WAREHOUSE`; its output is never consumed by a planner. Complete native and physical distance fields are checked against heap Dijkstra on four rows. All initial pairings stay within the same chosen task set; geometry-based groups are limited to 32.

V107 is complete: GRID 8900470. [Results and decision](../results/bootstrap-pickup-audit-v107/summary.md). Native-cost savings are appreciably larger than the physical shortest-route reduction. The all-robot complete-field pass is deprioritized because its nine-second diagnostic cost and modest observed route-length residual do not justify a production implementation. No competition preprocessing shortcut is assumed: the driver initializes before revealing initial tasks.

```sh
python3 experiments/construction-20260918/bootstrap_pickup/run.py \
  --raw runs/cgar-bootstrap-pickup-audit-UNIQUE \
  --output experiments/construction-20260918/results/bootstrap-pickup-audit-UNIQUE
```
