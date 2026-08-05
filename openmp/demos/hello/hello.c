/*
 * SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
 *
 * SPDX-License-Identifier: MIT
 */

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
