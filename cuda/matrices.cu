#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <iostream> 
#include <sys/time.h>
#include <cuda_runtime_api.h>

using namespace std;
using real = float;

#define ASSERT_CUDA(expr)                                    \
do {                                                         \
    cudaError_t status = (expr);                             \
    if (status != cudaSuccess) {                             \
        printf("%s\n", cudaGetErrorString(status));          \
        exit(1);                                             \
    }                                                        \
} while(0)  

const real MAX_VAL = 100.0;
const int NR_OF_EXPERIMENTS = 20;

int N;

void matrix_generate(real* A);

void gpu_matrix_multiply_v1_8(real* A, real* B, real* C);
void gpu_matrix_multiply_v1_16(real* A, real* B, real* C);
void gpu_matrix_multiply_v1_32(real* A, real* B, real* C);

void gpu_matrix_multiply_v2_32(real* A, real* B, real* C);
void gpu_matrix_multiply_v2_64(real* A, real* B, real* C);
void gpu_matrix_multiply_v2_96(real* A, real* B, real* C);

void gpu_matrix_multiply_v3_32(real* A, real* B, real* C);
void gpu_matrix_multiply_v3_64(real* A, real* B, real* C);

void gpu_matrix_multiply_v4_32(real* A, real* B, real* C);

void gpu_matrix_multiply_v5_32(real* A, real* B, real* C);

void cpu_matrix_multiply(real* A, real* B, real* C);


