// Copyright (C) 2023 Intel Corporation
// SPDX-FileCopyrightText: 2021 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include <iostream>
#include <numeric>
#include <sycl/sycl.hpp>

using namespace sycl;

int main() {
  constexpr size_t N = 1024;
  constexpr size_t B = 16;

  
  queue q;
  int *data=new int[N];//int* data = malloc_shared<int>(N, q);
  int *sum=new int[1]; //int* sum = malloc_shared<int>(1, q);
  std::iota(data, data + N, 1);
  *sum = 0;

  // Create buffers for data and sum
  buffer<int, 1> data_buffer(data, range<1>(N));
  buffer<int, 1> sum_buffer(sum, range<1>(1));

  q.submit([&](handler &h) {
    auto data_acc = data_buffer.get_access<access::mode::read>(h);
    auto sum_acc = sum_buffer.get_access<access::mode::read_write>(h);
    local_accessor<int, 1> shtmp(range<1>(2*B), h);

    h.parallel_for(nd_range<1>{N, B}, [=](nd_item<1> it) {
      int i = it.get_global_id(0);
      auto grp = it.get_group();

      int tid = it.get_local_id(0);


      shtmp[tid] = 0;
      shtmp[tid + B] = 0;
      if (i < N / 2) {
        shtmp[tid] = data_acc[i];
      }
      if (i + N / 2 < N) {
        shtmp[tid + B] = data_acc[i + N / 2];
      }
      it.barrier();

      for (int s = B; s > 0; s >>= 1) {
        if (tid < s) {
          shtmp[tid] += shtmp[tid + s];
        }
        it.barrier();
      }

      if (grp.leader()) {
        atomic_ref<int, memory_order::relaxed,
                   memory_scope::system,
                   access::address_space::global_space>(
            sum_acc[0]) += shtmp[0];
      }
    });

  }).wait();


  // Access the sum value from the buffer
  int final_sum = sum_buffer.get_access<access::mode::read>()[0];

  std::cout << "sum = " << final_sum << "\n";
  bool passed = (final_sum == ((N * (N + 1)) / 2));
  std::cout << ((passed) ? "SUCCESS" : "FAILURE") << "\n";

  // No need to free the buffers explicitly as they will be automatically released
  // when they go out of scope.

  return (passed) ? 0 : 1;
}
