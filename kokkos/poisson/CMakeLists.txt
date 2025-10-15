cmake_minimum_required(VERSION 3.22)

project(PoissonLANGUAGES CXX)

# include(../kokkos.cmake)
include(../common.cmake)

add_executable(poisson poisson.cpp)

target_link_libraries(poisson PRIVATE Kokkos::kokkos)
