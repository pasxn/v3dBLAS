#include <stdio.h>
#include "V3DLib.h"
#include "Support/Settings.h"

using namespace V3DLib;

V3DLib::Settings settings;

void relu(Int n, Float::Ptr x, Float::Ptr result) {
  Float a;
  For (Int i = 0, i < n, i += 16)
    a = x[i];
    Where (a < 0.0f)  
      a = 0.0f;      
    End
    result[i] = a;   
  End
}

int main() {
  int size = 100; 
  Float::Array x(size), result(size);


  for (int i = 0; i < size; i++) {
    x[i] = static_cast<float>(i) - 50.0f; 
  }

  auto k = compile(relu); // Compile the kernel
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

