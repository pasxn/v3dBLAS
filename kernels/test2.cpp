#include <stdio.h>
#include <chrono>
#include "V3DLib.h"
#include <CmdParameters.h>
#include "Support/Settings.h"

using namespace V3DLib;

V3DLib::Settings settings;

void tensor_sum(Int n, Int::Ptr p, Int::Ptr r) {
  Int qpu_id = me() * (n / numQPUs());
  Int qpu_end = (me() + 1) * (n / numQPUs());
  
  // Adjust for the last QPU to handle remainder
  Where(me() == numQPUs() - 1)
    qpu_end = n;
  End

  Int sum = 0;
  For (Int i = qpu_id, i < qpu_end, i++)
    Int val = p[i];
    sum += val;
  End

  r[me()] = sum;
}

int main() {
  int size = 12354;  // Size of the full vector
  Int::Array p(size);
  Int::Array r(8);

  for (int i = 0; i < size; ++i) {
    p[i] = 1;
  }

  auto k = compile(tensor_sum);
  k.setNumQPUs(8);
  k.load(size, &p, &r);
  settings.process(k);

  int total_sum = 0;
  for (int i = 0; i < 8; ++i) {
    total_sum += r[i];
  }

  printf("Sum = %d\n", total_sum);

  return 0;
}

