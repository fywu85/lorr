# Nearby CITY lane costs

Declared before benchmark results. CITY02 opposing-price24 improves all three
seeds over16, but32/48 give smaller gains. Test nearby20/28 with the same masks,
forward price4, turn cost, matching policy and fixed planning work. CITY01 gets
an independent20 comparison because both lower prices and24 lost to16. Do not
assume either instance improves. Initial full matrices: CITY01seed2 control16
versus20; CITY02seed4 control24 versus20/28. Four physical cores, strict1000ms,
32decimalGB, full3000steps. Replicate positive signals before claiming robustness.

Only explicit --trick CITY-01/CITY-02/GAME adapted fields admit these costs;
native and other map profiles remain unchanged. All old field hashes and masks
are checked before adding two identities. Existing independent coordinate-field
and real pickup/planning regressions cover the two new values; default16 identity
and the independent high-cost heap check remain required.


Verified results: CITY01 price16/20 =8440/8434; CITY02 price24/20/28 =16839/16254/16611. Keep CITY01price16 and CITY02price24. Controls match prior complete trajectories. The initial verifier omitted legal20/28 from its declaration whitelist; its failures are retained and the same solver outputs passed after that parser-only correction. [Results](first-results.json), [retry provenance](verification-retry.json).
