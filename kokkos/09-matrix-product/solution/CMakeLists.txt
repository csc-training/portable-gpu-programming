cmake_minimum_required(VERSION 3.20)

project(Gemm LANGUAGES CXX)

find_package(KokkosKernels REQUIRED)

add_executable(gemm gemm.cpp)

target_link_libraries(gemm Kokkos::kokkoskernels)
