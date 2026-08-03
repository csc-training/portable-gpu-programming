// SPDX-FileCopyrightText: 2024 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include <Kokkos_Core.hpp>
#include <iostream>

int main(int argc, char* argv[]) {

  // Initialize Kokkos
  Kokkos::initialize(argc, argv);

  {
    int n = 20;
    int m = 20;

    Kokkos::View<int**> a("a", n, m); // 1D array with runtime dimension 

    // Subviews of boundaries
    auto top_slice = Kokkos::subview(a, 0, Kokkos::ALL());
    auto bottom_slice = Kokkos::subview(a, n-1, Kokkos::ALL());
    auto left_slice = Kokkos::subview(a, Kokkos::ALL(), 0);
    auto right_slice = Kokkos::subview(a, Kokkos::ALL(), m-1);

    // Initialize boundaries in parallel
    Kokkos::parallel_for(m, KOKKOS_LAMBDA(const int i) {
      top_slice(i) = -1;
      bottom_slice(i) = -2;
    });

    Kokkos::parallel_for(n, KOKKOS_LAMBDA(const int i) {
      left_slice(i) = -5;
      right_slice(i) = -6;
    });
    Kokkos::fence();

    // For copying to host, we need contiguous buffers in device
    Kokkos::View<int*> top("top", top_slice.extent(0));
    Kokkos::deep_copy(top, top_slice);
    Kokkos::View<int*> bottom("bottom", bottom_slice.extent(0));
    Kokkos::deep_copy(bottom, bottom_slice);
    Kokkos::View<int*> left("left", left_slice.extent(0));
    Kokkos::deep_copy(left, left_slice);
    Kokkos::View<int*> right("right", right_slice.extent(0));
    Kokkos::deep_copy(right, right_slice);
    

    // Create mirror views
    auto h_top = Kokkos::create_mirror(top);
    auto h_bottom = Kokkos::create_mirror(bottom);
    auto h_left = Kokkos::create_mirror(left);
    auto h_right = Kokkos::create_mirror(right);

    // Copy to host
    Kokkos::deep_copy(h_top, top);
    Kokkos::deep_copy(h_bottom, bottom);
    Kokkos::deep_copy(h_left, left);
    Kokkos::deep_copy(h_right, right);

    Kokkos::fence();

    // Calculate the mean values in host, neglect corners
    int sum = 0;
    for (int i=1; i < h_top.extent(0) - 1; i++) {
      sum += h_top(i);
    }
    std::cout << "Top boundary " << sum << std::endl;

    sum = 0;
    for (int i=1; i < h_bottom.extent(0) - 1; i++) {
      sum += h_bottom(i);
    }
    std::cout << "Bottom boundary " << sum << std::endl;

    sum = 0;
    for (int i=1; i < h_left.extent(0) - 1; i++) {
      sum += h_left(i);
    }
    std::cout << "Left boundary " << sum << std::endl;

    sum = 0;
    for (int i=1; i < h_right.extent(0) - 1; i++) {
      sum += h_right(i);
    }
    std::cout << "Rigth boundary " << sum << std::endl;

  }
  Kokkos::finalize();
  return 0;
}
