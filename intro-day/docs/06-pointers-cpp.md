---
title:    Getting started with C++
subtitle: Portable GPU Programming
date:     October 2025
lang:     en
---

# Outline

- Refresher on C pointers
- Refresher on C++ features not present in C
- Introduction to C++ stdpar

# Pointers in C {.section}

# Pointers in C

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

- Demo: `pointers.cpp`

# Summary

- Pointer is one of the most fundamental concepts in programming


# Modern C++ for C programmers {.section}

# Introduction

- SYCL and Kokkos are modern C++ with classes, templates, lambdas, ...
- These constructs are reviewed


# Elements of a SYCL code

```cpp
#include <sycl/sycl.hpp>
using namespace sycl;

template <typename T>
void axpy(queue &q, const T &a, const std::vector<T> &x, std::vector<T> &y) {
  range<1> N{x.size()};
  buffer x_buf(x.data(), N); buffer y_buf(y.data(), N);

  q.submit([&](handler &h) {
    accessor x{x_buf, h, read_only};
    accessor y{y_buf, h, read_write};

    h.parallel_for(N, [=](id<1> i) {
      y[i] += a * x[i];
    });
  });
  q.wait_and_throw();
}
```

# Namespaces

- Namespace is a way of organizing variables, functions, classes, etc.

```cpp
// Fully qualified name
sycl::queue q{};

// Using names from the namespace
using namespace sycl;
queue q{};

```

# Placeholder type `auto`

- `auto` can be used in variable declaration if the compiler can deduce the type during compilation

```cpp
auto a = 5;

auto queue_event = queue.submit([&](handler& h) {...});

```

# Templates

- Templates allow writing generic functions and classes

```cpp
template <typename T>
T max(T a, T b) {
  return (a > b) ? a : b;
}

int a = 1, b = 2;
auto c = max(a, b);

double x = 3.4, y = 5.6;
auto z = max(x, y);

// Call int version explicitly
auto zi = max<int>(a, y);

```

# Abbreviated function templates with `auto`

- Since C++20, abbreviated function templates are possible

```cpp
auto max(auto a, auto b) {
  return (a > b) ? a : b;
}

int a = 1, b = 2;
auto c = max(a, b);

double x = 3.4, y = 5.6;
auto z = max(x, y);

auto zi = max(a, y);

```

# Pointers and references

- Raw pointer: Memory address of a variable (as in C)

```cpp
void foo1(int *a) { *a = 42; }

int x = 0;
int *x_ptr = &x;
foo1(x_ptr);
std::cout << x << std::endl;

```

- Reference: Alias of another variable

```cpp
void foo2(int &a) { a = 42; }


int y = 0;
foo2(y);
std::cout << y << std::endl;

```

# Containers

