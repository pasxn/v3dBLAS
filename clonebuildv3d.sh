#!/bin/bash

git clone https://github.com/wimrijnders/V3DLib.git
git clone https://github.com/wimrijnders/CmdParameter.git

machine_arch=$(uname -m)

cd CmdParameter
if [ "$machine_arch" = "x86_64" ]; then
  make DEBUG=1 all
else
  make DEBUG=0 all
fi
cd ..

cp generate.sh V3DLib/script
cp Makefile V3DLib
  
cd V3DLib
./script/generate.sh
if [ "$machine_arch" = "x86_64" ]; then
  make QPU=0 DEBUG=1 all
else
  make QPU=1 DEBUG=0 all
fi

cd ..
