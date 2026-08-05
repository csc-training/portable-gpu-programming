<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

## Data updates

In this exercise we practise data updates.

We have added to the model code of the previous exercise an I/O step that writes the array on every 1000th iteration to the disk:

```cpp
// Write data
if (it % 1000 == 0) {
    sprintf(filename, "u%06d.bin", it);
    write_array(filename, u, n2);
}
```

```fortranfree
! Write data
if (mod(it, 1000) == 0) then
  write(filename, '(A,I6.6,A)') 'u', it, '.bin'
  call write_array(filename, u)
end if
```

The problem is that this doesn't work as all the data is only on GPU.
Let's fix this!

The [solution directory](solution/) contains a model solution and discussion on the exercises below.

### Exercises

1. Add suitable data clauses to fix the I/O step.
   Run a test with a larger case than the default:

       srun -p dev-g --nodes=1 --ntasks-per-node=1 --cpus-per-task=7 --gpus-per-node=1 -t 0:10:00 ./poisson.x 4096 5000 3

   How does the runtime compare to the previous code without the I/O step?

   You can use the following sanity checks on command line:

       python3 compare.py u000000.bin u001000.bin  # These files should differ
       python3 compare.py u005000.bin u_end.bin    # These files should be the same

   Note that the files created with C and Fortran versions of the code are not comparable with each other.

   You can also use the attached Jupyter notebook to visualize the data dumps to check that they make sense.
   Steps to launch Jupyter notebook on LUMI web interface:
   - Login to https://www.lumi.csc.fi
   - Select "Jupyter for courses" app
   - Select the course project and the course module: plotting
   - Click Launch, wait, and then Connect to Jupyter
   - Navigate to your working directory and open `plot.ipynb`
