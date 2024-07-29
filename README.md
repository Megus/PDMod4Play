# PDMod4Play
Tracker module player demo for Playdate

## Building instructions

Create `build` and `build_device` folders.

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
