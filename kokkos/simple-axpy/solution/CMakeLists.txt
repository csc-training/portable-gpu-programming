cmake_minimum_required(VERSION 3.22)

project(Exercise LANGUAGES CXX)

# include(../kokkos.cmake)
include(../../common.cmake)

add_executable(axpy axpy.cpp)

target_link_libraries(axpy PRIVATE Kokkos::kokkos)
