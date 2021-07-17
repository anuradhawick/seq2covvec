#!/bin/bash

[ -d bin ] && rm -r bin
mkdir bin    

case $1 in

  osx | macos)
    echo "OSX Build"
    # OSX Build (modify the include path to suit you; you might need to run brew install libomp or brew install llvm)
    echo "BUILDING THE MetaBCC-LR 15 MER COMPUTATIONS"
    clang++ src/main.cpp -lomp -fopenmp -lpthread -Wall -o bin/seq2covvec -I/usr/local/include -L/usr/local/lib -lz -O3 -std=c++17
    echo "BUILD FINISHED"
    ;;

  *)
    echo "Linux Build"
    # Linux
    echo "BUILDING THE MetaBCC-LR 15 MER COMPUTATIONS"
    g++ src/main.cpp -fopenmp -lpthread -Wall -o bin/seq2covvec -lz -O3 -std=c++17
    echo "BUILD FINISHED"
    ;;
esac
