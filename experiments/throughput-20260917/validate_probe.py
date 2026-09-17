"""Validate the isolated object using existing regressions and both selection probes."""
from pathlib import Path
import argparse, os, re, shlex, subprocess
ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
parser = argparse.ArgumentParser()
parser.add_argument("--probe", type=Path, default=HERE / "build/v2")
args = parser.parse_args()
build = args.probe.resolve()
base_object = ROOT / "cgar/build/CMakeFiles/cgar_regression.dir/cgar_planner/cgar.cpp.o"
probe_object = build / "cgar.cpp.o"
link_template = shlex.split((ROOT / "cgar/build/CMakeFiles/cgar_regression.dir/link.txt").read_text())
flags = {}
for line in (ROOT / "cgar/build/CMakeFiles/cgar_regression.dir/flags.make").read_text().splitlines():
    if line.startswith("CXX_") and " = " in line:
        key, value = line.split(" = ", 1)
        flags[key] = shlex.split(value)
selection_object = build / "selection_probe.o"
subprocess.run([link_template[0]] + flags["CXX_DEFINES"] + flags["CXX_INCLUDES"] + flags["CXX_FLAGS"] +
               ["-c", str(HERE / "scheduler_selection_probe.cpp"), "-o", str(selection_object)], check=True)

def link(output, backend, test_object=None):
    command = list(link_template)
    command[command.index("-o") + 1] = str(output)
    command[command.index("CMakeFiles/cgar_regression.dir/cgar_planner/cgar.cpp.o")] = str(backend)
    if test_object is not None:
        command[command.index("CMakeFiles/cgar_regression.dir/tests/cgar_regression.cpp.o")] = str(test_object)
    subprocess.run(command, cwd=str(ROOT / "cgar/build"), check=True)

def run(binary, name, settings):
    env = {k: v for k, v in os.environ.items() if not k.startswith("CGAR_")}
    env.update(settings)
    result = subprocess.run([str(binary)], env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                            universal_newlines=True, check=True)
    print("VALIDATION variant=" + name, flush=True)
    print(result.stdout, end="", flush=True)
    return result.stdout

regression = build / "regression"
link(regression, probe_object)
for name, settings in [("sample", {"CGAR_DIAG_GLOBAL_SAMPLES": "64"}),
                       ("hybrid_sample", {"CGAR_DIAG_GLOBAL_SAMPLES": "64", "CGAR_DIAG_HYBRID": "1"})]:
    run(regression, name, settings)
for name, backend, settings, expected in [
    ("production", base_object, {}, 0),
    ("sample", probe_object, {"CGAR_DIAG_GLOBAL_SAMPLES": "64"}, 0),
    ("hybrid_sample", probe_object, {"CGAR_DIAG_GLOBAL_SAMPLES": "64", "CGAR_DIAG_HYBRID": "1"}, 16),
]:
    binary = build / ("selection_probe_" + name)
    link(binary, backend, selection_object)
    output = run(binary, name, settings)
    for fixture in ["CHAIN_CAP", "BUCKET_HRRN"]:
        match = re.search(r"^" + fixture + r" selected=(\d+)", output, re.MULTILINE)
        if match is None or int(match.group(1)) != expected:
            raise RuntimeError("Unexpected {} result for {}".format(fixture, name))
print("All experiment validation checks passed", flush=True)
