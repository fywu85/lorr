# RANDOM-05 order latency alongside throughput

Throughput is the primary objective. These secondary metrics use each
throughput-frontier run from RANDOM05_PROGRESS.md, without selecting a
different run for latency. All durations are simulation steps.

Completed latency is release to final waypoint; its mean and percentiles
exclude unfinished orders. Percentiles use the nearest-rank convention.
The oldest unfinished age and initial-cohort counts expose that censored
tail. An age of 2,000 is the observation horizon, not a waiting-time bound.
The initial cohort contains 1,200 orders. Later release times depend on
throughput because completed orders trigger replacements.

Source, input, allocation and raw-result hashes are checked by
`tools/audit_progress.py` and recorded in [the audit](results/progress-audit.json).
See [matched NMS waiting times](results/task-waiting-frontiers-20260920T1612/REPORT.md).

| Completed UTC | Source | Tasks | Workers / physical cores | Completed mean | Completed p95 | Completed max | Oldest unfinished | Initial unfinished / unopened |
|---|---|---:|---|---:|---:|---:|---:|---|
| 2026-09-20T07:55:37.615086+00:00 | [215fd6d167c96695e9ac6db6f8517791f2dab077](https://github.com/fywu85/lorr/commit/215fd6d167c96695e9ac6db6f8517791f2dab077) | 54 | 4 / 4 | 70.5 | 126 | 193 | 2000 | 1146 / 406 |
| 2026-09-20T07:58:41.020438+00:00 | [93b760412a3d346b0b9b7896f400cff43b3733be](https://github.com/fywu85/lorr/commit/93b760412a3d346b0b9b7896f400cff43b3733be) | 2113 | 4 / 4 | 602.5 | 1427 | 1998 | 2000 | 225 / 138 |
| 2026-09-20T07:58:41.152623+00:00 | [93b760412a3d346b0b9b7896f400cff43b3733be](https://github.com/fywu85/lorr/commit/93b760412a3d346b0b9b7896f400cff43b3733be) | 2230 | 4 / 4 | 576.9 | 1338 | 1993 | 2000 | 213 / 134 |
| 2026-09-20T07:58:41.330442+00:00 | [93b760412a3d346b0b9b7896f400cff43b3733be](https://github.com/fywu85/lorr/commit/93b760412a3d346b0b9b7896f400cff43b3733be) | 2346 | 4 / 4 | 558.1 | 1324 | 1997 | 2000 | 198 / 121 |
| 2026-09-20T07:58:42.118225+00:00 | [93b760412a3d346b0b9b7896f400cff43b3733be](https://github.com/fywu85/lorr/commit/93b760412a3d346b0b9b7896f400cff43b3733be) | 2431 | 4 / 4 | 560.9 | 1314 | 1960 | 2000 | 188 / 128 |
| 2026-09-20T08:02:13.099706+00:00 | [93b7604](https://github.com/fywu85/lorr/commit/93b7604) | 2488 | 4 / 4 | 547.3 | 1316 | 1990 | 2000 | 183 / 123 |
| 2026-09-20T08:05:44.745252+00:00 | [93b7604](https://github.com/fywu85/lorr/commit/93b7604) | 2562 | 4 / 4 | 543.5 | 1306 | 1990 | 2000 | 175 / 112 |
| 2026-09-20T08:05:50.770563+00:00 | [93b7604](https://github.com/fywu85/lorr/commit/93b7604) | 2594 | 4 / 4 | 538.0 | 1288 | 1991 | 2000 | 177 / 121 |
| 2026-09-20T08:10:24.936747+00:00 | [8993a43](https://github.com/fywu85/lorr/commit/8993a43) | 2658 | 4 / 4 | 555.4 | 1327 | 1989 | 2000 | 173 / 118 |
| 2026-09-20T08:18:53.857208+00:00 | [ca80563](https://github.com/fywu85/lorr/commit/ca80563) | 2669 | 4 / 4 | 547.2 | 1309 | 1995 | 2000 | 167 / 127 |
| 2026-09-20T08:19:08.147723+00:00 | [ca80563](https://github.com/fywu85/lorr/commit/ca80563) | 2677 | 4 / 4 | 542.8 | 1291 | 1996 | 2000 | 173 / 115 |
| 2026-09-20T08:32:56.449253+00:00 | [6fb222e](https://github.com/fywu85/lorr/commit/6fb222e) | 2729 | 4 / 4 | 546.7 | 1290 | 1999 | 2000 | 167 / 121 |
| 2026-09-20T08:41:54.502028+00:00 | [f7ca98c](https://github.com/fywu85/lorr/commit/f7ca98c) | 2766 | 4 / 4 | 544.8 | 1292 | 1993 | 2000 | 163 / 111 |
| 2026-09-20T08:41:56.906339+00:00 | [f7ca98c](https://github.com/fywu85/lorr/commit/f7ca98c) | 2767 | 4 / 4 | 544.4 | 1314 | 1993 | 2000 | 162 / 119 |
| 2026-09-20T08:48:21.604864+00:00 | [8619b95](https://github.com/fywu85/lorr/commit/8619b95) | 2841 | 4 / 4 | 539.7 | 1299 | 1979 | 2000 | 158 / 115 |
| 2026-09-20T08:55:52.615079+00:00 | [804fee5](https://github.com/fywu85/lorr/commit/804fee5) | 2842 | 4 / 4 | 525.9 | 1251 | 1980 | 2000 | 171 / 115 |
| 2026-09-20T08:55:55.708615+00:00 | [804fee5](https://github.com/fywu85/lorr/commit/804fee5) | 2868 | 4 / 4 | 536.2 | 1282 | 1990 | 2000 | 155 / 112 |
| 2026-09-20T08:59:55.016021+00:00 | [804fee5](https://github.com/fywu85/lorr/commit/804fee5) | 2872 | 4 / 4 | 527.2 | 1254 | 1959 | 2000 | 161 / 106 |
| 2026-09-20T09:01:43.509210+00:00 | [804fee5](https://github.com/fywu85/lorr/commit/804fee5) | 2910 | 2 / 2 | 506.0 | 1237 | 1987 | 2000 | 159 / 107 |
| 2026-09-20T09:05:15.036957+00:00 | [b79a218](https://github.com/fywu85/lorr/commit/b79a218) | 2916 | 4 / 4 | 520.2 | 1247 | 1991 | 2000 | 162 / 106 |
| 2026-09-20T09:14:27.889620+00:00 | [134faa8](https://github.com/fywu85/lorr/commit/134faa8) | 2997 | 4 / 4 | 501.2 | 1236 | 1945 | 2000 | 153 / 106 |
| 2026-09-20T09:19:02.607264+00:00 | [6aed8ba](https://github.com/fywu85/lorr/commit/6aed8ba) | 3127 | 4 / 4 | 493.2 | 1216 | 1926 | 2000 | 146 / 101 |
| 2026-09-20T09:21:46.636885+00:00 | [134faa8](https://github.com/fywu85/lorr/commit/134faa8) | 3231 | 32 / 16 | 485.9 | 1215 | 1962 | 2000 | 144 / 100 |
| 2026-09-20T09:40:30.891971+00:00 | [6aed8ba](https://github.com/fywu85/lorr/commit/6aed8ba) | 3299 | 32 / 16 | 472.6 | 1184 | 1921 | 2000 | 145 / 100 |
| 2026-09-20T10:03:44.348412+00:00 | [79d0e79](https://github.com/fywu85/lorr/commit/79d0e79) | 3299 | 4 / 4 | 472.6 | 1184 | 1921 | 2000 | 145 / 100 |
| 2026-09-20T10:19:47.676357+00:00 | [2ead4f4](https://github.com/fywu85/lorr/commit/2ead4f4) | 3299 | 4 / 4 | 472.6 | 1184 | 1921 | 2000 | 145 / 100 |
| 2026-09-20T10:23:12.419038+00:00 | [2ead4f4](https://github.com/fywu85/lorr/commit/2ead4f4) | 3351 | 4 / 4 | 468.1 | 1162 | 1971 | 2000 | 144 / 109 |
| 2026-09-20T10:23:13.900548+00:00 | [2ead4f4](https://github.com/fywu85/lorr/commit/2ead4f4) | 3363 | 4 / 4 | 469.1 | 1162 | 1967 | 2000 | 139 / 107 |
| 2026-09-20T10:57:32.632101+00:00 | [3228b9c](https://github.com/fywu85/lorr/commit/3228b9c) | 3374 | 4 / 4 | 466.3 | 1154 | 1951 | 2000 | 142 / 102 |
| 2026-09-20T11:09:45.903706+00:00 | [3228b9c](https://github.com/fywu85/lorr/commit/3228b9c) | 3374 | 32 / 16 | 466.3 | 1154 | 1951 | 2000 | 142 / 102 |
| 2026-09-20T11:17:50.924764+00:00 | [3228b9c](https://github.com/fywu85/lorr/commit/3228b9c) | 3379 | 4 / 4 | 466.8 | 1157 | 1933 | 2000 | 148 / 104 |
| 2026-09-20T11:25:19.439602+00:00 | [32b333a](https://github.com/fywu85/lorr/commit/32b333a) | 3379 | 32 / 16 | 466.8 | 1157 | 1933 | 2000 | 148 / 104 |
| 2026-09-20T11:51:07.120923+00:00 | [b824f5d](https://github.com/fywu85/lorr/commit/b824f5d) | 3395 | 32 / 16 | 462.4 | 1164 | 1940 | 2000 | 139 / 98 |
| 2026-09-20T12:08:42.946296+00:00 | [b824f5d](https://github.com/fywu85/lorr/commit/b824f5d) | 3395 | 4 / 4 | 462.4 | 1164 | 1940 | 2000 | 139 / 98 |
| 2026-09-20T12:21:57.244760+00:00 | [b824f5d](https://github.com/fywu85/lorr/commit/b824f5d) | 3197 | 32 / 16 | 471.0 | 1141 | 1997 | 2000 | 146 / 109 |
| 2026-09-20T12:37:00.784376+00:00 | [b824f5d](https://github.com/fywu85/lorr/commit/b824f5d) | 3197 | 4 / 4 | 471.0 | 1141 | 1997 | 2000 | 146 / 109 |
| 2026-09-20T13:32:10.140710+00:00 | [a6ad284](https://github.com/fywu85/lorr/commit/a6ad284) | 3400 | 32 / 16 | 456.5 | 1126 | 1948 | 2000 | 147 / 104 |
| 2026-09-20T13:33:08.542811+00:00 | [a6ad284](https://github.com/fywu85/lorr/commit/a6ad284) | 3422 | 32 / 16 | 451.2 | 1111 | 1931 | 2000 | 146 / 100 |
| 2026-09-20T13:34:31.273508+00:00 | [a6ad284](https://github.com/fywu85/lorr/commit/a6ad284) | 3450 | 32 / 16 | 455.6 | 1154 | 1955 | 2000 | 143 / 97 |
| 2026-09-20T13:36:34.649056+00:00 | [a6ad284](https://github.com/fywu85/lorr/commit/a6ad284) | 3492 | 32 / 16 | 450.8 | 1118 | 1949 | 2000 | 142 / 101 |
| 2026-09-20T13:44:31.351693+00:00 | [a6ad284](https://github.com/fywu85/lorr/commit/a6ad284) | 3400 | 4 / 4 | 456.5 | 1126 | 1948 | 2000 | 147 / 104 |
| 2026-09-20T13:46:39.608755+00:00 | [a6ad284](https://github.com/fywu85/lorr/commit/a6ad284) | 3422 | 4 / 4 | 451.2 | 1111 | 1931 | 2000 | 146 / 100 |
| 2026-09-20T13:52:47.072835+00:00 | [a6ad284](https://github.com/fywu85/lorr/commit/a6ad284) | 3501 | 32 / 16 | 451.5 | 1118 | 1932 | 2000 | 142 / 98 |
| 2026-09-20T13:57:09.026640+00:00 | [a6ad284](https://github.com/fywu85/lorr/commit/a6ad284) | 3555 | 32 / 16 | 446.8 | 1095 | 1930 | 2000 | 141 / 103 |
| 2026-09-20T14:00:11.259204+00:00 | [a6ad284](https://github.com/fywu85/lorr/commit/a6ad284) | 3492 | 4 / 4 | 450.8 | 1118 | 1949 | 2000 | 142 / 101 |
| 2026-09-20T14:04:45.937007+00:00 | [da00823](https://github.com/fywu85/lorr/commit/da00823) | 3492 | 4 / 4 | 450.8 | 1118 | 1949 | 2000 | 142 / 101 |
| 2026-09-20T14:15:04.122374+00:00 | [e896201](https://github.com/fywu85/lorr/commit/e896201) | 3555 | 32 / 16 | 446.8 | 1095 | 1930 | 2000 | 141 / 103 |
| 2026-09-20T14:24:41.462668+00:00 | [e896201](https://github.com/fywu85/lorr/commit/e896201) | 3501 | 4 / 4 | 451.5 | 1118 | 1932 | 2000 | 142 / 98 |
| 2026-09-20T14:29:52.858068+00:00 | [d933023](https://github.com/fywu85/lorr/commit/d933023) | 3596 | 32 / 16 | 440.1 | 1099 | 1962 | 2000 | 137 / 99 |
| 2026-09-20T14:45:45.463667+00:00 | [d933023](https://github.com/fywu85/lorr/commit/d933023) | 3509 | 4 / 4 | 442.8 | 1074 | 1948 | 2000 | 141 / 100 |
| 2026-09-20T14:52:24.059199+00:00 | [f9b1143](https://github.com/fywu85/lorr/commit/f9b1143) | 3520 | 4 / 4 | 447.4 | 1103 | 1932 | 2000 | 144 / 105 |
| 2026-09-20T14:57:15.935581+00:00 | [d933023](https://github.com/fywu85/lorr/commit/d933023) | 3657 | 32 / 16 | 434.0 | 1088 | 1954 | 2000 | 134 / 96 |
| 2026-09-20T14:57:35.059037+00:00 | [d933023](https://github.com/fywu85/lorr/commit/d933023) | 3408 | 32 / 16 | 449.1 | 1097 | 1997 | 2000 | 142 / 101 |
| 2026-09-20T15:02:34.822809+00:00 | [e896201](https://github.com/fywu85/lorr/commit/e896201) | 3285 | 4 / 4 | 462.9 | 1121 | 1986 | 2000 | 144 / 109 |
| 2026-09-20T15:29:14.931221+00:00 | [f9b1143](https://github.com/fywu85/lorr/commit/f9b1143) | 3562 | 4 / 4 | 445.1 | 1117 | 1950 | 2000 | 137 / 96 |
| 2026-09-20T15:29:21.178706+00:00 | [f9b1143](https://github.com/fywu85/lorr/commit/f9b1143) | 3562 | 4 / 4 | 441.3 | 1108 | 1925 | 2000 | 143 / 98 |
| 2026-09-20T15:50:14.795609+00:00 | [6ce9312](https://github.com/fywu85/lorr/commit/6ce9312) | 3637 | 4 / 4 | 440.8 | 1131 | 1947 | 2000 | 133 / 101 |
| 2026-09-20T16:15:24.583747+00:00 | [8eb59d3](https://github.com/fywu85/lorr/commit/8eb59d3) | 3689 | 32 / 16 | 425.8 | 1050 | 1929 | 2000 | 141 / 96 |
| 2026-09-20T16:25:39.768782+00:00 | [6ce9312](https://github.com/fywu85/lorr/commit/6ce9312) | 3648 | 4 / 4 | 435.5 | 1091 | 1890 | 2000 | 135 / 98 |
| 2026-09-20T16:34:48.526472+00:00 | [05559b7](https://github.com/fywu85/lorr/commit/05559b7) | 3705 | 32 / 16 | 426.3 | 1066 | 1922 | 2000 | 135 / 94 |
| 2026-09-20T17:09:29.991140+00:00 | [05559b7](https://github.com/fywu85/lorr/commit/05559b7) | 3655 | 4 / 4 | 431.9 | 1089 | 1916 | 2000 | 141 / 98 |
| 2026-09-20T17:38:53.567749+00:00 | [d6a3e0f](https://github.com/fywu85/lorr/commit/d6a3e0f) | 3724 | 32 / 16 | 428.7 | 1078 | 1941 | 2000 | 130 / 96 |
| 2026-09-20T17:45:06.759124+00:00 | [05559b7](https://github.com/fywu85/lorr/commit/05559b7) | 3741 | 32 / 16 | 426.7 | 1091 | 1927 | 2000 | 135 / 94 |
| 2026-09-20T17:52:23.244327+00:00 | [d6a3e0f](https://github.com/fywu85/lorr/commit/d6a3e0f) | 3743 | 32 / 16 | 422.8 | 1032 | 1939 | 2000 | 142 / 95 |
| 2026-09-20T18:07:16.977439+00:00 | [60c5f9b](https://github.com/fywu85/lorr/commit/60c5f9b) | 3655 | 4 / 4 | 431.9 | 1089 | 1916 | 2000 | 141 / 98 |
| 2026-09-20T18:59:20.474586+00:00 | [5f81613](https://github.com/fywu85/lorr/commit/5f81613) | 3755 | 32 / 16 | 424.3 | 1076 | 1919 | 2000 | 130 / 93 |
| 2026-09-20T18:59:34.727026+00:00 | [f81b760](https://github.com/fywu85/lorr/commit/f81b760) | 3709 | 4 / 4 | 424.2 | 1032 | 1947 | 2000 | 135 / 98 |
| 2026-09-20T19:21:39.535205+00:00 | [5f81613](https://github.com/fywu85/lorr/commit/5f81613) | 3776 | 32 / 16 | 418.2 | 1036 | 1943 | 2000 | 130 / 94 |
| 2026-09-20T19:23:22.219359+00:00 | [5f81613](https://github.com/fywu85/lorr/commit/5f81613) | 3794 | 32 / 16 | 416.0 | 1057 | 1950 | 2000 | 133 / 98 |
| 2026-09-20T19:47:51.315402+00:00 | [5f81613](https://github.com/fywu85/lorr/commit/5f81613) | 3718 | 4 / 4 | 427.9 | 1060 | 1935 | 2000 | 136 / 105 |
| 2026-09-20T20:02:29.833542+00:00 | [5f81613](https://github.com/fywu85/lorr/commit/5f81613) | 3770 | 4 / 4 | 421.8 | 1065 | 1941 | 2000 | 137 / 96 |
| 2026-09-20T20:07:08.133068+00:00 | [5f81613](https://github.com/fywu85/lorr/commit/5f81613) | 3852 | 32 / 16 | 406.2 | 1015 | 1940 | 2000 | 138 / 96 |
| 2026-09-20T20:36:40.537684+00:00 | [5f81613](https://github.com/fywu85/lorr/commit/5f81613) | 3632 | 32 / 16 | 418.8 | 1039 | 1957 | 2000 | 139 / 108 |
| 2026-09-20T20:47:29.818820+00:00 | [5f81613](https://github.com/fywu85/lorr/commit/5f81613) | 3503 | 4 / 4 | 433.3 | 1076 | 1965 | 2000 | 138 / 103 |
| 2026-09-20T22:04:50.502045+00:00 | [5f81613](https://github.com/fywu85/lorr/commit/5f81613) | 3857 | 32 / 16 | 411.3 | 1026 | 1937 | 2000 | 135 / 102 |
| 2026-09-20T22:49:18.963497+00:00 | [233f5bf](https://github.com/fywu85/lorr/commit/233f5bf) | 3872 | 32 / 16 | 411.2 | 1019 | 1944 | 2000 | 138 / 96 |
| 2026-09-20T23:58:38.632599+00:00 | [233f5bf](https://github.com/fywu85/lorr/commit/233f5bf) | 3877 | 32 / 16 | 421.1 | 1065 | 1966 | 2000 | 131 / 94 |
| 2026-09-21T00:05:35.106669+00:00 | [1e266b0](https://github.com/fywu85/lorr/commit/1e266b0) | 3917 | 32 / 16 | 414.9 | 1040 | 1953 | 2000 | 126 / 97 |
| 2026-09-21T00:19:49.212073+00:00 | [233f5bf](https://github.com/fywu85/lorr/commit/233f5bf) | 3928 | 32 / 16 | 415.0 | 1037 | 1952 | 2000 | 130 / 88 |
| 2026-09-21T01:28:12.433277+00:00 | [acdbfd7](https://github.com/fywu85/lorr/commit/acdbfd7) | 3933 | 32 / 16 | 415.6 | 1072 | 1959 | 2000 | 123 / 90 |
| 2026-09-21T01:50:46.795715+00:00 | [acdbfd7](https://github.com/fywu85/lorr/commit/acdbfd7) | 3941 | 32 / 16 | 414.4 | 1052 | 1956 | 2000 | 124 / 90 |
| 2026-09-21T02:16:03.845916+00:00 | [acdbfd7](https://github.com/fywu85/lorr/commit/acdbfd7) | 3978 | 32 / 16 | 410.4 | 1045 | 1959 | 2000 | 130 / 92 |
| 2026-09-21T03:06:09.957953+00:00 | [a2ff2b2](https://github.com/fywu85/lorr/commit/a2ff2b2) | 3985 | 32 / 16 | 412.7 | 1052 | 1962 | 2000 | 121 / 87 |
| 2026-09-21T03:26:06.798792+00:00 | [a2ff2b2](https://github.com/fywu85/lorr/commit/a2ff2b2) | 3990 | 32 / 16 | 408.0 | 1048 | 1945 | 2000 | 130 / 91 |
| 2026-09-21T03:50:40.471914+00:00 | [a2ff2b2](https://github.com/fywu85/lorr/commit/a2ff2b2) | 4011 | 32 / 16 | 409.5 | 1048 | 1944 | 2000 | 130 / 86 |
| 2026-09-21T15:23:12.914641+00:00 | [027df4d9](https://github.com/fywu85/lorr/commit/027df4d9) | 4090 | 32 / 16 | 390.3 | 1022 | 1971 | 2000 | 131 / 99 |
| 2026-09-21T15:23:22.109240+00:00 | [027df4d9](https://github.com/fywu85/lorr/commit/027df4d9) | 4175 | 32 / 16 | 359.2 | 1012 | 1964 | 2000 | 147 / 117 |
| 2026-09-21T16:24:44.415657+00:00 | [027df4d9](https://github.com/fywu85/lorr/commit/027df4d9) | 4197 | 32 / 16 | 360.3 | 1012 | 1977 | 2000 | 150 / 113 |
| 2026-09-21T17:28:48.985827+00:00 | [027df4d9](https://github.com/fywu85/lorr/commit/027df4d9) | 4236 | 32 / 16 | 359.0 | 1003 | 1994 | 2000 | 148 / 107 |
| 2026-09-21T18:11:42.053845+00:00 | [88551e69df5b6f5ee14600dfe3a7ae8fe586783c](https://github.com/fywu85/lorr/commit/88551e69df5b6f5ee14600dfe3a7ae8fe586783c) | 4242 | 32 / 16 | 358.0 | 990 | 1983 | 2000 | 147 / 110 |
| 2026-09-22T00:21:04.224019+00:00 | [4fb9498ef65e0b92dcb51dbbe2cd367dc9e0a4ad](https://github.com/fywu85/lorr/commit/4fb9498ef65e0b92dcb51dbbe2cd367dc9e0a4ad) | 4254 | 32 / 16 | 358.7 | 1019 | 1973 | 2000 | 147 / 109 |
| 2026-09-22T02:19:09.148099+00:00 | [30a5664757b8ffad1150f5eb763c747ba1001bed](https://github.com/fywu85/lorr/commit/30a5664757b8ffad1150f5eb763c747ba1001bed) | 4302 | 32 / 16 | 352.3 | 957 | 1986 | 2000 | 146 / 110 |
| 2026-09-22T08:06:15.158769+00:00 | [2d893f96e78d3fcc092c37a03a7b1888880b306e](https://github.com/fywu85/lorr/commit/2d893f96e78d3fcc092c37a03a7b1888880b306e) | 4302 | 32 / 16 | 352.3 | 957 | 1986 | 2000 | 146 / 110 |
