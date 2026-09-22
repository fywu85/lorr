# Physical-deadline matching gate: all-five full results

Audited 2026-09-21T23:58:29.465817+00:00. Source4fb9498e/build162. Ten full runs pass independent motion/task/waiting replay and strict1-second/32GB checks across the original and replacement batches. Two original disabled controls onresearch49 were refused before solver launch because the process exposed64physical cores instead of16; both refusals remain archived. The replacements use identical solver settings/input/binary. All five controls match all six full archived trace fields.

| Instance | Gate off | Gate on | Difference |
|---|---:|---:|---:|
| RANDOM-01 | 729 | 732 | +3 |
| RANDOM-02 | 1408 | 1403 | -5 |
| RANDOM-03 | 2620 | 2621 | +1 |
| RANDOM-04 | 2782 | 2782 | +0 |
| RANDOM-05 | 4242 | 4241 | -1 |

The gate helps the selected01/03 profiles only; leave it disabled on02/04/05. RANDOM-01 subsequent five paired seeds give+0.690%, allpositive, selected733; later mild annealing reaches735. RANDOM-03 selected2621 is+11.11% over matchedmax2359, but the one-task increase needs paired qualification. Its mean/max entry502.134/745.646ms, peakRSS463.966MB. RANDOM-04/05 selected2782/4242 remain+5.02%/+33.73% over matchedmax2649/3172.

This is explicitly a known-horizon trick. It filters only physically impossible unopened pairs, never drops opened tasks, and uses no hidden task stream. The extra unit-action table costs memory; there is no broad all-density gain. Exact01repetitions and03qualification are separate artifacts. No fresh-input qualification is claimed.
