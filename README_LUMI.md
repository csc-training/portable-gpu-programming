<!--
SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# General instructions for LUMI

## Logging in via ssh

You can access the [LUMI](https://docs.lumi-supercomputer.eu/) supercomputer with ssh:

    ssh <username>@lumi.csc.fi

If you don't have the ssh key in the default location, you need to give a path to the key file:

    ssh -i <path-to-private-key> <username>@lumi.csc.fi

See [wiki](../../wiki/Setting-up-CSC-account-and-SSH) for further details.

## First-time setup

All the exercises should be carried out in the scratch disk area.
This scratch area is shared between all the project members, so create a personal working directory there:

    mkdir -p /scratch/project_462001610/$USER
    cd /scratch/project_462001610/$USER

and clone the git repository there:

    git clone https://github.com/csc-training/portable-gpu-programming.git /scratch/project_462001610/$USER/portable-gpu-programming

Now, `/scratch/project_462001610/$USER/portable-gpu-programming` is your own clone of the repository on LUMI
and you can modify files there without causing conflicts with other participants.

After this initial setup, the files remain on the system and in the subsequent terminal sessions you can go
directly to your clone:

    cd /scratch/project_462001610/$USER/portable-gpu-programming

## Editing code

You can use terminal-based code editors for modifying source code directly on LUMI, e.g. when working on exercises.
For instance, using `nano`:
    nano prog.F90

(`^` in nano's shortcuts refer to **Ctrl** key, *i.e.* in order to save the file and exit the editor press `Ctrl+X`)
Other available editors include *emacs* and *vim*.


## Web interface

- LUMI web interface can be accessed through <https://www.lumi.csc.fi>


## Compiling


### GPU programming

LUMI has several programming environments.

For GPU programming use:
```bash
module load LUMI/25.03 partition/G rocm/6.3.4
```

#### OpenMP offload and MPI+OpenMP offload

Compilation of OpenMP offload and MPI+OpenMP offload programs:
```bash
cc -fopenmp -O3 -Wall prog.c -o prog.x
CC -fopenmp -O3 -Wall prog.cpp -o prog.x
ftn -fopenmp -O3 prog.F90 -o prog.x
```

**Note!** This will generate OpenMP offload code when the appropriate
GPU modules are loaded (in particular `craype-accel-amd-gfx90a` that is
loaded by `partition/G`).
It's important to ensure that GPU code is generated as the compilation
command is the same in CPU and GPU cases.

To obtain compiler diagnostics:

```bash
cc -fopenmp -O3 -Wall -fsave-loopmark prog.c -o prog.x
CC -fopenmp -O3 -Wall -fsave-loopmark prog.cpp -o prog.x
ftn -fopenmp -O3 -hmsgs -hlist=m prog.F90 -o prog.x
```

See [HPE Cray Clang C and C++ Quick Reference (17.0.1)](https://support.hpe.com/hpesc/public/docDisplay?docId=dp00004439en_us)
and [ftn man pages](https://cpe.ext.hpe.com/docs/24.03/cce/man1/crayftn.1.html)
for further information.

Alternatively to Cray compilers above, you can activate AMD Clang
on top of the earlierly loaded GPU programming modules:
```bash
module load PrgEnv-amd
```

Then `cc` and `CC` compiler wrappers correspond to `amdclang` and `amdclang++`,
respectively.

#### Kokkos

See Kokkos instructions in [separate document](kokkos/installing-using.md).

#### HIP and MPI+HIP

Compilation of HIP and multi-GPU MPI+HIP programs:
```bash
CC -xhip -O3 prog.cpp -o prog.x
```


### CPU programming

LUMI has several programming environments.

For CPU programming use:
```bash
module load LUMI/25.03 partition/C
```

#### OpenMP (threading with CPUs) and MPI+OpenMP

Compilation of OpenMP and OpenMP offload programs for CPU threading:
```bash
cc -fopenmp -O3 -Wall prog.c -o prog.x
CC -fopenmp -O3 -Wall prog.cpp -o prog.x
ftn -fopenmp -O3 prog.F90 -o prog.x
```

Compilation of MPI+OpenMP programs works the same way:
```bash
cc -fopenmp -O3 -Wall prog.c -o prog.x
CC -fopenmp -O3 -Wall prog.cpp -o prog.x
ftn -fopenmp -O3 prog.F90 -o prog.x
```


## Running

Programs need to be executed via the batch job system:
```bash
sbatch job.sh
```
The output of the job will be in the file `slurm-JOBID.out`. You can check the status of your jobs with `squeue --me` and kill possible hanging applications with `scancel JOBID`.

The file job script `job.sh` contains both the resource request (comment header lines starting with `#SBATCH`)
and the file is executed as a bash script in the allocation (lines starting with `#` are comments and ignored
during bash execution). Examples are provided below.

Note that you can override any of the options define in the `#SBATCH` header as command line options, for example:
```bash
sbatch --time=00:10:00 job.sh
```

### Slurm reservations

We have dedicated slurm reservations for the training days:
- `--reservation=portableGPU-1`: Valid on 2026-08-11 from 09:00 to 17:00
- `--reservation=portableGPU-2`: Valid on 2026-08-12 from 09:00 to 17:00
- `--reservation=portableGPU-3`: Valid on 2026-08-13 from 09:00 to 17:00
- `--reservation=portableGPU-4`: Valid on 2026-08-14 from 09:00 to 17:00

The reservations give access up to 4 nodes in `small-g` partition.

In order to activate the reservation, include the `--reservation=...` option in the job script.

In addition to the reservations, you can also access all the general partitions available on LUMI.


### GPU jobs

Example `job.sh` for running a GPU program reserving 1 GPU (= 1 GCD of the AMD MI250X GPU):

```bash
#!/bin/bash
#SBATCH --job-name=test
#SBATCH --account=project_462001610
#SBATCH --partition=small-g
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=4
#SBATCH --gpus-per-node=1
#SBATCH --time=00:02:00

# Enable GPU-aware MPI by uncommenting the line below
#export MPICH_GPU_SUPPORT_ENABLED=1

# Run the program
srun ./prog.x
```

Note that this script allocates also 4 CPU cores per task.
These extra CPU cores are especially useful for OpenMP runtime.

**Unless required by the exercise only always use 1 GPU!**

For multi-GPU jobs using MPI:
- Change the number of MPI tasks and GPUs per node: `--ntasks-per-node=<number_of_mpi_tasks_per_node>` and `--gpus-per-node=<number_of_gpus_per_node>`
- Uncomment `export MPICH_GPU_SUPPORT_ENABLED=1` to enable GPU-aware MPI

#### OpenMP offload runtime debug

Enable runtime debug information for Cray compiler:
```bash
export CRAY_ACC_DEBUG=2
```

Most verbose setting: `CRAY_ACC_DEBUG=3`.

Enable runtime debug information for AMD Clang compiler ([documentation](https://openmp.llvm.org/design/Runtimes.html#libomptarget-info)):
```bash
export LIBOMPTARGET_INFO=$((0x10 | 0x20))
```

Most verbose setting: `LIBOMPTARGET_INFO=-1`.

Alternatively ([documentation](https://rocm.docs.amd.com/projects/llvm-project/en/latest/conceptual/openmp.html#environment-variables)):
```bash
export LIBOMPTARGET_KERNEL_TRACE=1
export LIBOMPTARGET_KERNEL_TRACE=2
```


### CPU jobs

Example `job.sh` for running CPU program reserving 1 node, 1 MPI task per node, and 4 CPU cores per task, as well as 1 GB RAM per core:

```bash
#!/bin/bash
#SBATCH --job-name=test
#SBATCH --account=project_462001610
#SBATCH --partition=small
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=4
#SBATCH --mem-per-cpu=1G
#SBATCH --time=00:02:00

# Set the number of threads based on cpus-per-task, which Slurm stores in the SLURM_CPUS_PER_TASK environment variable.
# The following bash syntax evaluates to 1 if --cpus-per-task was not given.
export OMP_NUM_THREADS=${SLURM_CPUS_PER_TASK:-1}

# Place and bind threads to single hardware threads
# Comment the following lines if binding is not desired
export OMP_PLACES=cores
export OMP_PROC_BIND=spread

# Run the program
srun ./prog.x
```

Rules of thumb for choosing the resources based on the parallelization type:
- MPI only: `--ntasks-per-node=<number_of_mpi_tasks>` and `--cpus-per-task=1`
- OpenMP only: `--ntasks-per-node=1` and `--cpus-per-task=<number_of_threads>`
- MPI+OpenMP: `--ntasks-per-node=<number_of_mpi_tasks>` and `--cpus-per-task=<number_of_threads_per_mpi_task>`

Note that other ways might be reasonable in some cases too.
Some of such cases will be discussed in the exercises.


### Interactive jobs

Requesting an allocation:

```bash
salloc --account=project_462001610 --partition=small --nodes=1 --ntasks-per-node=1 --cpus-per-task=4 --time=00:30:00
```

Once the allocation is ready, you'll get a new shell on the login node.
In this shell, `srun` will launch jobs within the allocation:
```bash
srun --ntasks-per-node=2 --cpus-per-task=2 ./prog.x
```


### Interactive shell on a compute node

Create an interactive shell on a compute node:

```bash
srun --job-name=cn-shell --account=project_462001610 --partition=small --nodes=1 --ntasks-per-node=1 --cpus-per-task=2 --time=01:00:00 --pty env -i HOME="$HOME" TERM="$TERM" bash -l
```



## Resources

- [LUMI documentation](https://docs.lumi-supercomputer.eu/)
