#include <stdio.h>
#include <chrono>
#include "V3DLib.h"
#include <CmdParameters.h>
#include "Support/Settings.h"

using namespace V3DLib;

V3DLib::Settings settings;

// Kernel
void div(Int n, Int::Ptr x, Int::Ptr y, Int::Ptr z) {
  For (Int i = 0, i < n, i += 16)
    Int a = x[i];
    Int b = y[i];
    z[i] = a / b; // Ensure b is not zero
  End
}

// C++ function
void divArrays(int size, const int* aa, const int* bb, int* rr) {
    for (int i = 0; i < size; i++) {
    	int ca = aa[i];
    	int cb = bb[i]; // Ensure bb[i] is not zero
    	rr[i] = ca / cb;
    }
}

int main(int argc, const char *argv[]) {
  settings.init(argc, argv);

  for (int size = 2; size <= 2048; size *= 2) { // Adjust size range and multiplication factor as needed
    int iterations = 1000;

    // GPU arrays
    Int::Array a(size);
    Int::Array b(size);
    Int::Array r(size);
    for (int i = 0; i < size; i++) {
      a[i] = 1;
      b[i] = 2;
    }

    // CPU arrays
    int aa[size];
    int bb[size];
    int rr[size];
    for(int i = 0; i < size; i++) {
      aa[i] = 1;
      bb[i] = 2;
    }
    
    auto k = compile(div);
    k.setNumQPUs(8);

    auto start_gpu = std::chrono::high_resolution_clock::now();
    for(int y = 0; y < iterations; y++) {
      k.load(size, &a, &b, &r);
    }
    settings.process(k);
    auto end_gpu = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_gpu = end_gpu - start_gpu;

    auto start_cpu = std::chrono::high_resolution_clock::now();
    for(int y = 0; y < iterations; y++) {
      divArrays(size, aa, bb, rr);
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

