! SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
!
! SPDX-License-Identifier: MIT

module axpy_helper_functions
  implicit none

  contains

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

end module axpy_helper_functions
