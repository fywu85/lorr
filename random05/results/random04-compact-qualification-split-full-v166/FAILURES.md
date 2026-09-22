# Compressed RANDOM-04 qualification: startup failures

Three completed original attempts are independently audited failures. Their full scores are absent. The other three extension cases complete with 2,776 / 2,761 / 2,766 tasks. Compression alone does not establish robust subsecond runtime.

| Case | Elapsed ms | Assignment ms | Task-cost preparation ms | Look-ahead ms |
|---|---:|---:|---:|---:|
| trick-random-04-compact-qualification-control-seed2 | 1039.572 | 517.662 | 47.422 | 464.128 |
| trick-random-04-compact-qualification-candidate-seed2 | 1144.865 | 593.374 | 46.121 | 496.604 |
| trick-random-04-compact-qualification-control-seed4 | 1041.974 | 494.845 | 54.427 | 481.564 |

All failures occur at timestep0. Keep them even if later repeats pass. Neither the original noncompressed recipe nor this compressed qualification can be called robust. Address startup assignment/search cost next. Do not move task-dependent work into initialization: the simulator reveals tasks afterward. A smaller declared startup portfolio would be a new deterministic configuration, not a replacement for these failed attempts.

[Complete independent audit](audit.json), [failure timings](startup-failures.json).
