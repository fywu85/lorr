# Chain-scored CGAR seeds with rolling-window repair

The earlier RANDOM-05 full comparison scored3065 with the chain-scored root,
2844 after disabling that score,2829 with a fresh20-step window on that weaker
root, and2921 with10 retained actions and a regenerated tail. The latter is
+77 over its appropriate control but still below3065. This motivates testing
the same repair on the stronger root; it does not establish that gains compose.

General opt-in CGAR_WINDOW_CHAIN_SEED=1 requires an enabled window, chain mode1
(score only), and identical wait units. Both stages share the existing complete
immutable chain oracle. Root score remains terminal remaining potential; window
score remains paid actions plus remaining potential. No new search budget,
priority mode, deadline fallback or map-specific objective is introduced.
Default-off behavior and the previous rejection without explicit opt-in remain.

Regression checks complete fixed work, serial/parallel decisions and table
preprocessing, exact disabled and no-op trajectory/RNG identity, repeated
services, refreshed history, actual primary/recovery/pocket/capacity paths and
strict configuration rejection. Existing independent oracle, rolling-window
cost/rollback and deadline-after-improvement tests remain required.

Declared first matrix: RANDOM-05 seed10, full2000 steps,4 physical cores per case,
strict1000ms complete-entry limit,32decimalGB, node-local logs. Control3065 versus
chain1 windows H10keep0,H20keep0,H20keep10 with refreshed tail. All use256attempts
per island,4 islands,2048nodes,group4,wait20,protected first actions, complete
CGAR seed rollout and progress ties. Explicit --trick RANDOM-05 is required for
the unchanged guidance/horizon profile. Other agent's independent solver is
untouched. No throughput result yet.

All four full runs valid: control3065; H10keep0 3008; H20keep0 2896; H20keep10refresh2985. Maximum entry702.67/795.90/766.60/790.87ms. Control repeats its complete selected trace. The gains from repairing the weaker seed do not compose with the stronger chain root. Keep the window disabled on RANDOM05.
