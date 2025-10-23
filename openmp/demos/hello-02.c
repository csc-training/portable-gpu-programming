#include <stdio.h>
#include <omp.h>

int main(void)
{
    printf("Hello from host!\n");

    #pragma omp target
    {
        printf("Hello from device! I'm team %d/%d thread %d/%d\n",
               omp_get_team_num(), omp_get_num_teams(),
               omp_get_thread_num(), omp_get_num_threads());
    }

    return 0;
}
