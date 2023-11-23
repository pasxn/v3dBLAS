#include <stdio.h>
#include "V3DLib.h"
#include "Support/Settings.h"

using namespace V3DLib;

V3DLib::Settings settings;


void gemm4D(Int N, Int M, Int P, Int Q, Float::Ptr A, Float::Ptr B, Float::Ptr C) {
  For (Int h = 0, h < Q, h += 16)
    For (Int l = 0, l < P, l += 16)
      For (Int i = 0, i < N, i += 16)
        For (Int j = 0, j < M, j += 16)
          Float sum = 0.0f;

          For (Int k = 0, k < N, k += 16)
            Float val_a = A[h * N * M * P + l * N * M + i * M + k];
            Float val_b = B[h * N * M * P + l * N * M + k * M + j];
            sum += val_a * val_b; 
          End

          C[h * N * M * P + l * N * M + i * M + j] = sum;
        End
      End
    End
  End
}

int main() {
  int N = 3; 
  int M = 3; 
  int P = 3; 
  int Q = 3; 
  Float::Array A(N * M * P * Q), B(N * M * P * Q), C(N * M * P * Q);

  for (int h = 0; h < Q; h++) {
    for (int l = 0; l < P; l++) {
      for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
          A[h * N * M * P + l * N * M + i * M + j] = static_cast<float>(h + l + i + j) / 10.0f;
          B[h * N * M * P + l * N * M + i * M + j] = static_cast<float>(Q - h - l - i - j) / 10.0f;
        }
      }
    }
  }

  auto k = compile(gemm4D); // Compile the kernel
  k.load(N, M, P, Q, &A, &B, &C); // Load input data
  k.setNumQPUs(12); // Set the number of QPUs to use
  settings.process(k); // Run the kernel

  // Print the result (tensor C)
  for (int h = 0; h < Q; h++) {
    printf("Cube %d:\n", h);
    for (int l = 0; l < P; l++) {
      printf("Layer %d:\n", l);
      for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
          printf("%f ", C[h * N * M * P + l * N * M + i * M + j]);
        }
        printf("\n");
      }
    }
  }

  return 0;
}

