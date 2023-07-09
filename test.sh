#!/bin/bash

./clonebuildv3d.sh

directory="./kernels"

cd "$directory" || exit 1
cpp_files=$(find . -maxdepth 1 -type f -name "*.cpp")
cd ..

for file in $cpp_files; do
  filename=$(basename "$file" .cpp)
  ./make.sh $filename
done
