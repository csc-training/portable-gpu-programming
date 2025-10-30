#!/bin/bash
#SBATCH --job-name=test
#SBATCH --partition=dev-g
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=7
#SBATCH --gpus-per-node=1
#SBATCH --time=00:10:00

ml LUMI/24.03
ml partition/G
ml rocm/6.0.3

# cc
cc -O3 -fopenmp -fsave-loopmark axpy.c -o axpy.x
mv axpy.lst axpy_c.lst

CRAY_ACC_DEBUG=2 srun ./axpy.x &> axpy_c_debug2.out
CRAY_ACC_DEBUG=1 srun ./axpy.x &> axpy_c_debug1.out
CRAY_ACC_DEBUG=3 srun ./axpy.x &> axpy_c_debug3.out

# ftn
ftn -O3 -fopenmp -hlist=m helper_functions.F90 axpy.F90 -o axpy.x
mv axpy.lst axpy_f.lst
sed -i "s|$(dirname $(readlink -f axpy.F90))/||g" axpy_f.lst

CRAY_ACC_DEBUG=2 srun ./axpy.x &> axpy_f_debug2.out
CRAY_ACC_DEBUG=1 srun ./axpy.x &> axpy_f_debug1.out
CRAY_ACC_DEBUG=3 srun ./axpy.x &> axpy_f_debug3.out

# amdclang
amdclang -g -O3 -fopenmp --offload-arch=gfx90a axpy.c -o axpy.x
LIBOMPTARGET_INFO=$((0x10 | 0x20)) srun ./axpy.x &> axpy_c_clang_debug.out
LIBOMPTARGET_INFO=-1               srun ./axpy.x &> axpy_c_clang_debug_max.out
LIBOMPTARGET_KERNEL_TRACE=1        srun ./axpy.x &> axpy_c_clang_debug_trace1.out
LIBOMPTARGET_KERNEL_TRACE=2        srun ./axpy.x &> axpy_c_clang_debug_trace2.out
