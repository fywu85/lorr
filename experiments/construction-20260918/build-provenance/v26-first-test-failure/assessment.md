The first v26 build passes all new waypoint/window regressions and the original
guide-route suite, but fails the existing nearby-reconnection fixture. This is
an integration defect: descending the new route-to-go metric may follow a
parallel row until a distant rejoin, exceeding the connector limit even when a
short nearby connector exists. The fix searches explicitly for a shortest
aligned connector within the same prescribed action count and local box.
The original reconnection assertion is retained. No benchmark used this failed
build. The standalone scoring probe still independently reproduces and fixes
the old single-waypoint ranking defect.
