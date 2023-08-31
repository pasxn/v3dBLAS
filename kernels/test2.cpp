#include <stdio.h>
#include "V3DLib.h"
#include "Support/Settings.h"

using namespace V3DLib;

V3DLib::Settings settings;

void find_max(Int n, Int::Ptr x, Int::Ptr max_val, Int::Ptr qpu_num) {
  Int local_max = 0; 
  Int num_qpus = 8;
  Int num_elements_per_qpu = n / num_qpus;  
  Int start_index = me() * num_elements_per_qpu;
  Int end_index = start_index + num_elements_per_qpu;


  For (Int i = start_index, i < end_index, i++)
    Int a = x[i];
    Where (a > local_max)
      local_max = a;
    End
  End
  
  qpu_num[me()] = me();
  
  max_val[me()] = local_max;

  For (Int offset = 4, offset > 0, offset = offset / 2)
    If (me() < offset)
      Int neighbor_max = max_val[me() + offset];
      Where (neighbor_max > local_max)
        local_max = neighbor_max;
      End
      max_val[me()] = local_max;
    End
  End


  If (me() == 0)
    max_val[0] = local_max;
  End
}

int main() {
  int size = 1024; 
  Int::Array x(size), max_val(8), qpu_num(8); 


  for (int i = 0; i < size; i++) {
    x[i] = i;
  }

  auto k = compile(find_max); // Compile the kernel
  k.load(size, &x, &max_val, &qpu_num); // Load input data
  k.setNumQPUs(8); // Set the number of QPUs to use
  settings.process(k); // Run the kernel

  printf("Maximum value = %d\n", max_val[0]);
  
  for(int i=0;i<8;i++){
    printf("QPU: %i\n",qpu_num[i]);
  
  }

  return 0;
}

