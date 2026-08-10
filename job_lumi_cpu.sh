#!/bin/bash
#SBATCH --job-name=slurm-test
#SBATCH --account=project_462001610
#SBATCH --partition=debug
#SBATCH --time=00:05:00
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=4

export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK
srun hostname
