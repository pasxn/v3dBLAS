#!/bin/bash

git clone https://github.com/wimrijnders/V3DLib.git
git clone https://github.com/wimrijnders/CmdParameter.git

cd CmdParameter
make DEBUG=1 all
cd ..

cp generate.sh V3DLib/script
cp Makefile V3DLib
  
cd V3DLib
./script/generate.sh
make QPU=0 DEBUG=1 all

cd ..