int main(int argc, char **argv){
    if(argc != 2){
        cout << "Wywołanie: " << argv[0] << " N" << endl;
        return 0;
    }
    N = atoi(argv[1]);

    real *A, *B, *C_v1_8, *C_v1_16, *C_v1_32, *C_v2_32, *C_v2_64, *C_v2_96, *C_v3_32, *C_v3_64, *C_v4_32, *C_v5_32, *C_cpu;
    A = (real*)malloc(sizeof(real) * N * N);
    B = (real*)malloc(sizeof(real) * N * N);
    C_v1_8 = (real*)malloc(sizeof(real) * N * N);
    C_v1_16 = (real*)malloc(sizeof(real) * N * N);
    C_v1_32 = (real*)malloc(sizeof(real) * N * N);
    C_v2_32 = (real*)malloc(sizeof(real) * N * N);
    C_v2_64 = (real*)malloc(sizeof(real) * N * N);
    C_v2_96 = (real*)malloc(sizeof(real) * N * N);
    C_v3_32 = (real*)malloc(sizeof(real) * N * N);
    C_v3_64 = (real*)malloc(sizeof(real) * N * N);
    C_v4_32 = (real*)malloc(sizeof(real) * N * N);
    C_v5_32 = (real*)malloc(sizeof(real) * N * N);
    C_cpu = (real*)malloc(sizeof(real) * N * N);

    srand(777);
    matrix_generate(A);
    matrix_generate(B);

    time_t start, end;
    int dif_elems = 0;
    real deviation = 0;
    printf("id,cpu,gpu_v1_8,dif_elems,deviation,gpu_v1_16,dif_elems,deviation,gpu_v1_32,dif_elems,deviation,"
            "gpu_v2_32,dif_elems,deviation,gpu_v2_64,dif_elems,deviation,gpu_v2_64,dif_elems,deviation,"
            "gpu_v3_32,dif_elems,deviation,gpu_v3_64,dif_elems,deviation,gpu_v4_32,dif_elems,deviation,gpu_v5_32,dif_elems,deviation\n");
    for(int exp = 0; exp < NR_OF_EXPERIMENTS; exp++){
        printf("%d,", exp + 1);

        // CPU
        start = clock();
        cpu_matrix_multiply(A, B, C_cpu);
        end = clock();
        printf("%lf,", 1.0*(end-start)/CLOCKS_PER_SEC);

        // GPU_v1 8x8
        start = clock();
        gpu_matrix_multiply_v1_8(A, B, C_v1_8);
        end = clock();
        printf("%lf,", 1.0*(end-start)/CLOCKS_PER_SEC);
        for(int i = 0; i < N; i++){
            for(int j = 0; j < N; j++){
                if(C_v1_8[i*N + j] != C_cpu[i*N + j]){
                    dif_elems++;
                    deviation += abs(C_v1_8[i*N + j] - C_cpu[i*N + j]);
                }
            }
        }
        printf("%d,%lf,", dif_elems, deviation);
        dif_elems = 0;
        deviation = 0.0;

        // GPU_v1 16x16
        start = clock();
        gpu_matrix_multiply_v1_16(A, B, C_v1_16);
        end = clock();
        printf("%lf,", 1.0*(end-start)/CLOCKS_PER_SEC);
        for(int i = 0; i < N; i++){
            for(int j = 0; j < N; j++){
                if(C_v1_16[i*N + j] != C_cpu[i*N + j]){
                    dif_elems++;
                    deviation += abs(C_v1_16[i*N + j] - C_cpu[i*N + j]);
                }
            }
        }
        printf("%d,%lf,", dif_elems, deviation);
        dif_elems = 0;
        deviation = 0.0;

        // GPU_v1 32x32
        start = clock();
        gpu_matrix_multiply_v1_32(A, B, C_v1_32);
        end = clock();
        printf("%lf,", 1.0*(end-start)/CLOCKS_PER_SEC);
        for(int i = 0; i < N; i++){
            for(int j = 0; j < N; j++){
                if(C_v1_32[i*N + j] != C_cpu[i*N + j]){
                    dif_elems++;
                    deviation += abs(C_v1_32[i*N + j] - C_cpu[i*N + j]);
                }
            }
        }
        printf("%d,%lf,", dif_elems, deviation);
        dif_elems = 0;
        deviation = 0.0;
        
        // GPU_v2 32x1
        start = clock();
        gpu_matrix_multiply_v2_32(A, B, C_v2_32);
        end = clock();
        printf("%lf,", 1.0*(end-start)/CLOCKS_PER_SEC);
        for(int i = 0; i < N; i++){
            for(int j = 0; j < N; j++){
                if(C_v2_32[i*N + j] != C_cpu[i*N + j]){
                    dif_elems++;
                    deviation += abs(C_v2_32[i*N + j] - C_cpu[i*N + j]);
                }
            }
        }
        printf("%d,%lf,", dif_elems, deviation);
        dif_elems = 0;
        deviation = 0.0;

        // GPU_v2 64x1
        start = clock();
        gpu_matrix_multiply_v2_64(A, B, C_v2_64);
        end = clock();
        printf("%lf,", 1.0*(end-start)/CLOCKS_PER_SEC);
        for(int i = 0; i < N; i++){
            for(int j = 0; j < N; j++){
                if(C_v2_64[i*N + j] != C_cpu[i*N + j]){
                    dif_elems++;
                    deviation += abs(C_v2_64[i*N + j] - C_cpu[i*N + j]);
                }
            }
        }
        printf("%d,%lf,", dif_elems, deviation);
        dif_elems = 0;
        deviation = 0.0;

        // GPU_v2 96x1
        start = clock();
        gpu_matrix_multiply_v2_96(A, B, C_v2_96);
        end = clock();
        printf("%lf,", 1.0*(end-start)/CLOCKS_PER_SEC);
        for(int i = 0; i < N; i++){
            for(int j = 0; j < N; j++){
                if(C_v2_96[i*N + j] != C_cpu[i*N + j]){
                    dif_elems++;
                    deviation += abs(C_v2_96[i*N + j] - C_cpu[i*N + j]);
                }
            }
        }
        printf("%d,%lf,", dif_elems, deviation);
        dif_elems = 0;
        deviation = 0.0;

        // GPU_v3 32x1
        start = clock();
        gpu_matrix_multiply_v3_32(A, B, C_v3_32);
        end = clock();
        printf("%lf,", 1.0*(end-start)/CLOCKS_PER_SEC);
        for(int i = 0; i < N; i++){
            for(int j = 0; j < N; j++){
                if(C_v3_32[i*N + j] != C_cpu[i*N + j]){
                    dif_elems++;
                    deviation += abs(C_v3_32[i*N + j] - C_cpu[i*N + j]);
                }
            }
        }
        printf("%d,%lf,", dif_elems, deviation);
        dif_elems = 0;
        deviation = 0.0;
        
        // GPU_v3 64x1
        start = clock();
        gpu_matrix_multiply_v3_64(A, B, C_v3_64);
        end = clock();
        printf("%lf,", 1.0*(end-start)/CLOCKS_PER_SEC);
        for(int i = 0; i < N; i++){
            for(int j = 0; j < N; j++){
                if(C_v3_64[i*N + j] != C_cpu[i*N + j]){
                    dif_elems++;
                    deviation += abs(C_v3_64[i*N + j] - C_cpu[i*N + j]);
                }
            }
        }
        printf("%d,%lf,", dif_elems, deviation);
        dif_elems = 0;
        deviation = 0.0;
        
        // GPU_v4 32x1
        start = clock();
        gpu_matrix_multiply_v4_32(A, B, C_v4_32);
        end = clock();
        printf("%lf,", 1.0*(end-start)/CLOCKS_PER_SEC);
        for(int i = 0; i < N; i++){
            for(int j = 0; j < N; j++){
                if(C_v4_32[i*N + j] != C_cpu[i*N + j]){
                    dif_elems++;
                    deviation += abs(C_v4_32[i*N + j] - C_cpu[i*N + j]);
                }
            }
        }
        printf("%d,%lf,", dif_elems, deviation);
        dif_elems = 0;
        deviation = 0.0;

        // GPU_v5 32x1
        start = clock();
        gpu_matrix_multiply_v5_32(A, B, C_v5_32);
        end = clock();
        printf("%lf,", 1.0*(end-start)/CLOCKS_PER_SEC);
        for(int i = 0; i < N; i++){
            for(int j = 0; j < N; j++){
                if(C_v5_32[i*N + j] != C_cpu[i*N + j]){
                    dif_elems++;
                    deviation += abs(C_v5_32[i*N + j] - C_cpu[i*N + j]);
                }
            }
        }
        printf("%d,%lf\n", dif_elems, deviation);
        dif_elems = 0;
        deviation = 0.0;
    }
    return 0;
}

