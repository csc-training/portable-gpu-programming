<!--
SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# General instructions for Roihu

> [!IMPORTANT]
> Roihu has different CPU architectures on Roihu-CPU (x86) and Roihu-GPU (ARM).
> Therefore, there are **separate login nodes for building CPU and GPU programs and submitting respective jobs**:
> - `roihu-cpu.csc.fi`
> - `roihu-gpu.csc.fi`
> 
> Roihu-CPU and Roihu-GPU share the disk areas (i.e., `/scratch` and `$HOME` have the same files on both Roihu-CPU and Roihu-GPU).

## Logging in via ssh


You can access the [Roihu](https://docs.csc.fi/computing/systems-roihu/) supercomputer with ssh:

    ssh <username>@roihu-cpu.csc.fi

or:

    ssh <username>@roihu-gpu.csc.fi

If you don't have the ssh key in the default location, you need to give a path to the key file:

    ssh -i <path-to-private-key> <username>@roihu-???.csc.fi

See [wiki](../../wiki/Setting-up-CSC-account-and-SSH) for further details.

## First-time setup

All the exercises should be carried out in the scratch disk area.
This scratch area is shared between all the project members, so create a personal working directory there:

    mkdir -p /scratch/project_2019754/$USER
    cd /scratch/project_2019754/$USER

and clone the git repository there:

    git clone https://github.com/csc-training/portable-gpu-programming.git /scratch/project_2019754/$USER/portable-gpu-programming

Now, `/scratch/project_2019754/$USER/portable-gpu-programming` is your own clone of the repository on Roihu
and you can modify files there without causing conflicts with other participants.

After this initial setup, the files remain on the system and in the subsequent terminal sessions you can go
directly to your clone:

    cd /scratch/project_2019754/$USER/portable-gpu-programming

## Editing code

You can use terminal-based code editors for modifying source code directly on Roihu, e.g. when working on exercises.
For instance, using `nano`:
    nano prog.F90

(`^` in nano's shortcuts refer to **Ctrl** key, *i.e.* in order to save the file and exit the editor press `Ctrl+X`)
Other available editors include *emacs* and *vim*.


## Web interface

- Roihu web interface can be accessed through <https://www.roihu.csc.fi>


## Compiling

### CPU programming on Roihu-CPU

> [!IMPORTANT]
> For compiling CPU programs, use Roihu-CPU accessed through `roihu-cpu.csc.fi`.

Roihu has several programming environments.

For CPU programming use (this is the default environment activated without loading any modules):
```bash
module load gcc/15.2.0 openmpi/5.0.10
```

#### MPI

Compilation of MPI programs in C, C++, and Fortran:
```bash
mpicc -O3 -march=znver5 -Wall prog.c -o prog.x
mpicxx -O3 -march=znver5 -Wall prog.cpp -o prog.x
mpif90 -O3 -march=znver5 prog.F90 -o prog.x
```

The wrapper commands include automatically all the flags needed for building MPI programs.

#### MPI+OpenMP and pure OpenMP (threading with CPUs)

Both pure OpenMP and hybrid MPI+OpenMP programs can be compiled with the same wrappers
by including `-fopenmp` flag:
```bash
mpicc -fopenmp -O3 -march=znver5 -Wall prog.c -o prog.x
mpicxx -fopenmp -O3 -march=znver5 -Wall prog.cpp -o prog.x
mpif90 -fopenmp -O3 -march=znver5 prog.F90 -o prog.x
```

### GPU programming on Roihu-GPU

> [!IMPORTANT]
> For compiling GPU programs, use Roihu-GPU accessed through `roihu-gpu.csc.fi`.

Roihu has several programming environments and we recommend
using different environments for CUDA and OpenMP offload.

#### CUDA and MPI+CUDA

For GPU programming with CUDA use:
```bash
module load gcc/14.3.0 cuda/12.9.1 openmpi/5.0.8 openblas/0.3.30
```

Compilation of CUDA programs:
```bash
nvcc -O3 -gencode arch=compute_90a,code=sm_90a prog.cu -o prog.x
```

Compilation of MPI+CUDA programs is a bit more complex as we need to include the MPI flags.
This can be automated by following lines (only the last line needs to be edited):
```bash
# Parse MPI options for compiler
Xcompiler="-Xcompiler $(mpicxx --showme | tr ' ' '\n' | sed '/^-Wl,/d;1d' | paste -sd, -)"

# Parse MPI options for linker
Xlinker="-Xlinker $(mpicxx --showme | tr ' ' '\n' | sed -n 's/^-Wl,//p' | paste -sd, -)"

# Compile MPI code using nvcc
nvcc -O3 -gencode arch=compute_90a,code=sm_90a $Xcompiler $Xlinker prog.cu -o prog.x
```

#### OpenMP offload and MPI+OpenMP offload

For GPU programming with OpenMP offload use:
```bash
module purge
module load nvhpc/26.3
```

Compilation of OpenMP offload programs:
```bash
nvc -mp=gpu -O3 -gpu=cc90 prog.c -o prog.x
nvc++ -mp=gpu -O3 -gpu=cc90 prog.cpp -o prog.x
nvfortran -mp=gpu -O3 -gpu=cc90 prog.F90 -o prog.x
```

To obtain compiler diagnostics:
```bash
nvc -mp=gpu -O3 -gpu=cc90 -Minfo=mp prog.c -o prog.x
nvc++ -mp=gpu -O3 -gpu=cc90 -Minfo=mp prog.cpp -o prog.x
nvfortran -mp=gpu -O3 -gpu=cc90 -Minfo=mp prog.F90 -o prog.x
```

Compilation of MPI + OpenMP offload programs works with the usual wrappers in this environment:
```bash
mpicc -mp=gpu -O3 -gpu=cc90 prog.c -o prog.x
mpicxx -mp=gpu -O3 -gpu=cc90 prog.cpp -o prog.x
mpif90 -mp=gpu -O3 -gpu=cc90 prog.F90 -o prog.x
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

The reservations give access up to 4 nodes in `gpumedium` partition.

In order to activate the reservation, include the `--reservation=...` option in the job script.

In addition to the reservations, you can also access all the general partitions available on Roihu.


### CPU jobs on Roihu-CPU

> [!IMPORTANT]
> For running CPU programs, use Roihu-CPU accessed through `roihu-cpu.csc.fi`.

Example `job.sh` for running MPI+OpenMP program reserving 1 node, 4 tasks per node, and 2 CPU core per task, as well as 1 GB RAM per core, i.e., 8 CPU cores and 8 GB RAM within one node in total:

```bash
#!/bin/bash
#SBATCH --job-name=test
#SBATCH --account=project_2019754
#SBATCH --partition=small
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=4
#SBATCH --cpus-per-task=2
#SBATCH --mem-per-cpu=1G
#SBATCH --time=00:05:00

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


### GPU jobs on Roihu-GPU

> [!IMPORTANT]
> For running GPU programs, use Roihu-GPU accessed through `roihu-gpu.csc.fi`.

Example `job.sh` for running a GPU program reserving 1 NVIDIA GH200 superchip:

```bash
#!/bin/bash
#SBATCH --job-name=test
#SBATCH --account=project_2019754
#SBATCH --partition=gpumedium
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=4
#SBATCH --gres=gpu:gh200:1
#SBATCH --time=00:05:00

# Run the program
srun ./prog.x
```

Note that this script allocates also 4 CPU cores per task.
These extra CPU cores are especially useful for OpenMP runtime.

**Unless required by the exercise only always use 1 GPU!**

For multi-GPU jobs using MPI:
- Change the number of MPI tasks and GPUs per node: `--ntasks-per-node=<number_of_mpi_tasks_per_node>` and `--gres=gpu:gh200:<number_of_gpus_per_node>`


### Interactive jobs

Requesting an allocation:

```bash
salloc --account=project_2019754 --partition=small --nodes=1 --ntasks-per-node=1 --cpus-per-task=4 --time=00:30:00
```

Once the allocation is ready, you'll get a new shell on the login node.
In this shell, `srun` will launch jobs within the allocation:
```bash
srun --ntasks-per-node=2 --cpus-per-task=2 ./prog.x
```


### Interactive shell on a compute node

Create an interactive shell on a compute node:

```bash
srun --job-name=cn-shell --account=project_2019754 --partition=small --nodes=1 --ntasks-per-node=1 --cpus-per-task=2 --time=01:00:00 --pty ./tools/launch-cn-shell.sh
```

The script [launch-cn-shell.sh](tools/launch-cn-shell.sh) launches a bash shell
with the environment cleaned so that submitting slurm jobs works from the shell
in the same way as from the login node.


## Resources

- [CSC documentation: Roihu supercomputer](https://docs.csc.fi/computing/systems-roihu/)
- [CSC documentation: Compiling on Roihu](https://docs.csc.fi/computing/compiling-roihu/)
- [CSC documentation: Example batch job scripts for Roihu](https://docs.csc.fi/computing/running/example-job-scripts-roihu/)
