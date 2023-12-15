#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

int check(long long *temp_count, int size)
{
    int res = 0;

    for (int i = 0; i < size; i++)
    {
        if (temp_count[i] != 0)
            res += 1;
    }

    return (res == size);
}

int main(int argc, char **argv)
{
    // --- DON'T TOUCH ---
    MPI_Init(&argc, &argv);
    double start_time = MPI_Wtime();
    double pi_result;
    long long int tosses = atoi(argv[1]);
    int world_rank, world_size;
    // ---

    MPI_Win win;

    // TODO: MPI init
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    unsigned int seed = time(NULL) + world_rank;
    long long toss = tosses / world_size, count = 0;
    double x, y, distance_squared;

    for (long long i = 0; i < toss; ++i)
    {
        unsigned int rand_val = rand_r(&seed);
        x = ((rand_val & 0xFFFF) / (double)0xFFFF) * 2 - 1;
        y = ((rand_val >> 16) / (double)0xFFFF) * 2 - 1;
        distance_squared = x * x + y * y;

        if (x * x + y * y <= 1)
            ++count;
    }

    if (world_rank == 0)
    {
        // Master
        long long *temp_count = (long long *)calloc((world_size - 1), sizeof(long long));
        MPI_Win_create(temp_count, (world_size - 1) * sizeof(long long), sizeof(long long), MPI_INFO_NULL, MPI_COMM_WORLD, &win);

        int ready = 0;
        while (!ready)
        {
            MPI_Win_lock(MPI_LOCK_SHARED, 0, 0, win);
            ready = check(temp_count, world_size - 1);
            MPI_Win_unlock(0, win);
        }

        for (int i = 0; i < world_size - 1; ++i)
        {
            count += temp_count[i];
        }
    }
    else
    {
        // Workers
        MPI_Win_create(NULL, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &win);

        MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, win);
        MPI_Put(&count, 1, MPI_LONG_LONG, 0, world_rank - 1, 1, MPI_LONG_LONG, win);
        MPI_Win_unlock(0, win);
    }

    MPI_Win_free(&win);

    if (world_rank == 0)
    {
        // TODO: handle PI result
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