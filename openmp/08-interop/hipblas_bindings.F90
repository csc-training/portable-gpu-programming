module hipblas_bindings
  use, intrinsic :: iso_c_binding
  implicit none

  integer(c_int), parameter :: HIPBLAS_POINTER_MODE_HOST = 0
  integer(c_int), parameter :: HIPBLAS_POINTER_MODE_DEVICE = 1

  interface
    integer(c_int) function c_hipblasCreate(handle) bind(C, name="hipblasCreate")
      import :: c_ptr, c_int
      type(c_ptr) :: handle
    end function c_hipblasCreate

    integer(c_int) function c_hipblasDestroy(handle) bind(C, name="hipblasDestroy")
      import :: c_ptr, c_int
      type(c_ptr), value :: handle
    end function c_hipblasDestroy

    integer(c_int) function c_hipblasSetPointerMode(handle, mode) bind(C, name="hipblasSetPointerMode")
      import :: c_ptr, c_int
      type(c_ptr), value :: handle
      integer(c_int), value :: mode
    end function c_hipblasSetPointerMode

    integer(c_int) function c_hipblasDaxpy(handle, n, alpha, x, incx, y, incy) bind(C, name="hipblasDaxpy")
      import :: c_ptr, c_int
      type(c_ptr), value :: handle
      integer(c_int), value :: n
      type(c_ptr), value :: alpha
      type(c_ptr), value :: x
      integer(c_int), value :: incx
      type(c_ptr), value :: y
      integer(c_int), value :: incy
    end function c_hipblasDaxpy
  end interface

contains

  subroutine hipblasCreate(handle, ierr)
    type(c_ptr) :: handle
    integer, optional, intent(out) :: ierr
    integer(c_int) :: errcode

    errcode = c_hipblasCreate(handle)
    if (present(ierr)) ierr = errcode
  end subroutine hipblasCreate

  subroutine hipblasDestroy(handle, ierr)
    type(c_ptr), value :: handle
    integer, optional, intent(out) :: ierr
    integer(c_int) :: errcode

    errcode = c_hipblasDestroy(handle)
    if (present(ierr)) ierr = errcode
  end subroutine hipblasDestroy

  subroutine hipblasSetPointerMode(handle, mode, ierr)
    type(c_ptr), value :: handle
    integer(c_int), value :: mode
    integer, optional, intent(out) :: ierr
    integer(c_int) :: errcode

    errcode = c_hipblasSetPointerMode(handle, mode)
    if (present(ierr)) ierr = errcode
  end subroutine hipblasSetPointerMode

  subroutine hipblasDaxpy(handle, n, alpha, x, incx, y, incy, ierr)
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

    errcode = c_hipblasDaxpy(handle, n, c_loc(alpha), c_loc(x), incx, c_loc(y), incy)
    if (present(ierr)) ierr = errcode
  end subroutine hipblasDaxpy


end module hipblas_bindings
