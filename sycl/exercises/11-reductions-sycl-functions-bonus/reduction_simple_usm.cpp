// SPDX-FileCopyrightText: 2021 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

// We use built-in sycl::reduction mechanism in this example.
// The manual implementation of the reduction kernel can be found in folder 04-reduction
#include <sycl/sycl.hpp>

using namespace sycl;

int main() {
  queue q;
  unsigned n = 10;

  // Allocate memory for sum using malloc_shared
  int   *sum = malloc_shared<int>(1, q);

  // Initialize sum
   *sum = 0;

  // Submit a SYCL kernel into a queue
  q.submit([&](handler &cgh) {
    // Create temporary object describing variables with reduction semantics
    auto sum_acc = reduction(sum, plus<int>());
    
    // A reference to the reducer is passed to the lambda
    cgh.parallel_for(range<1>{n}, sum_acc,
                    [=](id<1> idx, auto &reducer) { reducer.combine(idx[0]); });
  }).wait();

  // The contents of sum_usm are already available in host memory

  // Print results
  std::cout << "sum = " << *sum << "\n";
  bool passed = (*sum == (((n-1) * n) / 2));
  std::cout << ((passed) ? "SUCCESS" : "FAILURE") << "\n";

  // Free the allocated memory
  free(sum, q);
  
  return 0;
}
