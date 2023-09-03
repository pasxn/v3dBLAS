#include <stdio.h>
#include "V3DLib.h"
#include "Support/Settings.h"
#include <chrono>

using namespace V3DLib;

V3DLib::Settings settings;

const int TILE_SIZE = 9;
void gemm_gpu(Int N, Float::Ptr A, Float::Ptr B, Float::Ptr result_gpu) {
  For (Int row_tile = 0, row_tile < N, row_tile += TILE_SIZE)
    For (Int col_tile = 0, col_tile < N, col_tile += TILE_SIZE)
      For (Int i = row_tile, i < min(N, row_tile + TILE_SIZE), i++)
        For (Int j = col_tile, j < min(N, col_tile + TILE_SIZE), j++)
          Float sum = 0.0f;
          For (Int k = 0, k < N, k++)
            Float val_a = A[i * N + k];
            Float val_b = B[k * N + j];
            sum += val_a * val_b;
          End

          result_gpu[i * N + j] = sum;

        End
      End

    End
  End
}


void gemm_cpu(int N, float* A, float* B, float* result_cpu) {
    for (int row_tile = 0; row_tile < N; row_tile += TILE_SIZE) {
        for (int col_tile = 0; col_tile < N; col_tile += TILE_SIZE) {
            for (int i = row_tile; i < std::min(N, row_tile + TILE_SIZE); i++) {
                for (int j = col_tile; j < std::min(N, col_tile + TILE_SIZE); j++) {
                    float sum = 0.0f;
                    for (int k = 0; k < N; k++) {
                        float val_a = A[i * N + k];
                        float val_b = B[k * N + j];
                        sum += val_a * val_b;
                    }
                    result_cpu[i * N + j] = sum;
                }
            }
        }
    }
}


int main() {
  const int N = 9; 
  Float::Array A_gpu(N * N), B_gpu(N * N), result_gpu(N * N); 
  
  float A_cpu[N*N], B_cpu[N*N] , result_cpu[N*N];
  
  
  

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      A_gpu[i * N + j] = 1;
      B_gpu[i * N + j] = 1;
      A_cpu[i * N + j] = 1;
      B_cpu[i * N + j] = 1;
    }
  }

//gpu run
  auto k = compile(gemm_gpu); // Compile the kernel
  auto start_gpu = std::chrono::high_resolution_clock::now();
  k.load(N, &A_gpu, &B_gpu, &result_gpu); // Load input data
  k.setNumQPUs(8); // Set the number of QPUs to use
  settings.process(k); // Run the kernel
  auto end_gpu = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> duration_gpu = end_gpu - start_gpu;
  

//cpu run
  
  auto start_cpu = std::chrono::high_resolution_clock::now();
  gemm_cpu(N, A_cpu, B_cpu, result_cpu);
  auto end_cpu = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> duration_cpu = end_cpu - start_cpu;
  
    //functional verification
  for(int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      if(result_cpu[i * N + j] != result_gpu[i*N + j]){
        printf("CPU output and GPU output is not equal at j = %d \n", j);
        printf("GPU output = %f and CPU output = %f \n",result_gpu[j],result_cpu[j]);
      }
    } 
  } 
  
  
  printf(".........Execution Time.........\n");
  printf("Execution time for CPU: %f seconds\n", duration_cpu.count());
  printf("Execution time for GPU: %f seconds\n", duration_gpu.count());

  // Print the result (matrix C)
/*  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      printf("%f ", result_gpu[i * N + j]);
    }
    printf("\n");
  } */

  return 0;
}

