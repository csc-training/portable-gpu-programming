! SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
!
! SPDX-License-Identifier: MIT

module helper_functions
  use, intrinsic :: iso_c_binding
  implicit none

#ifdef TRACE
  interface
#else
  contains
#endif

#ifdef TRACE
  function c_roctxRangePush(message) result(level) bind(C, name="roctxRangePushA")
    import :: c_int, c_char
#else
  function c_roctxRangePush(message) result(level)
#endif
    character(kind=c_char), intent(in) :: message(*)
    integer(c_int) :: level
#ifndef TRACE
    level = 0
#endif
  end function c_roctxRangePush

#ifdef TRACE
  function c_roctxRangePop() result(level) bind(C, name="roctxRangePop")
    import :: c_int
#else
  function c_roctxRangePop() result(level)
#endif
    integer(c_int) :: level
#ifndef TRACE
    level = 0
#endif
  end function c_roctxRangePop

#ifdef TRACE
  end interface
  contains
#endif

  subroutine print_array(name, x)
    character(len=*), intent(in) :: name
    real(8), intent(in) :: x(:)
    integer :: i, n
    integer, parameter :: PRINT_N = 4

    n = size(x)

    write(*,'(A)', advance='no') trim(name) // " ="
    do i = 1, n
      if (i <= PRINT_N .or. i > n - PRINT_N) then
        write(*,'(" ", F8.4)', advance='no') x(i)
      else if (i == PRINT_N + 1) then
        write(*,'(A)', advance='no') " ..."
      end if
    end do
    write(*,*)
  end subroutine print_array

  subroutine create_input(u, Lx, Ly)
    implicit none
    real(8), intent(out) :: u(:, :)
    real(8), intent(in) :: Lx, Ly
    integer :: i, j
    integer(kind=8) :: nx, ny
    real(8) :: dx, dy, x, y, acsch

    nx = size(u, 2)
    ny = size(u, 1)
    dx = Lx / (nx - 1)
    dy = Ly / (ny - 1)

    do j = 1, nx
      do i = 1, ny
        x = (j - 1) * dx - 0.5d0 * Lx
        y = (i - 1) * dy - 0.5d0 * Ly
        acsch = asinh(1.0d0 / x)

        if (acsch > y .and. y > 0 .and. y < -0.75d0 * x + 2.75d0) then
          u(i,j) = 100.0d0
        else if (acsch < y .and. y < 0 .and. y > -0.75d0 * x - 2.75d0) then
          u(i,j) = -100.0d0
        else
          u(i,j) = 0.0d0
        end if
      end do
    end do
  end subroutine create_input

  subroutine write_array(filename, array, Lx, Ly, ierr)
    implicit none
    character(len=*), intent(in) :: filename
    real(8), intent(in) :: array(:, :)
    real(8), intent(in) :: Lx, Ly
    integer, intent(out), optional :: ierr
    integer :: unit, ios, local_err
    integer(c_size_t) :: nx, ny
    integer(c_int) :: level

    nx = size(array, 2)
    ny = size(array, 1)

    level = c_roctxRangePush(c_char_"write_array")

    open(newunit=unit, file=filename, form='unformatted', access='stream', status='replace', action='write', iostat=ios)
    if (ios /= 0) then
      write(0,*) "Failed to open file"
      local_err = 1
      if (present(ierr)) ierr = local_err
      level = c_roctxRangePop()
      return
    end if

    write(unit) Lx
    write(unit) Ly
    write(unit) nx
    write(unit) ny

    ! Write the array layout (1 = column-major / Fortran order)
    write(unit) int(1, kind=1)

    write(unit, iostat=ios) array
    close(unit)

    if (ios /= 0) then
      write(0,*) "Failed to write all elements to file"
      local_err = 2
    else
      local_err = 0
    end if

    if (present(ierr)) ierr = local_err
    level = c_roctxRangePop()
  end subroutine write_array

end module helper_functions
