#include <stdio.h>

void print_array(const char* name, const double* x, size_t n)
{
    const size_t PRINT_N = 4; // Limit the print size
    printf("%s =", name);
    for (size_t i = 0; i < n; i++) {
        if (i < PRINT_N || i > n - 1 - PRINT_N) {
            printf(" %8.4f", x[i]);
        } else if (i == PRINT_N) {
            printf(" ...");
        }
    }
    printf("\n");
}
