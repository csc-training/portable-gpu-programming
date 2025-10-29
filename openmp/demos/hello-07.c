/*
 * SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <omp.h>

int main(void)
{
    printf("Hello from host!\n");

    int n = 4;
    int m = 8;

    #pragma omp target
    {
        #pragma omp teams distribute parallel for
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                printf("Hello from device! I'm team %d/%d thread %d/%d; i=%d, j=%d\n",
                       omp_get_team_num(), omp_get_num_teams(),
                       omp_get_thread_num(), omp_get_num_threads(),
                       i, j);
            }
        }
    }

    return 0;
}
