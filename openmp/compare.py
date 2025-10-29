# SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
#
# SPDX-License-Identifier: MIT

import argparse
import numpy as np

def read_array(filename):
    with open(filename, "rb") as f:
        # Read the size
        count = np.fromfile(f, dtype=np.uint64, count=1)[0]

        # Read the array
        array = np.fromfile(f, dtype=np.float64, count=count)

    n = int(np.sqrt(count))
    assert n**2 == count
    return array.reshape(n, n)


parser = argparse.ArgumentParser(description="Compare two files")
parser.add_argument("file1", type=str, help="Path to the first file")
parser.add_argument("file2", type=str, help="Path to the second file")
args = parser.parse_args()

u1 = read_array(args.file1)
u2 = read_array(args.file2)

max_val1 = np.max(np.abs(u1))
max_val2 = np.max(np.abs(u2))
max_diff = np.max(np.abs(u1 - u2))

print(f"Maximum absolute value in file1: {max_val1}")
print(f"Maximum absolute value in file2: {max_val2}")
print(f"Maximum absolute difference:     {max_diff}")
