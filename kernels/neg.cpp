#include <stdio.h>
#include <chrono>
#include "V3DLib.h"
#include <CmdParameters.h>
#include "Support/Settings.h"

using namespace V3DLib;

V3DLib::Settings settings;

// Kernel for negation
void neg(Int n, Int::Ptr x, Int::Ptr z) {
  For (Int i = 0, i < n, i += 16)
    Int a = x[i];
    z[i] = 0-a;
  End
}

// C++ function for negation
void negArrays(int size, const int* aa, int* rr) {
    for (int i = 0; i < size; i++) {
    	int ca = aa[i];
    	rr[i] = 0-ca;
    }
}

int main(int argc, const char *argv[]) {
  settings.init(argc, argv);

  for (int size = 2; size <= 2048; size *= 2) {
    int iterations = 1000;

    // GPU arrays
    Int::Array a(size);
    Int::Array r(size);
    for (int i = 0; i < size; i++) {
      a[i] = 1; // Or any other value that you want to negate
    }

    // CPU arrays
    int aa[size];
    int rr[size];
    for(int i = 0; i < size; i++) {
      aa[i] = 1; // Same as above
    }
    
    auto k = compile(neg);
    k.setNumQPUs(8);

    auto start_gpu = std::chrono::high_resolution_clock::now();
    for(int y = 0; y < iterations; y++) {
      k.load(size, &a, &r);
    }
    settings.process(k);
    auto end_gpu = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_gpu = end_gpu - start_gpu;

    auto start_cpu = std::chrono::high_resolution_clock::now();
    for(int y = 0; y < iterations; y++) {
      negArrays(size, aa, rr);
    }
    auto end_cpu = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_cpu = end_cpu - start_cpu;
    
    // Functional verification
    for(int j = 0; j < size; j++) {
      if(r[j] != rr[j])
        printf("CPU output and GPU output is not equal at j = %d \n", j);
    }
    
    printf("[%d, %f, %f]\n", size, duration_cpu.count(), duration_gpu.count());
  }

  return 0;
}

