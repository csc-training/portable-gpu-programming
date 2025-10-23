cmake_minimum_required(VERSION 3.20)

project(Hello LANGUAGES CXX)

include(../common.cmake)

add_executable(hello hello.cpp)

target_link_libraries(hello PRIVATE Kokkos::kokkos)
