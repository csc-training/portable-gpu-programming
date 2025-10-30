#!/bin/bash
#SBATCH --job-name=test
#SBATCH --partition=gputest
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=4
#SBATCH --gres=gpu:a100:1
#SBATCH --time=00:10:00

ml purge
ml use /appl/opt/nvhpc/modulefiles
ml nvhpc-hpcx-cuda12/25.1

export OMP_TARGET_OFFLOAD=MANDATORY

# nvc
nvc -O3 -mp=gpu -gpu=cc80 -Minfo=mp axpy.c -o axpy.x &> axpy_c_nv_info.out

NVCOMPILER_ACC_NOTIFY=$((0x1 | 0x2))  srun ./axpy.x 2>&1 | sed "s|$(dirname $(readlink -f axpy.c))/||g" > axpy_c_nv_debug.out
NVCOMPILER_ACC_NOTIFY=$((0x1F))       srun ./axpy.x 2>&1 | sed "s|$(dirname $(readlink -f axpy.c))/||g" > axpy_c_nv_debug_max.out

# nvfortran
nvfortran -O3 -mp=gpu -gpu=cc80 -Minfo=mp helper_functions.F90 axpy.F90 -o axpy.x &> axpy_f_nv_info.out

NVCOMPILER_ACC_NOTIFY=$((0x1 | 0x2))  srun ./axpy.x 2>&1 | sed "s|$(dirname $(readlink -f axpy.F90))/||g" > axpy_f_nv_debug.out
NVCOMPILER_ACC_NOTIFY=$((0x1F))       srun ./axpy.x 2>&1 | sed "s|$(dirname $(readlink -f axpy.F90))/||g" > axpy_f_nv_debug_max.out
