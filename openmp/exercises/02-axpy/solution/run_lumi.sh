#!/bin/bash

# SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>
#
# SPDX-License-Identifier: MIT

#SBATCH --job-name=test
#SBATCH --partition=dev-g
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=4
#SBATCH --gpus-per-node=1
#SBATCH --time=00:10:00

set -xeuo pipefail

rm -f ../*.x *.x

cc="cc -fopenmp -O3 -Wall -fsave-loopmark"
ft="ftn -fopenmp -O3 -hlist=m"

mkdir -p data

#########################################################
# OpenMP
#########################################################
for f in *.c; do
    $cc "$f" -o "c-${f%.c}.x"
    mv ${f%.c}.lst data/c-${f%.c}.lst
done
for f in *.F90; do
    [[ $(basename "$f") == "helper_functions.F90" ]] && continue
    $ft "$f" -o "f-${f%.F90}.x"
    sed "s|$(dirname $(readlink -f $f))/||g" ${f%.F90}.lst > data/f-${f%.F90}.lst
done

export CRAY_ACC_DEBUG=2
set +e
for f in *.x; do
    out="data/${f%.x}.out"
    srun -o "$out" "$f"
done
