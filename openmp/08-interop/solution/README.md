## Discussion

### Exercises: Examine the array locations

1. Output:

       address of x in host: 0x314f20
       address of x in dev:  0x152f98200000

   The fact that the addresses are the same means that
   there are two separate arrays 'x' in the code:
   the one in the host and the one in the device.

2. Output:

       address of x in host: 0x224850
       address of x in dev:  0x224850

   For CPU target, the host and "device" arrays are the same.


### Exercises: Call axpy using hipblas

1. Running the code fails with

       Memory access fault by GPU node-4 (Agent handle: 0xb60410) on address 0xe83000. Reason: Unknown.

   With the fixed code, we get the same output as in the first exercises:

       Using n = 102400
       Input:
       a =   3.0000
       x =   0.0000   0.0000   0.0000   0.0000 ...   1.0000   1.0000   1.0000   1.0000
       y =   0.0000   0.0010   0.0020   0.0029 ...  99.9971  99.9980  99.9990 100.0000
       Output:
       y =   0.0000   0.0010   0.0020   0.0030 ... 102.9970 102.9980 102.9990 103.0000
