cmake_minimum_required(VERSION 3.20)

project(AXPY LANGUAGES CXX)

include(../common.cmake)

add_executable(views views.cpp)

target_link_libraries(views PRIVATE Kokkos::kokkos)
