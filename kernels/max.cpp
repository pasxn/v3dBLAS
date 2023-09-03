#include <stdio.h>
#include "V3DLib.h"
#include "Support/Settings.h"
#include <chrono>

using namespace V3DLib;

V3DLib::Settings settings;

void max_gpu(Int n, Int::Ptr x, Int::Ptr result_gpu) {
  Int local_max = 0; 

  
  For (Int i = 0, i < n, i++)
    Int a = x[i];
    Where (a > local_max)
      local_max = a;
    End
  End
  
  result_gpu[me()] = local_max;
  
  For (Int offset = 4, offset > 0, offset = offset / 2)
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
  End
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
  int size = 2000; 
  Int::Array x_gpu(size), result_gpu(8); 
  
  int x_cpu[size], result_cpu[size];

 
  for (int i = 0; i < size; i++) {
    x_gpu[i] = i; 
    x_cpu[i] = i; 
  }


  //GPU run
  auto k = compile(max_gpu); // Compile the kernel
  auto start_gpu = std::chrono::high_resolution_clock::now();
  k.load(size, &x_gpu, &result_gpu); // Load input data
  k.setNumQPUs(8); // Set the number of QPUs to use
  settings.process(k); // Run the kernel
  auto end_gpu = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> duration_gpu = end_gpu - start_gpu;
  
  //CPU run
  auto start_cpu = std::chrono::high_resolution_clock::now();
  max_cpu(size,x_cpu, result_cpu);
  auto end_cpu = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> duration_cpu = end_cpu - start_cpu;
  
  //functional verification
    if(result_cpu[0] != result_gpu[0]){
      printf("CPU output and GPU output is not equal at j \n");
      printf("GPU output = %d and CPU output = %d \n",result_gpu[0],result_cpu[0]);
    }
  
  printf(".........Execution Time.........\n");
  printf("Execution time for CPU: %f seconds\n", duration_cpu.count());
  printf("Execution time for GPU: %f seconds\n", duration_gpu.count());

 // printf("Maximum value GPU = %d\n", result_gpu[0]);
 // printf("Maximum value CPU = %d\n", result_cpu[0]);
  

  return 0;
}

