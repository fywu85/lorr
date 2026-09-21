# Fresh V6 order waiting

| Run | Tasks | Longest completed order | Oldest unfinished (censored) | Initial orders unfinished | Initial unopened |
|---|---:|---:|---:|---:|---:|
| seed50015-ours | 4182 | 1955 | 2000 | 138 | 111 |
| seed50015-baseline | 4001 | 1966 | 2000 | 114 | 71 |
| seed50015-nms-repeat1 | 3154 | 1983 | 2000 | 201 | 80 |
| seed50015-nms-repeat2 | 3155 | 2000 | 2000 | 195 | 85 |
| seed50016-ours | 4177 | 1968 | 2000 | 129 | 98 |
| seed50016-baseline | 3941 | 1946 | 2000 | 114 | 67 |
| seed50016-nms-repeat1 | 3178 | 1993 | 2000 | 195 | 67 |
| seed50016-nms-repeat2 | 3149 | 1989 | 2000 | 203 | 75 |

Times are simulation steps measured from release. Unfinished orders are censored at2,000steps, so eventual maximum waiting time is unknown. Throughput selected the candidate. Full event-accounting checks are in [the replay audit](replay-audit.json).
