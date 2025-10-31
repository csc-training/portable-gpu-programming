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

    auto start = std::chrono::high_resolution_clock::now().time_since_epoch().count();

    range<1> work_items { N*N};
    //# Define queue with default device for offloading computation
    sycl::property_list q_prof{property::queue::enable_profiling{}, sycl::property::queue::in_order{}};
    //queue q{property::queue::enable_profiling{}};
    queue q{default_selector_v,q_prof};
    //queue q{property::queue::enable_profiling{}};

    auto matrix_a=malloc_shared<float>(N*N, q);
    auto matrix_b=malloc_shared<float>(N*N, q);
    auto matrix_c=malloc_shared<float>(N*N, q);
    auto matrix_d=malloc_shared<float>(N*N, q);
    float v1 = 2.f;
    float v2 = 3.f;
    for (int i=0; i<N; i++)
        for (int j=0; j<N; j++){
            matrix_a[i*N+j] = v1++;
            matrix_b[i*N+j] = v2++;
            matrix_c[i*N+j] = 0.f;
            matrix_d[i*N+j] = 0.f;
    }
    q.submit([&](handler &h){
        
        //# Define size for ND-Range and work-group size
        range<2> global_size(N,N);
        range<2> work_group_size(M,M);
        //# Parallel Compute Matrix Multiplication
        h.parallel_for(nd_range<2>{global_size, work_group_size}, [=](nd_item<2> item){
            const int i = item.get_global_id(1);
            const int j = item.get_global_id(0);
                //# Use private mem to store intermediate result
                float temp=0.f;
                for (int k = 0; k < N; k++) {
                   temp += matrix_a[i*N+k] * matrix_b[k*N+j];
               }
               matrix_c[i*N+j] = temp;

         });
    });
    q.fill(matrix_c, 0.0f, N * N).wait();
    
    //Measure the execution time via events
    event e;
    std::cout << "Offload Device        : " << q.get_device().get_info<info::device::name>() << "\n";
    std::cout << "max_work_group_size   : " << q.get_device().get_info<info::device::max_work_group_size>() << "\n";
    std::cout << "Configuration         : MATRIX_SIZE= " << N << "x" << N << "\n";
    
    
    

    std::cout << "Now the matrix-matrix  multiplication." << "\n"; 
    
    //# Submit command groups to execute on device
    e = 
    q.submit([&](handler &h){


            //# Define size for ND-Range and work-group size
            range<2> global_size(N,N);
            range<2> work_group_size(M,M);

             //# Create local accessors. They use the memory closer to the chip.
            //# In SYCL called local memory. On nvidia and AMD thw so-called shared memory
            //accessor<float, 2, access::mode::read_write, access::target::local> A_tile(range<2>(M, M), h);
            //accessor<float, 2, access::mode::read_write, access::target::local> B_tile(range<2>(M, M), h);
            local_accessor<float, 2> A_tile(range<2>(M, M), h);
	    local_accessor<float, 2> B_tile(range<2>(M, M), h);
	    //# Parallel Compute Matrix Multiplication
            h.parallel_for(nd_range<2>{global_size, work_group_size}, [=](nd_item<2> item){
                const int i = item.get_global_id(0);
                const int j = item.get_global_id(1);
                const int x = item.get_local_id(0);
                const int y = item.get_local_id(1);
                float temp=0.f;
                int k;
            for (int t = 0; t < N; t+=M) {
                // save a tile locally for fast access by all threads in a group (block in cuda/hip)
                A_tile[x][y] = matrix_a[i * N + (t + y)]; //Contiguous access?
                B_tile[x][y] = matrix_b[(t + x) * N + j]; //Contiguous access?
                item.barrier(access::fence_space::local_space); // barrier within the group
                for (k = 0; k < M; k++) {
                    temp += A_tile[x][k] * B_tile[k][y];
                }
                item.barrier(access::fence_space::local_space); // barrier within the group
              }
              matrix_c[i*N+j] = temp;

            });           
        });
    

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
                matrix_d[i*N+j] =0.0f;
                for(int k=0; k<N; k++){
                    matrix_d[i*N+j] += matrix_a[i*N+k] * matrix_b[k*N+j];
                  //matrix_c[i*N+j] += matrix_a[i*N+k] * matrix_b[k*N+j];
                }
                if(std::abs(matrix_c[i*N+j] - matrix_d[i*N+j])/matrix_c[i*N+j]>1.0e-5) {
                    fail = 1;
                    std::cout << i <<  "  " << j << "\n";
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
