# SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
#
# SPDX-License-Identifier: MIT

ft=nvfortran -mp=gpu -O3 -gpu=cc90 -Wall

main.x: kernels.o main.o
	$(ft) $^ -o $@

%.o: %.F90
	$(ft) -c $<

.PHONY: clean
clean:
	rm -vf *.x *.o *.mod
