#!/bin/zsh

cd build_device
cmake -DCMAKE_TOOLCHAIN_FILE=${PLAYDATE_SDK_PATH}/C_API/buildsupport/arm.cmake ..
make
cd ..
