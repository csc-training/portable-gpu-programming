<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Discussion

## Tasks

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
