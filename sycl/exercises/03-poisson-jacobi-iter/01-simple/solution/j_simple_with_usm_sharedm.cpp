#include <chrono>
#include <cstdio>
#include <iostream>
#include <vector>

#include <chrono>
#include <getopt.h>
#include <sycl/sycl.hpp>

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

    //# Define vectors for matrices
    const int nx=N, ny=N;
    const int mx=M, my=M;
    const int niter=100;
    const double factor =0.5;

    //# Define queue with default device for offloading computation
    sycl::property_list q_prof{property::queue::enable_profiling{}, sycl::property::queue::in_order{}};
    //queue q{property::queue::enable_profiling{}};
    queue q{default_selector_v,q_prof};
    //queue q{property::queue::enable_profiling{}};
    //queue q();
    //queue q{property::queue::enable_profiling{}, property::queue::in_order()};


    auto matrix_u=malloc_shared<float>(nx*ny, q);
    auto matrix_unew=malloc_shared<float>(nx*ny, q);
    
    range<1> work_items { N*N};
    
    

    // Initialize u
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            int ind = i * ny + j;
            matrix_u[ind] = ((i - nx / 2) * (i - nx / 2)) / nx +
                            ((j - ny / 2) * (j - ny / 2)) / ny;
        }
    }

    std::cout << "Offload Device        : " << q.get_device().get_info<info::device::name>() << "\n";
    std::cout << "max_work_group_size   : " << q.get_device().get_info<info::device::max_work_group_size>() << "\n";
    std::cout << "Configuration         : MATRIX_SIZE= " << nx << "x" << ny << "\n";

    std::cout << " [0][0] = " << matrix_u[0] << "\n";

    std::cout << "Warm up the device  \n";
    
    q.submit([&](handler &h){
        
        range<2> global_size(nx,ny);
        
        //# Parallel Compute
        h.parallel_for(global_size, [=](id<2> item){
            const int i = item[0];
            const int j = item[1];
            int ind = i * ny + j;
            int ip = (i + 1) * ny + j;
            int im = (i - 1) * ny + j;
            int jp = i * ny + j + 1;
            int jm = i * ny + j - 1;
            if(i>0 && i<nx-1 && j>0 && j< ny-1){
                matrix_unew[ind] = factor * ( matrix_u[ip]-2.0*matrix_u[ind] +  matrix_u[im] +
                                                   matrix_u[jp]-2.0*matrix_u[ind] +  matrix_u[jm]);
            }
        });
    }); 
    q.wait();
    std::cout << "Warm up done  \n";
    auto start = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    event e;
    auto kernel_duration=0.0;
    for(int iter=0;iter<niter; iter++){
        e = q.submit([&](handler &h){
            
            
            range<2> global_size(nx,ny);
            
            h.parallel_for(global_size, [=](id<2> item){
                const int i = item[0];
                const int j = item[1];
                int ind = i * ny + j;
                int ip = (i + 1) * ny + j;
                int im = (i - 1) * ny + j;
                int jp = i * ny + j + 1;
                int jm = i * ny + j - 1;
                if(i>0 && i<nx-1 && j>0 && j< ny-1){
                     matrix_unew[ind] = factor * ( matrix_u[ip]-2.0*matrix_u[ind] +  matrix_u[im] +
                                                   matrix_u[jp]-2.0*matrix_u[ind] +  matrix_u[jm]);
                }
            });
        }); 
        q.wait();
        kernel_duration += (e.get_profiling_info<info::event_profiling::command_end>() - e.get_profiling_info<info::event_profiling::command_start>());


        e = q.submit([&](handler &h){ 
            range<2> global_size(nx,ny);
          
            h.parallel_for(global_size, [=](id<2> item){
                const int i = item[0];
                const int j = item[1];
                int ind = i * ny + j;
                int ip = (i + 1) * ny + j;
                int im = (i - 1) * ny + j;
                int jp = i * ny + j + 1;
                int jm = i * ny + j - 1;
                if(i>0 && i<nx-1 && j>0 && j< ny-1){
                     matrix_u[ind] = factor * ( matrix_unew[ip]-2.0*matrix_unew[ind] +  matrix_unew[im] +
                                                matrix_unew[jp]-2.0*matrix_unew[ind] +  matrix_unew[jm]);
                }
            });
        });
        q.wait();
        kernel_duration += (e.get_profiling_info<info::event_profiling::command_end>() - e.get_profiling_info<info::event_profiling::command_start>());          
    }
    q.wait();

 
    std::cout << "Kernel Execution Time : " << kernel_duration / 1e+9 << " seconds" << "\n";

    auto duration = std::chrono::high_resolution_clock::now().time_since_epoch().count() - start;
    std::cout << "Compute Duration      : " << duration / 1e+9 << " seconds\n";

    //# Print Output
    if (PRINT_OUTPUT_MATRIX){
        /*for (int i=0; i<N; i++){
            for (int j=0; j<N; j++){
                std::cout << matrix_c[i*N+j] << " ";
            }
            std::cout << "\n";
        }*/
    } else {
        std::cout << " [0][0] = " << matrix_u[0] << "\n";
    }
}
