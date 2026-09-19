The first v25 build compiled successfully but its new refinement fixture failed.
The fixture incorrectly assumed a detour saved four actions. With arbitrary goal
orientation the route has three turns and two extra forward moves, saving five.
The corrected assertion derives the previous complete route length before the
other flow retires and compares it with the independently checked six-action
shortest route. No production algorithm was changed to resolve this test error.
This failed build was never used for benchmarks.
