#!/bin/bash

# Check if an argument was provided
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <kernel_name>"
    exit 1
fi

KERNEL_NAME=$1

# List of valid kernel names
VALID_KERNELS=("add" "div" "exp" "GEMM1" "GEMM2" "LOG" "max" "mul" "neg" "pow" "relu" "sub" "sum")

# Check if provided kernel name is valid
if ! [[ " ${VALID_KERNELS[@]} " =~ " ${KERNEL_NAME} " ]]; then
    echo "Invalid kernel name. Please choose from: ${VALID_KERNELS[@]}"
    exit 1
fi

cd V3DLib
# compile the kernel // make command for the given kernel and redirect output
make QPU=0 DEBUG=1 $KERNEL_NAME

#run the kernel
../target/emu-debug/bin/$KERNEL_NAME > ../extra/python_graph_plot_script/output.txt
# Change directory and run the Python script
cd ../extra/python_graph_plot_script
python3 plot.py &

