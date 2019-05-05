#!/bin/bash

cc stb_image_write.cpp main.cpp -o ray -lc++ && ./ray > output.ppm