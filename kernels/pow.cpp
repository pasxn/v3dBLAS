#include <stdio.h>
#include "V3DLib.h"
#include "Support/Settings.h"
#include <chrono>
#include <math.h>

using namespace V3DLib;

V3DLib::Settings settings;

void pow_gpu(Int n, Float::Ptr x, Float::Ptr exponent, Float::Ptr result_gpu) {
  Float a;
  Float exp_val;
  Float res;
  For (Int i = 0, i < n, i += 16)
    a = x[i];
    exp_val = *exponent;
    res = 1.0; 
    Float j = 0.0;
    While (j < exp_val)
      res = res * a; 
      j = j + 1.0; 
    End
    result_gpu[i] = res; 
  End
}

void pow_cpu(int n, float *x, float exponent, float *result_cpu) {
    for (int i = 0; i < n; i++) {
        result_cpu[i] = pow(x[i], exponent);
    }
}

int main() {
  int size = 10; 
  Float::Array x_gpu(size), result_gpu(size), exponent_gpu(1);
  exponent_gpu[0] = 2; 
  
  float x_cpu[size] , result_cpu[size], exponent_cpu;
  exponent_cpu = 2;
  
  for (int i = 0; i < size; i++) {
    x_gpu[i] = 3; 
    x_cpu[i] = 3;
  }

  //GPU run
  auto k = compile(pow_gpu); // Compile the kernel
  auto start_gpu = std::chrono::high_resolution_clock::now();
  k.load(size, &x_gpu, &exponent_gpu, &result_gpu); // Load input data
  k.setNumQPUs(8); // Set the number of QPUs to use
  settings.process(k); // Run the kernel
  auto end_gpu = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> duration_gpu = end_gpu - start_gpu;
  
  
  //CPU run
  auto start_cpu = std::chrono::high_resolution_clock::now();
  pow_cpu(size,x_cpu, exponent_cpu, result_cpu);
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
/*  printf("Result:\n");
  for (int i = 0; i < size; i++) {
    printf("%f\n", result_gpu[i]);
  } */

  return 0;
}

