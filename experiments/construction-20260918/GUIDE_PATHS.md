# Further routing candidate: online guide paths

[Chen et al., AAAI 2024](https://ojs.aaai.org/index.php/AAAI/article/view/30054)
construct congestion-aware guide paths from agents' current goals and update
them online. Their implementation is [Guided-PIBT](https://github.com/nobodyczcz/Guided-PIBT).
This offers a stronger routing hypothesis than freezing executed-traffic counts
once. Their cardinal-move model and ten-second experimental decision limit differ
from LoRR's rotation actions and our one-second requirement; published scores
are not directly comparable to these competition runs. No external code has
been imported.

A possible CGAR adaptation would maintain per-agent route suffixes and counts
of intended directed edge use, then recompute a prescribed batch using current
goals. Opposing-flow and total-load costs would discourage congestion. Ordinary
robots could follow nearby route waypoints using bounded local orientation BFS;
missing or incomplete guidance would use the existing complete unit metric.
The primary, recovery paths, pocket rules, fixed temporal repair and complete
collision validation would remain authoritative.

Before implementation: establish route-count conservation, correct goal/state
invalidation, deterministic work, and a bounded memory layout. Before adoption:
full-horizon deadline, throughput, fairness and memory checks, then six seeds.
This is a research direction, not an implemented feature or measured gain.
