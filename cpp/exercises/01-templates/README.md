<!--
SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Exercise: Using C++ templates

In this exercise you can practice using C++ templates.
Templates allow one to write generic code that works
with multiple types.

## Tasks

The code `axpy.cpp` performs the basic "axpy" operation
$y = a \times x + y$ both for double and single precision 
floating point values. Current code implements `axpy` function
separately for both types. 

1. Avoid the repetition and make the code more generic by using
templated function

2. Try to use the same generic function e.g. with integer values


