#!/bin/bash
#SBATCH --job-name=slurm-test
#SBATCH --account=project_2019754
#SBATCH --partition=gpumedium
#SBATCH --reservation=portableGPU-1 # This changes every day to -2 (wed), -3 (thu) and -4 (fri), valid 09:00 to 17:00
#SBATCH --time=00:05:00
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1 --cpus-per-task=72
#SBATCH --gres=gpu:gh200:1

srun nvidia-smi
