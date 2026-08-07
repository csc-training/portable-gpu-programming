<!--
SPDX-FileCopyrightText: 2024 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

---
title: Pointers in C and C++
event: Portable GPU Programming 2026
lang:  en
---

# Outline

- Refresher on C pointers

# Pointers in C and C++ {.section}

# Pointers in C and C++

- The (virtual) *memory* of a program is an array of bytes,
  from the first index 0 up to index 140,737,488,355,327 (for example)
  - 1 byte = 8 bits
- *Memory address* is an index to this array of bytes
  - When a variable is stored in memory, it gets stored to a fixed address
  - Addresses are often represented as hexadecimals: `3,226,528 = 0x314f20`
- *Pointer* is another variable that can store memory addresses
- A pointer can be used to refer to a large amount of data (contiguous block of bytes)
  - The address of the first element is stored in the pointer

# Analogy

- Locations of buildings are often expressed as *street addresses*
  - For example, CSC Espoo office is located at `Keilaranta 14, 02150 Espoo`
- A piece of paper containing this address would be a "pointer"
- It's much easier to pass around the address information (pointer)
  than move around the whole building (large amount of data in memory)
- The pointer is not the same as the building (data), it just points to it

# Examples

```c
int a = 10;  // declaration of a variable
int *p;      // declaration of a pointer
p = &a;      // & operator gives the address of a
int b = *p;  // * operator dereferences (gives the value at the address p)

// Allocate a block of bytes and store the starting address at arr
int *arr = (int*)malloc(4 * sizeof(int));

arr[0] = 10;  // set first element
*arr = 10;    // equivalent to line above

arr[2] = 12;      // set third element
*(arr + 2) = 12;  // equivalent to line above

// Free the block of bytes
free(arr);
```

- Demo: `pointers`

# Swapping pointers

```c
// Allocate two arrays
int *arr1 = (int*)malloc(4 * sizeof(int));
int *arr2 = (int*)malloc(4 * sizeof(int));

// Set values
arr1[0] = 10;
arr2[0] = 20;

// Swap the pointers (only addresses; data does not move in memory)
double *tmp = arr1;
arr1 = arr2;
arr2 = tmp;

// What is now in arr1 and arr2? Did the addresses of values 10 and 20 change?

// Free the arrays
free(arr2); free(arr1);
```

# Summary

- Pointer is one of the most fundamental concepts in programming
