#pragma GCC optimize(3, "Ofast", "inline")
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    // --- DON'T TOUCH ---
    MPI_Init(&argc, &argv);
    double start_time = MPI_Wtime();
    double pi_result;
    long long int tosses = atoi(argv[1]);
    int world_rank, world_size;
    // ---

    // TODO: MPI init
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    unsigned int seed = time(NULL) + world_rank;
    long long toss = tosses / world_size, count = 0;

    const double scale = 2.0 / 0xFFFF;
    const double offset = -1.0;
    double x, y;

    for (long long i = 0; i < toss; ++i)
    {
        unsigned int rand_val = rand_r(&seed);
        x = (rand_val & 0xFFFF) * scale + offset;
        y = (rand_val >> 16) * scale + offset;

        if (x * x + y * y <= 1)
            ++count;
    }

    if (world_rank > 0)
    {
        // TODO: MPI workers
        MPI_Send(&count, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);
    }
    else if (world_rank == 0)
    {
        // TODO: non-blocking MPI communication.
        // Use MPI_Irecv, MPI_Wait or MPI_Waitall.
        MPI_Request requests[world_size - 1];
        long long temp_count[world_size - 1];

        for (int i = 0; i < world_size - 1; ++i)
            MPI_Irecv(&temp_count[i], 1, MPI_LONG_LONG, i + 1, 0, MPI_COMM_WORLD, &requests[i]);
        MPI_Waitall(world_size - 1, requests, MPI_STATUSES_IGNORE);

        for (int i = 0; i < world_size - 1; ++i)
            count += temp_count[i];
    }

    if (world_rank == 0)
    {
        // TODO: PI result
        pi_result = 4 * (double)count / tosses;

        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }

    MPI_Finalize();
    return 0;
}
