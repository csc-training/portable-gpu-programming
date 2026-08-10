# SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
#
# SPDX-License-Identifier: MIT

cc=nvc -mp=gpu -O3 -gpu=cc90 -Wall -Minfo=mp
nvcc=nvcc -O3 -arch=sm_90

heat.x: heat.o kernels.o
	$(cc) $^ -L${NVHPC_ROOT}/cuda/13.1/lib64 -lcudart -lstdc++ -o $@

heat.o: heat.c
	$(cc) -c $<

kernels.o: kernels.cu
	$(nvcc) -c $<

.PHONY: clean
clean:
	rm -vf *.x *.o
