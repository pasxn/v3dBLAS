#include <stdio.h>
#include "V3DLib.h"
#include "Support/Settings.h"
#include <chrono>

using namespace V3DLib;

V3DLib::Settings settings;

void max_gpu(Int n, Int::Ptr x, Int::Ptr result_gpu) {
  Int start_val = me() * (n / numQPUs());
  Int end_val = (me() + 1) * (n / numQPUs());
  
  Where(me() == numQPUs() - 1)
    end_val = n;
  End
  Int local_max = 0; 

  
  For (Int i = start_val, i < end_val, i++)
    Int a = x[i];
    Where (a > local_max)
      local_max = a;
    End
  End
  
  result_gpu[me()] = local_max;
  
  /*For (Int offset = 4, offset > 0, offset = offset / 2) 
    If (me() < offset)
      Int neighbor_max = result_gpu[me() + offset];
      Where (neighbor_max > local_max)
        local_max = neighbor_max;
      End
      result_gpu[me()] = local_max;
    End
  End 
  

  
  If (me() == 0)
    result_gpu[0] = local_max;
  End */
}

void max_cpu(int n, int *x, int *result_cpu) {
    *result_cpu = x[0];  // Initialize with the first value

    for (int i = 1; i < n; i++) {
        if (x[i] > *result_cpu) {
            *result_cpu = x[i];
        }
    }
}

int main() {

  // Iterate over a range of sizes
  for (int size = 256; size <= 8192; size *= 2) {
    Int::Array x_gpu(size), result_gpu(8); // Assume 8 QPUs are available
    int x_cpu[size];
    int result_cpu; // Only need a single int for the CPU result
    int iterations = 20000;

    // Initialize arrays
    for (int i = 0; i < size; i++) {
      x_gpu[i] = i;
      x_cpu[i] = i;
    }

    // GPU run
    auto k = compile(max_gpu);
    auto start_gpu = std::chrono::high_resolution_clock::now();
    k.setNumQPUs(8);
    for(int i=0;i<iterations; i++){
      k.load(size, &x_gpu, &result_gpu);
    }
    settings.process(k);
    auto end_gpu = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_gpu = end_gpu - start_gpu;
    
    int global_max_gpu = 0;
    for (int i = 0; i < 8; i++) {
      if (result_gpu[i] > global_max_gpu) {
        global_max_gpu = result_gpu[i];
      }
    }

    // CPU run
    auto start_cpu = std::chrono::high_resolution_clock::now();
    for(int i=0;i<iterations; i++){
      max_cpu(size, x_cpu, &result_cpu);
    }
    auto end_cpu = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_cpu = end_cpu - start_cpu;

    // Functional verification
    if (result_cpu != global_max_gpu) {
      printf("Discrepancy found: CPU max = %d, GPU max = %d\n", result_cpu, global_max_gpu);
    }

    // Execution time log
    printf("[%d, %f, %f]\n", size, duration_cpu.count(), duration_gpu.count());
  }

  return 0;
}

