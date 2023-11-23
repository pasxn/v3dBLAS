import matplotlib.pyplot as plt
import matplotlib.ticker as ticker

def plot_graph():
    # Read data from output file
    data = []
    with open('output.txt', 'r') as f:
        lines = f.readlines()
        for line in lines:
            items = line.strip().replace("[", "").replace("]", "").split(",")  # Remove brackets and split by comma
            data.append([float(item) for item in items])

    # Extract tensor sizes, CPU times, and GPU times
    tensor_sizes = [item[0] for item in data]
    cpu_times = [item[1] for item in data]
    gpu_times = [item[2] for item in data]

    # Plotting the data
    plt.figure(figsize=(10,5))
    plt.plot(tensor_sizes, cpu_times, label='CPU', color='blue', marker='o')
    plt.plot(tensor_sizes, gpu_times, label='GPU', color='red', marker='o')
    plt.xlabel('Tensor Size')
    plt.ylabel('Execution Time (seconds)')
    plt.title('Execution Time Comparison between CPU and GPU')
    plt.legend()
    plt.grid(True)

    # Format y axis to display exact values
    y_formatter = ticker.ScalarFormatter(useOffset=False)
    plt.gca().yaxis.set_major_formatter(y_formatter)
    plt.gca().xaxis.set_major_formatter(y_formatter)

    plt.show()

plot_graph()

