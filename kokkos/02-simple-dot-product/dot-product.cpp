#include <iostream>
#include <cmath>

template <typename T>
T dot_product(const T *x, const T *y, size_t N) 
{
  T result = 0.0;
  for (size_t i = 0; i < N; i++) 
  {
    result += x[i] * y[i];
  }
  return result;
}

template <typename T>
void init(T *x, T *y, size_t N) 
{
  for (size_t i = 0; i < N; i++) 
  {
    x[i] = cos(i * 1.2);
    y[i] = sin(i * -1.12);
  }
}

int main(int argc, char** argv)
{

  using fp_type = double;

  constexpr size_t N = 100;

  fp_type *x = (fp_type *) malloc(N * sizeof(fp_type));
  fp_type *y = (fp_type *) malloc(N * sizeof(fp_type));

  init(x, y, N);
  std::cout << "First and last elements before dot product: " << std::endl
            << "x: " << x[0] << "," << x[N-1] << std::endl
            << "y: " << y[0] << "," << y[N-1] << std::endl;

  auto result = dot_product(x, y, N);

  // Check results
  std::cout << "Result (should be 6.78105): " << result << std::endl;
}
