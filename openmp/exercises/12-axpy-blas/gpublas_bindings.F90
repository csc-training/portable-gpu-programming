! SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
!
! SPDX-License-Identifier: MIT

module gpublas_bindings
  use, intrinsic :: iso_c_binding
  implicit none

  integer(c_int), parameter :: BLAS_POINTER_MODE_HOST   = 0
  integer(c_int), parameter :: BLAS_POINTER_MODE_DEVICE = 1

  interface
    integer(c_int) function c_blasCreate(handle) &
#ifdef CUDA
        bind(C, name="cublasCreate_v2")
#else
        bind(C, name="hipblasCreate")
#endif
      import :: c_ptr, c_int
      type(c_ptr) :: handle
    end function c_blasCreate

    integer(c_int) function c_blasDestroy(handle) &
#ifdef CUDA
        bind(C, name="cublasDestroy_v2")
#else
        bind(C, name="hipblasDestroy")
#endif
      import :: c_ptr, c_int
      type(c_ptr), value :: handle
    end function c_blasDestroy

    integer(c_int) function c_blasSetPointerMode(handle, mode) &
#ifdef CUDA
        bind(C, name="cublasSetPointerMode_v2")
#else
        bind(C, name="hipblasSetPointerMode")
#endif
      import :: c_ptr, c_int
      type(c_ptr), value :: handle
      integer(c_int), value :: mode
    end function c_blasSetPointerMode

    integer(c_int) function c_blasDaxpy(handle, n, alpha, x, incx, y, incy) &
#ifdef CUDA
        bind(C, name="cublasDaxpy_v2")
#else
        bind(C, name="hipblasDaxpy")
#endif
      import :: c_ptr, c_int
      type(c_ptr), value :: handle
      integer(c_int), value :: n
      type(c_ptr), value :: alpha
      type(c_ptr), value :: x
      integer(c_int), value :: incx
      type(c_ptr), value :: y
      integer(c_int), value :: incy
    end function c_blasDaxpy
  end interface

contains

  subroutine blas_create(handle, ierr)
    type(c_ptr) :: handle
    integer, optional, intent(out) :: ierr
    integer(c_int) :: errcode

    errcode = c_blasCreate(handle)
    if (present(ierr)) ierr = errcode
  end subroutine blas_create

  subroutine blas_destroy(handle, ierr)
    type(c_ptr), value :: handle
    integer, optional, intent(out) :: ierr
    integer(c_int) :: errcode

    errcode = c_blasDestroy(handle)
    if (present(ierr)) ierr = errcode
  end subroutine blas_destroy

  subroutine blas_set_pointer_mode(handle, mode, ierr)
    type(c_ptr), value :: handle
    integer(c_int), value :: mode
    integer, optional, intent(out) :: ierr
    integer(c_int) :: errcode

    errcode = c_blasSetPointerMode(handle, mode)
    if (present(ierr)) ierr = errcode
  end subroutine blas_set_pointer_mode

  subroutine blas_daxpy(handle, n, alpha, x, incx, y, incy, ierr)
    type(c_ptr), value :: handle
    integer, value :: n, incx, incy
    real(8), intent(in) :: alpha
    real(8), intent(in) :: x(:)
    real(8), intent(inout) :: y(:)
    integer, optional, intent(out) :: ierr
    integer(c_int) :: errcode

    ! Type compatibility checks
    if (storage_size(real(0.0, kind=8)) /= storage_size(real(0.0, kind=c_double))) then
       stop "Error: real(8) is not compatible with C double precision"
    end if
    if (storage_size(int(0)) /= storage_size(int(0, kind=c_int))) then
       stop "Error: integer is not compatible with C int"
    end if

    errcode = c_blasDaxpy(handle, n, c_loc(alpha), c_loc(x), incx, c_loc(y), incy)
    if (present(ierr)) ierr = errcode
  end subroutine blas_daxpy

end module gpublas_bindings
