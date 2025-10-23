cmake_minimum_required(VERSION 3.20)

project(AXPY LANGUAGES CXX)

include(../../common.cmake)

add_executable(axpy axpy.cpp)
add_executable(axpy-shared axpy-shared.cpp)

target_link_libraries(axpy PRIVATE Kokkos::kokkos)
target_link_libraries(axpy-shared PRIVATE Kokkos::kokkos)
