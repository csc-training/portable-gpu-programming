cmake_minimum_required(VERSION 3.20)

project(subviews LANGUAGES CXX)

include(../../common.cmake)

add_executable(subviews subviews.cpp)

target_link_libraries(subviews PRIVATE Kokkos::kokkos)
