# RANDOM-04 whole-chain work price: selected2,799, runtime unresolved

All eight declared attempts are complete and independently audited. Seven pass full replay/strict limits; coefficient0.5seed1 fails atstep0/1046.301ms. Both coefficient0.25 controls reproduce six complete trace fields, including exact2,795 repetition.

| Length price | Seed1 | Seed0 | Status |
|---|---:|---:|---|
| 0.25 | 2795 | 2776 | Both valid |
| 0.125 | 2777 | 2785 | Both valid; paired total lower |
| 0.5 | Timeout | 2799 | No complete pair; keep failure |
| 1 | 709 | 709 | Both valid; large regression |

The new selected maximum2,799 uses source0bb92cea, plannerseed0, coefficient0.5 and the same32optional-idle price. Mean467.634/max984.795ms, RSS685.867MB; finished2026-09-22T04:26:21.345951+00:00. It is+5.66%over matched max(NMS,KK)=2649 and115short of2914. The valid selected run is recorded, but this timeout-bearing configuration is not robust.

The failed seed1 startup spends630.108ms in assignment (57.582matrix,572.335solve),64.482ms preparing task costs and340.098ms in look-ahead. Existing exact optional-column compression is the next bounded runtime test; no past failure is replaced. The low coefficient1score may reflect unchanged optional idle price32 rejecting many now-costlier tasks. Coupled cost/idle-price trials are hypotheses, frozen before launch.

The better-qualified coefficient0.25 recipe remains2,795, exact, with+0.5714%over five paired seeds. All comparisons use one archived task/start input; no fresh-input claim.

[Independent audit including failure](audit.json), [two exact controls](control-equivalence.json), [frozen runtime/price protocol](../../experiments/random04-chain-runtime-v166-protocol.json).
