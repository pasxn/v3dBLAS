#include <stdio.h>
#include "V3DLib.h"
#include <chrono>
#include "Support/Settings.h"
#include <math.h>

using namespace V3DLib;

V3DLib::Settings settings;

void exponential_gpu(Int n, Float::Ptr x, Float::Ptr result_gpu) {
  const Float log2e = 1.4426950408889634;  // log2(e)
  For (Int i = 0, i < n, i += 16) 
    Float a = x[i];
    result_gpu[i] = exp(log2e * a); 
  End
}



void exp_CPU(int n,float* x, float* result_cpu){
  for (int i = 0; i < n; i++) {
        result_cpu[i] = expf(x[i]);
  }
}


int main() {
    // Define the range of sizes and number of iterations for benchmarking
    int min_size = 2;
    int max_size = 2048;
    int iterations = 1000;

    // Loop over the range of sizes
    for (int size = min_size; size <= max_size; size *= 2) {
        int arr_size=8;
        Float::Array x_gpu(arr_size), result_gpu(arr_size);
        float x_cpu[arr_size], result_cpu[arr_size];

        // Initialize the arrays with test data
        for (int i = 0; i < arr_size; i++) {
            x_gpu[i] = static_cast<float>(i);
            x_cpu[i] = static_cast<float>(i);
        }

        // Compile and set up the GPU kernel
        auto k = compile(exponential_gpu);
        k.setNumQPUs(8);

        // GPU run
        auto start_gpu = std::chrono::high_resolution_clock::now();
        for (int y = 0; y < iterations; y++) {
            k.load(arr_size, &x_gpu, &result_gpu);
        }
        settings.process(k); // Run the kernel
        auto end_gpu = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration_gpu = end_gpu - start_gpu;

        // CPU run
        auto start_cpu = std::chrono::high_resolution_clock::now();
        for (int y = 0; y < iterations; y++) {
            exp_CPU(arr_size, x_cpu, result_cpu);
        }
        auto end_cpu = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration_cpu = end_cpu - start_cpu;

        // Functional verification with a tolerance
        float epsilon = 5e-1;  // Use an appropriate small value
        bool valid = true;
        for (int j = 0; j < arr_size; j++) {
            if (fabs(result_cpu[j] - result_gpu[j]) > epsilon) {
                printf("Mismatch at index %d: CPU = %f, GPU = %f\n", j, result_cpu[j], result_gpu[j]);
                valid = false;
                break;
            }
        }

        // Only print the timing if the results are valid
        if (valid) {
            printf("[%d, %f, %f]\n", size, duration_cpu.count(), duration_gpu.count());
        } else {
            printf("Functional verification failed at size %d\n", arr_size);
        }
    }

    return 0;
}

