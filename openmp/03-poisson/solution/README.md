## Discussion

### Exercises

1. Example code is provided: `poisson-1.c`.
   This runs on GPU and gives correct result, but is slow (2.443 s).

2. Example code is provided: `poisson-2.c`.
   This is faster but still rather slow (1.743 s).
   The serial CPU execution is still faster.

3. A few alternatives:

   `poisson-2.c`:

   ```cpp
   #pragma omp target teams distribute parallel for
       for (int i = 1; i < ny - 1; i++) {
           for (int j = 1; j < nx - 1; j++) {
   ```

   1.743 seconds

   `poisson-3a.c`:

   ```cpp
   #pragma omp target teams distribute
       for (int i = 1; i < ny - 1; i++) {
       #pragma omp parallel for
           for (int j = 1; j < nx - 1; j++) {
   ```

   0.260 seconds

   `poisson-3b.c`:

   ```cpp
   #pragma omp target teams distribute parallel for collapse(2)
       for (int i = 1; i < ny - 1; i++) {
           for (int j = 1; j < nx - 1; j++) {
   ```

   0.263 seconds

   This is about the same than the serial CPU execution.

4. Example code is provided: `poisson-4.c` corresponding to the last parallelization above.
   The second run takes 0.021 seconds, and the third run and further runs the same.

   This corresponds to about 10x speed up to the serial CPU execution.

5. Running the fastest GPU code with `./poisson.x 4096 1000`
   takes 0.357 seconds whereas with serial CPU execution it takes 56.840 seconds,
   so GPU is now over 150 times faster.

   This shows that the default system size is too small for GPU to see a proper benefit.

   Note: we also see for this larger case that the first GPU run takes 0.629 seconds, so
   about 0.3 seconds longer than the later runs (as in the smaller case above, but
   there is quite some random variation).
   This shows that the subsecond "wake up time" becomes effectively amortized in real runs.

   You are encouraged to experiment with even larger sizes and with the other code variants of this exercise.
