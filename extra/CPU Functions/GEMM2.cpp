#include <stdio.h>
#include <algorithm>  // For std::min

const int N = 9;  // Example size, you can adjust as needed
const int TILE_SIZE = 3;  // Example tile size, adjust as needed

void gemm_cpu(int N, float* A, float* B, float* result_cpu, int TILE_SIZE) {
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
    float A[N * N], B[N * N], result_cpu[N * N];

    // Initialize matrices A and B with some values, for simplicity set all to 1
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i * N + j] = 1;
            B[i * N + j] = 1;
        }
    }

    // Perform GEMM using block method
    gemm_cpu(N, A, B, result_cpu, TILE_SIZE);

    // Print the result
    printf("Resulting matrix:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%f ", result_cpu[i * N + j]);
        }
        printf("\n");
    }

    return 0;
}

