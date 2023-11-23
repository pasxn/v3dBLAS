#include <stdio.h>
#include <chrono>
#include "V3DLib.h"
#include <CmdParameters.h>
#include "Support/Settings.h"

using namespace V3DLib;

V3DLib::Settings settings;

void add(Int n, Int::Ptr x, Int::Ptr y, Int::Ptr z) {
  For (Int i = 0, i < n, i += 16)
    Int a = x[i];
    Int b = y[i];
    z[i] = a + b;
  End
}

void addArrays(int size, const int* aa, const int* bb, int* rr) {
    for (int i = 0; i < size; i++) {
    	int ca = aa[i];
    	int cb = bb[i];
    	rr[i] = ca + cb;
    }
}

int main(int argc, const char *argv[]) {
  settings.init(argc, argv);
  
  for (int size = 2; size <= 2048; size *= 2) { // Adjust size range and multiplication factor as needed
    int iterations = 1000;

    Int::Array a(size);
    Int::Array b(size);
    Int::Array r(size);
    
    for (int i = 0; i < size; i++) {
      a[i] = 1;
      b[i] = 1;
    }

    int aa[size];
    int bb[size];
    int rr[size];

    for(int i = 0; i < size; i++) {
      aa[i]=1;
      bb[i]=1;
    }
    
    auto k = compile(add);
    k.setNumQPUs(8);

    auto start_gpu = std::chrono::high_resolution_clock::now();
    for(int y = 0; y < iterations ; y++) {
      k.load(size, &a, &b, &r);
    }
    settings.process(k);
    auto end_gpu = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_gpu = end_gpu - start_gpu;

    auto start_cpu = std::chrono::high_resolution_clock::now();
    for(int y = 0; y < iterations ; y++) {
      addArrays(size, aa, bb, rr);
    }
    auto end_cpu = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_cpu = end_cpu - start_cpu;

    printf("Tensor size: %d\n", size);
    printf("Execution time for CPU: %f seconds\n", duration_cpu.count());
    printf("Execution time for GPU: %f seconds\n", duration_gpu.count());
    printf("------------------------------\n");
  }

  return 0;
}

