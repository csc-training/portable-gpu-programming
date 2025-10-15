cmake_minimum_required(VERSION 3.22)

project(AXPY LANGUAGES CXX)

#find_package(Kokkos 4.2 REQUIRED CONFIG) # Find Kokkos version 4.2 or later

#add_executable(axpy axpy.cpp)
kokkos_include_directories(${Kokkos_ROOT})
kokkos_add_executable(axpy SOURCES axpy.cpp)

#target_link_libraries(axpy PRIVATE Kokkos::kokkos)
