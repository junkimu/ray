#!/bin/bash
NUMBER_OF_PROCESSORS=`nproc --all`
export NUMBER_OF_PROCESSORS

#gcc main.cpp stb_image_write.cpp -std=c++17 -O3 -lstdc++ -lm -lpthread -o ray && time ./ray
g++ -std=c++17 -O3 stb_image_write.cpp main.cpp -o ray -lpthread && time ./ray