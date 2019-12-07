#!/bin/bash

cmake -H. -BBin -DCMAKE_BUILD_TYPE=DEBUG

cmake --build Bin

rm -r ./Executable/
mkdir Executable
cp ./Bin/RemMux ./Executable/

