# Cancelled protection-defect trial

V62 full matching benchmark8899360 and held analysis8899361 were cancelled
before interpreting any quality results. Fable found a stale-ticket next-primary
protection defect; local review found missing fair-admission registration for
matching. These affect the experimental matching path. The confirmed solver has
matching disabled. Partial trajectories are retained only under ignored runs/.
No partial task count is a valid quality score.

Production regression build8899362 reproduced both defects. Its extra positive
fresh-holder fixture initially also failed because the fixture placed the primary
behind a robot already at its pickup, creating a pending-move interaction. That
fixture is corrected by moving the primary to a separate row. This correction
is independent of the two reproduced protection defects.

Fix build8899365 runs the complete suite after separating existing-holder ticket
selection from fresh-task eligibility and registering mandatory fair admissions.
