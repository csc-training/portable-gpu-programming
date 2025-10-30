// SPDX-FileCopyrightText: 2024 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include <cstdio>
#include <cstdlib>
#include <array>
#include <vector>

int main(int argc, char *argv[]) {

  {
    printf("Case 1\n");

    int a = 10;  // declaration of a variable
    int *p;      // declaration of a pointer
    p = &a;      // & operator gives the address of a
    int b = *p;  // * operator dereferences (gives the value at the address p)

    printf("a = %d; p = %p; b = %d\n", a, p, b);
    printf("\n");
  }

  {
    printf("Case 2\n");

    // Allocate a block of bytes and store the starting address at arr
    int *arr = (int*)malloc(4 * sizeof(int));

    arr[0] = 10;  // set first element
    *arr = 10;    // equivalent to line above

    arr[2] = 12;      // set third element
    *(arr + 2) = 12;  // equivalent to line above

    printf("value of first  = %d\n", arr[0]);
    printf("value of second = %d\n", arr[1]);
    printf("value of third  = %d\n", arr[2]);

    printf("address of first  = %p\n", arr);
    printf("address of second = %p\n", arr + 1);
    printf("address of third  = %p\n", arr + 2);

    // alternative way
    printf("address of first  = %p\n", &arr[0]);
    printf("address of second = %p\n", &arr[1]);
    printf("address of third  = %p\n", &arr[2]);
    printf("\n");

    // Free the block of bytes
    free(arr);
  }

  {
    printf("Case 3\n");

    int *p; // declaration of a raw pointer
    std::vector<int> a(5);  // allocated from heap
    // std::array<int, 5> a;   // allocated from stack

    p = a.data();  // memory address of the first element
    p = &a[0];     // equivalent to the line above

    p = a.data() + 2;  // memory address of the third element
    p = &a[2];         // equivalent to the line above

    printf("address of first  = %p\n", a.data());
    printf("alternative       = %p\n", &a[0]);
    printf("\n");
  }
}

