cmake_minimum_required(VERSION 3.22)

project(Hello LANGUAGES CXX)

# include(../kokkos.cmake)
include(../common.cmake)

add_executable(hello hello.cpp)

target_link_libraries(hello PRIVATE Kokkos::kokkos)
