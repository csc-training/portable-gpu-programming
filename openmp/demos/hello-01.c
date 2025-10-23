#include <stdio.h>


int main(void)
{
    printf("Hello from host!\n");

    #pragma omp target
    {
        printf("Hello from device!\n");
    }

    return 0;
}
