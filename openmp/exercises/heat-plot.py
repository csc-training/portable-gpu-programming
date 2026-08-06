# SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
#
# SPDX-License-Identifier: MIT

import argparse
import numpy as np
import matplotlib.pyplot as plt

def read_array(filename):
    with open(filename, "rb") as f:
        # Read the box size
        Lx, Ly = np.fromfile(f, dtype=np.float64, count=2)

        # Read the array size
        nx, ny = np.fromfile(f, dtype=np.uint64, count=2)

        # Read the array layout (0 = C, 1 = Fortran)
        layout = np.fromfile(f, dtype=np.uint8, count=1)[0]

        # Read the array
        array = np.fromfile(f, dtype=np.float64, count=nx * ny)

    array = array.reshape(nx, ny)
    if layout == 1:
        array = array.T

    return array, Lx, Ly


parser = argparse.ArgumentParser(description="Plot file")
parser.add_argument("file", type=str, help="Path to the file")
args = parser.parse_args()

u, Lx, Ly = read_array(args.file)

plt.figure(1)

x = np.linspace(0, Lx, u.shape[0]) - Lx * 0.5
y = np.linspace(0, Ly, u.shape[1]) - Ly * 0.5
X, Y = np.meshgrid(x, y)
plt.pcolormesh(X, Y, u, shading='nearest', cmap='PiYG_r')
plt.gca().set_aspect('equal', adjustable='box')
plt.xlabel('x')
plt.ylabel('y')
plt.title(args.file)
plt.colorbar()
plt.show()
