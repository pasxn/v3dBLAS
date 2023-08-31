#include <stdio.h>
#include <chrono>
#include "V3DLib.h"
#include <CmdParameters.h>
#include "Support/Settings.h"

using namespace V3DLib;

V3DLib::Settings settings;

void partial_sum(Int n, Int::Ptr p_temp, Int::Ptr r_temp) {
  Int sum = 0;
  For (Int i = 0, i < n, i++)
    Int a = p_temp[i];
    Int b = sum;
    sum = a + b;
  End
  r_temp[0] = sum;
}

int main() {
  int size = 1000;  // Size of the full vector
  int chunk_remainder = size % 8;
  int chunk_size = (size - chunk_remainder) / 8;  // Size of each segment
  Int::Array p(size);
  Int::Array r_temp(1);  // One partial sum per QPU
  Int::Array p_temp(size); // Size adjusted to accommodate remainder
  Int::Array r(8);

  for (int i = 0; i < size; ++i) {
    p[i] = 1;
  }

  auto k = compile(partial_sum);
  k.setNumQPUs(8);

  // Each QPU sums a separate segment of the vector
  for (int i = 0; i < 8; ++i) {
    for (int y = 0; y < chunk_size; y++) {
      p_temp[y] = p[i * chunk_size + y];
    }
    k.load(chunk_size, &p_temp, &r_temp);
    settings.process(k);
    r[i] = r_temp[0];
  }

  // Handle the remainder using the compute kernel
  if (chunk_remainder > 0) {
    for (int y = 0; y < chunk_remainder; y++) {
      p_temp[y] = p[8 * chunk_size + y];
    }
    k.load(chunk_remainder, &p_temp, &r_temp);
    settings.process(k);
    r[7] += r_temp[0]; // Add the remainder sum to the last partial sum
  }

  int total_sum = 0;
  for (int i = 0; i < 8; ++i) {
    total_sum += r[i];
  }

  printf("Sum = %d\n", total_sum);

  return 0;
}
