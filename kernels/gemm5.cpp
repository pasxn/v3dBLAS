#include <stdio.h>
#include <chrono>
#include "V3DLib.h"
#include "Support/Settings.h"

using namespace V3DLib;

V3DLib::Settings settings;

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

int main() {
  int N = 2; 
  int M = 2; 
  int P = 2; 
  int Q = 2; 
  int R = 2; 
  Float::Array A(N * M * P * Q * R), B(N * M * P * Q * R), C(N * M * P * Q * R);

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
  k.setNumQPUs(12);
  settings.process(k);

  // Print the result (tensor C)
  printf("Result (tensor C):\n");
  for (int s = 0; s < R; s++) {
    printf("Hypercube %d:\n", s);
    for (int h = 0; h < Q; h++) {
      printf("  Cube %d:\n", h);
      for (int l = 0; l < P; l++) {
        printf("    Layer %d:\n", l);
        for (int i = 0; i < N; i++) {
          for (int j = 0; j < M; j++) {
            printf("      %f ", C[s * N * M * P * Q + h * N * M * P + l * N * M + i * M + j]);
          }
          printf("\n");
        }
      }
    }
  }

  return 0;
}

