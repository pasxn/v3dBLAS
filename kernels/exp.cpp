#include <stdio.h>
#include "V3DLib.h"
#include "Support/Settings.h"

using namespace V3DLib;

V3DLib::Settings settings;

void compute_exponential(Int n, Float::Ptr x, Float::Ptr result) {
  For (Int i = 0, i < n, i += 16)
    Float a = x[i];
    result[i] = exp(a); 
  End
}

int main() {
  int size = 8; 
  Float::Array x(size), result(size);

  for (int i = 0; i < size; i++) {
    x[i] = static_cast<float>(i) / 100.0f; 
  }

  auto k = compile(compute_exponential); 
  k.load(size, &x, &result); 
  k.setNumQPUs(12); 
  settings.process(k); // Run the kernel

  // Print the result array
  printf("Result:\n");
  for (int i = 0; i < size; i++) {
    printf("%f\n", result[i]);
  }

  return 0;
}

