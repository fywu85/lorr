# TRICK: pickup emphasis with static lanes and bounded matching

V65 improved lanes-only to147,328/147,204 by reducing empty trips; completed chain
length stayed near242.25cells. Short preference plus matching completes shorter
chains (~235.1cells) but spends82.1–82.2empty steps per completion and remains below
lanes+matching. This tests whether NMS-inspired pickup weight5 can reduce that
assignment tradeoff on the current static-lane/matching policy.

Four arms: pickup weight1/5 crossed with short preference OFF/ON, all static lanes
ON and matching64 ON, seeds0/2, full5,000steps. The exact frozen V65 binary is used;
there is no production code change. Weight1 controls must exactly reproduce V65.
Remaining-flow and mixed pickup grouping stay OFF; fixed search work, retarget
protections and4reservedphysicalcores per case stay unchanged. Every arm requires
`--trick WAREHOUSE` and is labelled TRICK. Started tasks remain protected.

This is not a reproduction of NMS's whole scheduler or its numeric chain metric.
Older pickup5/direct-cost CGAR experiments collapsed on seed2; static lanes and
later scheduling/motion changes justify one controlled retest, not a prior gain
claim. Full totals, final1,000steps, empty travel, ages and valid resource use decide
the result. Shared5s development deadline and32GB processRSS; timeout is failure.
