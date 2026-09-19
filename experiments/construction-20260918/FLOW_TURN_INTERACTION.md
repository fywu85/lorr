# Refreshed flow threshold and turn-table construction

The margin25 field became an intermediate six-seed reference:136426.2 mean tasks,
+1.364% over paired margin50 controls. Every full total and final1000-step window
improves, and outstanding task age p90 falls on each seed. All12 paired full runs
validate, with exact controls, maximum entry0.7874s and peak RSS14.052GB.
The original two-seed margin25 mean was136424 (+1.419%); the full six-seed
result is the basis for adoption, not the selected single-run peak.

More table construction at margin50 also improves both tested seeds. At counts
32/128 the paired means are134515/136950.5. Its remaining four seeds now confirm a137073.3 mean, winning all six against
both build32/margin50 and build32/margin25; margin50/build128 is now the reference. Neither this two-seed result nor the margin25 gain proves that
the changes combine constructively: both alter routes and subsequent traffic.

The interaction experiment crosses margin50/25 with turn-build32/128, retaining
all other settings. The four constituent/control cases are compared using the
same frozenv39 source and complete5000-step horizon on seeds0/2. Individual
sources, fixed candidate work, cores and deadlines are identical within the matrix.

Screen8898758 passes all four cases. Margin50/build32, margin25/build32 and
margin50/build128 exactly reproduce their prior short-run fingerprints. The new
margin25/build128 combination completes all200 steps with maximum0.879650s
and5.213GB peak RSS. Prefix task totals are not used to rank policies.

Full8898759 with independent analysis8898760 runs all eight cases concurrently,
four disjoint physical cores each,128GiB aggregate reservation and32decimalGB
per-process RSS limit. A late whole decision fails the run. Full controls must
repeat and task totals/final windows/ages must be evaluated before choosing the
combined setting. The selected profile then needs the remaining four seeds.

[Six-seed margin evidence](results/flow-refresh-margin25-six-seed-v36.json),
[explicit reference configuration](warehouse-reference-variants.json),
[interaction screen](results/flow-turn-interaction-screen-v39/),
[interaction configuration](flow-turn-interaction-variants.json).