void matrix_generate(real* A){
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            r = (r - 0.5) * 2;  // r is in [-1, 1]
            A[i*N + j] = r * MAX_VAL;
        }
    }
}

inline void make_gpu_matrices(real*& A_gpu, real*& B_gpu, real*& C_gpu, real* A, real* B){
    ASSERT_CUDA(cudaMalloc((void**)&A_gpu, sizeof(real) * N * N));
    ASSERT_CUDA(cudaMalloc((void**)&B_gpu, sizeof(real) * N * N));
    ASSERT_CUDA(cudaMalloc((void**)&C_gpu, sizeof(real) * N * N));

    ASSERT_CUDA(cudaMemcpy(A_gpu, A, sizeof(real) * N * N, cudaMemcpyHostToDevice));
    ASSERT_CUDA(cudaMemcpy(B_gpu, B, sizeof(real) * N * N, cudaMemcpyHostToDevice));
}

inline void delete_gpu_matrices(real* A_gpu, real* B_gpu, real* C_gpu, real* C){
    ASSERT_CUDA(cudaMemcpy(C, C_gpu, sizeof(real) * N * N, cudaMemcpyDeviceToHost));

    ASSERT_CUDA(cudaFree(A_gpu));
    ASSERT_CUDA(cudaFree(B_gpu));
    ASSERT_CUDA(cudaFree(C_gpu));
}

// GPU_v1

__global__ void kernel_v1(real* A, real* B, real* C, int N){
    int i = threadIdx.x + blockIdx.x * blockDim.x;
    int j = threadIdx.y + blockIdx.y * blockDim.y;

    if(i >= N || j >= N)
        return;

    C[i*N + j] = 0;
    for(int k = 0; k < N; k++)
        C[i*N + j] += A[i*N + k] * B[k*N + j];
}

void gpu_matrix_multiply_v1_8(real* A, real* B, real* C){
    real *A_gpu = NULL, *B_gpu = NULL, *C_gpu = NULL;
    make_gpu_matrices(A_gpu, B_gpu, C_gpu, A, B);
    dim3 threadsPerBlock(8, 8, 1);
    dim3 numBlocks(N / 8 + 1, N / 8 + 1, 1);
    kernel_v1<<<numBlocks, threadsPerBlock, 0>>>(A_gpu, B_gpu, C_gpu, N);
    delete_gpu_matrices(A_gpu, B_gpu, C_gpu, C);
}

void gpu_matrix_multiply_v1_16(real* A, real* B, real* C){
    real *A_gpu = NULL, *B_gpu = NULL, *C_gpu = NULL;
    make_gpu_matrices(A_gpu, B_gpu, C_gpu, A, B);
    dim3 threadsPerBlock(16, 16, 1);
    dim3 numBlocks(N / 16 + 1, N / 16 + 1, 1);
    kernel_v1<<<numBlocks, threadsPerBlock, 0>>>(A_gpu, B_gpu, C_gpu, N);
    delete_gpu_matrices(A_gpu, B_gpu, C_gpu, C);
}

