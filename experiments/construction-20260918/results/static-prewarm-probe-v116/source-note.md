The original four-core V116 probe hardcoded four workers. The published
helper in commit3e319f175859396b91f97364b320c6794a85cbb7 adds a thread argument
for the eight-core follow-up. `probe-source.patch`, applied to that commit,
reconstructs the original frozen `run.py` and `probe.cpp`; their SHA256 values
are recorded in request.json. Production oracle sources remain the exact V113
sources recorded in the request. The probes do not edit or run a live solver.
