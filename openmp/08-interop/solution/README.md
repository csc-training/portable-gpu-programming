<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

## Discussion

### Exercises: Examine the array locations

1. Output:

       printing from host the address of x in host: 0x34cd40
       printing from host the address of x in dev:  0x152607000000
       printing from dev  the address of x in dev:  0x152607000000

   The fact that the addresses are different means that
   there are two separate arrays 'x' in the code:
   the one in the host and the one in the device.

2. Output:

       printing from host the address of x in host: 0x224850
       printing from host the address of x in dev:  0x224850
       printing from dev  the address of x in dev:  0x224850

   For CPU target, device is the host and the host and "device" arrays are the same.


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
