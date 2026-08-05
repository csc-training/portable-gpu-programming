#include <stdio.h>

int main()
{
#if defined(__cray__)
    printf("Code was compiled with Cray compiler\n");
#elif defined(__clang__)
    printf("Code was compiled with Clang compiler (other than Cray)\n");
#elif defined(__NVCOMPILER)
    printf("Code was compiled with NVIDIA compiler\n");
#elif defined(__GNUC__)
    printf("Code was compiled with GNU compiler\n");
#else
    printf("Code was compiled with an unknown compiler\n");
#endif
}

