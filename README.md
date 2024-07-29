# PDMod4Play
Tracker module player demo for Playdate based on [mod4play](https://github.com/dashodanger/mod4play) library by [dashodanger](https://github.com/dashodanger).

This project is not meant to be a full-scale module player app for Playdate. This is a demo for Playdate developers who want to use tracker music in their projects.

You can add MOD, XM, S3M, and IT file to the `Source` folder and the demo app will recognize them. Press UP or DOWN buttons to scan through modules.

Playdate is quite slow with memory operations, so by default the performance is not very good. 8-channel S3Ms choke on a RevA Playdates, but 8-channels XMs play well. RevB Playdates do better. I'm going to experiment with mixing routines and try to optimize them.

## Building instructions

This is a regular CMake-based Playdate app, so no special instructions. Create `build` and `build_device` folders.

Build for the simulator:

```
cd build_device
cmake ..
make
```

Build for the device (there is `device.sh` script for convenience):

```
cd build_device
cmake -DCMAKE_TOOLCHAIN_FILE=${PLAYDATE_SDK_PATH}/C_API/buildsupport/arm.cmake ..
make
```
