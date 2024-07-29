cmake_minimum_required(VERSION 3.14)
set(CMAKE_C_STANDARD 11)

set(ENVSDK $ENV{PLAYDATE_SDK_PATH})

if (NOT ${ENVSDK} STREQUAL "")
  # Convert path from Windows
  file(TO_CMAKE_PATH ${ENVSDK} SDK)
else()
  execute_process(
      COMMAND bash -c "egrep '^\\s*SDKRoot' $HOME/.Playdate/config"
      COMMAND head -n 1
      COMMAND cut -c9-
      OUTPUT_VARIABLE SDK
      OUTPUT_STRIP_TRAILING_WHITESPACE
  )
endif()

if (NOT EXISTS ${SDK})
  message(FATAL_ERROR "SDK Path not found; set ENV value PLAYDATE_SDK_PATH")
  return()
endif()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CMAKE_XCODE_GENERATE_SCHEME TRUE)

# Game Name Customization
set(PLAYDATE_GAME_NAME PDMod4Play)
set(PLAYDATE_GAME_DEVICE PDMod4Play_DEVICE)

project(${PLAYDATE_GAME_NAME} C ASM)

file(GLOB_RECURSE MAIN_SRC
  "src/*.c"
  "src/*.h"
  "mod4play/*.c"
  "mod4play/*.h"
)

if (TOOLCHAIN STREQUAL "armgcc")
  set(CMAKE_EXE_LINKER_FLAGS "--specs=nosys.specs" CACHE INTERNAL "")
  set(CMAKE_BUILD_TYPE Release)
  add_executable(${PLAYDATE_GAME_DEVICE} ${MAIN_SRC})
  target_compile_options(${PLAYDATE_GAME_DEVICE} PUBLIC -O3)
else()
  set(CMAKE_OSX_ARCHITECTURES "arm64")
  add_library(${PLAYDATE_GAME_NAME} SHARED ${MAIN_SRC} )
endif()

include(${SDK}/C_API/buildsupport/playdate_game.cmake)

