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

cc="nvc -mp=gpu -O3 -gpu=cc90 -Wall"
ft="nvfortran -mp=gpu -O3 -gpu=cc90 -Wall"

for f in hello-*.c; do
    $cc "$f" -o "${f%.c}.x"
done

mkdir -p data

export NVCOMPILER_ACC_NOTIFY=3
for f in hello-*.x; do
    out="data/${f%.x}.out"
    srun -o "$out" "$f"
    sed -i "s|$(dirname $(readlink -f $f))/||g" "$out"
done
