<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

## Discussion

### Exercises for LUMI and Mahti

1. No code changes needed. Output:

       Hello from MPI rank 1/2 with GPU 1/2
       Rank 1 received 42.000000 .. 42.000000
       Hello from MPI rank 0/2 with GPU 0/2
       Rank 0 sent 42.000000 .. 42.000000

   For LUMI: If `MPICH_GPU_SUPPORT_ENABLED` is not set, you'll see `Bus error`.
