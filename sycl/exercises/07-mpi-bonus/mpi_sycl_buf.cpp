// SPDX-FileCopyrightText: 2021 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include <cstdio>
#include <iostream>
#include <sstream>
#include <vector>

#include <mpi.h>
#include <sycl/sycl.hpp>

#if defined(__ACPP__)
  #define INTEROP_TASK                         AdaptiveCpp_enqueue_custom_operation
  #if defined(SYCL_BACKEND_HIP)
      #define SYCL_BACKEND                     sycl::backend::hip
  #elif defined(SYCL_BACKEND_CUDA)
      #define SYCL_BACKEND                     sycl::backend::cuda
  #else
      #error Define SYCL_BACKEND_HIP or SYCL_BACKEND_CUDA
  #endif
#elif defined(__INTEL_LLVM_COMPILER)
  #ifdef SYCL_EXT_ONEAPI_ENQUEUE_NATIVE_COMMAND
    #define INTEROP_TASK                       ext_codeplay_enqueue_native_command
  #else
    #define INTEROP_TASK                       host_task
  #endif
  #if defined(SYCL_BACKEND_HIP)
      #define SYCL_BACKEND                     sycl::backend::ext_oneapi_hip
  #elif defined(SYCL_BACKEND_CUDA)
      #define SYCL_BACKEND                     sycl::backend::ext_oneapi_cuda
  #else
      #error Define SYCL_BACKEND_HIP or SYCL_BACKEND_CUDA
  #endif
#endif

// See https://en.wikipedia.org/wiki/C_preprocessor#Token_stringification
#define STR(s) #s
#define PRINT_MACRO(s) printf("%s=%s\n", #s, STR(s))

template <typename T>
void init(sycl::queue &q, const size_t n, T &x_buf)
{
    q.submit([&](sycl::handler& h) {
        sycl::accessor x{x_buf, h, sycl::write_only};

        auto kernel = [=](sycl::id<1> i) {
            x[i] = 1.1 * (size_t)i;
        };
        h.parallel_for(sycl::range{x_buf.size()}, kernel);
    });
}

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    const size_t n = argc > 1 ? (size_t)std::stoll(argv[1]) : 1024;
    const size_t nbytes = sizeof(double) * n;

    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    auto gpu_devices = sycl::device::get_devices(sycl::info::device_type::gpu);
    auto count = std::size(gpu_devices);
    auto device = gpu_devices[rank];
    sycl::queue q{device, sycl::property::queue::in_order{}};

    printf("Hello from MPI rank %d/%d with a GPU of %zu\n", rank, size, count);
    PRINT_MACRO(INTEROP_TASK);
    PRINT_MACRO(SYCL_BACKEND);

    // Device data
    std::vector<double> h_x(n);

    sycl::buffer<double, 1> x(h_x.data(), sycl::range<1>(n));
    q.wait();

    if (rank == 0) {
        // Initialize data on rank 0
        init(q, n, x);
        q.wait();

        // Send with rank 0
        q.submit([&](sycl::handler &h) {
            sycl::accessor x_acc{x, h, sycl::read_only};
            h. INTEROP_TASK([=](sycl::interop_handle ih) {
                auto x_ptr = reinterpret_cast<void *>(ih.get_native_mem<SYCL_BACKEND>(x_acc));
                MPI_Send(x_ptr, n, MPI_DOUBLE, 1, 123, MPI_COMM_WORLD);
            });
        });
        printf("Rank %d sent\n", rank);
    } else if (rank == 1) {
        // Receive with rank 1
        q.submit([&](sycl::handler &h) {
            sycl::accessor x_acc{x, h, sycl::write_only};
            h. INTEROP_TASK([=](sycl::interop_handle ih) {
                auto x_ptr = reinterpret_cast<void *>(ih.get_native_mem<SYCL_BACKEND>(x_acc));
                MPI_Recv(x_ptr, n, MPI_DOUBLE, 0, 123, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            });
        });
        printf("Rank %d received\n", rank);
    }
    q.wait();

    // Copy result to CPU and print
    {
        sycl::host_accessor h_y{x, sycl::read_only};
        std::stringstream ss;
        ss << "Rank " << rank << " has";
        for (int i = 0; i < std::min(8ul, n); ++i) ss << " " << h_y[i];
        if (n > 8) ss << " ...";
        ss << "\n";
        std::cout << ss.str();
    }

    MPI_Finalize();
}
