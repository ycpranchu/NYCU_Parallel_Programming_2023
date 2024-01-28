#include <iostream>
#include <pthread.h>
#include <bits/stdc++.h>

long long THREADS_MAX = 4;
long long number_of_tosses = 1000000000;
double number_in_circle = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// void *thread_function(void *args)
// {
//     int steps = THREADS_MAX;
//     int thread_id = *(int *)args;
//     long long toss = (long long)thread_id;
//     long long toss_times = number_of_tosses;
//     double count = 0;

//     unsigned int seed = time(NULL) + toss;
//     double x, y, distance_squared;

//     for (toss; toss < toss_times; toss += steps)
//     {
//         unsigned int rand_val = rand_r(&seed);
//         x = ((rand_val & 0xFFFF) / (double)0xFFFF) * 2 - 1;
//         y = ((rand_val >> 16) / (double)0xFFFF) * 2 - 1;
//         distance_squared = x * x + y * y;

//         if (x * x + y * y <= 1)
//             ++count;
//     }

//     pthread_mutex_lock(&mutex);
//     number_in_circle += count;
//     pthread_mutex_unlock(&mutex);

//     if (thread_id == 0)
//         return 0;

//     pthread_exit(NULL);
// }

#include <immintrin.h>

// SIMD-aware LCG random number generator function
inline __m256i simd_random(__m256i seed)
{
    __m256i a = _mm256_set1_epi32(1664525);
    __m256i c = _mm256_set1_epi32(1013904223);
    __m256i m = _mm256_set1_epi32(0x7FFFFFFF);

    seed = _mm256_add_epi32(_mm256_mullo_epi32(seed, a), c); // (a * seed + c) % m
    seed = _mm256_and_si256(seed, m);

    return seed;
}

void *thread_function(void *args)
{
    long long steps = THREADS_MAX;
    long long thread_id = *(int *)args;
    long long toss_times = number_of_tosses / THREADS_MAX;

    unsigned int seed1 = time(NULL) + (thread_id + 1);
    unsigned int seed2 = time(NULL) + (thread_id + 1) * 2;
    unsigned int seed3 = time(NULL) + (thread_id + 1) * 3;
    unsigned int seed4 = time(NULL) + (thread_id + 1) * 4;
    unsigned int seed5 = time(NULL) + (thread_id + 1) * 5;
    unsigned int seed6 = time(NULL) + (thread_id + 1) * 6;
    unsigned int seed7 = time(NULL) + (thread_id + 1) * 7;
    unsigned int seed8 = time(NULL) + (thread_id + 1) * 8;
    __m256i seed = _mm256_set_epi32(seed1, seed2, seed3, seed4, seed5, seed6, seed7, seed8);

    __m256 x, y, distance_squared, hit;
    __m256 bound = _mm256_set1_ps((float)0xFFFF);
    __m256d count1 = _mm256_set1_pd(0x0);
    __m256d count2 = _mm256_set1_pd(0x0);

    for (long long toss = 0; toss < toss_times; toss += 8)
    {
        // Generate random numbers in parallel
        seed = simd_random(seed);

        // x = ((rand_val & 0xFFFF) / (double)0xFFFF) * 2 - 1;
        x = _mm256_cvtepi32_ps(_mm256_and_si256(seed, _mm256_set1_epi32(0xFFFF)));
        x = _mm256_div_ps(x, bound);
        x = _mm256_mul_ps(x, _mm256_set1_ps(2.0));
        x = _mm256_sub_ps(x, _mm256_set1_ps(1.0));

        // y = ((rand_val >> 16) / (double)0xFFFF) * 2 - 1;
        y = _mm256_cvtepi32_ps(_mm256_srai_epi32(seed, 16));
        y = _mm256_div_ps(y, bound);
        y = _mm256_mul_ps(y, _mm256_set1_ps(2.0));
        y = _mm256_sub_ps(y, _mm256_set1_ps(1.0));

        // Calculate distance squared for all points
        distance_squared = _mm256_add_ps(_mm256_mul_ps(x, x), _mm256_mul_ps(y, y));

        // If inside the circle, bit = 1
        hit = _mm256_cmp_ps(distance_squared, _mm256_set1_ps((float)1.0), _CMP_LE_OQ);
        hit = _mm256_and_ps(hit, _mm256_set1_ps((float)1.0));

        // Count the number of points inside the circle
        __m128 low = _mm256_castps256_ps128(hit);
        __m128 high = _mm256_extractf128_ps(hit, 1);
        __m256d low_d = _mm256_cvtps_pd(low);
        __m256d high_d = _mm256_cvtps_pd(high);

        count1 = _mm256_add_pd(count1, low_d);
        count2 = _mm256_add_pd(count2, high_d);
    }

    double results[4], sum = 0;

    count1 = _mm256_add_pd(count1, count2);
    _mm256_store_pd(results, count1);
    sum += results[0] + results[1] + results[2] + results[3];

    pthread_mutex_lock(&mutex);
    number_in_circle += sum;
    pthread_mutex_unlock(&mutex);

    if (thread_id == 0)
        return 0;

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <number_of_threads> <number_of_tosses>" << std::endl;
        return 1;
    }

    THREADS_MAX = atoi(argv[1]);
    number_of_tosses = atoll(argv[2]);

    pthread_t thread[THREADS_MAX];
    long long thread_id[THREADS_MAX];

    // create pthread
    for (long long i = 0; i < THREADS_MAX; ++i)
    {
        thread_id[i] = i;
        if (i > 0)
            pthread_create(&thread[i], NULL, thread_function, (void *)&(thread_id[i]));
    }

    thread_function(&thread_id[0]);

    // pthread join
    for (long long i = 1; i < THREADS_MAX; ++i)
    {
        pthread_join(thread[i], NULL);
    }

    // final estimate value
    double pi_estimate = 4 * number_in_circle / number_of_tosses;
    std::cout << pi_estimate << std::endl;

    return 0;
}