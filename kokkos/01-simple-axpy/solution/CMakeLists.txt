cmake_minimum_required(VERSION 3.20)

project(AXPY LANGUAGES CXX)

include(../../common.cmake)

add_executable(axpy-functor axpy-functor.cpp)
add_executable(axpy-lambda axpy-lambda.cpp)
add_executable(axpy-lambda-sharedspace axpy-lambda-sharedspace.cpp)

target_link_libraries(axpy-functor PRIVATE Kokkos::kokkos)
target_link_libraries(axpy-lambda PRIVATE Kokkos::kokkos)
target_link_libraries(axpy-lambda-sharedspace PRIVATE Kokkos::kokkos)
