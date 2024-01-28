#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>

__global__ void mandelKernel(int* result, int resX, int resY, float lowerX, float lowerY, float dx, float dy, int maxIterations)
{
	int thisX = blockIdx.x * blockDim.x + threadIdx.x;
	int thisY = blockIdx.y * blockDim.y + threadIdx.y;

    if (thisX >= resX) return;
    if (thisY >= resY) return;

    float c_re = lowerX + thisX * dx;
	float c_im = lowerY + thisY * dy;
	float z_re = c_re;
	float z_im = c_im;
    int i;

    #pragma unroll
    for (i = 0; i < maxIterations; ++i)
    {
        if (z_re * z_re + z_im * z_im > 4.f) break;

        float new_re = z_re * z_re - z_im * z_im;
        float new_im = 2.f * z_re * z_im;
        z_re = c_re + new_re;
        z_im = c_im + new_im;
    }

    int index = thisX + thisY * resX;
    result[index] = i;
}

void hostFE (float upperX, float upperY, float lowerX, float lowerY, int* img, int resX, int resY, int maxIterations)
{
	float dx = (upperX - lowerX) / resX;
    float dy = (upperY - lowerY) / resY;
    int size = resX * resY * sizeof(int);

    int *hostMemory = (int *)malloc(size);
    int *result;
    cudaMalloc(&result, size);

    dim3 blockSize(32, 32);
    dim3 numBlocks(resX / blockSize.x + 1, resY / blockSize.y + 1);

    mandelKernel<<<numBlocks, blockSize>>> (result, resX, resY, lowerX, lowerY, dx, dy, maxIterations);

    cudaMemcpy(hostMemory, result, size, cudaMemcpyDeviceToHost);
    memcpy(img, hostMemory, size);
    
    cudaFree(result);
    free(hostMemory);
}
