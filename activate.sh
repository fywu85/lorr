# source this: activates the LoRR 2024 build env (conda-forge, GCC 11 / Boost 1.74)
eval "$(conda shell.bash hook)"
conda activate "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/env"
