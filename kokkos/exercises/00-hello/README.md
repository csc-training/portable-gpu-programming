<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Building and running Kokkos programs

In this exercise you can practice and how to build
applications utilizing Kokkos, and how to build and install Kokkos,

## Task: building Kokkos applications

Build the [hello.cpp](hello.cpp) code by adding the necessary Kokkos
definitions to the provided `CMakeList.txt`.

Try to test CUDA and HIP backends, as well as OpenMP backend (on either of the
supercomputers).

You can start with the existing installations by using the following modules:

In Roihu
```bash
module use /projappl/project_2019754/modulefiles
module load kokkos/cuda # or module load kokkos/omp
```

In LUMI
```bash
module use /projappl/project_462001610/modulefiles
module load kokkos/omp # or module load kokkos/hip
```

Once the `CMakeList.txt` is completed, the code can be configured and build with:
```
cmake -Bbuild-mybackend
cmake --build build-mybackend
```

Note that in LUMI you need to specify the CXX compiler both for HIP and OpenMP.

For HIP:
```
cmake -Bbuild-hip -DCMAKE_CXX_COMPILER=hipcc
cmake --build build-hip
```

For OpenMP in LUMI:
```
cmake -Bbuild-omp -DCMAKE_CXX_COMPILER=CC  # add -DKokkos_ROOT if not using module
cmake --build build-omp
```

## Task: running the application

The hello program here only initializes and finalizes Kokkos, but providing
`--kokkos-print-configuration` command line option you see information about the
current backend:
```
srun ./build-xxx/hello --kokkos-print-configuration
```

See how the output differs between backends

## Bonus task: installing Kokkos

Try to install Kokkos at least on one system and with the one backend along following
instructions in [installing-using.md](../installing-using.md). It is strongly suggested to try
to install Kokkos also on your local workstation.

In order to use your own installation, provide -DKokkos_ROOT when configuring the application:
```
cmake -Bbuild-mycuda -DKokkos_ROOT=/scratch/project_2015315/$USER/kokkos-cuda
cmake --build build-mycuda
```

