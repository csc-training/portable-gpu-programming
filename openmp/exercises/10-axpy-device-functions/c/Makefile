# SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
#
# SPDX-License-Identifier: MIT

cc=nvc -mp=gpu -O3 -gpu=cc90 -Wall

main.x: main.o kernels.o
	$(cc) $^ -o $@

%.o: %.c
	$(cc) -c $<

.PHONY: clean
clean:
	rm -vf *.x *.o
