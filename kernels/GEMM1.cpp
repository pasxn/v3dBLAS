#include <stdio.h>
#include "V3DLib.h"
#include "Support/Settings.h"

using namespace V3DLib;

V3DLib::Settings settings;


void gemm(Int n, Float::Ptr a, Float::Ptr b, Float::Ptr c) {
  Float sum = 0.0f; 

  For (Int i = 0, i < n, i += 16)
    Float val_a = a[i];
    Float val_b = b[i];
    sum += val_a * val_b; 
  End

  // Store the result in the output tensor
  c[0] = sum;
}

int main() {
  int size = 100; // Size of the vectors
  Float::Array a(size), b(size), c(1); //cant make variable pointers, should be an array

  // Initialize vectors 'a' and 'b' with example values
  for (int i = 0; i < size; i++) {
    a[i] = static_cast<float>(i) / 10.0f;
    b[i] = static_cast<float>(size - i) / 10.0f;
  }

  auto k = compile(gemm); // Compile the kernel
  k.load(size, &a, &b, &c); // Load input data
  k.setNumQPUs(12); // Set the number of QPUs to use
  settings.process(k); // Run the kernel

  // Print the result (dot product)
  printf("Dot Product: %f\n", c[0]);

  return 0;
}

