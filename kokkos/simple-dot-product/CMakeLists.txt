cmake_minimum_required(VERSION 3.20)

project(AXPY LANGUAGES CXX)

include(../common.cmake)

add_executable(dot-product dot-product.cpp)

target_link_libraries(dot-product PRIVATE Kokkos::kokkos)
