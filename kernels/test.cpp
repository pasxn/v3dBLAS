#include <stdio.h>
#include "V3DLib.h"
#include "Support/Settings.h"

using namespace V3DLib;

V3DLib::Settings settings;


void gemm3D(Int N, Int M, Int P, Float::Ptr A, Float::Ptr B, Float::Ptr C) {
  For (Int l = 0, l < P, l += 16)
    For (Int i = 0, i < N, i += 16)
      For (Int j = 0, j < M, j += 16)
        Float sum = 0.0f;
        For (Int k = 0, k < N, k += 16)
          Float val_a = A[l * N * M + i * M + k];
          Float val_b = B[l * N * M + k * M + j];
          sum += val_a * val_b; 
        End

        C[l * N * M + i * M + j] = sum;
      End
    End
  End
}

int main() {
  int N = 3; 
  int M = 3; 
  int P = 3; 
  Float::Array A(N * M * P), B(N * M * P), C(N * M * P); 


  for (int l = 0; l < P; l++) {
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < M; j++) {
        A[l * N * M + i * M + j] = static_cast<float>(l + i + j) / 10.0f;
        B[l * N * M + i * M + j] = static_cast<float>(P - l - i - j) / 10.0f;
      }
    }
  }

  auto k = compile(gemm3D); // Compile the kernel
  k.load(N, M, P, &A, &B, &C); // Load input data
  k.setNumQPUs(12); // Set the number of QPUs to use
  settings.process(k); // Run the kernel

  // Print the result (tensor C)
  for (int l = 0; l < P; l++) {
    printf("Layer %d:\n", l);
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < M; j++) {
        printf("%f ", C[l * N * M + i * M + j]);
      }
      printf("\n");
    }
  }

  return 0;
}

