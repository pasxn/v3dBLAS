#include <iostream>
#include <vector>

// Function to calculate the sum of a long tensor using a divide-and-conquer approach
long long calculate_tensor_sum(const std::vector<int>& tensor, int start, int end) {
    // Base case: If the tensor has only one element, return that element as the sum
    if (start == end) {
        return tensor[start];
    }

    // Divide the tensor into two halves
    int mid = (start + end) / 2;
    long long left_sum = calculate_tensor_sum(tensor, start, mid);
    long long right_sum = calculate_tensor_sum(tensor, mid + 1, end);

    // Combine the sums of the left and right halves to get the final sum
    long long tensor_sum = left_sum + right_sum;

    // Return the final sum
    return tensor_sum;
}

int main() {
    // Example usage:
    std::vector<int> tensor = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // Calculate the sum of the entire tensor
    long long sum = calculate_tensor_sum(tensor, 0, tensor.size() - 1);
    std::cout << "Sum of the tensor: " << sum << std::endl;

    return 0;
}

