#!/bin/bash
#SBATCH --job-name=test
#SBATCH --partition=debug
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=4
#SBATCH --time=00:10:00

ml LUMI/24.03
ml partition/C

# cc
cc -O3 -fopenmp axpy.c -o axpy.x
OMP_DISPLAY_AFFINITY=true  srun ./axpy.x &> axpy_c_cpu.out

# ftn
ftn -O3 -fopenmp helper_functions.F90 axpy.F90 -o axpy.x
OMP_DISPLAY_AFFINITY=true  srun ./axpy.x &> axpy_f_cpu.out
