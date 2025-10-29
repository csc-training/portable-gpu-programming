# SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
#
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.20)

project(AXPY LANGUAGES CXX)

include(../../common.cmake)

add_executable(dot-product dot-product.cpp)

target_link_libraries(dot-product PRIVATE Kokkos::kokkos)
