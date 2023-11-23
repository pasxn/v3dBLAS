#include <stdio.h>
#include <chrono>
#include "V3DLib.h"
#include <CmdParameters.h>
#include "Support/Settings.h"

using namespace V3DLib;

V3DLib::Settings settings;

void sum_gpu(Int n, Int::Ptr x, Int::Ptr result_gpu) {
  Int qpu_id = me() * (n / numQPUs());
  Int qpu_end = (me() + 1) * (n / numQPUs());
  
  // Adjust for the last QPU to handle remainder
  Where(me() == numQPUs() - 1)
    qpu_end = n;
  End

  Int sum = 0;
  For (Int i = qpu_id, i < qpu_end, i++)
    Int val = x[i];
    sum += val;
  End

  result_gpu[me()] = sum;
}


void sum_cpu(int n, int x[], int *result_cpu) {
    *result_cpu = 0;
    for (int i = 0; i < n; i++) {
        *result_cpu += x[i];
    }
}

int main() {
    int min_size = 2;     // Starting size for the array
    int max_size = 2048;  // Maximum size for the array
    int step_factor = 2;  // Factor to increase the size in each step
    int qpu_count = 8;    // Number of QPUs to use
    long int size_multiplied;
    int iterations = 20000;

    // Loop over the range of sizes
    for (int size = min_size; size <= max_size; size *= step_factor) {
        Int::Array x_gpu(size), result_gpu(qpu_count);
        int x_cpu[size], result_cpu;
	
        // Initialize the arrays with test data
        for (int i = 0; i < size; ++i) {
            x_gpu[i] = 1;
            x_cpu[i] = 1;
        }

        // Compile and set up the GPU kernel
        auto k = compile(sum_gpu);
        k.setNumQPUs(qpu_count);

        // GPU run
        auto start_gpu = std::chrono::high_resolution_clock::now();
        for(int i=0;i<iterations; i++){
          k.load(size, &x_gpu, &result_gpu);
        }
        settings.process(k);
        auto end_gpu = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration_gpu = end_gpu - start_gpu;

        // Sum up partial results from each QPU to get the total sum
        int total_sum_gpu = 0;
        for (int i = 0; i < qpu_count; ++i) {
            total_sum_gpu += result_gpu[i];
        }

        // CPU run
        auto start_cpu = std::chrono::high_resolution_clock::now();
        for(int i=0;i<iterations; i++){
          sum_cpu(size, x_cpu, &result_cpu);
        }
        auto end_cpu = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration_cpu = end_cpu - start_cpu;

        // Compare GPU and CPU results
        if (result_cpu != total_sum_gpu) {
            printf("Mismatch: CPU sum = %d, GPU sum = %d\n", result_cpu, total_sum_gpu);
        } else {
            // Print the execution time if the results match
            printf("[%d, %f, %f]\n", size, duration_cpu.count(), duration_gpu.count());
        }
    }

    return 0;
}

