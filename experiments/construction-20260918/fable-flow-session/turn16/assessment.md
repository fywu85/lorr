# Local assessment of Fable turn16

Successful read-only Fable5.1/max review in the same session, completed2026-09-19T22:57:25.273155UTC.
No tools or tests were run by Fable; no permission denials. Reported78.0940595USD
is cumulative session cost. It accepted the prior three source-based corrections.

Frozen typical/slow decision replay would provide more representative profiling
than the small synthetic probe. A debug rebuild must have its text hash checked,
not merely assumed equivalent; patched seed paths would also need capture, rather
than rebuilding every path solely from geometry. There is no perf warehouse data.

Colocated vertex/edge reservations preserve the slot values and scan order. The
canonical edge encoding is indeed2*min(endpoint)+axis. An isolated patch was tested
with30alternating pairs per synthetic mode; all checksums/counters match, but median
ratios candidate/baseline are1.0073 global,0.9976regional,1.0060warm. This supplies no
meaningful speedup; do not adopt on the strength of the source hypothesis alone.

The proposed skip for some redundant edge lookups requires proving the invariant
at every stable recursion state: all resident robots have complete reservations,
and same-direction edge owners were already seen in the preceding vertex slot.
It is not implemented. Huge-page, setup-copy and depth-cache changes are also only
hypotheses. No cache/TLB counters justify them yet. Expected100ms recovery and
'certain' setup savings remain estimates, not measured facts.

The component clock check adds~4.59% to median regional kernel time, with unchanged
search checksums. This is not a bound on warehouse overhead or on the overrun from
less frequent checks. In particular, a sub-millisecond maximum delay cannot be
promised without bounding the work between checks. Complete-entry1s enforcement
and explicit timeout remain unchanged. Fixed regional candidate caps would be a
separate work-policy experiment, requiring full quality validation.
