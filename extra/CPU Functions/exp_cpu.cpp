#include <stdio.h>
#include <math.h>

int main() {
    int n;
    printf("Enter the size of the tensor: ");
    scanf("%d", &n);

    // Allocate memory for the tensor and result
    float x[n];
    float result[n];

    printf("Enter the elements of the tensor:\n");
    for (int i = 0; i < n; i++) {
        scanf("%f", &x[i]);
    }

    // Compute the exponential for each element
    for (int i = 0; i < n; i++) {
        result[i] = expf(x[i]);
    }

    printf("Exponential of the tensor:\n");
    for (int i = 0; i < n; i++) {
        printf("%f ", result[i]);
    }

    return 0;
}

