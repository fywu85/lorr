# Fresh RANDOM-05 validation protocol

Declared on20 September2026 before observing these solver results.

Freeze the archived-input frontier at source `b824f5d`, planner seed3, field15,
one directional pair reversed with flip seed5, K1024 and known-horizon cutoff1.5.
The full configuration is copied into `experiments/fresh-validation-full.json`.
None of the new completion-reward, task-turnover or operation-policy options is
used. The field and all settings were selected on the archived input alone.

Generate two independent starts/task streams with base seeds50001 and50002,
using the map helper and seed offsets in the archived KittyKnight snapshot.
The generator chooses800 distinct free starting cells,100000 tasks with2–5
uniformly sampled stops, and a1.5-times-agent-count reveal pool. The map remains
`random-32-32-20`. Record the helper hash, NumPy version, input hashes and both
actual RNG seeds. These are new validation inputs, not reproductions of the
colleague's private41001/41002 instances; their exact generation protocol is
unavailable. Recreate with:

```sh
env/bin/python random05/tools/generate_fresh.py \
  --output runs/random05/fresh-inputs-v1 --seeds 50001 50002
```

For each input, run the frozen candidate once and NMS twice on four physical
EPYC9354 cores,1s per step,30s initialization,2000 steps, and a32GB process limit.
NMS is the same combined-track snapshot used in the existing reference: only
its worker constant is4, in addition to the previously documented constructor
initialization repair. No combined-track features are removed.

Every job has its own checked physical-core binding. Shared hosts are allowed,
so record allocation and latency; this is not a claim of complete host isolation.
Candidate runs carry `--trick RANDOM-05`, including the known-horizon cutoff.
Report all results, validity, deadline violations and both NMS repetitions.
Compare against the stronger valid NMS repetition for each input. Do not silently
replace a seed or tune on these inputs and continue calling them held out.
A new configuration would need a separately declared validation set.
