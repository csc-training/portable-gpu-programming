# SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
#
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.20)

set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type")

project(Gemm LANGUAGES CXX)

find_package(KokkosKernels REQUIRED)

add_executable(gemm gemm.cpp)

target_link_libraries(gemm Kokkos::kokkoskernels)
