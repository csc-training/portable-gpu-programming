<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Using Kokkos with MPI

This exercise should be done only in LUMI as course reservation in Mahti allows only single GPU
runs.

Build the [hello.cpp](hello.cpp) code with the provided `CMakeList.txt` using the HIP backend.
Run the program on two nodes with two tasks and two GPUs on each node i.e.
```
...
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=2
#SBATCH --gpus-per-node=2
...

srun ./build-hip/hello
```

The output is pretty verbose, as all MPI tasks print information about all the GPUs available in their nodes.
You can, however, see which GPU was selected for each MPI tasks by grepping both the MPI rank and "selected":
```
egrep 'rank|Selected' slurm-xxxx.out
```
