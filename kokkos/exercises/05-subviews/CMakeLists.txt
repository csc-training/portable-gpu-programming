# SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
#
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.20)

project(subviews LANGUAGES CXX)

include(../common.cmake)

add_executable(subviews subviews.cpp)

target_link_libraries(subviews PRIVATE Kokkos::kokkos)
