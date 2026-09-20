Continue the SAME Fable5.1/max session. Read-only design review. No tools/unperformed tests.

NEW USER SCOPE: keep generic improvement separate from map-specific tricks. If closing the generic gap is hard, map-specific tricks from NMS/KK/beyond ARE NOW AUTHORIZED, but every implementation must be gated solely by the CLI --trick <map-instance-name> (e.g. --trick WAREHOUSE), with [trick] in commit titles and TRICK in the log. No flag => generic policy. Separate frontiers and explicit assumptions. 1s complete decisions,32decimalGB,isolatedCPU,nohiddenfuturetasks remain. This supersedes our earlier absolute ban on copying lane masks/warehouse weights. No trick implemented yet.

The complete KK 2x2 has finished: supplied+rematch152872 (repeat152786), supplied+keep142510, uniform+rematch24574, uniform+keep23620. All five have exactly1entrytimeout, zero planner/scheduleerrors, RSS38.18-38.85GB. These are competitor diagnostics, never qualified scores. Four action fixtures, all5 50M-step accounting identities, no after-pickup reassignment, keepmode zero reassignments, hashes/configs/source/bindings verified. Timeout action markers included separately. NMS's initial missing-schedule issue does not recur here: the frozen pre-fix decoder passes for allKKcases. Pool follow-up still running.

The huge uniform-weight collapse is a whole-policy intervention, not a pure guidance causal effect: shared costs alter scheduling/task cohorts, geometry and clock-selected work. No retuning of uniform policy. With supplied guidance, keep leaves loaded detours/turns/waits similar but all empty steps/completion rise65.73->88.85; completed-chain233.83->235.01. Removing rematching is not the main explanation of the uniform collapse. Uniform+rematch loaded waits300.27,turns72.83,detours35.11,unfinishedloaded890.30/completion, despite roughly40.9k merged LNS iteration records vs21.1k supplied. Per-step planner budget broadlysimilar; records are not all speculative attempts. Formal control spread not claimed because everynewcontrolhas1timeout.

CGAR32snapshotearly67/134screen groups:16root+1nonefeasible;16root+2twofeasible,onescalargain,nonebothscalar+fullaggregatepotentialgain. Complete enumeration with outsidefixed, sixfixturespass. Earlysampleonly; full seeds0/2capturing now, every decision mustpass. Generic bestsingle144510,sixmean143941.1667 unchanged.

Please prioritize ONE next generic change and ONE first isolated trick experiment, with minimal implementation and correct ablation design. In particular:
- Given KK fullaccounting, how much does this shift priority toward generic guidance coherence versus assignment? Don't claim 'most gap outsidekernel' from these unpaired policies. Existingfailedrouteguide andpurecostvariants remainnegativeevidence.
- For firsttrick, NMS's warehouse lane directions/bands are simple, but exactnativecosts20/200forward,1turn (bands+1) differ fromCGAR's score units. CurrentCGARbaseunit4,turn4,oppreference*4; weightedforward cost is paid in TemporalGeometry::forward_surcharge, while NMS scores remaining weighted potential without the same paidforwardextra. Merely copyingweights is NOT identicalpolicy. Which factorization first: NMS lane DIRECTIONS at our boundedcosts4/16 (withoutclaimingexactNMS), versus exact20/200costs andscaledunits, versusKK suppliedfield? Keep assignmentnativeatfirst to isolate asfaraspossible; pickupquotes sharingguidance is a confound unlessexplicitseparated.
- CurrentTurnDistanceOracle stores uint8_t forwardcosts, requiresbase<=cost<=16, constantturn<=16, builds complete tables usingDialbuckets(maxedge+1). Stored TurnTable autochoosesuint16 onlywhen allfinitevalues<65535, otherwiseint32. Init forwardbase1..16. CGARflow_scale4needsflow_strengthnonzero; pickupflowalso gatedonobservedflowconfig. A staticfield mustnotbeoverwrittenbygenericpublication, mustpreservecostconsistencyandwholecacheinvalidations. Avoida riskylargeoracle/refactortocompletefirsttrick. Explainunitsandwhichtermschanged.
- Any temporaryexperimentknob is subordinate to --trick WAREHOUSE; environment-only activation notallowed. Mismatched/unknowninstance shouldfailclearly. Keepmap-specificcodeisolated and testnoflagtrajectoryidentity andexplicitflagreceipt.
- Assignmentdesignstillordinaryschedulefirst thenexistingeligibleholdersforretarget-only; prepassallocatingfreerobotsconfoundsarm. Retainfinitecaps/progressprotections. Endpoint-holder merit subtractsoldremainingcost; eventualpickupnotacausalbound.

