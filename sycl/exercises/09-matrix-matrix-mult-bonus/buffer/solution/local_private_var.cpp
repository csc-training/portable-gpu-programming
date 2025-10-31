// Copyright © 2021 Intel Corporation
// SPDX-FileCopyrightText: 2021 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include <sycl/sycl.hpp>
#include <ctime>
#include <chrono>
#include <getopt.h>

using namespace sycl;

int main(int argc, char *argv[]) {

    size_t N = 1024;
    size_t M = 16;
    int VERIFY = 0;
    int PRINT_OUTPUT_MATRIX = 0;

    int arg;
    while ((arg = getopt (argc, argv, "n:m:vp")) != -1)
        switch (arg){
            case 'n':
                N = std::atoi(optarg);
                break;
            case 'm':
                M = std::atoi(optarg);
                break;
            case 'v':
                VERIFY = 1;
                break;
            case 'p':
                PRINT_OUTPUT_MATRIX = 1;
                break;
            case 'h':
                std::cout << std::endl;
                std::cout << "Usage   : ./a.out -n <MATRIX_SIZE> -m <WORK_GROUP_SIZE> -v -p\n\n";
                std::cout << "          [-n] size for matrix, eg: 1024\n";
                std::cout << "          [-m] size of work_group, eg: 8/16\n";
                std::cout << "          [-v] verify output with linear computation on cpu\n";
                std::cout << "          [-p] print output matrix\n";
                std::cout << "Example : ./a.out -n 1024 -m 16 -v -p\n\n";
                std::exit(0);
        }

    //# Define vectors for matricies
    std::vector<float> matrix_a(N*N);
    std::vector<float> matrix_b(N*N);
    std::vector<float> matrix_c(N*N);
    std::vector<float> matrix_d(N*N);

    //# Initialize matricies with values
    float v1 = 2.f;
    float v2 = 3.f;
    for (int i=0; i<N; i++)
        for (int j=0; j<N; j++){
            matrix_a[i*N+j] = v1++;
            matrix_b[i*N+j] = v2++;
            matrix_c[i*N+j] = 0.f;
            matrix_d[i*N+j] = 0.f;
    }

    auto start = std::chrono::high_resolution_clock::now().time_since_epoch().count();

    //# Define queue with default device for offloading computation
    queue q{property::queue::enable_profiling{}};


    // First we warm-up the device
    std::cout << "Warm-up first" << "\n"; 

    {
        //# Create buffers for matrices

        buffer<float, 1> a(matrix_a.data(), range<1>(N*N));
        buffer<float, 1> b(matrix_b.data(), range<1>(N*N));
        buffer<float, 1> c(matrix_c.data(), range<1>(N*N));
        
        //  buffer a(matrix_a);
        // buffer b(matrix_b);
        // buffer c(matrix_c); 
         //# Submit command groups to execute on device
         q.submit([&](handler &h){
            //# Create accessors to copy buffers to the device
            auto A = a.get_access<access::mode::read>(h);
            auto B = b.get_access<access::mode::read>(h);
            auto C = c.get_access<access::mode::write>(h);

            //# Define size for ND-Range and work-group size
            range<2> global_size(N,N);
            range<2> work_group_size(M,M);

            //# Parallel Compute Matrix Multiplication
            h.parallel_for(nd_range<2>{global_size, work_group_size}, [=](nd_item<2> item){
                const int i = item.get_global_id(0);
                const int j = item.get_global_id(1);
                //# Use private mem to store intermediate result
                float temp=0.f;
                for (int k = 0; k < N; k++) {
                   temp += A[i*N+k] * B[k*N+j];
               }
               C[i*N+j]  = temp;
            });
        });
    } 

    //# Initialize matrices with values
    v1 = 2.f;
    v2 = 3.f;
    for (int i=0; i<N; i++)
        for (int j=0; j<N; j++){
            matrix_a[i*N+j] = v1++;
            matrix_b[i*N+j] = v2++;
            matrix_c[i*N+j] = 0.f;
            matrix_d[i*N+j] = 0.f;
    }

    event e;
    std::cout << "Offload Device        : " << q.get_device().get_info<info::device::name>() << "\n";
    std::cout << "max_work_group_size   : " << q.get_device().get_info<info::device::max_work_group_size>() << "\n";
    std::cout << "Configuration         : MATRIX_SIZE= " << N << "x" << N << "\n";
    {
        //# Create buffers for matrices

        buffer<float, 1> a(matrix_a.data(), range<1>(N*N));
        buffer<float, 1> b(matrix_b.data(), range<1>(N*N));
        buffer<float, 1> c(matrix_c.data(), range<1>(N*N));
        /*buffer a(matrix_a);
        buffer b(matrix_b);
        buffer c(matrix_c);*/

        //# Submit command groups to execute on device
        e = q.submit([&](handler &h){
            //# Create accessors to copy buffers to the device
            auto A = a.get_access<access::mode::read>(h);
            auto B = b.get_access<access::mode::read>(h);
            auto C = c.get_access<access::mode::write>(h);

            //# Define size for ND-Range and work-group size
            range<2> global_size(N,N);
            range<2> work_group_size(M,M);

            //# Parallel Compute Matrix Multiplication
            h.parallel_for(nd_range<2>{global_size, work_group_size}, [=](nd_item<2> item){
                const int i = item.get_global_id(0);
                const int j = item.get_global_id(1);
                for (int k = 0; k < N; k++) {
                    C[i*N+j] += A[i*N+k] * B[k*N+j];
                }
            });
        });
    }


    auto kernel_duration = (e.get_profiling_info<info::event_profiling::command_end>() - e.get_profiling_info<info::event_profiling::command_start>());
    std::cout << "Kernel Execution Time : " << kernel_duration / 1e+9 << " seconds" << "\n";

    auto duration = std::chrono::high_resolution_clock::now().time_since_epoch().count() - start;
    std::cout << "Compute Duration      : " << duration / 1e+9 << " seconds\n";

    //# Print Output
    if (PRINT_OUTPUT_MATRIX){
        for (int i=0; i<N; i++){
            for (int j=0; j<N; j++){
                std::cout << matrix_c[i*N+j] << " ";
            }
            std::cout << "\n";
        }
    } else {
        std::cout << " [0][0] = " << matrix_c[0] << "\n";
    }

    //# Compute local and compare with offload computation
    if (VERIFY){
        int fail = 0;
        for(int i=0; i<N; i++){
            for (int j = 0; j < N; j++) {
                for(int k=0; k<N; k++){
                    matrix_d[i*N+j] += matrix_a[i*N+k] * matrix_b[k*N+j];
                }
                if(fabs(matrix_c[i*N+j] - matrix_d[i*N+j])>1.0e-1) 
                {
                    fail = 1;
                     //std::cout << matrix_c[i*N+j] << "   " <<  matrix_d[i*N+j] << "  " <<matrix_c[i*N+j] - matrix_d[i*N+j] <<" FAIL\n";
                }

            }
        }
        if(fail == 1){
            std::cout << "FAIL\n";
        } else {
            std::cout << "PASS\n";
        }
    }
}
