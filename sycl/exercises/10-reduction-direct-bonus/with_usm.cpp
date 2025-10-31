// Copyright (C) 2023 Intel Corporation
// SPDX-FileCopyrightText: 2021 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include <cstdio>
#include <numeric>
#include <sycl/sycl.hpp>

using namespace sycl;

int main() {
  constexpr size_t N = 1024;
  constexpr size_t B = 16;

  queue q;
  int* data = malloc_shared<int>(N, q);
  int* sum = malloc_shared<int>(1, q);
  std::iota(data, data + N, 1);
  *sum = 0;
  
   
  q.submit([&](handler &h)
  {
    local_accessor<int, 1> shtmp(range<1>(2*B), h);
     h.parallel_for(nd_range<1>{N, B}, [=](nd_item<1> it)
     {
      int i = it.get_global_id(0);
      auto grp = it.get_group();

      int tid = it.get_local_id(0);
      shtmp[tid] = 0;
      shtmp[tid + B] = 0;
      if (i < N / 2) 
      {
        shtmp[tid] = data[i];
      }
      if (i + N / 2 < N) 
      {
         shtmp[tid + B] = data[i + N / 2];
      }
      it.barrier();
      for (int s = B; s > 0; s >>= 1)
      {
        if (tid < s) 
        {
          shtmp[tid] += shtmp[tid + s];
        }
        it.barrier();
      }

      if (grp.leader()) {
       atomic_ref<int, memory_order::relaxed,
                  memory_scope::system,
                  access::address_space::global_space>(
           *sum) += shtmp[0];
     }

     });
    
  }).wait();
  
  std::cout << "sum = " << *sum << "\n";
  bool passed = (*sum == ((N * (N + 1)) / 2));
  std::cout << ((passed) ? "SUCCESS" : "FAILURE") << "\n";

  free(sum, q);
  free(data, q);
  return (passed) ? 0 : 1;
}
