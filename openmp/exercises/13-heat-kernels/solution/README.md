<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Discussion

## Tasks

1. The case `./heat.x 16384 1000 3` takes 1.831 s on Roihu.

2. The code prints incorrect `u[8191,8191] = -100.000000`, i.e., data is not updated correctly.
   The reason is that CPU pointers are passed to the GPU kernel.

   See `heat.c` with `use_device_ptr` for a fix.

   The fixed code prints correct `u[8191,8191] = -1.783901` and takes 1.400 s on Roihu.

3. See `kernels.c`. This pure OpenMP version takes 1.548 s on Roihu, i.e., sizable improvement
   in comparison to the original code, but not as fast as the CUDA/HIP version although
   both implement basically the same code.
