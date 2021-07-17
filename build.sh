#!/bin/bash

[ -d bin ] && rm -r bin
mkdir bin    

case $1 in

  osx | macos)
    echo "OSX Build"
    # OSX Build (modify the include path to suit you; you might need to run brew install libomp or brew install llvm)
    echo "BUILDING THE MetaBCC-LR 15 MER COMPUTATIONS"
    clang++ src/search-15mers.cpp -lomp -fopenmp -lpthread -Wall -o bin/search-15mers -I/usr/local/include -L/usr/local/lib -lz -O3 -std=c++17
    echo "BUILD FINISHED"
    ;;

  *)
    echo "Linux Build"
    # Linux
    echo "BUILDING THE MetaBCC-LR 15 MER COMPUTATIONS"
    g++ src/search-15mers.cpp -fopenmp -lpthread -Wall -o bin/search-15mers -lz -O3 -std=c++17
    echo "BUILD FINISHED"
    ;;
esac
