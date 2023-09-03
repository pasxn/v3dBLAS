#include <stdio.h>
#include "V3DLib.h"
#include "Support/Settings.h"
#include <chrono>
#include <math.h>

using namespace V3DLib;

V3DLib::Settings settings;

void log_gpu(Int n, Float::Ptr x, Float::Ptr result_gpu) {
  Float ln2 = 0.693147; // Value of natural log of 2
  For (Int i = 0, i < n, i += 16)
    Float a = x[i];
    result_gpu[i] = log(a) * ln2;  // Convert base-2 log to natural log
  End
}

void log_cpu(int n, float *x, float *result_cpu) {
    for (int i = 0; i < n; ++i) {
        result_cpu[i] = log(x[i]);
    }
}


int main() {
  int size = 10000; 
  Float::Array x_gpu(size), result_gpu(size);
  
  float x_cpu[size] , result_cpu[size];

  
  for (int i = 1; i <= size; i++) {
    x_gpu[i - 1] = 10; 
    x_cpu[i - 1] = 10; 
  }
  
  
  //GPU run
  auto k = compile(log_gpu); // Compile the kernel
  auto start_gpu = std::chrono::high_resolution_clock::now();
  k.load(size, &x_gpu, &result_gpu); // Load input data
  k.setNumQPUs(8); // Set the number of QPUs to use
  settings.process(k); // Run the kernel
  auto end_gpu = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> duration_gpu = end_gpu - start_gpu;
  
  //CPU run
  auto start_cpu = std::chrono::high_resolution_clock::now();
  log_cpu(size,x_cpu, result_cpu);
  auto end_cpu = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> duration_cpu = end_cpu - start_cpu;
  
  //functional verification
/*  for(int j = 0; j < size; j++) {
    if(result_cpu[j] != result_gpu[j]){
      printf("CPU output and GPU output is not equal at j = %d \n", j);
      printf("GPU output = %f and CPU output = %f \n",result_gpu[j],result_cpu[j]);
    }
  } */
  
  printf(".........Execution Time.........\n");
  printf("Execution time for CPU: %f seconds\n", duration_cpu.count());
  printf("Execution time for GPU: %f seconds\n", duration_gpu.count());

  // Print the result array
/*  printf("Result:\n");
  for (int i = 0; i < size; i++) {
    printf("%f\n", result_gpu[i]);
  }   */

  return 0;
}

