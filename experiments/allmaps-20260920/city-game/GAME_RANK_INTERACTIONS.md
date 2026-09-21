# GAME guidance and dispatch interactions after14664

Fixed squared remaining-chain ranks now compare a2x2: adapted4/16 versus
native20/200/turn1/pure-potential, and existing HRRN1/pickup1 versus the direct
pickup4/HRRN0/matching64 bundle. Reproduce adapted14664 exactly. The earlier
native field lost with equal ranks; its behavior with strong priorities is a
separate question. The11146 scheduling gain on equal weights is not assumed to
add to the new ranking gain. All four explicit `--trick GAME`, full5000steps,
seed0, strict1000ms,32decimalGB, four physical cores per case. Sourcea573aa5,
binary4d4c1225. Retain forced-oldest admission and CGAR protections. No horizon,
fleet cap, promise, or change to fixed search work. Record waiting tails.
