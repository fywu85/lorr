# Selected 4,302 qualification: incomplete, with a preserved failure

The disabled-credit seed3 control failed at step0 after **1,045.313ms**, exit124.
It has no score. Assignment took498.259ms (Hungarian469.424ms) and look-ahead
522.687ms. The cause of the changed latency is unproven. Its earlier source162
run succeeded, but that historical score does not replace this failed pair.

The other six original runs are pending. The selected4,302 run itself remains
independently verified; this qualification cannot currently support a complete
four-pair strict-valid result. All original attempts remain preserved.

Separate startup-budget follow-ups use credit.5 on seeds0/3, FIRST_K5952/4032
and unchanged laterK16320. Those are different declared configurations, never
substitutions into the original protocol. [Failure details](failure-details.json),
[partial source/runtime audit](audit.json).
