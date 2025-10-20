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

    //# Define vectors for matrices
    const int nx=N, ny=N;
    const int niter=100;
    const float factor =0.25;
    std::vector<float> matrix_u(nx*ny);
    std::vector<float> matrix_unew(nx*ny);
    
    // Initialize u
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            int ind = i * ny + j;
            matrix_u[ind] = ((i - nx / 2) * (i - nx / 2)) / nx +
                            ((j - ny / 2) * (j - ny / 2)) / ny;
        }
    }

    //# Define queue with default device for offloading computation
    sycl::property_list q_prof{property::queue::enable_profiling{}, sycl::property::queue::in_order{}};
    queue q{default_selector_v,q_prof};

    std::cout << "Offload Device        : " << q.get_device().get_info<info::device::name>() << "\n";
    std::cout << "max_work_group_size   : " << q.get_device().get_info<info::device::max_work_group_size>() << "\n";
    std::cout << "Configuration         : MATRIX_SIZE= " << nx << "x" << ny << "\n";
    std::cout << " [0][0] = " << matrix_u[0] << "\n";
    std::cout << "Warm up the device  \n";

    {
        //# Create buffers for matrices
        buffer<float, 1> u(matrix_u.data(), range<1>(nx*ny));
        buffer<float, 1> unew(matrix_unew.data(), range<1>(nx*ny));

        range<2> local_size(M, M);
        range<2> global_size(((nx + M - 1)/M)*M, ((ny + M - 1)/M)*M);
        nd_range<2> r(global_size, local_size);

        //# Submit command group for warm-up
        q.submit([&](handler &h){
            accessor U(u, h, read_only);
            accessor UNEW(unew, h, write_only);

            // Updated to use local_accessor
            local_accessor<float, 2> tile(range<2>(M+2, M+2), h);

            h.parallel_for(r, [=](nd_item<2> item){
                int global_i = item.get_global_id(0);
                int global_j = item.get_global_id(1);

                int local_i = item.get_local_id(0) + 1;
                int local_j = item.get_local_id(1) + 1;

                if(global_i < nx && global_j < ny)
                    tile[local_i][local_j] = U[global_i * ny + global_j];

                if(item.get_local_id(0) == 0 && global_i > 0)
                    tile[0][local_j] = U[(global_i-1)*ny + global_j];
                if(item.get_local_id(0) == local_size[0]-1 && global_i < nx-1)
                    tile[M+1][local_j] = U[(global_i+1)*ny + global_j];
                if(item.get_local_id(1) == 0 && global_j > 0)
                    tile[local_i][0] = U[global_i*ny + global_j-1];
                if(item.get_local_id(1) == local_size[1]-1 && global_j < ny-1)
                    tile[local_i][M+1] = U[global_i*ny + global_j+1];

                item.barrier(access::fence_space::local_space);

                if(global_i > 0 && global_i < nx-1 && global_j > 0 && global_j < ny-1){
                    UNEW[global_i * ny + global_j] =
                        factor * (tile[local_i+1][local_j] - 2.0f * tile[local_i][local_j] + tile[local_i-1][local_j] +
                                  tile[local_i][local_j+1] - 2.0f * tile[local_i][local_j] + tile[local_i][local_j-1]);
                }
            });
        });
    }
    q.wait();
    std::cout << "Warm up done!  \n";
    
    auto start = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    event e;
    auto kernel_duration=0.0;

    {
        buffer<float, 1> u(matrix_u.data(), range<1>(nx*ny));
        buffer<float, 1> unew(matrix_unew.data(), range<1>(nx*ny));

        range<2> local_size(M, M);
        range<2> global_size(((nx + M - 1)/M)*M, ((ny + M - 1)/M)*M);
        nd_range<2> r(global_size, local_size);

        for(int iter=0;iter<niter; iter++)
        { 
            e = q.submit([&](handler &h){
                accessor U(u, h, read_only);
                accessor UNEW(unew, h, write_only);

                // Local_accessor
                local_accessor<float, 2> tile(range<2>(M+2, M+2), h);

                h.parallel_for(r, [=](nd_item<2> item){
                    int global_i = item.get_global_id(0);
                    int global_j = item.get_global_id(1);

                    int local_i = item.get_local_id(0) + 1;
                    int local_j = item.get_local_id(1) + 1;

                    if(global_i < nx && global_j < ny)
                        tile[local_i][local_j] = U[global_i * ny + global_j];

                    if(item.get_local_id(0) == 0 && global_i > 0)
                        tile[0][local_j] = U[(global_i-1)*ny + global_j];
                    if(item.get_local_id(0) == local_size[0]-1 && global_i < nx-1)
                        tile[M+1][local_j] = U[(global_i+1)*ny + global_j];
                    if(item.get_local_id(1) == 0 && global_j > 0)
                        tile[local_i][0] = U[global_i*ny + global_j-1];
                    if(item.get_local_id(1) == local_size[1]-1 && global_j < ny-1)
                        tile[local_i][M+1] = U[global_i*ny + global_j+1];

                    item.barrier(access::fence_space::local_space);

                    if(global_i > 0 && global_i < nx-1 && global_j > 0 && global_j < ny-1){
                        UNEW[global_i * ny + global_j] =
                            factor * (tile[local_i+1][local_j] - 2.0f * tile[local_i][local_j] + tile[local_i-1][local_j] +
                                      tile[local_i][local_j+1] - 2.0f * tile[local_i][local_j] + tile[local_i][local_j-1]);
                    }
                });
            });
            q.wait();
            kernel_duration += (e.get_profiling_info<info::event_profiling::command_end>() - e.get_profiling_info<info::event_profiling::command_start>());
            e = q.submit([&](handler &h){
                accessor U(unew, h, read_only);
                accessor UNEW(u, h, write_only);

                // Local_accessor
                local_accessor<float, 2> tile(range<2>(M+2, M+2), h);

                h.parallel_for(r, [=](nd_item<2> item){
                    int global_i = item.get_global_id(0);
                    int global_j = item.get_global_id(1);

                    int local_i = item.get_local_id(0) + 1;
                    int local_j = item.get_local_id(1) + 1;

                    if(global_i < nx && global_j < ny)
                        tile[local_i][local_j] = U[global_i * ny + global_j];

                    if(item.get_local_id(0) == 0 && global_i > 0)
                        tile[0][local_j] = U[(global_i-1)*ny + global_j];
                    if(item.get_local_id(0) == local_size[0]-1 && global_i < nx-1)
                        tile[M+1][local_j] = U[(global_i+1)*ny + global_j];
                    if(item.get_local_id(1) == 0 && global_j > 0)
                        tile[local_i][0] = U[global_i*ny + global_j-1];
                    if(item.get_local_id(1) == local_size[1]-1 && global_j < ny-1)
                        tile[local_i][M+1] = U[global_i*ny + global_j+1];

                    item.barrier(access::fence_space::local_space);

                    if(global_i > 0 && global_i < nx-1 && global_j > 0 && global_j < ny-1){
                        UNEW[global_i * ny + global_j] =
                            factor * (tile[local_i+1][local_j] - 2.0f * tile[local_i][local_j] + tile[local_i-1][local_j] +
                                      tile[local_i][local_j+1] - 2.0f * tile[local_i][local_j] + tile[local_i][local_j-1]);
                    }
                });
            });
            q.wait();
            kernel_duration += (e.get_profiling_info<info::event_profiling::command_end>() - e.get_profiling_info<info::event_profiling::command_start>());
        }
    }

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
