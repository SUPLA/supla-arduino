cmake_minimum_required(VERSION 3.13)

project(supla-freertos-example)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_BUILD_TYPE Debug)

# Current board sources
set(BOARD_SRC
  main.cpp
  )

# All dependencies (supla-device + freeRTOS) will be compiled together
# because freeRTOS build configuration depends on local board
# FreeRTOSConfig.h file. supla-device depends on FreeRTOS.h and beacuse of
# that everything depends on local board files...
add_library(supladevice "")
add_subdirectory($ENV{SUPLA_DEVICE_PATH} supla-device)
add_subdirectory($ENV{SUPLA_DEVICE_PATH}/extras/porting/freertos_linux supla-device-freertos)

target_sources(supladevice PUBLIC ${FREERTOS_SRC} )

include_directories(
  $ENV{SUPLA_DEVICE_PATH}/src
  .
  )

add_executable(supla-freertos-example
  ${BOARD_SRC}
  )

set_target_properties(supla-freertos-example PROPERTIES LINK_LIBRARIES -pthread)
target_link_libraries (supla-freertos-example LINK_PUBLIC
  supladevice
  )

