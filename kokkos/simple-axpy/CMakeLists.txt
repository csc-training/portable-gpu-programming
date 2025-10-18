cmake_minimum_required(VERSION 3.20)

project(AXPY LANGUAGES CXX)

include(../common.cmake)

add_executable(axpy axpy.cpp)

target_link_libraries(axpy PRIVATE Kokkos::kokkos)
