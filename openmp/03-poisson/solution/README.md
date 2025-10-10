## Discussion

### Exercise: Offload to GPU

1. Example code is provided: `poisson-1.c`.
   This runs on GPU and gives correct result, but is slow (5.1 s).

2. Example code is provided: `poisson-2.c`.
   This is faster but still rather slow (4.4 s).
   The serial CPU execution is still faster.

3. A few alternatives:

   `poisson-2.c`:


   ```cpp
   #pragma omp target teams distribute parallel for
       for (int i = 1; i < ny - 1; i++) {
           for (int j = 1; j < nx - 1; j++) {
   ```

   4.4 seconds

   `poisson-3a.c`:

   ```cpp
   #pragma omp target teams distribute parallel for collapse(2)
       for (int i = 1; i < ny - 1; i++) {
           for (int j = 1; j < nx - 1; j++) {
   ```

   1.6 seconds

   `poisson-3b.c`:

   ```cpp
   #pragma omp target teams distribute
       for (int i = 1; i < ny - 1; i++) {
       #pragma omp parallel for
           for (int j = 1; j < nx - 1; j++) {
   ```

   0.6 seconds

   This is about 5 times faster than the serial CPU execution.
 
4. Running the fastest GPU code with `./poisson.x 2048 1024`
   takes 2.33 seconds whereas with serial CPU execution it takes 26.0 seconds,
   so GPU is now 11 times faster.
   It seems that our problem is likely too small for a GPU.
