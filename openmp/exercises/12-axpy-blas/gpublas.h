/*
 * SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

// Auto-detect NVIDIA HPC SDK compilers (nvc, nvc++)
#if defined(__NVCOMPILER) && !defined(CUDA)
  #define CUDA
#endif

#if defined(CUDA)
  #include <cublas_v2.h>
  typedef cublasHandle_t          blas_handle_t;
  #define BLAS_POINTER_MODE_HOST   CUBLAS_POINTER_MODE_HOST
  #define BLAS_POINTER_MODE_DEVICE CUBLAS_POINTER_MODE_DEVICE
  #define blas_create(h)           cublasCreate(h)
  #define blas_destroy(h)          cublasDestroy(h)
  #define blas_set_pointer_mode(h, m) \
                                   cublasSetPointerMode(h, m)
  #define blas_daxpy(h, n, a, x, incx, y, incy) \
                                   cublasDaxpy(h, n, a, x, incx, y, incy)
#else
  #include <hipblas/hipblas.h>
  typedef hipblasHandle_t          blas_handle_t;
  #define BLAS_POINTER_MODE_HOST   HIPBLAS_POINTER_MODE_HOST
  #define BLAS_POINTER_MODE_DEVICE HIPBLAS_POINTER_MODE_DEVICE
  #define blas_create(h)           hipblasCreate(h)
  #define blas_destroy(h)          hipblasDestroy(h)
  #define blas_set_pointer_mode(h, m) \
                                   hipblasSetPointerMode(h, m)
  #define blas_daxpy(h, n, a, x, incx, y, incy) \
                                   hipblasDaxpy(h, n, a, x, incx, y, incy)
#endif
