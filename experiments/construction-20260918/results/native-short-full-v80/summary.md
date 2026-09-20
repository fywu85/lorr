# TRICK: native short preference loses on both seeds

All eight full5000-step/10000-robot cases pass40kcomplete entries, source85faca5,
32decimalGB RSS/shared5s deadline; all four ordinary-fair controls exactly reproduce
V78. Short preference only removes HRRN aging and oldest admission; started tasks
stay protected. Generic reference and best152383 are unchanged.

| Pickup weight | Seed | Ordinary fairness | Short preference | Difference | Final1000 difference |
|---|---:|---:|---:|---:|---:|
|1|0|151901|148256|-3645|-1708|
|1|2|152066|148397|-3669|-1742|
|5|0|152383|151870|-513|-1071|
|5|2|152048|151804|-244|-922|

Paired means fall2.4062% atweight1 and0.24866% atweight5. The initial1000 steps
improve, but all later1000-step windows lose: startup ranking would have misled us.
Atweight5, completed shortest chains shrink by6.559/6.642 steps, while empty travel
rises by7.201/7.048 per completed task. Outstanding-agep90 rises3388ticks on both.
These trajectory/cohort associations are not an exact causal decomposition.

Keep short preference OFF. V85 saved motion audit verifies400Mrobotsteps and all
input/field/phase fixtures. The separate budget shadow remains read-only. The next
policy test is new-admission configured-horizon preference under ordinary fairness.
No strict1s certification is claimed.

The reproduced best-profile controls on source85faca5 take444.713/441.612ms mean,
987.472/971.894ms maximum,1.504/1.511averageCPUcores of4reserved and11.879/11.918GB
RSS. These observed times are below1s in this shared5s run, but this is not formal
exclusive-host/one-second certification or a controlled runtime-speedup claim.
