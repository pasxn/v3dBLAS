#include <stdio.h>
#include "V3DLib.h"
#include "Support/Settings.h"

using namespace V3DLib;

V3DLib::Settings settings;


void gemm(Int N, Float::Ptr A, Float::Ptr B, Float::Ptr C) {
  For (Int i = 0, i < N, i += 16) 
    For (Int j = 0, j < N, j += 16) 
      Float sum = 0.0f;

      For (Int k = 0, k < N, k += 16)
        Float val_a = A[i * N + k];
        Float val_b = B[k * N + j];
        sum += val_a * val_b;
      End

      C[i * N + j] = sum;
     End
   End
}

int main() {
  int N = 7; 
  Float::Array A(N * N), B(N * N), C(N * N); 

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      A[i * N + j] = static_cast<float>(i + j) / 10.0f;
      B[i * N + j] = static_cast<float>(N - i - j) / 10.0f;
    }
  }

  auto k = compile(gemm); // Compile the kernel
  k.load(N, &A, &B, &C); // Load input data
  k.setNumQPUs(12); // Set the number of QPUs to use
  settings.process(k); // Run the kernel

  // Print the result (matrix C)
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      printf("%f ", C[i * N + j]);
    }
    printf("\n");
  }

  return 0;
}

