#!/usr/bin/env bash

# If project not ready, generate cmake file.
if [[ ! -d build ]]; then
    echo "good"
else
    rm -rf build
fi
cmake -B build
cmake --build build

# Run all testcases. 
# You can comment some lines to disable the run of specific examples.
mkdir -p output
#build/PA1 testcases/scene01_basic.txt output/scene01.bmp
#build/PA1 testcases/scene02_cube.txt output/scene02.bmp
#build/PA1 testcases/scene03_sphere.txt output/scene03.bmp
#build/PA1 testcases/scene04_axes.txt output/scene04.bmp
#build/PA1 testcases/scene05_bunny_200.txt output/scene05.bmp
#build/PA1 testcases/scene06.txt output/scene06.bmp
#build/PA1 testcases/scene07_shine.txt output/scene07.bmp
#build/PA1 testcases/scene09_norm.txt output/scene09.bmp
#build/PA1 testcases/scene10_wineglass.txt output/scene10.bmp
#build/PA1 testcases/scene10_wineglass_720p.txt output/scene10_720p.bmp
build/PA1 testcases/scene11_smallpt.txt output/scene11.bmp
#build/PA1 testcases/scene12.txt output/scene12.bmp
#build/PA1 testcases/scene13.txt output/scene13.bmp
