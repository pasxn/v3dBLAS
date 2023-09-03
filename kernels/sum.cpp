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
  int size = 12354;  // Size of the full vector
  Int::Array x_gpu(size), result_gpu(8);
  
  int x_cpu[size] , result_cpu;

  for (int i = 0; i < size; ++i) {
    x_gpu[i] = 1; 
    x_cpu[i] = 1; 
  }

  //GPU run
  auto k = compile(sum_gpu);
  auto start_gpu = std::chrono::high_resolution_clock::now();
  k.setNumQPUs(8);
  k.load(size, &x_gpu, &result_gpu);
  settings.process(k);
  auto end_gpu = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> duration_gpu = end_gpu - start_gpu;
  
  //CPU run
  auto start_cpu = std::chrono::high_resolution_clock::now();
  sum_cpu(size,x_cpu, &result_cpu);
  auto end_cpu = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> duration_cpu = end_cpu - start_cpu;

  int total_sum_gpu = 0;
  for (int i = 0; i < 8; ++i) {
    total_sum_gpu += result_gpu[i];
  }
  
    if(result_cpu != total_sum_gpu){
      printf("CPU output and GPU output is not equal \n");
      printf("GPU output = %d and CPU output = %d \n", total_sum_gpu,result_cpu);
    }  
  
  printf(".........Execution Time.........\n");
  printf("Execution time for CPU: %f seconds\n", duration_cpu.count());
  printf("Execution time for GPU: %f seconds\n", duration_gpu.count());

 // printf("Sum = %d\n", total_sum_gpu);

  return 0;
}

