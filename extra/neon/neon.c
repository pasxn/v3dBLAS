#include <stdio.h>
#include <arm_neon.h>

void add_neon(int size, float* a, float* b, float* r) {
  float32x4_t va = vdupq_n_f32(0);
  float32x4_t vb = vdupq_n_f32(0);
  float32x4_t vr = vdupq_n_f32(0);
  for (int i = 0; i < size; i += 4) {
    va = vld1q_f32(a + i);
    vb = vld1q_f32(b + i);
    vr = vaddq_f32(va, vb);
    vst1q_f32(r + i, vr);
  }
}

void add_cpu(int size, float* a, float* b, float* r) {
  for (int i = 0; i < size; i += 1) {
    *r = *a + *b;
  }
}

int main() {
  float a[8] = {1.1f, 2.2f, 3.3f, 4.4f, 5.5f, 6.6f, 7.7f, 8.8f};
  float b[8] = {0.1f, 0.1f, 0.1f, 0.1f, 0.1f, 0.1f, 0.1f, 0.1f};
  float r[8];

  add_neon(8, a, b, r);

  printf("result: ");
  for (int i = 0; i < 8; i++) {
    printf("%f ", r[i]);
  }
  printf("\n");

  return 0;
}