Verified fullKK rows:

[
  {
    "case": {
      "name": "supplied_rematch-r0",
      "guidance": "supplied",
      "rematch": 1,
      "repeat": 0
    },
    "result": {
      "teamSize": 10000,
      "numTaskFinished": 152872,
      "makespan": 5000,
      "numPlannerErrors": 0,
      "numScheduleErrors": 0,
      "numEntryTimeouts": 1
    },
    "peak_rss_bytes": 38842884096,
    "per_completed_task": {
      "completed_chain_lower_bound": 233.83356664398974,
      "completed_forward_detour": 4.601235020147574,
      "completed_turns": 5.684625045789942,
      "completed_waits": 8.543735935946414,
      "completed_other": 0.052095871055523574,
      "empty_including_unfinished": 65.73384269192528,
      "loaded_unfinished": 8.621912449631063,
      "idle": 0.0
    },
    "diagnostic_summaries": {
      "entry": {
        "scheduler_ms": {
          "n": 4999,
          "mean": 25.752227145829167,
          "median": 23.0114,
          "p99": 49.5116,
          "min": 0.01201,
          "max": 89.365
        },
        "elapsed_before_diag_ms": {
          "n": 4999,
          "mean": 954.3232370474095,
          "median": 948.37,
          "p99": 951.433,
          "min": 946.015,
          "max": 30579.5
        }
      },
      "lns": {
        "limit_s": {
          "n": 4999,
          "mean": 0.9137073414682937,
          "median": 0.916,
          "p99": 0.922,
          "min": 0.85,
          "max": 0.939
        },
        "remaining_before_s": {
          "n": 4999,
          "mean": 0.8735181692338467,
          "median": 0.898566,
          "p99": 0.908718,
          "min": 0.740266,
          "max": 0.927699
        },
        "iteration_records": {
          "n": 4999,
          "mean": 21141.902380476095,
          "median": 21066,
          "p99": 30570,
          "min": 10875,
          "max": 43463
        },
        "failures": {
          "n": 4999,
          "mean": 18852.77195439088,
          "median": 18810,
          "p99": 28214,
          "min": 8421,
          "max": 41671
        }
      },
      "rematching": {
        "agents": {
          "n": 25,
          "mean": 2271,
          "median": 1926,
          "p99": 10000,
          "min": 1746,
          "max": 10000
        },
        "tasks": {
          "n": 25,
          "mean": 7271,
          "median": 6926,
          "p99": 15000,
          "min": 6746,
          "max": 15000
        }
      }
    }
  },
  {
    "case": {
      "name": "supplied_keep-r0",
      "guidance": "supplied",
      "rematch": 0,
      "repeat": 0
    },
    "result": {
      "teamSize": 10000,
      "numTaskFinished": 142510,
      "makespan": 5000,
      "numPlannerErrors": 0,
      "numScheduleErrors": 0,
      "numEntryTimeouts": 1
    },
    "peak_rss_bytes": 38358962176,
    "per_completed_task": {
      "completed_chain_lower_bound": 235.00688372745773,
      "completed_forward_detour": 4.581867939091993,
      "completed_turns": 5.636474633359063,
      "completed_waits": 7.964914742825065,
      "completed_other": 0.05314714756859168,
      "empty_including_unfinished": 88.85158234509859,
      "loaded_unfinished": 8.757701213949899,
      "idle": 0.0
    },
    "diagnostic_summaries": {
      "entry": {
        "scheduler_ms": {
          "n": 4999,
          "mean": 15.002686713542708,
          "median": 11.6628,
          "p99": 40.863,
          "min": 0.01186,
          "max": 91.0383
        },
        "elapsed_before_diag_ms": {
          "n": 4999,
          "mean": 953.4739583916784,
          "median": 948.404,
          "p99": 950.451,
          "min": 946.358,
          "max": 26169.4
        }
      },
      "lns": {
        "limit_s": {
          "n": 4999,
          "mean": 0.9241006201240248,
          "median": 0.927,
          "p99": 0.936,
          "min": 0.849,
          "max": 0.939
        },
        "remaining_before_s": {
          "n": 4999,
          "mean": 0.8821657839567913,
          "median": 0.906466,
          "p99": 0.921489,
          "min": 0.735415,
          "max": 0.927405
        },
        "iteration_records": {
          "n": 4999,
          "mean": 21957.53570714143,
          "median": 22441,
          "p99": 32102,
          "min": 11638,
          "max": 45215
        },
        "failures": {
          "n": 4999,
          "mean": 19731.661332266453,
          "median": 20321,
          "p99": 29587,
          "min": 8724,
          "max": 42863
        }
      },
      "rematching": {
        "agents": {
          "n": 25,
          "mean": 429.84,
          "median": 31,
          "p99": 10000,
          "min": 19,
          "max": 10000
        },
        "tasks": {
          "n": 25,
          "mean": 5429.84,
          "median": 5031,
          "p99": 15000,
          "min": 5019,
          "max": 15000
        }
      }
    }
  },
  {
    "case": {
      "name": "uniform_rematch-r0",
      "guidance": "uniform",
      "rematch": 1,
      "repeat": 0
    },
    "result": {
      "teamSize": 10000,
      "numTaskFinished": 24574,
      "makespan": 5000,
      "numPlannerErrors": 0,
      "numScheduleErrors": 0,
      "numEntryTimeouts": 1
    },
    "peak_rss_bytes": 38816677888,
    "per_completed_task": {
      "completed_chain_lower_bound": 189.3455684870188,
      "completed_forward_detour": 35.10580288109384,
      "completed_turns": 72.83132579148693,
      "completed_waits": 300.2651176039717,
      "completed_other": 0.18438186701391715,
      "empty_including_unfinished": 546.6391714820542,
      "loaded_unfinished": 890.2994221534956,
      "idle": 0.0
    },
    "diagnostic_summaries": {
      "entry": {
        "scheduler_ms": {
          "n": 4999,
          "mean": 27.332785349269855,
          "median": 26.0842,
          "p99": 52.8357,
          "min": 0.01216,
          "max": 91.3787
        },
        "elapsed_before_diag_ms": {
          "n": 4999,
          "mean": 951.0644584916984,
          "median": 950.1,
          "p99": 953.236,
          "min": 947.012,
          "max": 5618.34
        }
      },
      "lns": {
        "limit_s": {
          "n": 4999,
          "mean": 0.9121556311262252,
          "median": 0.913,
          "p99": 0.939,
          "min": 0.848,
          "max": 0.939
        },
        "remaining_before_s": {
          "n": 4999,
          "mean": 0.8771048071614322,
          "median": 0.895475,
          "p99": 0.921593,
          "min": 0.736346,
          "max": 0.9275
        },
        "iteration_records": {
          "n": 4999,
          "mean": 40871.77715543109,
          "median": 41047,
          "p99": 47284,
          "min": 20603,
          "max": 53352
        },
        "failures": {
          "n": 4999,
          "mean": 40127.77355471094,
          "median": 40438,
          "p99": 46696,
          "min": 18313,
          "max": 52741
        }
      },
      "rematching": {
        "agents": {
          "n": 25,
          "mean": 2927.8,
          "median": 2658,
          "p99": 10000,
          "min": 1867,
          "max": 10000
        },
        "tasks": {
          "n": 25,
          "mean": 7927.8,
          "median": 7658,
          "p99": 15000,
          "min": 6867,
          "max": 15000
        }
      }
    }
  },
  {
    "case": {
      "name": "uniform_keep-r0",
      "guidance": "uniform",
      "rematch": 0,
      "repeat": 0
    },
    "result": {
      "teamSize": 10000,
      "numTaskFinished": 23620,
      "makespan": 5000,
      "numPlannerErrors": 0,
      "numScheduleErrors": 0,
      "numEntryTimeouts": 1
    },
    "peak_rss_bytes": 38180204544,
    "per_completed_task": {
      "completed_chain_lower_bound": 197.6570702794242,
      "completed_forward_detour": 45.82607959356478,
      "completed_turns": 92.36845893310753,
      "completed_waits": 341.02497883149874,
      "completed_other": 0.20224386113463166,
      "empty_including_unfinished": 687.0063505503811,
      "loaded_unfinished": 752.7649449618967,
      "idle": 0.0
    },
    "diagnostic_summaries": {
      "entry": {
        "scheduler_ms": {
          "n": 4999,
          "mean": 11.592269675535107,
          "median": 8.83453,
          "p99": 38.1297,
          "min": 0.01381,
          "max": 88.9719
        },
        "elapsed_before_diag_ms": {
          "n": 4999,
          "mean": 954.9423668733747,
          "median": 949.043,
          "p99": 952.065,
          "min": 947.181,
          "max": 30301
        }
      },
      "lns": {
        "limit_s": {
          "n": 4999,
          "mean": 0.9274690938187637,
          "median": 0.93,
          "p99": 0.939,
          "min": 0.851,
          "max": 0.939
        },
        "remaining_before_s": {
          "n": 4999,
          "mean": 0.8927440434086817,
          "median": 0.912331,
          "p99": 0.923994,
          "min": 0.742516,
          "max": 0.927262
        },
        "iteration_records": {
          "n": 4999,
          "mean": 38155.21524304861,
          "median": 38725,
          "p99": 44918,
          "min": 23839,
          "max": 52175
        },
        "failures": {
          "n": 4999,
          "mean": 37487.28625725145,
          "median": 38092,
          "p99": 44275,
          "min": 22265,
          "max": 51597
        }
      },
      "rematching": {
        "agents": {
          "n": 24,
          "mean": 421.0416666666667,
          "median": 3.0,
          "p99": 10000,
          "min": 1,
          "max": 10000
        },
        "tasks": {
          "n": 24,
          "mean": 5421.041666666667,
          "median": 5003.0,
          "p99": 15000,
          "min": 5001,
          "max": 15000
        }
      }
    }
  },
  {
    "case": {
      "name": "supplied_rematch-r1",
      "guidance": "supplied",
      "rematch": 1,
      "repeat": 1
    },
    "result": {
      "teamSize": 10000,
      "numTaskFinished": 152786,
      "makespan": 5000,
      "numPlannerErrors": 0,
      "numScheduleErrors": 0,
      "numEntryTimeouts": 1
    },
    "peak_rss_bytes": 38854623232,
    "per_completed_task": {
      "completed_chain_lower_bound": 233.80619297579622,
      "completed_forward_detour": 4.605958661133873,
      "completed_turns": 5.710032332805362,
      "completed_waits": 8.671232966371264,
      "completed_other": 0.052007382875394345,
      "empty_including_unfinished": 65.7944314269632,
      "loaded_unfinished": 8.615259251502101,
      "idle": 0.0
    },
    "diagnostic_summaries": {
      "entry": {
        "scheduler_ms": {
          "n": 4999,
          "mean": 24.396967906981395,
          "median": 22.1877,
          "p99": 47.2886,
          "min": 0.0139,
          "max": 114.896
        },
        "elapsed_before_diag_ms": {
          "n": 4999,
          "mean": 953.1788055611122,
          "median": 948.118,
          "p99": 949.65,
          "min": 946.403,
          "max": 26068
        }
      },
      "lns": {
        "limit_s": {
          "n": 4999,
          "mean": 0.9150428085617124,
          "median": 0.917,
          "p99": 0.923,
          "min": 0.825,
          "max": 0.939
        },
        "remaining_before_s": {
          "n": 4999,
          "mean": 0.876398900780156,
          "median": 0.9014,
          "p99": 0.909616,
          "min": 0.703132,
          "max": 0.928252
        },
        "iteration_records": {
          "n": 4999,
          "mean": 22662.55451090218,
          "median": 22788,
          "p99": 32138,
          "min": 12182,
          "max": 39369
        },
        "failures": {
          "n": 4999,
          "mean": 20276.662932586518,
          "median": 20537,
          "p99": 29471,
          "min": 9646,
          "max": 37421
        }
      },
      "rematching": {
        "agents": {
          "n": 25,
          "mean": 2261.32,
          "median": 1895,
          "p99": 10000,
          "min": 1734,
          "max": 10000
        },
        "tasks": {
          "n": 25,
          "mean": 7261.32,
          "median": 6895,
          "p99": 15000,
          "min": 6734,
          "max": 15000
        }
      }
    }
  }
]
