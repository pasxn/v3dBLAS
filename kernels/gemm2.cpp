#include <stdio.h>
#include "V3DLib.h"
#include "Support/Settings.h"
#include <chrono>

using namespace V3DLib;

V3DLib::Settings settings;

const int TILE_SIZE = 32;
/*void gemm_gpu(Int N, Float::Ptr A, Float::Ptr B, Float::Ptr result_gpu) {
  Int qpu_id = me();
  Int num_qp_us = numQPUs();
  Int rows_per_qpu = (N + num_qp_us - 1) / num_qp_us; // Ceiling division

  Int start_row = qpu_id * rows_per_qpu;
  Int end_row = min(start_row + rows_per_qpu, N);

  For (Int row_tile = start_row, row_tile < end_row, row_tile += TILE_SIZE)
    For (Int col_tile = 0, col_tile < N, col_tile += TILE_SIZE)
      For (Int i = row_tile, i < min(end_row, row_tile + TILE_SIZE), i++)
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
}*/
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

//gemm cpu with tiling

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
  int min_size = 2;     // Starting size for the matrices
  int max_size = 32;    // Maximum size for the matrices
  int step_factor = 2;  // Factor to increase the size in each step
  int iterations = 100000;

  for (int N = min_size; N <= max_size; N *= step_factor) {
    Float::Array A_gpu(N * N), B_gpu(N * N), result_gpu(N * N); 
    float A_cpu[N*N], B_cpu[N*N], result_cpu[N*N];
  
    // Initialize matrices A and B
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < N; j++) {
        A_gpu[i * N + j] = 1;
        B_gpu[i * N + j] = 1;
        A_cpu[i * N + j] = 1;
        B_cpu[i * N + j] = 1;
      }
    }

    // GPU run
    auto k = compile(gemm_gpu);
    auto start_gpu = std::chrono::high_resolution_clock::now();
    k.setNumQPUs(8);
    for(int i=0; i<iterations;i++){
      k.load(N, &A_gpu, &B_gpu, &result_gpu);
    }
    settings.process(k);
    auto end_gpu = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_gpu = end_gpu - start_gpu;
  
    // CPU run
    auto start_cpu = std::chrono::high_resolution_clock::now();
    for(int i=0; i<iterations;i++){
      gemm_cpu(N, A_cpu, B_cpu, result_cpu);
    }
    auto end_cpu = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_cpu = end_cpu - start_cpu;
  
    // Functional verification
    bool is_equal = true;
    for(int i = 0; i < N; i++) {
      for (int j = 0; j < N; j++) {
        if(result_cpu[i * N + j] != result_gpu[i * N + j]){
          printf("Mismatch at size %d: CPU and GPU results differ at index [%d, %d]\n", N, i, j);
          is_equal = false;
          break;
        }
      }
      if (!is_equal) break;
    } 

    // Print execution times
    if (is_equal) {
      printf("[%d, %f, %f]\n", N, duration_cpu.count(), duration_gpu.count());
    }
  }

  return 0;
}
