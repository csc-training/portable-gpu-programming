// SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#pragma omp declare target
double axpy(double alpha, double x, double y) {
    return alpha * x + y;
}
#pragma omp end declare target
