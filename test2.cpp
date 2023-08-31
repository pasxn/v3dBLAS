#include <stdio.h>
#include "V3DLib.h"
#include "Support/Settings.h"

using namespace V3DLib;

V3DLib::Settings settings;

// Kernel for General Matrix Multiplication (GEMM) for 5D tensors
void gemm5D(Int N, Int M, Int P, Int Q, Int R, Float::Ptr A, Float::Ptr B, Float::Ptr C) {
  For (Int s = 0, s < R, s += 16)
    For (Int h = 0, h < Q, h += 16)
      For (Int l = 0, l < P, l += 16)
        For (Int i = 0, i < N, i += 16)
          For (Int j = 0, j < M, j += 16)
            Float sum = 0.0f;
            For (Int k = 0, k < N, k += 16)
              Float val_a = A[s * N * M * P * Q + h * N * M * P + l * N * M + i * M + k];
              Float val_b = B[s * N * M * P * Q + h * N * M * P + l * N * M + k * M + j];
              sum += val_a * val_b;
            End
            C[s * N * M * P * Q + h * N * M * P + l * N * M + i * M + j] = sum;
          End
        End
      End
    End
  End
}

void gemm5D_cpu(int N, int M, int P, int Q, int R, float *A, float *B, float *C) {
    for (int s = 0; s < R; s++) {
        for (int h = 0; h < Q; h++) {
            for (int l = 0; l < P; l++) {
                for (int i = 0; i < N; i++) {
                    for (int j = 0; j < M; j++) {
                        float sum = 0.0f;
                        for (int k = 0; k < N; k++) {
                            float val_a = A[s * N * M * P * Q + h * N * M * P + l * N * M + i * M + k];
                            float val_b = B[s * N * M * P * Q + h * N * M * P + l * N * M + k * M + j];
                            sum += val_a * val_b;
                        }
                        C[s * N * M * P * Q + h * N * M * P + l * N * M + i * M + j] = sum;
                    }
                }
            }
        }
    }
}

int main() {
  int N = 2;  // Adjust these dimensions as needed
  int M = 2;
  int P = 2;
  int Q = 2;
  int R = 2;

  Float::Array A(N * M * P * Q * R), B(N * M * P * Q * R), C(N * M * P * Q * R);

  // Initialize tensors 'A' and 'B' with example values
  for (int s = 0; s < R; s++) {
    for (int h = 0; h < Q; h++) {
      for (int l = 0; l < P; l++) {
        for (int i = 0; i < N; i++) {
          for (int j = 0; j < M; j++) {
            A[s * N * M * P * Q + h * N * M * P + l * N * M + i * M + j] = static_cast<float>(s + h + l + i + j);
            B[s * N * M * P * Q + h * N * M * P + l * N * M + i * M + j] = static_cast<float>(R - s - h - l - i - j);
          }
        }
      }
    }
  }

  auto k = compile(gemm5D);
  k.load(N, M, P, Q, R, &A, &B, &C);

  // GPU Execution and Timing
  auto start_gpu = std::chrono::high_resolution_clock::now();
  settings.process(k);
  auto stop_gpu = std::chrono::high_resolution_clock::now();
  auto duration_gpu = std::chrono::duration_cast<std::chrono::microseconds>(stop_gpu - start_gpu);

  // CPU Execution and Timing
  auto start_cpu = std::chrono::high_resolution_clock::now();
  gemm5D_cpu(N, M, P, Q, R, A.data(), B.data(), C.data());
  auto stop_cpu = std::chrono::high_resolution_clock::now();
  auto duration_cpu = std::chrono::duration_cast<std::chrono::microseconds>(stop_cpu - start_cpu);

  // Print the execution times
  printf("Time taken by GPU: %ld microseconds\n", duration_gpu.count());
  printf("Time taken by CPU: %ld microseconds\n", duration_cpu.count());

  return 0;
}
