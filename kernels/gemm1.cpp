#include <stdio.h>
#include "V3DLib.h"
#include <chrono>
#include "Support/Settings.h"

using namespace V3DLib;

V3DLib::Settings settings;

void gemm_gpu(Int n, Float::Ptr a_gpu, Float::Ptr b_gpu, Float::Ptr result_gpu) {
    Int qpu_id = me() * (n / numQPUs());
    Int qpu_end = (me() + 1) * (n / numQPUs());

    // Adjust for the last QPU to handle remainder
    Where(me() == numQPUs() - 1)
        qpu_end = n;
    End

    Float sum = 0.0f;
    For (Int i = qpu_id, i < qpu_end, i++) {
        sum += a_gpu[i] * b_gpu[i];
    }
    End

    result_gpu[me()] = sum;
}

void gemm_cpu(int n, float *a_cpu, float *b_cpu, float* result_cpu) {
    float sum = 0.0f;
    for (int i = 0; i < n; i++) {
        sum += a_cpu[i] * b_cpu[i];
    }
    *result_cpu = sum;
}

int main() {
    int min_size = 2;  // Starting size for the arrays
    int max_size = 2048; // Maximum size for the arrays
    int iterations = 100000;

    for (int size = min_size; size <= max_size; size *= 2) {
        Float::Array a_gpu(size), b_gpu(size), result_gpu(8);
        float a_cpu[size], b_cpu[size] , result_cpu;

        // Initialize vectors 'a' and 'b'
        for (int i = 0; i < size; i++) {
            a_gpu[i] = 1;
            b_gpu[i] = 1;
            a_cpu[i] = 1;
            b_cpu[i] = 1;
        }

        // GPU run
        auto k = compile(gemm_gpu);
        k.setNumQPUs(8);
        auto start_gpu = std::chrono::high_resolution_clock::now();
        for(int i=0;i<iterations;i++){
          k.load(size, &a_gpu, &b_gpu, &result_gpu);
        }
        settings.process(k);
        auto end_gpu = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration_gpu = end_gpu - start_gpu;
	float total_sum_gpu = 0.0f;
        for (int i = 0; i < 8; i++) {
            total_sum_gpu += result_gpu[i];
        }
        
        
        // CPU run
        auto start_cpu = std::chrono::high_resolution_clock::now();
        for(int i=0;i<iterations;i++){        
          gemm_cpu(size, a_cpu, b_cpu, &result_cpu);
        }
        auto end_cpu = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration_cpu = end_cpu - start_cpu;

        // Compare results and print
        if (result_cpu != total_sum_gpu) {
            printf("Mismatch at size %d: CPU = %f, GPU = %f\n", size, result_cpu, total_sum_gpu);
        } else {
            printf("[%d, %f, %f]\n", size, duration_cpu.count(), duration_gpu.count());
        }
    }

    return 0;
}

