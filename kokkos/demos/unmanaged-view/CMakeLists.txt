# SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
#
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.20)

project(AXPY LANGUAGES CXX)

find_package(Kokkos REQUIRED CONFIG)

add_executable(unmanaged-view unmanaged-view.cpp)

target_link_libraries(unmanaged-view PRIVATE Kokkos::kokkos)
