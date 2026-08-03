# SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
#
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.20)

project(Hello LANGUAGES CXX)

find_package(MPI REQUIRED)
include(../common.cmake)

add_executable(hello hello.cpp)

target_link_libraries(hello PRIVATE Kokkos::kokkos)
target_link_libraries(hello PRIVATE MPI::MPI_CXX)