- C++ standard library provides `std::array` and `std::vector` containers for data
  - Follows [RAII](https://en.cppreference.com/w/cpp/language/raii) principle
  - Prefer over explicit memory management via raw pointers

```cpp
#include <vector>

std::vector<double> a(10);
a[0] = 5;
a[1] = 7;
```

# Obtaining pointers in C++ (aka raw pointers)

- For getting raw pointer to the data in `std::array` and `std::vector` containers, use `array.data()` or `&array[i]`

```cpp
int *p; // declaration of a raw pointer
std::vector<int> a(5);

p = a.data();  // memory address of the first element
p = &a[0];     // equivalent to the line above

p = a.data() + 2;  // memory address of the third element
p = &a[2];         // equivalent to the line above
```

- Demo: `pointers.cpp`


# Classes

- Composite data type grouping variables and functions

<div class="column">
```cpp
template <typename T>
class Particle {
private:
    T x, y;
public:
    Particle(T x, T y) : x(x), y(y) {}
    void move(T dx, T dy) {
        x += dx;
        y += dy;
    }
    void print() {
        std::cout << x << " " << y << std::endl;
    }
};

```
</div>

<div class="column">
```cpp
Particle<double> p{1.2, 3.4};
p.print();
p.move(5.6, 7.8);
p.print();

```
</div>


# Function objects

- Objects that behave like functions

```cpp
class Adder {
private:
    const int constant;
public:
    Adder(const int c) : constant{c} {}
    int operator()(const int a) const { return constant + a; }
};

Adder add{5};
int sum = add(2);
std::cout << "The sum is: " << sum << std::endl;

```


# Lambda expressions

- Unnamed function objects that can capture variables in scope
- Syntax: `[ captures ] (parameters) -> return-type { body }`

```cpp
int a = 1, b = 2, c = 3;

// Capture `a` by value
auto func1 = [a](int x) { return a + x; };
c = func1(4);  // 5

a = -1;
c = func1(4);  // 5

// Capture to a new variable
auto func2 = [d = 2*a](int x) { return d + x; };
c = func2(4);  // 2

```

# Lambda expressions cont'd

```cpp
...
b = 2;

// This will fail; `b` not captured
auto func3 = [a](int x) { return b + x; };

// Capture everything by value
auto func3 = [=](int x) { return b + x; };
c = func3(4);  // 6

```

- Demo: `lambdas.cpp`

# Lambda expressions cont'd

```cpp
...
b = 2;

// Capture `b` by reference
auto func4 = [&b](int x) { return b + x; };
c = func4(4);  // 6

b = -2;
c = func4(4);  // 2

// Capture everything by reference
auto func5 = [&](int x) { a = x; b = -x; };
func5(4);  // a = 4, b = -4

```

- Demo: `lambdas.cpp`

# Lambda expressions cont'd

```cpp
...
a = 4; b = -4;

// Mix and match
auto func6 = [=,&b](int x) { return a + b + x; };
c = func6(4);  // 4

a = b = 0;
c = func6(4);  // 8

```

- Demo: `lambdas.cpp`


# Error handling

- Errors are handled via C++ exceptions

```cpp
int main() {
  int x, y;
  std::cout << "Enter two numbers: ";
  std::cin >> x >> y;

  try {
    if (y == 0) throw "Division by zero error";
    std::cout << "x / y = " << x / y << std::endl;
  } catch (const char* msg) {
    std::cerr << "Error: " << msg << std::endl;
  }
  return 0;
}

```

# Summary

- Modern C++ allows generic programming
- Classes, templates, lambdas, ...
- Reusable, expressive, and efficient code


# C++ Standard Parallelism {.section}

# Introduction

- ''Traditional'' loop processing data

```cpp
#include <vector>

double alpha = 5;
std::vector<double> x = {1, 2, 3, 4}, y(4, 0);

for (int i = 0; i < 4; ++i) {
    y[i] = alpha * x[i] + y[i];
}

```

# Introduction cont'd

- Separating computation and iteration:

```cpp
// Kernel: what to do for each data element
auto kernel = [=](const double x, const double y) {
    return alpha * x + y;
};

// Loop: how to process through all data
for (int i = 0; i < 4; ++i) {
    y[i] = kernel(x[i], y[i]);
}

```

# C++ algorithms library

- Algorithms abstract the looping part

```cpp
#include <algorithm>

auto kernel = [=](const double x, const double y) {
    return alpha * x + y;
};

// Process through all data
std::transform(begin(x), end(x), begin(y), begin(y), kernel);

```

# C++ standard parallelism

- Since C++17, C++ algorithms have an optional execution policy

```cpp
#include <execution>

// Process through all data in parallel
std::transform(std::execution::par_unseq, begin(x), end(x), begin(y), begin(y), kernel);

```

- This kernel can now run in parallel on GPU or CPU!
  - Only a suitable compiler needed

- Note! With `std::execution::par_unseq`, the compiler *assumes* that the operations defined by kernel are independent
  - It is the responsibility of the programmer to ensure that this is the case


# Notes on GPU acceleration

- The call `std::transform(std::execution::par_unseq, ...)` launches a kernel on GPU
  - Memory copied implicitly to GPU (managed memory)
  - The host CPU thread waits until GPU finishes calculation (see also [P2300](https://wg21.link/p2300))

- Kernel should not hold references to CPU memory to avoid memory access faults or performance hits
  - Capture everything by value: `[=](...) {...}` &rarr; values get copied to GPU memory in kernel launch


# Available C++ algorithms

- C++ standard library has algorithms for generic batch operation, reductions, searching, sorting, ...
  - [List of algorithms](https://en.cppreference.com/w/cpp/algorithm)

- Use existing algorithms when possible
  - Shorter and more efficient code than hand-written custom code

- Example: parallel inner product

```cpp
std::vector<double> x(N), y(N);

auto prod = std::transform_reduce(std::execution::par_unseq,
                                  begin(x), end(x), begin(y),
                                  0.0, std::plus<>(), std::multiplies<>());

```

# Summary

- C++ standard library enables parallel programming without external dependencies
- Fully portable C++ code
- No full control over GPU
  - No explicit management of GPU memory
    - Simplifies programming
    - Possibly low performance due to suboptimal memory access
- Performance depends on the compiler
