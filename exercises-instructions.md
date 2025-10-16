# General exercises instructions

## Getting the materials

All course materials, slides and hands-out are available in the github repository. They can be downloaded with the command

```
git clone https://github.com/csc-training/portable-gpu-programming.git
```

If you have a GitHub account you can also **Fork** this repository and clone
then your fork. That way you can easily commit and push your own solutions
to exercises.

### Repository structure

The exercise assignments are provided in various `README.md`s.
For most of the exercises, some skeleton codes are provided as starting point. In addition, all of the exercises have exemplary full codes
(that can be compiled and run) in the `solutions` folder. **Note that these are
seldom the only or even the best way to solve the problem.**

## Using supercomputers

Exercises can be carried out using the [LUMI](https://docs.lumi-supercomputer.eu/)  and/or  [Mahti](https://docs.csc.fi/computing/systems-mahti/) supercomputers.
 ![](docs/img/cluster_diagram.jpeg)

In order to access the systems, you need to setup ssh keys, see [LUMI](https://docs.lumi-supercomputer.eu/firststeps/SSH-keys/) and [Mahti](https://docs.csc.fi/computing/connecting/ssh-keys/) documentation for details. Once you have set up SSH keys, use a command like below to connect over SSH:
```
# Replace <username> with the name of your CSC user account and
# <host> with "lumi" or "mahti"

ssh <username>@<host>.csc.fi
```

### Disk area

The (computing and storage) resources can be accessed on supercomputers via project-based allocation system, where users are granted access based on the specific needs and goals of their projects. Running applications and storage area are directly linked ot this projects. For this event we have been granted access to the training `project_2012125` on Mahti and `project_462001074` on LUMI.

All the exercises in the supercomputers have to be carried out in the **scratch** disk area. The name of the scratch directory can be queried with the commands `csc-workspaces` on Mahti and `lumi-workspaces` onLUMI. As the base directory is shared between members of the project, you should create your own
directory:

on Mahti
```
cd /scratch/project_2015315
mkdir -p $USER
cd $USER
```

on LUMI
```
cd /scratch/project_462001074
mkdir -p $USER
cd $USER
```
The `scratch` area has quota of 1-2TB per project. More than enough for the training. In addition to this other areas are disks areas available. The `projappl/project_xyz` area is faster and can be used for storing the project applications (should not be used for data storage) and on LUMI the so called `flash/project_xyz` disk area can be used for IO intensive runs.

### Editors

For editing program source files you can use e.g. *nano* editor:

```
nano prog.f90
```
(`^` in nano's shortcuts refer to **Ctrl** key, *i.e.* in order to save file and exit editor press `Ctrl+X`)
Also other popular editors such as emacs and vim are available.

## Running applications in supercomputers

Programs need to be executed via the Slurm batch job system:

    sbatch job.sh

The `job.sh` file contains all the necessary information (number of nodes, tasks per node, cores per taks, number of gpus per node, etc.)  for the Slurm to execute the program.
Example job scripts for Mahti and LUMI are provided below.
The output will be by default in file `slurm-xxxxx.out`.
You can check the status of your jobs with `squeue --me` and cancel possible hanging applications with `scancel JOBID`.

Alternatively to `sbatch`, you can submit directly to the batch job system with useful one-liners:

```
# Mahti
srun --account=project_2015315 --partition=gpusmall --reservation=portgp-2025-tue --ntasks=1 --cpus-per-task=1 --gres=gpu:a100:1 --time=00:05:00 ./my_gpu_exe
```
```
# LUMI
srun --account=project_462001074 --partition=small-g --reservation=portgp-2025-tue  --ntasks=1 --cpus-per-task=1 --gpus-per-node=1 --time=00:05:00 ./my_gpu_exe
```
The possible options here for `srun` are the same as in the job scripts below.

**NOTE** Some exercises have additional instructions of how to run!

### Running GPU applications on Mahti

Example `job.sh`  using 1 GPU:
```bash
#!/bin/bash
#SBATCH --job-name=example
#SBATCH --account=project_2015315
#SBATCH --partition=gpusmall
#SBATCH --reservation=portgp-2025-tue # This changes every day to -wed, -thu and -fri, valid 09:00 to 17:00 
#SBATCH --time=00:05:00
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --gres=gpu:a100_1g.5gb:1

srun ./my_gpu_exe
```

The reservation `-tue` is valid on Tuesday, 09:00 to 17:00. On Wednesday we will use `...-wed`, on Thursday `...-thu`, while on Friday `...-fri`.
On Mahti we have 1 gpu node reserved for us. 

At any time , one can use `--partition=gputest`  without the reservation argument and with `--gres=gpu:a100:x` (`x=1,2,3,` or `4`).

For multi-gpu applications one has to use `--gres=gpu:a100:x`, where `x` is the number of gpus matching the number of tasks per node.

### Running GPU applications on LUMI

Example `job.sh` using 1 GPU:

```bash
#!/bin/bash
#SBATCH --job-name=example
#SBATCH --account=project_462001074
#SBATCH --partition=small-g
#SBATCH --reservation=portgp-2025-tue # This changes every day to -wed, -thu and -fri, valid 09:00 to 17:00 
#SBATCH --time=00:05:00
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --gpus=1

srun ./my_gpu_exe
```
On LUMI we have 6 gpu nodes reserved for us. 

At any time , one can use `--partition=dev-g`  without the reservation argument.

For multi-gpu applications one has to use `--gpus-per-node=x`, where `x` is the number of gpus per node. 

Other useful options for both Mahti and LUMI are `--nodes` and `--ntasks-per-node` which replace the `--ntasks`. 

**Unless required by the exercise only always use 1 GPU!**

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









## Using SYCL

SYCL is not part of the module system at the moment. The SYCL compilers were build for this training. We recommend that you use one of the two SYCL implementations.

### AdaptiveCpp

This is another SYCL  implementation with support for many type of devices. No special set-up is needed, expect from loading the modules related to the backend (cuda or rocm).

#### AdaptiveCpp on Mahti

Set up the environment:

    module load gcc/10.4.0
    module load cuda/12.6.1
    module load openmpi/4.1.5-cuda # Needed for using GPU-aware MPI
    export PATH=/projappl/project_2015315/apps/ACPP/bin/:$PATH
    export LD_LIBRARY_PATH=$CUDA_HOME/lib64:$LD_LIBRARY_PATH
    export LD_LIBRARY_PATH=/appl/spack/v020/install-tree/gcc-8.5.0/gcc-10.4.0-2oazqj/lib64/:$LD_LIBRARY_PATH
    export LD_PRELOAD=/projappl/project_2015315/apps/LLVM/lib/x86_64-unknown-linux-gnu/libomp.so
    

Compile sycl code:

    acpp -fuse-ld=lld -O3 -L/appl/spack/v020/install-tree/gcc-8.5.0/gcc-10.4.0-2oazqj/lib64/   --acpp-targets="omp.accelerated;cuda:sm_80"   -lstdc++  <sycl_code>.cpp

#### AdaptiveCpp on LUMI

Set up the environment:
    
    module load LUMI
    module load partition/G
    module load rocm/6.2.2
    export  HSA_XNACK=1 # enables managed memory
    export MPICH_GPU_SUPPORT_ENABLED=1                                # Needed for using GPU-aware MPI
    export PATH=/projappl/project_462001074/apps/ACPP/bin:$PATH
    export LD_LIBRARY_PATH=/appl/lumi/SW/LUMI-24.03/G/EB/Boost/1.83.0-cpeCray-24.03/lib64/
    export LD_PRELOAD=/appl/lumi/SW/LUMI-24.03/G/EB/rocm/6.2.2/llvm/lib/libomp.so

Compile sycl code:

    acpp -O3 --acpp-targets="omp.accelerated;hip:gfx90a" <sycl_code>.cpp

### MPI

MPI (Message Passing Interface) is a standardized and portable message-passing standard designed for parallel computing architectures. It allows communication between processes running on separate nodes in a distributed memory environment. MPI plays a pivotal role in the world of High-Performance Computing (HPC), this is why is important to know we could combine SYCL and MPI.

The SYCL implementation do not know anything about MPI. Intel oneAPI contains mpi wrappers, however they were not configure for Mahti and LUMI. Both Mahti and LUMI provide wrappers that can compile applications which use MPI, but they can not compile SYCL codes. We can however extract the MPI related flags and add them to the SYCL compilers.

For exampl on Mahti in order to use CUDA-aware MPI we would first load the modules:
```
    module load gcc/10.4.0
    module load cuda/12.6.1  # Needed for compiling to NVIDIA GPUs
    module load  openmpi/4.1.5-cuda # Needed for using GPU-aware MPI
```
The environment would be setup for compiling a CUDA code which use GPU to GPU communications. We can inspect the `mpicxx` wrapper:
```
$ mpicxx -showme
/appl/spack/v017/install-tree/gcc-8.5.0/gcc-11.2.0-zshp2k/bin/g++ -I/appl/spack/v017/install-tree/gcc-11.2.0/openmpi-4.1.2-bylozw/include -I/appl/spack/v017/install-tree/gcc-11.2.0/openmpi-4.1.2-bylozw/include/openmpi -I/appl/spack/syslibs/include -pthread -L/appl/spack/v017/install-tree/gcc-11.2.0/openmpi-4.1.2-bylozw/lib -L/appl/spack/syslibs/lib -Wl,-rpath,/appl/spack/v017/install-tree/gcc-8.5.0/gcc-11.2.0-zshp2k/lib/gcc/x86_64-pc-linux-gnu/11.2.0 -Wl,-rpath,/appl/spack/v017/install-tree/gcc-8.5.0/gcc-11.2.0-zshp2k/lib64 -Wl,-rpath -Wl,/appl/spack/v017/install-tree/gcc-11.2.0/openmpi-4.1.2-bylozw/lib -Wl,-rpath -Wl,/appl/spack/syslibs/lib -lmpi
```
We note that underneath `mpicxx` is calling `g++` with a lots of MPI related flags. We can obtain and use these programmatically with `mpicxx --showme:compile` and `mpicxx --showme:link`
for compiling the SYCL+MPI codes:
```
export PATH=/projappl/project_2015315/apps/ACPP/bin/:$PATH
export LD_LIBRARY_PATH=$CUDA_HOME/lib64:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/appl/spack/v020/install-tree/gcc-8.5.0/gcc-10.4.0-2oazqj/lib64/:$LD_LIBRARY_PATH
export LD_PRELOAD=/projappl/project_2015315/apps/LLVM/lib/x86_64-unknown-linux-gnu/libomp.so

acpp -fuse-ld=lld -O3 -L/appl/spack/v020/install-tree/gcc-8.5.0/gcc-10.4.0-2oazqj/lib64/ --acpp-targets="omp.accelerated;cuda:sm_80" `mpicxx --showme:compile` `mpicxx --showme:link` <sycl_mpi_code>.cpp
```

Similarly on LUMI. First we set up the environment and load the modules as indicated above
```bash
module load LUMI
module load partition/G
module load rocm/6.2.2
export  HSA_XNACK=1 # enables managed memory
export MPICH_GPU_SUPPORT_ENABLED=1                                # Needed for using GPU-aware MPI
```

Now compile  with:
```
export PATH=/projappl/project_462001074/apps/ACPP/bin:$PATH
export LD_LIBRARY_PATH=/appl/lumi/SW/LUMI-24.03/G/EB/Boost/1.83.0-cpeCray-24.03/lib64/
export LD_PRELOAD=/appl/lumi/SW/LUMI-24.03/G/EB/rocm/6.2.2/llvm/lib/libomp.so

acpp -O3 --acpp-targets="omp.accelerated;hip:gfx90a" `CC --cray-print-opts=cflags` <sycl_mpi_code>.cpp `CC --cray-print-opts=libs`
```

## Installing and using Kokkos

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
Building an application with Kokkos CUDA backend (for GPUs):
```
cmake -Bbuild-cuda -DKokkos_ROOT=/scratch/project_2015315/$USER/kokkos-cuda
...
```

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

