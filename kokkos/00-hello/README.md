<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Building and running Kokkos programs

## Installing Kokkos

Start by installing Kokkos at least on one system and with the one backend along following
instructions in [installing-using.md](../installing-using.md). It is strongly suggested to try
to install Kokkos also on your local workstation.

For systems/backends you did not perform the installation yourself, you may use the modules:

In LUMI
```bash
module use /scratch/project_462001074/modulefiles
module load kokkos/omp # or module load kokkos/hip
```

In Mahti
```bash
module use /scratch/project_2015315/modulefiles
module load kokkos/omp # or module load kokkos/cuda
```

## Building Kokkos applications

Build the [hello.cpp](hello.cpp) code with the provided `CMakeList.txt`.
Try to test HIP and CUDA backends, as well as OpenMP backend (on either of the
supercomputers or on your own laptop).

Note that in LUMI you need to specify the CXX compiler both for HIP and OpenMP.

For HIP:
```
cmake -Bbuild-hip -DCMAKE_CXX_COMPILER=hipcc  # add -DKokkos_ROOT if not using module
cmake --build build-hip
```

For LUMI:
```
cmake -Bbuild-omp -DCMAKE_CXX_COMPILER=CC  # add -DKokkos_ROOT if not using module
cmake --build build-omp
```

## Running the application

The hello program here only initializes and finalizes Kokkos, but providing
`--kokkos-print-configuration` command line option you see information about the
current backend:
```
srun ./build-xxx/hello --kokkos-print-configuration
```

See how the output differs between backends

