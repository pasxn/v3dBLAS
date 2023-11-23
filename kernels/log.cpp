#include <stdio.h>
#include "V3DLib.h"
#include <chrono>
#include <math.h>
#include <CmdParameters.h>
#include "Support/Settings.h"

using namespace V3DLib;

V3DLib::Settings settings;

void log_gpu(Int n, Float::Ptr x, Float::Ptr result_gpu) {
  Float ln2 = 0.693147; // Value of natural log of 2
  For (Int i = 0, i < n, i += 16) {
    Float a = x[i];
    result_gpu[i] = log(a) * ln2;  // Convert base-2 log to natural log
  }
  End
}

void log_cpu(int n, float *x, float *result_cpu) {
    for (int i = 0; i < n; i++) {
        result_cpu[i] = log(x[i]);
    }
}

int main() {

  int iterations = 1000;

  
  for (int size = 512; size <= 131072; size *= 2) { // Adjust size range and multiplication factor as needed
    Float::Array x_gpu(size), result_gpu(size);
    float x_cpu[size], result_cpu[size];
    
    for (int i = 0; i < size; i++) {
      x_gpu[i] = 10;  // Use a constant or varying input values
      x_cpu[i] = 10;
    }

    auto k = compile(log_gpu);
    k.setNumQPUs(8);

    auto start_gpu = std::chrono::high_resolution_clock::now();
    for(int i=0;i<iterations; i++){
      k.load(size, &x_gpu, &result_gpu);
    }
    settings.process(k);
    auto end_gpu = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_gpu = end_gpu - start_gpu;

    auto start_cpu = std::chrono::high_resolution_clock::now();
    for(int i=0;i<iterations; i++){
      log_cpu(size, x_cpu, result_cpu);
    }
    auto end_cpu = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_cpu = end_cpu - start_cpu;

    // Functional verification with tolerance
    float epsilon = 1e-5;
    for (int j = 0; j < size; j++) {
      if (fabs(result_cpu[j] - result_gpu[j]) > epsilon) {
        printf("Difference at index %d: GPU = %f, CPU = %f\n", j, result_gpu[j], result_cpu[j]);
      }
    }

    printf("[%d, %f, %f]\n", size, duration_cpu.count(), duration_gpu.count());
  }

  return 0;
}

