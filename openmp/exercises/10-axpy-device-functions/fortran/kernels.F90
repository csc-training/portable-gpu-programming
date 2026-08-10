! SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
!
! SPDX-License-Identifier: MIT

module kernels
  implicit none
contains

  pure function axpy(alpha, x, y) result(res)
    real(8), intent(in) :: alpha, x, y
    real(8) :: res

    res = alpha * x + y
  end function axpy

end module kernels
