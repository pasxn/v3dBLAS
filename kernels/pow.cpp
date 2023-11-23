#include <stdio.h>
#include "V3DLib.h"
#include <chrono>
#include <math.h>
#include "Support/Settings.h"

using namespace V3DLib;

V3DLib::Settings settings;

void pow_gpu(Int n, Float::Ptr x, Float::Ptr exponent, Float::Ptr result_gpu) {
  Float a;
  Float exp_val;
  Float res;
  For (Int i = 0, i < n, i += 16) {
    a = x[i];
    exp_val = *exponent;
    res = 1.0; 
    Float j = 0.0;
    While (j < exp_val) {
      res = res * a; 
      j = j + 1.0; 
    }
    End
    result_gpu[i] = res; 
  }
  End
}

void pow_cpu(int n, float *x, float exponent, float *result_cpu) {
    for (int i = 0; i < n; i++) {
        result_cpu[i] = pow(x[i], exponent);
    }
}

/*void pow_cpu(int n, float *x, float exponent, float *result_cpu) {
    for (int i = 0; i < n; i++) {
        float a = x[i];
        float res = 1.0;

        for (float j = 0.0; j < exponent; j++) {
            res *= a;
        }

        result_cpu[i] = res;
    }
}
*/

int main() {
  
  for (int size = 2; size <= 2048; size *= 2) { // Adjust size range and multiplication factor as needed
    Float::Array x_gpu(size), result_gpu(size), exponent_gpu(1);
    exponent_gpu[0] = 2;  // Exponent value
    int iterations = 5000;
    
    float x_cpu[size], result_cpu[size], exponent_cpu = 2;
    
    for (int i = 0; i < size; i++) {
      x_gpu[i] = 3;  // Use a constant or varying input values
      x_cpu[i] = 3;
    }

    auto k = compile(pow_gpu);
    k.setNumQPUs(8);

    auto start_gpu = std::chrono::high_resolution_clock::now();
    for(int i=0;i<iterations; i++){
      k.load(size, &x_gpu, &exponent_gpu, &result_gpu);
    }
    settings.process(k);
    auto end_gpu = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_gpu = end_gpu - start_gpu;

    auto start_cpu = std::chrono::high_resolution_clock::now();
    for(int i=0;i<iterations; i++){
      pow_cpu(size, x_cpu, exponent_cpu, result_cpu);
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

