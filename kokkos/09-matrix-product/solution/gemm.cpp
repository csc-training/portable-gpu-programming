// SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include <Kokkos_Core.hpp>
#include <Kokkos_Random.hpp>
#include <KokkosBlas3_gemm.hpp>

void gemm(const int M, const int N, const int K, const int iterations) {

    // Define Kokkos views for matrices
    Kokkos::View<double**> A("A", M, K);
    Kokkos::View<double**> B("B", K, N);
    Kokkos::View<double**> C("C", M, N);

    double alpha = 1.234567;
    double beta = 0.89987234;

    // Initialize matrices with random numbers
    Kokkos::Random_XorShift64_Pool<> random_pool(12345);
    Kokkos::fill_random(A, random_pool, -100.0, 100.0);
    Kokkos::fill_random(B, random_pool, -100.0, 100.0);
    Kokkos::fill_random(C, random_pool, -100.0, 100.0);

    Kokkos::Timer timer;
    double gemm_time;
    
    for (int iter = 0; iter<=iterations; iter++) {

      // One warm-up iteration
      if (iter==1) gemm_time = timer.seconds();
 
      KokkosBlas::gemm("N", "N", alpha, A, B, beta, C);
      Kokkos::fence();

    }

    gemm_time = timer.seconds() - gemm_time;

    double avgtime = gemm_time / (double) iterations;
    double nflops = 2.0 * M * N * K;
    std::cout << "Performance with random matrices (TF/s): " << 1.0e-12 * nflops/avgtime
              << std::endl;
 
   
    // Fill matrices with "nice" integers
    // In some GPU hardware the performance is much higher than with the
    // random numbers due to fact the with integers there are fewer bits to flip
    // which consumes less power and GPU can run with higher clock rate

    alpha = 2.0;
    beta = 3.0;

    Kokkos::parallel_for("Init_A", Kokkos::MDRangePolicy<Kokkos::Rank<2>>({0, 0}, {M, K}),
      KOKKOS_LAMBDA(const int i, const int j) {
        A(i, j) = i + j;
    });

    Kokkos::parallel_for("Init_B", Kokkos::MDRangePolicy<Kokkos::Rank<2>>({0, 0}, {K, N}),
      KOKKOS_LAMBDA(const int i, const int j) {
        B(i, j) = 4*j - 2*i;
    });

    Kokkos::parallel_for("Init_C", Kokkos::MDRangePolicy<Kokkos::Rank<2>>({0, 0}, {M, N}),
      KOKKOS_LAMBDA(const int i, const int j) {
        C(i, j) = i - j;
    });

    for (int iter = 0; iter<=iterations; iter++) {

      // One warm-up iteration
      if (iter==1) gemm_time = timer.seconds();
 
      KokkosBlas::gemm("N", "N", alpha, A, B, beta, C);
      Kokkos::fence();

    }

    gemm_time = timer.seconds() - gemm_time;

    avgtime = gemm_time / (double) iterations;
    nflops = 2.0 * M * N * K;
    std::cout << "Performance with integer matrices (TF/s): " << 1.0e-12 * nflops/avgtime 
              << std::endl;
}

int main(int argc, char** argv) {
    Kokkos::initialize(argc, argv);

    int niter;
    int n;
    try {
        if (argc < 2) {
          throw "Usage: <# iterations> <matrix order>";
        }

        niter  = std::atoi(argv[1]);
        if (niter < 1) {
          throw "ERROR: iterations must be >= 1";
        }

        n = std::atoi(argv[2]);
        if (n <= 0) {
          throw "ERROR: Matrix Order must be greater than 0";
        }
    }
    catch (const char * e) {
      std::cout << e << std::endl;
      return 1;
    }

    std::cout << "Number of iterations = " << niter << std::endl;
    std::cout << "Matrix order         = " << n << std::endl;

    std::cout << "Execution Space: " <<
      Kokkos::DefaultExecutionSpace::name() << std::endl;
    std::cout << "Memory Space: " <<
      Kokkos::DefaultExecutionSpace::memory_space::name() << std::endl;

    const int M = n;  
    const int N = n;  
    const int K = n;  

    gemm(M, N, K, niter);

    Kokkos::finalize();
    return 0;
}

