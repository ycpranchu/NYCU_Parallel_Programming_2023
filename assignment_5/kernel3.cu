#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>
using byte = unsigned char;

__global__ void mandelKernel(int* result, int resX, int resY, float lowerX, float lowerY, float dx, float dy, int maxIterations, int pitch, int group_size)
{
	int thisX = (blockIdx.x * blockDim.x + threadIdx.x) * group_size;
	int thisY = (blockIdx.y * blockDim.y + threadIdx.y) * group_size;

    if (thisX >= resX) return;
    if (thisY >= resY) return;
    
    for (int offset_y = 0; offset_y < group_size; offset_y++)
    {
        for (int offset_x = 0; offset_x < group_size; offset_x++)
        { 
            float c_re = lowerX + (thisX + offset_x) * dx;
            float c_im = lowerY + (thisY + offset_y) * dy;
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

            int* row = (int*)((byte*)result + (thisY + offset_y) * pitch); // char size = 8-bit
            row[(thisX + offset_x)] = i;
        }
    }
}

void hostFE (float upperX, float upperY, float lowerX, float lowerY, int* img, int resX, int resY, int maxIterations)
{
	float dx = (upperX - lowerX) / resX;
    float dy = (upperY - lowerY) / resY;
    int size = resX * resY * sizeof(int);
    int group_size = 10;

    int *hostMemory, *result;
    size_t pitch = 0;

    cudaHostAlloc(&hostMemory, size, cudaHostAllocMapped);
    cudaMallocPitch(&result, &pitch, sizeof(int) * resX, resY);

    dim3 blockSize(32, 32);
    dim3 numBlocks(resX / blockSize.x / group_size + 1, resY / blockSize.y / group_size + 1);
    mandelKernel<<<numBlocks, blockSize>>> (result, resX, resY, lowerX, lowerY, dx, dy, maxIterations, pitch, group_size);

    cudaMemcpy2D(hostMemory, sizeof(int) * resX, result, pitch, sizeof(int) * resX, resY, cudaMemcpyDeviceToHost);
    memcpy(img, hostMemory, size);
    
    cudaFree(result);
    cudaFreeHost(hostMemory);
}
