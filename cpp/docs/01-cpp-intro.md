<!--
SPDX-FileCopyrightText: 2024 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

---
title: Getting started with C++
event: Portable GPU Programming
lang:  en
---

# Outline

- Refresher on C++ features not present in C


# Modern C++ for C programmers {.section}

# Introduction

- Kokkos utilizes heavily modern C++ features
    - classes, templates, functors, lambdas, ...
- These constructs are reviewed


# Example of Kokkos code

```cpp
template <typename T>
void axpy(T *x, T *y, T a) {
Kokkos::parallel_for(N,
   [=] (const int i) {
        y[i] += a * x[i];
   });
```


# Automatic type deduction with `auto`

- `auto` makes compiler to deduce a variable's type from initiliazer

```cpp
auto a = 5;

auto a_slice = Kokkos::subview(a, std::make_pair(2, 4), std::make_pair(3, 7));
```
- Reduces verbosity, especially with complex template types.
- Think where using `auto` makes sense, overuse can make code less readable


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

# Classes

- Classes are declared either with `class` or `struct` keywords
- With `class` members are private by default
- With `struct` members are public by default


# Functors 

- Functors (function objects) behave like functions after initialization
- Functors have a state which is determined at runtime

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

- Lambda expressions create unnamed function objects 
- Resulting functor gets it state by capturing variables from the scope
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



# Summary

- Modern C++ allows generic programming
- Classes, templates, lambdas, ...
- Reusable, expressive, and efficient code

