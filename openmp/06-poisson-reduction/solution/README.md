## Discussion

### Exercises

1. An initial improved code `poisson-1.c` runs in 2.6 seconds.

2. The code run quite much longer than the previous code without norm calculation (1.9 s).
   From profiler (`trace-1.json`) we see that the reason is that the I/O step is no more overlapping
   with the GPU computation!

   The reason is that the convergence check is run on the same host thread as the I/O,
   effectively blocking the host thread to do the I/O at the same time.

   The code `poisson-2.c` shows a possible solution to this using two host threads
   and OpenMP tasks. We can easily afford extra threads on LUMI as every LUMI-G node
   has 7 CPU cores per a MI250X GCD.

   This executes in 1.9 seconds like the previous code without norm calculation.
   See also the corresponding `trace-2.json`.

   As a bonus, OpenMP tasks allow the I/O code to be written in a more logical way than
   the previous version with a single host thread.

   Note the use of `write_flag` to ensure that the next set of data is not copied from
   GPU before the previous write has finished.
