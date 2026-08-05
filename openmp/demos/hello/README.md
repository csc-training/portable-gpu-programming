<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Demos

## Hello

1. Use `module` command to preprare the environment for compiling GPU code,
   following the general instructions for [Roihu](../../../README_Roihu.md).

   Compile the code:

       nvc -mp=gpu -O3 -gpu=cc90 -Wall hello.c -o hello.x

   Run the program:

       sbatch job.sh

2. Compile and run the different `hello-*.c` by `sbatch run_lumi.sh`.
