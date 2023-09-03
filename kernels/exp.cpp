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
  int size = 8; 
  Float::Array x_gpu(size), result_gpu(size);
  
  float x_cpu[size], result_cpu[size];

  for (int i = 0; i < size; i++) {
    x_gpu[i] = i; 
    x_cpu[i] = i; 
  }
  
  //GPU run
  auto k = compile(exponential_gpu); 
  auto start_gpu = std::chrono::high_resolution_clock::now();
  k.load(size, &x_gpu, &result_gpu); 
  k.setNumQPUs(8); 
  settings.process(k); // Run the kernel
  auto end_gpu = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> duration_gpu = end_gpu - start_gpu;
  
  
  //CPU run
  auto start_cpu = std::chrono::high_resolution_clock::now();
  exp_CPU(size,x_cpu, result_cpu);
  auto end_cpu = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> duration_cpu = end_cpu - start_cpu;
  
  //functional verification
  for(int j = 0; j < size; j++) {
    if(result_cpu[j] != result_gpu[j]){
      printf("CPU output and GPU output is not equal at j = %d \n", j);
      printf("GPU output = %f and CPU output = %f \n",result_gpu[j],result_cpu[j]);
    }
  }  
  
  printf(".........Execution Time.........\n");
  printf("Execution time for CPU: %f seconds\n", duration_cpu.count());
  printf("Execution time for GPU: %f seconds\n", duration_gpu.count());


  // Print the result array
 /* printf("Result:\n");
  for (int i = 0; i < size; i++) {
    printf("%f\n", result_gpu[i]);
  }*/

  return 0;
}

