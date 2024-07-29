#!/bin/zsh

cd build_device
cmake -DCMAKE_TOOLCHAIN_FILE=/Users/romanpetrov/Developer/PlaydateSDK/C_API/buildsupport/arm.cmake ..
make
cd ..
