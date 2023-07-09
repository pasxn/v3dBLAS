#!/bin/bash

./clonebuildv3d.sh

sleep 1

directory="./kernels"

cd "$directory" || exit 1
cpp_files=$(find . -maxdepth 1 -type f -name "*.cpp")
cd ..

printf "\n\n================================ Building ================================\n\n"
for file in $cpp_files; do
  filename=$(basename "$file" .cpp)
  ./make.sh $filename
done

sleep 1

machine_arch=$(uname -m)

if [ "$machine_arch" = "x86_64" ]; then
    cd target/emu-debug/bin
else
    cd target/qpu/bin
fi

printf "\n\n================================ Running Tests ================================\n\n"
for file in $cpp_files; do
  filename=$(basename "$file" .cpp)
  ./$filename
done