void gpu_matrix_multiply_v1_32(real* A, real* B, real* C){
    real *A_gpu = NULL, *B_gpu = NULL, *C_gpu = NULL;
    make_gpu_matrices(A_gpu, B_gpu, C_gpu, A, B);
    dim3 threadsPerBlock(32, 32, 1);
    dim3 numBlocks(N / 32 + 1, N / 32 + 1, 1);
    kernel_v1<<<numBlocks, threadsPerBlock, 0>>>(A_gpu, B_gpu, C_gpu, N);
    delete_gpu_matrices(A_gpu, B_gpu, C_gpu, C);
}

// GPU_v2

__global__ void kernel_v2_32(real* A, real* B, real* C, int N){
    __shared__ real result[32 * 32];
    __shared__ real bvec[32];
    int x0 = blockIdx.x * 32;
    int y0 = blockIdx.y * 32;
    int x = x0 + threadIdx.x;
    int y = y0 + threadIdx.x;

    for (int i = 0; i < 32; i++) {
        result[threadIdx.x * 32 + i] = 0;
    }

    for (int i = 0; i < N; i++) {
        // Set the value of currently needed B element in shared memory.
        if(x < N) {
            bvec[threadIdx.x] = B[x + (N * i)];
        } else {
            bvec[threadIdx.x] = 0;
        }

        // Get value of A element currently needed.
        real a;
        if(y < N) {
            a = A[y * N + i];
        } else {
            a = 0;
        }

        // Sync so all bvec values are set.
        __syncthreads();
        for (int j = 0; j < 32; j++) {
            result[threadIdx.x * 32 + j] += a * bvec[j];
        }
        // Sync so every thread gets to finish the job with proper bvec values.
        __syncthreads();
    }

    // Copy result to C array.
    if(y < N) {
        for (int i = 0; i < 32; i++) {
            if(x0 + i < N) {
                C[y * N + x0 + i] = result[threadIdx.x * 32 + i];
            } else
                break;
        }
    }
}

void gpu_matrix_multiply_v2_32(real* A, real* B, real* C){
    real *A_gpu = NULL, *B_gpu = NULL, *C_gpu = NULL;
    make_gpu_matrices(A_gpu, B_gpu, C_gpu, A, B);
    dim3 threadsPerBlock(32, 1, 1);
    dim3 numBlocks(N / 32 + 1, N / 32 + 1, 1);
    kernel_v2_32<<<numBlocks, threadsPerBlock, 0>>>(A_gpu, B_gpu, C_gpu, N);
    delete_gpu_matrices(A_gpu, B_gpu, C_gpu, C);
}

__global__ void kernel_v2_64(real* A, real* B, real* C, int N){
    __shared__ real result[64 * 64];
    __shared__ real bvec[64];
    int x0 = blockIdx.x * 64;
    int y0 = blockIdx.y * 64;
    int x = x0 + threadIdx.x;
    int y = y0 + threadIdx.x;

    for (int i = 0; i < 64; i++) {
        result[threadIdx.x * 64 + i] = 0;
    }

    for (int i = 0; i < N; i++) {
        if(x < N) {
            bvec[threadIdx.x] = B[x + (N * i)];
        } else {
            bvec[threadIdx.x] = 0;
        }

        real a;
        if(y < N) {
            a = A[y * N + i];
        } else {
            a = 0;
        }

        __syncthreads();
        for (int j = 0; j < 64; j++) {
            result[threadIdx.x * 64 + j] += a * bvec[j];
        }
        __syncthreads();
    }

    if(y < N) {
        for (int i = 0; i < 64; i++) {
            if(x0 + i < N) {
                C[y * N + x0 + i] = result[threadIdx.x * 64 + i];
            } else
                break;
        }
    }
}

void gpu_matrix_multiply_v2_64(real* A, real* B, real* C){
    real *A_gpu = NULL, *B_gpu = NULL, *C_gpu = NULL;
    make_gpu_matrices(A_gpu, B_gpu, C_gpu, A, B);
    dim3 threadsPerBlock(64, 1, 1);
    dim3 numBlocks(N / 64 + 1, N / 64 + 1, 1);
    kernel_v2_64<<<numBlocks, threadsPerBlock, 0>>>(A_gpu, B_gpu, C_gpu, N);
    delete_gpu_matrices(A_gpu, B_gpu, C_gpu, C);
}

