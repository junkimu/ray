#!/bin/zsh
NUMBER_OF_PROCESSORS=`sysctl -n hw.ncpu`
export NUMBER_OF_PROCESSORS

clang -std=c++17 -O3 stb_image_write.cpp main.cpp -o ray -lc++ && time ./ray