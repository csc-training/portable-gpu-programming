<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Discussion

## Tasks

1. See `heat-1.c`.
   This runs on GPU and gives correct result (time spent 0.065 s).

2. We see that data is being copied back and forth every time step.
   This is not great for the performance.

3. See `heat-2.c`.
   This is faster now (0.007 s).

4. A few alternatives:

   `heat-2.c`:

   ```cpp
   #pragma omp target teams distribute parallel for collapse(2)
   for (int i = 1; i < ny - 1; i++) {
       for (int j = 1; j < nx - 1; j++) {
   ```

   0.007 seconds

   `heat-2a.c`:

   ```cpp
   #pragma omp target teams distribute
   for (int i = 1; i < ny - 1; i++) {
       #pragma omp parallel for
       for (int j = 1; j < nx - 1; j++) {
   ```

   0.013 seconds

   `heat-2b.c`:

   ```cpp
   #pragma omp target teams distribute parallel for
   for (int i = 1; i < ny - 1; i++) {
       for (int j = 1; j < nx - 1; j++) {
   ```

   0.352 seconds

   Optimizing the work distributions makes a difference for the performance.

5. With the fastest `heat-2.c` code:

   `./heat.x 2048 1000` takes 0.037 seconds (serial CPU execution 2.311 seconds): speedup 62 times.

   `./heat.x 4096 1000` takes 0.122 seconds (serial CPU execution 8.895 seconds): speedup 73 times.

   `./heat.x 8192 1000` takes 0.457 seconds (serial CPU execution 37.061 seconds): speedup 80 times.

   `./heat.x 16384 1000` takes 1.835 seconds (serial CPU execution 146.918 seconds): speedup 80 times.

   This shows that the default system size is too small for GPU to see a proper benefit.
