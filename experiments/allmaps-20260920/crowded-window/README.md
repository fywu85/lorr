# Crowded-map check of the existing CGAR-seeded window

Longer joint forecasts and retained-tail refresh helped sparse RANDOM01and
RANDOM03. Common-future selection over regional roots instead loses on both
crowded cases. Test the already-qualified window layer on crowded maps before
assuming that its sparse behavior carries over. This remains CGAR primary/recovery
plus a protected ordinary-path repair layer, not PILOT's two-phase pipeline.

RANDOM04seed0: replay the no-future root control(previous1999), then horizon10
without history, horizon20without history, and horizon20with retained10and a
complete refreshed tail. All use wait20,256attempts per island, four islands,
2048nodes/search, protected real first actions and complete joint seed extension.
The selected global-future2059remains the separate frontier unless surpassed.

RANDOM05seed10: selected chain-score3065control, chain-score-off control, then
horizon20without history and horizon20with retained10/refreshedtail on that
chain-score-off base. The existing window parser rejects simultaneous chain mode;
its own exact-chain repair objective is not presented as an isolated addition to
the selected chain-score root. Same work limits as RANDOM04. No changes to the
separate random05 algorithm or its streams.

Full1000/2000steps, strict1000ms wholeentry,4physicalcores/case,32decimalGB.
Explicit --trick RANDOM04/05 (actual CLI names contain the hyphen). Node-local
logspooling; source9b0d0036, all40source/testhashes and fullregressionqualified.
No early exit on a useful result; timeouts fail the case.
