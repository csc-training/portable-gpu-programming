# SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
#
# SPDX-License-Identifier: MIT

# Roihu
cc=nvc -mp=gpu -O3 -gpu=cc90 -Wall -Minfo=mp
nvcc=nvcc -O3 -arch=sm_90
libs=-lcudart

# LUMI
# cc=cc -fopenmp -O3 -Wall
# nvcc=CC -xhip -O3 -Wall
# libs=

heat.x: heat.o kernels.o
	$(cc) $^ $(libs) -lstdc++ -o $@

heat.o: heat.c
	$(cc) -c $<

kernels.o: kernels.cu
	$(nvcc) -c $<

.PHONY: clean
clean:
	rm -vf *.x *.o
