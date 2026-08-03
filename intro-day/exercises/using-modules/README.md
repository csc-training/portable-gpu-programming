<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Using modules and compiling

Typically, supercomputers have available a default set of compilers / modules,
but in order to use some programming approaches a specific module environment
might need to be loaded. In this exercise you will compile a simple code using
OpenMP offloading, which both in Lumi and Mahti requires non-standard set of modules.

## LUMI

In order to use OpenMP offloading targeting AMD GPUs, execute the following module commands:
```
module load LUMI/24.03  #  or "ml LUMI/24.03", shorthand for "module load"
module load partition/G
module load rocm/6.0.3
```
Verify the module environment with
```
module list #  or "ml", as such shorthand for module list
```
Compile the code [poisson.c](poisson.c) with
```
cc -O3 -fopenmp poisson.c -o poisson
```
and try to run it via Slurm on a single GPU

## Mahti
§
In order to use OpenMP offloading targeting NVIDIA GPUs, execute the following module commands:
```
ml purge  # clean up the default environment
ml use /appl/opt/nvhpc/modulefiles  # take the non-standard NVIDIA modules into use
ml nvhpc-hpcx-cuda12/25.1
```
Verify the module environment with
```
module list #  or "ml", as such shorthand for module list
```
Compile the code [poisson.c](poisson.c) with
```
nvc -mp=gpu -gpu=cc80 poisson.c -o poisson
```
and try to run it via Slurm on a single GPU



