<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Solving two dimensional heat equation with Kokkos

The repository https://github.com/cschpc/heat-equation contains various implementations
for solving the heat equation, including a Kokkos version under **kokkos** subdirectory of the repository.

Clone the repository to a location of your choice with
```
git clone https://github.com/cschpc/heat-equation.git
```
and go to the Kokkos version:
```
cd heat-equation/2d/kokkos
```

Have a look how Kokkos is used in the code, pay special attention how the possible
different data layouts are treated in the context of MPI communication (`core.cpp`).
(Note that this is not necessarily the only or even optimum solution).

Try to build and run the code in the system of your choice (or in both LUMI and Mahti)
by adjusting the `Makefile` as needed. 

Note: the code relies on GPU-aware MPI communication, the following modules need to be loaded when building:

Mahti:
```
module load cuda openmpi/4.1.5-cuda
``` 


Furthermore, in LUMI `MPICH_GPU_SUPPORT_ENABLED` needs to be set when running, *i.e.*
```
export MPICH_GPU_SUPPORT_ENABLED=true
srun ... --nodes=1 --ntasks-per-node=8 --gpus-per-node=8 ./heat_kokkos
```
