<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Installing and using Kokkos

## LUMI

It is suggested that you work under the scratch directory:
```
cd /scratch/project_462001074/$USER
git clone -b 4.7.01 https://github.com/kokkos/kokkos.git kokkos-src
cd kokkos-src
```

Build and install a CPU version (OpenMP backend):
```
cmake -Bbuild-omp -DCMAKE_BUILD_TYPE=Release \
                  -DCMAKE_CXX_COMPILER=CC \
                  -DKokkos_ENABLE_OPENMP=ON \
                  -DKokkos_ARCH_NATIVE=ON
cmake --build build-omp -j4
cmake --install build-omp --prefix /scratch/project_462001074/$USER/kokkos-omp
```

Build and install a GPU version
```
module load rocm craype-accel-amd-gfx90a
cmake -Bbuild-hip -DCMAKE_BUILD_TYPE=Release \
                  -DCMAKE_CXX_COMPILER=hipcc \
                  -DKokkos_ENABLE_HIP=ON \
                  -DKokkos_ARCH_AMD_GFX90A=ON
cmake --build build-hip -j4
cmake --install build-hip --prefix /scratch/project_462001074/$USER/kokkos-hip
```

Building an application with Kokkos OpenMP backend (for CPUs):
```
cmake -Bbuild-omp -DKokkos_ROOT=/scratch/project_462001074/$USER/kokkos-omp \
                  -DCMAKE_CXX_COMPILER=CC
...
```
Building an application with Kokkos HIP backend (for GPUs):
```
cmake -Bbuild-hip -DKokkos_ROOT=/scratch/project_462001074/$USER/kokkos-hip \
                  -DCMAKE_CXX_COMPILER=hipcc
...
```

Note that GPU modules cannot be loaded when building with OpenMP 
backends, *i.e.* if you want to move from HIP to OpenMP you must do
```
module unload rocm craype-accel-amd-gfx90a
```

### Using existing Kokkos installation

We suggest that you try installing at least one backend yourself, but during the
course it is possible to use also existing installation:

```bash
module use /scratch/project_462001074/modulefiles
module load kokkos/omp # or module load kokkos/hip
```
With the modules, `-DKokkos_ROOT` does not need to be specified when building applications.

## Mahti

It is suggested that you work under the scratch directory:
```
cd /scratch/project_2015315/$USER
git clone -b 4.7.01 https://github.com/kokkos/kokkos.git kokkos-src
cd kokkos-src
```

Build and install a CPU version (OpenMP backend):
```
cmake -Bbuild-omp -DCMAKE_BUILD_TYPE=Release \
                  -DKokkos_ENABLE_OPENMP=ON \
                  -DKokkos_ARCH_NATIVE=ON
cmake --build build-omp -j4
cmake --install build-omp --prefix /scratch/project_2015315/$USER/kokkos-omp
```

Build and install a GPU version
```
module load gcc/10.4.0 cuda/12.6.1
cmake -Bbuild-cuda -DCMAKE_BUILD_TYPE=Release \
                   -DKokkos_ENABLE_CUDA=ON \
                   -DKokkos_ARCH_AMPERE80=ON
cmake --build build-cuda -j4
cmake --install build-cuda --prefix /scratch/project_2015315/$USER/kokkos-cuda
```

Building an application with Kokkos OpenMP backend (for CPUs):
```
cmake -Bbuild-omp -DKokkos_ROOT=/scratch/project_2015315/$USER/kokkos-omp 
...
```

Note that on Mahti one does not need to specify the C++ compiler, but defaults
work fine.

Building an application with Kokkos CUDA backend (for GPUs):
```
cmake -Bbuild-cuda -DKokkos_ROOT=/scratch/project_2015315/$USER/kokkos-cuda
...
```

### Using existing Kokkos installation

We suggest that you try installing at least one backend yourself, but during the
course it is possible to use also existing installation:

```bash
module use /scratch/project_2015315/modulefiles
module load kokkos/omp # or module load kokkos/cuda
```
With the modules, `-DKokkos_ROOT` does not need to be specified when building applications.