__global__ void kernel_v2_96(real* A, real* B, real* C, int N){
    __shared__ real result[96 * 96];
    __shared__ real bvec[96];
    int x0 = blockIdx.x * 96;
    int y0 = blockIdx.y * 96;
    int x = x0 + threadIdx.x;
    int y = y0 + threadIdx.x;

    for (int i = 0; i < 96; i++) {
        result[threadIdx.x * 96 + i] = 0;
    }

    for (int i = 0; i < N; i++) {
        if(x < N) {
            bvec[threadIdx.x] = B[x + (N * i)];
        } else {
            bvec[threadIdx.x] = 0;
        }

        real a;
        if(y < N) {
            a = A[y * N + i];
        } else {
            a = 0;
        }

        __syncthreads();
        for (int j = 0; j < 96; j++) {
            result[threadIdx.x * 96 + j] += a * bvec[j];
        }
        __syncthreads();
    }

    if(y < N) {
        for (int i = 0; i < 96; i++) {
            if(x0 + i < N) {
                C[y * N + x0 + i] = result[threadIdx.x * 96 + i];
            } else
                break;
        }
    }
}

void gpu_matrix_multiply_v2_96(real* A, real* B, real* C){
    real *A_gpu = NULL, *B_gpu = NULL, *C_gpu = NULL;
    make_gpu_matrices(A_gpu, B_gpu, C_gpu, A, B);
    dim3 threadsPerBlock(96, 1, 1);
    dim3 numBlocks(N / 96 + 1, N / 96 + 1, 1);
    kernel_v2_96<<<numBlocks, threadsPerBlock, 0>>>(A_gpu, B_gpu, C_gpu, N);
    delete_gpu_matrices(A_gpu, B_gpu, C_gpu, C);
}

// GPU_v3

__global__ void kernel_v3_32(real* A, real* B, real* C, int N){
    __shared__ real result[32 * 32];
    __shared__ real bvec[32];
    int x0 = blockIdx.x * 32;
    int y0 = blockIdx.y * 32;
    int x = x0 + threadIdx.x;
    int y = y0 + threadIdx.x;

    for (int i = 0; i < 32; i++) {
        result[threadIdx.x * 32 + i] = 0;
    }

    for (int i = 0; i < N; i++) {
        if(x < N) {
            bvec[threadIdx.x] = B[x + (N * i)];
        } else {
            bvec[threadIdx.x] = 0;
        }

        real a;
        if(y < N) {
            a = A[y * N + i];
        } else {
            a = 0;
        }

        __syncthreads();
        // Resolve conflicts by adding offset (threadIdx.x). Now no two threads access same memory address.
        for (int j = 0; j < 32; j++) {
            result[threadIdx.x * 32 + (j + threadIdx.x) % 32] += a * bvec[(j + threadIdx.x) % 32];
        }
        __syncthreads();
    }

    if(y < N) {
        for (int i = 0; i < 32; i++) {
            if(x0 + i < N) {
                C[y * N + x0 + i] = result[threadIdx.x * 32 + i];
            } else
                break;
        }
    }
}

void gpu_matrix_multiply_v3_32(real* A, real* B, real* C){
    real *A_gpu = NULL, *B_gpu = NULL, *C_gpu = NULL;
    make_gpu_matrices(A_gpu, B_gpu, C_gpu, A, B);
    dim3 threadsPerBlock(32, 1, 1);
    dim3 numBlocks(N / 32 + 1, N / 32 + 1, 1);
    kernel_v3_32<<<numBlocks, threadsPerBlock, 0>>>(A_gpu, B_gpu, C_gpu, N);
    delete_gpu_matrices(A_gpu, B_gpu, C_gpu, C);
}

__global__ void kernel_v3_64(real* A, real* B, real* C, int N){
    __shared__ real result[64 * 64];
    __shared__ real bvec[64];
    int x0 = blockIdx.x * 64;
    int y0 = blockIdx.y * 64;
    int x = x0 + threadIdx.x;
    int y = y0 + threadIdx.x;

    for (int i = 0; i < 64; i++) {
        result[threadIdx.x * 64 + i] = 0;
    }

    for (int i = 0; i < N; i++) {
        if(x < N) {
            bvec[threadIdx.x] = B[x + (N * i)];
        } else {
            bvec[threadIdx.x] = 0;
        }

        real a;
        if(y < N) {
            a = A[y * N + i];
        } else {
            a = 0;
        }

        __syncthreads();
        for (int j = 0; j < 64; j++) {
            result[threadIdx.x * 64 + (j + threadIdx.x) % 64] += a * bvec[(j + threadIdx.x) % 64];
        }
        __syncthreads();
    }

    if(y < N) {
        for (int i = 0; i < 64; i++) {
            if(x0 + i < N) {
                C[y * N + x0 + i] = result[threadIdx.x * 64 + i];
            } else
                break;
        }
    }
}

