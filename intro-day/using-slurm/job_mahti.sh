#!/bin/bash
#SBATCH --job-name=slurm-test
#SBATCH --account=project_2015315
#SBATCH --partition=gpusmall
#SBATCH --reservation=portgp-2025-tue # This changes every day to -wed, -thu and -fri, valid 09:00 to 17:00
#SBATCH --time=00:05:00
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH --gres=gpu:a100_1g.5gb:1

srun nvidia-smi
