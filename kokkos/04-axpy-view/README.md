<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# axpy with Views

Implement the axpy code of exercise [Simple axpy](../01-simple-axpy) using
Kokkos View as data structure for arrays. Implement the computational body with
lambda function.

Try to use both explit device and host memory spaces (and copying between them when necessary),
as well as a shared memory space. 

## Bonus: using explicit functor and templating

For simple computational kernels like axpy, explicit functor is clearly an overkill.
However, for more complex cases using an explicit functor might be beneficial. Due to
behaviour of CUDA `nvcc` compiler, explicit functors have to be defined in different 
scope than the parallel dispatch (*e.g.* `parallel_for`). As the exact type of the View
is not known when the functor is defined, a more advanced template metaprogramming 
techniques are needed, such as template packs and type traits. Even though one would
not be using functors, similar techniques are needed when defining functions that take
Views as arguments.

Here is an example of templated function definition that takes as an argument a View (`x`),
as well as a scalar (`a`) that is of the same type as the View:
```
template <typename DT, typename... DP>
void myfunc(Kokkos::View<DT, DP...> x, 
           typename Kokkos::ViewTraits<DT, DP...>::const_value_type a)
{
 // function body
}
```

- the template parameter `DT` captures the type and dimensionality of the View, *e.g.*
  `<int **>`
- the template pack `DP` captures all the optional parameters
- type trait `ViewTraits` is used for deducing the type for `a`, *i.e.* here we use 
  constant of the same type as the contents of View. With the `<int **>` above that would be
  `const int`. There is also type trait `value_type` if non-const is needed.

