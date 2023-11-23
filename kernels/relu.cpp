#include <stdio.h>
#include "V3DLib.h"
#include <chrono>
#include "Support/Settings.h"

using namespace V3DLib;

V3DLib::Settings settings;

void relu_gpu(Int n, Float::Ptr x, Float::Ptr result_gpu) {
    Int qpu_id = me() * (n / numQPUs());
    Int qpu_end = (me() + 1) * (n / numQPUs());

    // Adjust for the last QPU to handle the remainder
    Where(me() == numQPUs() - 1)
        qpu_end = n;
    End

    For (Int i = qpu_id, i < qpu_end, i++)
        Float a = x[i];
        Where (a < 0.0f)
            a = 0.0f;
        End
        result_gpu[i] = a;
    End
}

void relu_cpu(int n, float x[], float result_cpu[]) {
    for (int i = 0; i < n; i++) {
        result_cpu[i] = (x[i] > 0) ? x[i] : 0;
    }
}

int main() {
    int min_size = 2;     // Starting size for the array
    int max_size = 2048;  // Maximum size for the array
    int step_factor = 2;  // Factor to increase the size in each step
    int qpu_count = 8;    // Number of QPUs to use
    int iterations = 30000;

    // Loop over the range of sizes
    for (int size = min_size; size <= max_size; size *= step_factor) {
        Float::Array x_gpu(size), result_gpu(size);
        float x_cpu[size], result_cpu[size];

        // Initialize the arrays with test data
        for (int i = 0; i < size; ++i) {
            x_gpu[i] = i - size / 2;  // Some test data
            x_cpu[i] = i - size / 2;
        }

        // Compile and set up the GPU kernel
        auto k = compile(relu_gpu);
        k.setNumQPUs(qpu_count);
        // GPU run
        auto start_gpu = std::chrono::high_resolution_clock::now();
        for(int i=0;i<iterations; i++){
          k.load(size, &x_gpu, &result_gpu);
        }
        settings.process(k);
        auto end_gpu = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration_gpu = end_gpu - start_gpu;

        // CPU run
        auto start_cpu = std::chrono::high_resolution_clock::now();
        for(int i=0;i<iterations; i++){
          relu_cpu(size, x_cpu, result_cpu);
        }
        auto end_cpu = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration_cpu = end_cpu - start_cpu;

        // Compare GPU and CPU results
        bool mismatch = false;
        for (int i = 0; i < size; i++) {
            if (result_cpu[i] != result_gpu[i]) {
                mismatch = true;
                break;
            }
        }

        if (mismatch) {
            printf("Mismatch detected at size %d\n", size);
        } else {
            // Print the execution time if the results match
            printf("[%d, %f, %f]\n", size, duration_cpu.count(), duration_gpu.count());
        }
    }

    return 0;
}

