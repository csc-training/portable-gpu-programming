#include <iostream>
#include <Kokkos_Core.hpp>

using std::size;

template <typename T>
void init(T &x, T &y)
{
    auto kernel = KOKKOS_LAMBDA(const size_t i) {
        x[i] = sin((double)i) * 2.3;
        y[i] = cos((double)i) * 1.1;
    };

    Kokkos::parallel_for(size(x), kernel);
}

template <typename T1, typename T2>
void axpy(const T1 a, const T2 &x, T2 &y)
{
    auto kernel = KOKKOS_LAMBDA(const size_t i) {
        y[i] += a * x[i];
    };

    Kokkos::parallel_for(size(x), kernel);
}

template <typename T>
bool check(const T &y);

int main(int argc, char *argv[])
{
    const size_t n = argc > 1 ? (size_t)std::stoll(argv[1]) : 1000000;
    const size_t nit = argc > 2 ? (size_t)std::stoll(argv[2]) : 4;
    const size_t nbytes = sizeof(double) * n;
    double a = 3.4;

#ifndef BENCHMARK
    printf("Vector size %zu (%.2f MiB)\n", n, (double)nbytes / (1024*1024));
    fflush(stdout);
#endif

    Kokkos::initialize(argc, argv);
    {

    Kokkos::View<double*> x("x", n);
    Kokkos::View<double*> y("y", n);

    // Initialize data on GPU
    init(x, y);

    // Calculate on GPU
    axpy(a, x, y);

#ifndef BENCHMARK
    // Copy result to CPU and check correctness
    auto h_y = Kokkos::create_mirror_view_and_copy(Kokkos::HostSpace(), y);
    if (check(h_y)) {
        printf("Correctness OK!\n");
        fflush(stdout);
    } else {
        printf("Correctness ERROR!\n");
        return 1;
    }
#endif

    // Measure performance
    Kokkos::fence();
    using clock = std::chrono::high_resolution_clock;
    auto t0 = clock::now();
    for (size_t i = 0; i < nit; i++) {
        axpy(a, x, y);
    }
    Kokkos::fence();
    auto t1 = clock::now();
    double time = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count() / 1e9;
    double gflops = 2.0 * n * nit / time / 1e9;
    double gbytess = 3.0 * nbytes * nit / time / 1e9;

#ifndef BENCHMARK
    printf("Time: %.4f s\n", time);
    printf("Performance: %.3f GFLOPS, %.3f GB/s\n", gflops, gbytess);
#else
    printf("%16zu %16.4f %16.3f %16.3f\n", n, (double)nbytes / pow(1024, 2), gflops, gbytess);
#endif

    }
    Kokkos::finalize();

    return 0;
}

template <typename T>
bool check(const T &y)
{
    double tolerance = 1e-6;
    for (size_t i = 0; i < y.size(); i++) {
        double y_ref = 3.4 * sin(i) * 2.3 + cos(i) * 1.1;

        if (abs(y_ref - y[i]) > tolerance) {
            return false;
        }
    }
    return true;
}
