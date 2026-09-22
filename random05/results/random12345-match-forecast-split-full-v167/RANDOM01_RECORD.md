# RANDOM-01 forecast radius8: selected740

The original seed0run at radius8 scores740, versus736 with forecasts disabled;
seed1scores739with or without forecasts. Both complete600steps with strict
checks and independent replay. Source6074498c, mean72.242/max115.926ms,
RSS393.945MB; finished2026-09-22T04:24:36.347075+00:00. The initial two pairs
total1479versus1475 (+0.2712%); one gain and one tie. Smaller radii2/4 lose
in paired total. This is a small selected improvement, not a broad transfer claim.

The frozen exact repeat and three additional pairs2/3/4 are in the record740
batch. General01radius4 loses727to721; general04 loses1616to1510; general03
loses1634to1628; selected03 loses2646to2625. General02radius4fails step2at
1016.576ms; retain it. General05remains pending in this partial audit.

[Independent completed-run audit](audit.json), [completed off controls](partial-control-equivalence.json),
[actual sampled forecast activity](forecast-activity.json),
[qualification protocol](../../experiments/random01-record740-v167-protocol.json).
