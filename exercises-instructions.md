<!--
SPDX-FileCopyrightText: 2021 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

### Repository structure

The exercise assignments are provided in various `README.md`s.
For most of the exercises, some skeleton codes are provided as starting point. In addition, all of the exercises have exemplary full codes
(that can be compiled and run) in the `solutions` folder. **Note that these are
seldom the only or even the best way to solve the problem.**


## Performance analysis tools

You may use performance analysis tools to get a visual insight into host (CPU) - device (GPU) interaction, as well as
look for potential performance issues.

### Using rocprof and omnitrace in LUMI

[rocprof](https://rocm.docs.amd.com/projects/rocprofiler/en/latest/index.html) is the standard AMD tool for getting a flat profile and trace about GPU activity. Simple usage:
```
module load rocm
srun rocprof --hip-trace my_exe
```
Flat profile is by default in a file `results.stats.csv`, and the trace in a file `results.json`. The trace can be analyzed via a web
browser in your local workstation as follows:

1. In LUMI, find out the full path name of the `results.json`
```
realpath results.json
```
2. Copy the trace file to your workstation e.g. with `rsync` (you can copy-paste the output of the previous command):
```
rsync <username>@lumi.csc.fi:<full_path_to_results.json>
```
3. In your local workstation, go with a web browser to <https://ui.perfetto.dev>, click "Open trace file", and select the 
`results.json` file. You can get a brief help about keyboard shortcuts and mouse controls by pressing `?` in Perfetto.

[Omnitrace](https://rocm.docs.amd.com/projects/omnitrace/en/docs-6.2.4/index.html) is another AMD tool that is able to provide 
information also about CPU activities in addition to GPU. Omnitrace is not available by default, but user needs to install 
it themself. For this course there is a ready to use installation, however, it won't be available after the course project finishes.

Simple usage:
```
module use /scratch/project_462001074/modulefiles
module load omnitrace
srun omnitrace-sample -H false -- myexe # -H false disables some unnecessary clutter
```
By default, Omnitrace outputs goes to directory `omnitrace-myexe-output/time-stamp/`, where the file `perfetto-trace-xxxxx.proto`
contains the actual trace. This file can be opened with Perfetto on local browser similar to above.

Installing Omnitrace yourself is also very straightforward:
```
wget https://github.com/ROCm/omnitrace/releases/download/v1.13.0/omnitrace-1.13.0-opensuse-15.4-ROCm-60000-PAPI-OMPT-Python3.sh
chmod u+x omnitrace-1.13.0-opensuse-15.4-ROCm-60000-PAPI-OMPT-Python3.sh
./omnitrace-1.13.0-opensuse-15.4-ROCm-60000-PAPI-OMPT-Python3.sh --prefix=/some/path/to/install
```

### Using Nsight Systems in Mahti

Simple usage:
```
module load gcc/10.4.0 cuda/12.6.1
srun nsys profile my_exe
```
The GUI does not work properly in Mahti, so for the visual analysis one should install Nsight systems on a local workstation:
<https://developer.nvidia.com/nsight-systems/get-started>

The trace is by default in the file `report1.nsys-rep`, which can be copied to local workstation following similar steps as with
`rocprof` above. Once copied, one can launch `nsys-ui` application and open the file.




## Installing and using Kokkos


### LUMI

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

#### Using existing Kokkos installation

We suggest that you try installing at least one backend yourself, but during the
course it is possible to use also existing installation:

```bash
module use /scratch/project_462001074/modulefiles
module load kokkos/omp # or module load kokkos/hip
```
With the modules, `-DKokkos_ROOT` does not need to be specified when building applications.

### Mahti

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

#### Using existing Kokkos installation

We suggest that you try installing at least one backend yourself, but during the
course it is possible to use also existing installation:

```bash
module use /scratch/project_2015315/modulefiles
module load kokkos/omp # or module load kokkos/cuda
```
With the modules, `-DKokkos_ROOT` does not need to be specified when building applications.

