#pragma optimize(3, "Ofast", "inline")
#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>

template <int maxIterations>

__global__ void mandelKernel(int* result, int resX, int resY, float lowerX, float lowerY, float dx, float dy)
{
	int thisX = blockIdx.x * blockDim.x + threadIdx.x;
	int thisY = blockIdx.y * blockDim.y + threadIdx.y;

    if (thisX >= resX || thisY >= resY) return;

    float c_re = lowerX + thisX * dx;
	float c_im = lowerY + thisY * dy;
	float z_re = c_re, z_im = c_im;
    int i = 0;

    #pragma unroll
    for (; i < maxIterations; ++i) {
        float a = z_re * z_re;
        float b = z_im * z_im;

        if (a + b > 4.f) break;

        float new_re = a - b;
        float new_im = 2.f * z_re * z_im;
        z_re = c_re + new_re;
        z_im = c_im + new_im;
    }

    result[thisX + thisY * resX] = i;
}

__global__ void mandelKernelDynamic(int* result, int resX, int resY, float lowerX, float lowerY, float dx, float dy, int maxIterations)
{
    int thisX = blockIdx.x * blockDim.x + threadIdx.x;
	int thisY = blockIdx.y * blockDim.y + threadIdx.y;

    if (thisX >= resX || thisY >= resY) return;

    float c_re = lowerX + thisX * dx;
	float c_im = lowerY + thisY * dy;
	float z_re = c_re, z_im = c_im;
    int i = 0;

    #pragma unroll
    for (; i < maxIterations; ++i) {
        float a = z_re * z_re;
        float b = z_im * z_im;

        if (a + b > 4.f) break;

        float new_re = a - b;
        float new_im = 2.f * z_re * z_im;
        z_re = c_re + new_re;
        z_im = c_im + new_im;
    }

    result[thisX + thisY * resX] = i;
}

void hostFE (float upperX, float upperY, float lowerX, float lowerY, int* img, int resX, int resY, int maxIterations)
{
	float dx = (upperX - lowerX) / resX;
    float dy = (upperY - lowerY) / resY;
    int size = resX * resY * sizeof(int);

    int *result;
    cudaMalloc(&result, size);

    dim3 blockSize(8, 8);
    dim3 numBlocks(resX / blockSize.x + 1, resY / blockSize.y + 1);

    switch (maxIterations) {
        case 256:
            mandelKernel<256><<<numBlocks, blockSize>>>(result, resX, resY, lowerX, lowerY, dx, dy);
            break;
        case 1000:
            mandelKernel<1000><<<numBlocks, blockSize>>>(result, resX, resY, lowerX, lowerY, dx, dy);
            break;
        case 10000:
            mandelKernel<10000><<<numBlocks, blockSize>>>(result, resX, resY, lowerX, lowerY, dx, dy);
            break;
        case 100000:
            mandelKernel<100000><<<numBlocks, blockSize>>>(result, resX, resY, lowerX, lowerY, dx, dy);
            break;
        default:
            mandelKernelDynamic<<<numBlocks, blockSize>>>(result, resX, resY, lowerX, lowerY, dx, dy, maxIterations);
            break;
    }

    cudaMemcpy(img, result, size, cudaMemcpyDeviceToHost);
    cudaFree(result);
}
