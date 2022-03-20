cmake_minimum_required(VERSION 3.13)

project(empty-supla-linux-example)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_BUILD_TYPE Debug)

find_package(OpenSSL REQUIRED)
# Current board sources
set(BOARD_SRC
  main.cpp
  )

add_library(supladevice "")
add_subdirectory($ENV{SUPLA_DEVICE_PATH} supla-device)
add_subdirectory($ENV{SUPLA_DEVICE_PATH}/extras/porting/linux supla-device-linux)

include_directories(
  $ENV{SUPLA_DEVICE_PATH}/src
  .
  )

add_executable(empty-supla-linux-example
  ${BOARD_SRC}
  )

set_target_properties(empty-supla-linux-example PROPERTIES LINK_LIBRARIES -pthread)
target_link_libraries (empty-supla-linux-example LINK_PUBLIC
  supladevice
  OpenSSL::SSL
  )

