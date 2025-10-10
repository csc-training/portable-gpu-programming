## Asynchronous kernel execution

In this exercise we practise data updates.

We have added to the previous code an I/O step that writes the array on every 100th iteration to the disk:

```cpp
// Write data
if (it % 100 == 0) {
    sprintf(filename, "u%06d.bin", it);
    write_array(filename, u, n2);
}
```

The problem is that this doesn't work as all the data is only on GPU.
Let's fix this!

The [solution directory](solution/) contains a model solution and discussion on the exercises below.

### Exercise: Offload to GPU

1. Add suitable data clauses to fix the I/O step.

   You can use the following sanity checks on command line:

       diff u000000.bin u000100.bin  # These files should differ
       diff u000500.bin u_end.bin  # These files should be the same (no output from diff)

   You can also use the attached Jupyter notebook to visualize the data dumps to check that they make sense.
