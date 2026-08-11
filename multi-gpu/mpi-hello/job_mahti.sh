#!/bin/bash
#SBATCH --job-name=mpi-hello
#SBATCH --output=%x.%j.out
#SBATCH --error=%x.%j.err
#SBATCH --account=project_2015315
#SBATCH --partition=test
#SBATCH --time=00:05:00
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=4
#SBATCH --cpus-per-task=1

srun ./hello
