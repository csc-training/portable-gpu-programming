#!/bin/bash

# SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>
#
# SPDX-License-Identifier: MIT

#SBATCH --job-name=test
#SBATCH --partition=gpumedium
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=4
#SBATCH --gres=gpu:gh200:1
#SBATCH --time=00:10:00

module purge
module load nvhpc/26.3

set -xeuo pipefail

rm -f ../*.x *.x

cc="nvc -mp=gpu -O3 -gpu=cc90 -Wall -Minfo=mp"
ft="nvfortran -mp=gpu -O3 -gpu=cc90 -Wall -Minfo=mp"

mkdir -p data

for f in *.c; do
    $cc "$f" -o "c-${f%.c}.x" 2> "data/c-${f%.c}.lst"
done
for f in *.F90; do
    [[ $(basename "$f") == "axpy_helper_functions.F90" ]] && continue
    $ft "$f" -o "f-${f%.F90}.x" 2> "data/f-${f%.F90}.lst"
done

export NVCOMPILER_ACC_NOTIFY=3
set +e
for f in *.x; do
    out="data/${f%.x}.out"
    srun -o "$out" "$f"
    sed -i "s|$(dirname $(readlink -f $f))/||g" "$out"
done
