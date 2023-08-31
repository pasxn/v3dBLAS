#include <stdio.h>
#include "V3DLib.h"
#include "Support/Settings.h"

using namespace V3DLib;

V3DLib::Settings settings;

void compute_logarithm(Int n, Float::Ptr x, Float::Ptr result) {
  For (Int i = 0, i < n, i += 16)
    Float a = x[i];
    result[i] = log(a); 
  End
}

int main() {
  int size = 10; 
  Float::Array x(size), result(size);

  
  for (int i = 1; i <= size; i++) {
    x[i - 1] = static_cast<float>(i) / 100.0f; 
  }

  auto k = compile(compute_logarithm); // Compile the kernel
  k.load(size, &x, &result); // Load input data
  k.setNumQPUs(12); // Set the number of QPUs to use
  settings.process(k); // Run the kernel

  // Print the result array
  printf("Result:\n");
  for (int i = 0; i < size; i++) {
    printf("%f\n", result[i]);
  }

  return 0;
}

