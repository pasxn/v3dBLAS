#include <stdio.h>
#include "V3DLib.h"
#include "Support/Settings.h"

using namespace V3DLib;

V3DLib::Settings settings;

void compute_power(Int n, Float::Ptr x, Float::Ptr exponent, Float::Ptr result) {
  Float a;
  Float exp_val;
  Float res;
  For (Int i = 0, i < n, i += 16)
    a = x[i];
    exp_val = *exponent;
    res = 1.0f; 
    Float j = 0.0f;
    While (j < exp_val)
      res = res * a; 
      j = j + 1.0f; 
    End
    result[i] = res; 
  End
}

int main() {
  int size = 10; 
  Float::Array x(size), result(size), exponent(1);
  exponent[0] = 2.0f; 
  
  for (int i = 0; i < size; i++) {
    x[i] = static_cast<float>(i) / 100.0f; 
  }

  auto k = compile(compute_power); // Compile the kernel
  k.load(size, &x, &exponent, &result); // Load input data
  k.setNumQPUs(12); // Set the number of QPUs to use
  settings.process(k); // Run the kernel

  // Print the result array
  printf("Result:\n");
  for (int i = 0; i < size; i++) {
    printf("%f\n", result[i]);
  }

  return 0;
}

