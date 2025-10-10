## Discussion

### Exercises

1. We can use a single `target update` statement.

   Note that we need to specify the size of the transfer to get correct results.

   Again, `CRAY_ACC_DEBUG` is your friend figuring that out!

   The runtime for case `4096 5000` is increased from 1.7 to 2.5 seconds due to I/O.
   This is not ideal and we'll see soon how to improve this.
