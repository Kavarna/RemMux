#!/bin/bash

cmake -H. -BBin

cmake --build Bin

rm -r ./Executable/
mkdir Executable
cp ./Bin/RemMux ./Executable/