void gpu_matrix_multiply_v3_64(real* A, real* B, real* C){
    real *A_gpu = NULL, *B_gpu = NULL, *C_gpu = NULL;
    make_gpu_matrices(A_gpu, B_gpu, C_gpu, A, B);
    dim3 threadsPerBlock(64, 1, 1);
    dim3 numBlocks(N / 64 + 1, N / 64 + 1, 1);
    kernel_v3_64<<<numBlocks, threadsPerBlock, 0>>>(A_gpu, B_gpu, C_gpu, N);
    delete_gpu_matrices(A_gpu, B_gpu, C_gpu, C);
}

// GPU_v4

__global__ void kernel_v4_32(real* A, real* B, real* C, int N){
    __shared__ real result[32 * 32];
    int x0 = blockIdx.x * 32;
    int y0 = blockIdx.y * 32;
    int x = x0 + threadIdx.x;
    int y = y0 + threadIdx.x;

    for (int i = 0; i < 32; i++) {
        result[threadIdx.x * 32 + i] = 0;
    }

    for (int i = 0; i < N; i++) {
        real b;
        if(x < N) {
            b = B[x + (N * i)];
        } else {
            b = 0;
        }

        real a;
        if(y < N) {
            a = A[y * N + i];
        } else {
            a = 0;
        }

        __syncthreads();
        // We take value from thread 'j' using by shuffling.
        for (int j = 0; j < 32; j++) {
            result[threadIdx.x * 32 + (j + threadIdx.x) % 32] += a * __shfl_sync(0xffffffff, b, j);
        }
        __syncthreads();
    }

    if(y < N) {
        for (int i = 0; i < 32; i++) {
            if(x0 + i < N) {
                C[y * N + x0 + i] = result[threadIdx.x * 32 + i];
            } else
                break;
        }
    }
}

void gpu_matrix_multiply_v4_32(real* A, real* B, real* C){
    real *A_gpu = NULL, *B_gpu = NULL, *C_gpu = NULL;
    make_gpu_matrices(A_gpu, B_gpu, C_gpu, A, B);
    dim3 threadsPerBlock(32, 1, 1);
    dim3 numBlocks(N / 32 + 1, N / 32 + 1, 1);
    kernel_v4_32<<<numBlocks, threadsPerBlock, 0>>>(A_gpu, B_gpu, C_gpu, N);
    delete_gpu_matrices(A_gpu, B_gpu, C_gpu, C);
}

// GPU_v5

__global__ void kernel_v5_32(real* A, real* B, real* C, int N){
    // We only store our result row.
    real reg[32];
    int x0 = blockIdx.x * 32;
    int y0 = blockIdx.y * 32;
    int x = x0 + threadIdx.x;
    int y = y0 + threadIdx.x;

    for(int i = 0; i < 32; i++)
        reg[i] = 0;

    for (int i = 0; i < N; i++) {
        real b;
        if(x < N) {
            b = B[x + (N * i)];
        } else {
            b = 0;
        }

        real a;
        if(y < N) {
            a = A[y * N + i];
        } else {
            a = 0;
        }
        
        // Get needed values from other threads.
        for(int j = 0; j < 32; j++){
            reg[j] += a * __shfl_sync(0xffffffff, b, j);
        }
    }

    // Load our row to C array.
    if(y < N) {
        for(int i = 0; i < 32; i++){
            if(x0 + i < N){
                C[y * N + y0 + i] = reg[i];
            }
        }
    }
}

void gpu_matrix_multiply_v5_32(real* A, real* B, real* C){
    real *A_gpu = NULL, *B_gpu = NULL, *C_gpu = NULL;
    make_gpu_matrices(A_gpu, B_gpu, C_gpu, A, B);
    dim3 threadsPerBlock(32, 1, 1);
    dim3 numBlocks(N / 32 + 1, N / 32 + 1, 1);
    kernel_v5_32<<<numBlocks, threadsPerBlock, 0>>>(A_gpu, B_gpu, C_gpu, N);
    delete_gpu_matrices(A_gpu, B_gpu, C_gpu, C);
}

void cpu_matrix_multiply(real* A, real* B, real* C){
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            C[i*N + j] = 0;
            for(int k = 0; k < N; k++){
                C[i*N + j] += A[i*N + k] * B[k*N + j];
            }
        }
    }
}
