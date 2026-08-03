# SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
#
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.20)

project(AXPY LANGUAGES CXX)

include(../../common.cmake)

add_executable(axpy axpy.cpp)
add_executable(axpy-sharedspace axpy-sharedspace.cpp)

target_link_libraries(axpy PRIVATE Kokkos::kokkos)
target_link_libraries(axpy-sharedspace PRIVATE Kokkos::kokkos)
